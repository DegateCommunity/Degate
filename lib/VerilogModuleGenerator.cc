/* -*-c++-*-

 This file is part of the IC reverse engineering tool degate.

 Copyright 2008, 2009, 2010 by Martin Schobert

 Degate is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 any later version.

 Degate is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with degate. If not, see <http://www.gnu.org/licenses/>.

*/


#include <VerilogModuleGenerator.h>
#include <boost/format.hpp>
#include <boost/foreach.hpp>
#include <boost/algorithm/string/join.hpp>

using namespace boost;
using namespace degate;

VerilogModuleGenerator::VerilogModuleGenerator(Module_shptr module, bool do_not_output_gates /* = false */) :
  VerilogCodeTemplateGenerator(module->get_name(), module->get_entity_name()), 
  mod(module),
  no_gates(do_not_output_gates) {

  // set module ports
  for(Module::port_collection::const_iterator iter = module->ports_begin();
      iter != module->ports_end(); ++iter) {

    const std::string mod_port_name = iter->first;
    const GatePort_shptr gp = iter->second;
    const GateTemplatePort_shptr tmpl_port = gp->get_template_port();
    assert(tmpl_port != NULL);
    add_port(mod_port_name, tmpl_port->is_inport());
  }

}


VerilogModuleGenerator::~VerilogModuleGenerator() {
}


std::string VerilogModuleGenerator::generate_common_code_for_gates(Module_shptr module,
								   std::set<GateTemplate_shptr> & already_dumped) const {
  std::string common_code;

  if(!no_gates) {
    for(Module::gate_collection::const_iterator iter = module->gates_begin();
	iter != module->gates_end(); ++iter) {
      Gate_shptr gate = *iter;
      
      if(GateTemplate_shptr gtmpl = gate->get_gate_template()) {
	
	if(already_dumped.find(gtmpl) == already_dumped.end()) {
	  
	  try {
	    common_code += gtmpl->get_implementation(GateTemplate::VERILOG);
	  }
	  catch(CollectionLookupException const& ex) {
	    // maybe we should pass the exception?
	    common_code += "// Error: failed to lookup Verilog implementation for module " + gtmpl->get_name() + ".\n\n";
	  }
	  already_dumped.insert(gtmpl);
	}
      }
    }
  }

  // traverse sub-modules
  for(Module::module_collection::const_iterator iter = module->modules_begin();
      iter != module->modules_end(); ++iter) {

    Module_shptr sub = *iter;
    common_code += generate_common_code_for_gates(sub, already_dumped);

    VerilogModuleGenerator codegen(sub, true);   
    common_code += codegen.generate();
    
  }

  return common_code;
}

std::string VerilogModuleGenerator::generate_common() const {
  std::set<GateTemplate_shptr> already_dumped;
  return generate_common_code_for_gates(mod, already_dumped);
}

std::string VerilogModuleGenerator::generate_impl(std::string const& logic_class /* unused parameter */ ) const {
  
  std::string impl;
  std::string wire_definitions;

  unsigned int wire_counter = 0;

  typedef std::map<object_id_t /* net */, std::string> net_names_table;
  net_names_table nets;


  // generate signal names
  for(Module::gate_collection::const_iterator iter = mod->gates_begin();
      iter != mod->gates_end(); ++iter) {

    Gate_shptr gate = *iter;
    GateTemplate_shptr gate_tmpl = gate->get_gate_template();
    for(Gate::port_const_iterator p_iter = gate->ports_begin(); p_iter != gate->ports_end(); ++p_iter) {
      const GatePort_shptr gport = *p_iter;
      if(gport->is_connected()) {
	const Net_shptr net = gport->get_net();
	
	// first, check if the gate port is directly adjacent to a module port
	boost::optional<std::string> is_module_port = mod->lookup_module_port_name(gport);
	if(is_module_port) {
	  nets[net->get_object_id()] = *is_module_port;
	}
	else if(nets.find(net->get_object_id()) == nets.end()) {
	  boost::format f("w%1%");
	  f % (wire_counter++);
	  nets[net->get_object_id()] = f.str();
	}
      }
    }
  }


  for(Module::module_collection::const_iterator iter = mod->modules_begin();
      iter != mod->modules_end(); ++iter) {

    Module_shptr sub = *iter;
    std::list<std::string> ports;

    // iterate over its module ports
    for(Module::port_collection::const_iterator p_iter = sub->ports_begin();
	p_iter != sub->ports_end(); ++p_iter) {

      std::string const& submod_port_name= p_iter->first;
      const GatePort_shptr gport = p_iter->second;

      if(gport->is_connected()) {
	const Net_shptr net = gport->get_net();

	nets[net->get_object_id()] = submod_port_name;
      }
    }
  }


  // genereate wire definitions
  BOOST_FOREACH(net_names_table::value_type const& v, nets) {
    if(!mod->exists_module_port_name(v.second)) 
      wire_definitions += "  wire " + v.second + ";\n";
  }


  
  // place single standard cells

  for(Module::gate_collection::const_iterator iter = mod->gates_begin();
      iter != mod->gates_end(); ++iter) {

    Gate_shptr gate = *iter;
    GateTemplate_shptr gate_tmpl = gate->get_gate_template();
    
    std::list<std::string> ports;

    for(Gate::port_const_iterator p_iter = gate->ports_begin(); p_iter != gate->ports_end(); ++p_iter) {
      const GatePort_shptr gport = *p_iter;
      const GateTemplatePort_shptr tmpl_port = gport->get_template_port();

      if(gport->is_connected()) {
	const Net_shptr net = gport->get_net();
      
	std::string port_name = generate_identifier(tmpl_port->get_name());
	std::transform(port_name.begin(), port_name.end(), port_name.begin(), ::tolower);
	
	boost::format f("    .%1% (%2%)");
	f % port_name
	  % nets[net->get_object_id()];
	ports.push_back(f.str());
      }
    }


    boost::format gate_placement("  %1% %2% (\n"
				 "%3% );\n\n");
    gate_placement 
      % generate_identifier(gate_tmpl->get_name(), "dg_")
      % generate_identifier(gate->get_name())
      % boost::algorithm::join(ports, ",\n");

    impl += gate_placement.str();
  }


  // place sub-modules
  for(Module::module_collection::const_iterator iter = mod->modules_begin();
      iter != mod->modules_end(); ++iter) {

    Module_shptr sub = *iter;
    std::list<std::string> ports;

    // iterate over its module ports
    for(Module::port_collection::const_iterator p_iter = sub->ports_begin();
	p_iter != sub->ports_end(); ++p_iter) {

      std::string const& submod_port_name= p_iter->first;
      const GatePort_shptr gport = p_iter->second;

      if(gport->is_connected()) {
	const Net_shptr net = gport->get_net();
      
	boost::format f("    .%1% (%2%)");
	f % submod_port_name
	  % nets[net->get_object_id()];
	ports.push_back(f.str());
      }
    }

    
    boost::format gate_placement("  %1% %2% (\n"
				 "%3% );\n\n");
    gate_placement 
      % generate_identifier(sub->get_entity_name() != "" ? sub->get_entity_name() : sub->get_name() , "dg_")
      % generate_identifier(sub->get_name())
      % boost::algorithm::join(ports, ",\n");

    impl += gate_placement.str();
    
  }


  // place sub-modules
  return 
    (wire_definitions != "" ? "  // net definitions\n" : "") +
    wire_definitions + "\n" + 
    "  // sub-modules\n\n" +
    impl;
}
