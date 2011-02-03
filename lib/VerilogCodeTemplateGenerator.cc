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


#include <VerilogCodeTemplateGenerator.h>
#include <boost/format.hpp>
#include <boost/foreach.hpp>
#include <boost/algorithm/string/join.hpp>

using namespace boost;
using namespace degate;

VerilogCodeTemplateGenerator::VerilogCodeTemplateGenerator(std::string const& entity_name,
							   std::string const& description,
							   std::string const& logic_class) :
  CodeTemplateGenerator(entity_name, description, logic_class) {
}


VerilogCodeTemplateGenerator::~VerilogCodeTemplateGenerator() {
}

std::string VerilogCodeTemplateGenerator::generate() const {
  return
    generate_header() +
    generate_module(entity_name, generate_port_list()) +
    generate_port_definition() +
    generate_impl(logic_class) + 
    "\n\n"
    "endmodule\n\n";
}

std::string VerilogCodeTemplateGenerator::generate_header() const {
  boost::format f("/** \n"
		  " * This is a Verilog implementation for a gate of type %1%.\n"
		  " */\n\n"
		  "// Please customize this code template according to your needs.\n"
		  "\n\n");
  f % entity_name;
  return f.str();
}

std::string VerilogCodeTemplateGenerator::generate_module(std::string const& entity_name,
							  std::string const& port_description) const {

  boost::format f("module %1% (\n"
		  "%2%"
		  ");\n\n");
  f % generate_identifier(entity_name, "dg_")
    % port_description;
  return f.str();
}

std::string VerilogCodeTemplateGenerator::generate_port_list() const {

  boost::format f("  %1%%2%\n"
		  "  %3%\n");
  f % boost::algorithm::join(generate_identifier<std::vector<std::string> >(get_inports()), ", ")
    % (get_inports().size() > 0 ? ", " : "")
    % boost::algorithm::join(generate_identifier<std::vector<std::string> >(get_outports()), ", ");
  return f.str();
}


std::string VerilogCodeTemplateGenerator::generate_port_definition() const {
  std::string ret;

  ret += "  // input ports\n";
  BOOST_FOREACH(std::string const& port_name, 
		generate_identifier<std::vector<std::string> >(get_inports())) {
    boost::format f("  input %1%;\n");
    f % port_name;    
    ret += f.str();
  }


  ret += "\n  // output ports\n";
  BOOST_FOREACH(std::string const& port_name, 
		generate_identifier<std::vector<std::string> >(get_outports())) {
    boost::format f("  output %1%;\n");
    f % port_name;    
    ret += f.str();
  }

  ret += "\n";

  return ret;
}


std::string VerilogCodeTemplateGenerator::generate_impl(std::string const& logic_class) const {

  std::vector<std::string> in = get_inports();
  std::vector<std::string> out = get_outports();

  if(in.size() == 0 || out.size() == 0) 
    throw DegateRuntimeException("The standard cell has either no input port or no ouput port.");

  std::string clock_name = get_port_name_by_type(CLOCK);
  if(clock_name.empty()) clock_name = "clock";
  std::string reset_name = get_port_name_by_type(RESET);
  if(reset_name.empty()) reset_name = "reset";
  std::string enable_name = get_port_name_by_type(ENABLE);
  if(reset_name.empty()) reset_name = "enable";

  if(logic_class == "inverter" &&
     in.size() == 1 && out.size() == 1) {

    boost::format f("  assign %1% = !%2%;");
    f % generate_identifier(out[0]) % generate_identifier(in[0]);
    return f.str();
  }
  else if(logic_class == "inverter" && 
	  in.size() == 1 && out.size() == 1) {
    boost::format f("  assign %1% = !%2%");
    f % generate_identifier(out[0]) % generate_identifier(in[0]);
    return f.str();
  }
  else if(logic_class == "tristate-inverter" ||
	  logic_class == "tristate-inverter-lo-actiSve" ||
	  logic_class == "tristate-inverter-hi-active") {

    bool low_active = logic_class == "tristate-inverter-lo-active";

    boost::format f("  tri %1%; // ???\n\n"
		    "  assign %2% = %3%%4% ? !%5% : 1'bz;");
    f % generate_identifier(out[0]) 
      % generate_identifier(out[0])
      % (low_active ? "!" : "")
      % generate_identifier(enable_name)
      % generate_identifier(get_first_port_name_not_in(in, enable_name));
    return f.str();
  }
  else if((logic_class == "xor" ||
	   logic_class == "or" ||
	   logic_class == "and" ||
	   logic_class == "nor" ||
	   logic_class == "nand" ||
	   logic_class == "xnor") &&
	  in.size() >= 2 && out.size() == 1) {

    std::string inner_op, outer_op = "!";

    if(logic_class == "nand") inner_op = "&";
    else if(logic_class == "nor") inner_op = "|";
    else if(logic_class == "xnor") inner_op = "^";
    else {
      outer_op = "";
      if(logic_class == "and") inner_op = "&";
      else if(logic_class == "or") inner_op = "|";
    }

    boost::format f("  assign %1% = %2%%3%%4%%5%;"); 
    f % generate_identifier(out[0])
      % outer_op
      % (outer_op.empty() ? "" : "(")
      % boost::algorithm::join(generate_identifier<std::vector<std::string> >(in),
			       std::string(" ") + inner_op + std::string(" "))
      % (outer_op.empty() ? "" : ")");

    return f.str();
  }
  else if(logic_class == "buffer" && in.size() > 0 && out.size() > 0) {
    boost::format f("  assign %1% = %2%; // ???");
    f % generate_identifier(out[0]) % generate_identifier(in[0]);
    return f.str();

  }
  else if(logic_class == "buffer-tristate-hi-active" ||
	  logic_class == "buffer-tristate-lo-active") {
    boost::format f("  tri %1%; // ???\n\n"
		    "  bufif%2%(%3%, %4%, %45);");
    f % generate_identifier(out[0])
      % (logic_class == "buffer-tristate-lo-active" ? "0" : "1")
      % generate_identifier(out[0])
      % generate_identifier(get_first_port_name_not_in(in, enable_name))
      % generate_identifier(enable_name);
    return f.str();
  }
  else if(logic_class == "latch-generic") {
    boost::format f("  reg %1%;\n\n"
		    "  always @(*)\n"
		    "    if (%2%) %3% = %4%;\n");
    f % generate_identifier(out[0])
      % generate_identifier(enable_name)
      % generate_identifier(out[0])
      % generate_identifier(get_first_port_name_not_in(in, enable_name));
    return f.str();

  }
  else if(logic_class == "latch-sync-enable") {
    return "  // stub not implemented, yet";
  }
  else if(logic_class == "latch-async-enable") {
    return "  // stub not implemented, yet";
  }
  else if(logic_class == "flipflop") {
    boost::format f("  reg %1%;\n"
		    "\n"
		    "  always @(posedge %2%)\n"
		    "    %3% <= %4%;\n");
    f % generate_identifier(get_port_name_by_type(Q))
      % generate_identifier(get_port_name_by_type(CLOCK))
      % generate_identifier(get_port_name_by_type(Q))
      % generate_identifier(get_port_name_by_type(D));

    if(get_port_name_by_type(NOT_Q).empty())
      return f.str();
    else {
      boost::format f2("\n"
		       "  reg %1%;\n"
		       "\n"
		       "  always @*\n"
		       "    %2% <= !%3%;\n");
      f2 % generate_identifier(get_port_name_by_type(NOT_Q))
	% generate_identifier(get_port_name_by_type(NOT_Q))
	% generate_identifier(get_port_name_by_type(Q));

	return f.str() + f2.str();
    }
  }
  else if(logic_class == "flipflop-sync-rst") {
    boost::format f("  reg %1%;\n"
		    "\n"
		    "  always @(posedge %2%)\n"
		    "    if(%3%) // synchronous reset\n"
		    "      %4% <= 1'b0;\n"
		    "    else\n"
		    "      %5% <= %6%;\n");
    f % generate_identifier(get_port_name_by_type(Q)) // reg
      % generate_identifier(get_port_name_by_type(CLOCK)) // always

      % generate_identifier(get_port_name_by_type(RESET)) // if

      % generate_identifier(get_port_name_by_type(Q))
      % generate_identifier(get_port_name_by_type(Q))
      % generate_identifier(get_port_name_by_type(D));


    if(get_port_name_by_type(NOT_Q).empty())
      return f.str();
    else {
      boost::format f2("\n"
		       "  reg %1%;\n"
		       "\n"
		       "  always @*\n"
		       "    %2% <= !%3%;\n");
      f2 % generate_identifier(get_port_name_by_type(NOT_Q))
	% generate_identifier(get_port_name_by_type(NOT_Q))
	% generate_identifier(get_port_name_by_type(Q));

	return f.str() + f2.str();
    }
    
  }
  else if(logic_class == "flipflop-async-rst") {
    boost::format f("  reg %1%;\n"
		    "\n"
		    "  always @(posedge %2% or posedge %3%)\n"
		    "    if(%4%) // asynchronous reset\n"
		    "      %5% <= 1'b0;\n"
		    "    else\n"
		    "      %6% <= %7%;\n");
    f % generate_identifier(get_port_name_by_type(Q)) // reg
      % generate_identifier(get_port_name_by_type(CLOCK)) // always
      % generate_identifier(get_port_name_by_type(RESET)) // always

      % generate_identifier(get_port_name_by_type(RESET)) // if

      % generate_identifier(get_port_name_by_type(Q))
      % generate_identifier(get_port_name_by_type(Q))
      % generate_identifier(get_port_name_by_type(D));

    if(get_port_name_by_type(NOT_Q).empty())
      return f.str();
    else {
      boost::format f2("\n"
		       "  reg %1%;\n"
		       "\n"
		       "  always @*\n"
		       "    %2% <= !%3%;\n");
      f2 % generate_identifier(get_port_name_by_type(NOT_Q))
	% generate_identifier(get_port_name_by_type(NOT_Q))
	% generate_identifier(get_port_name_by_type(Q));

	return f.str() + f2.str();
    }

  }
  else if(logic_class == "generic-combinational-logic" ||
	  logic_class == "ao" ||
	  logic_class == "aoi" ||
	  logic_class == "oa" ||
	  logic_class == "oai") {
    
    std::string ret;
    BOOST_FOREACH(std::string const& oport,
		  generate_identifier<std::vector<std::string> >(get_outports())) {
      boost::format f("  assign %1% = ...;\n");
      f % oport;
      ret += f.str();
    }
		  
    return ret;
  }
  else if(logic_class == "half-adder") {
    return "  // assign {cout,sum} = a + b + cin;\n";
  }
  else if(logic_class == "full-adder") {
    return "  // stub not implemented, yet";
  }
  else if(logic_class == "mux") {

    boost::format f("  reg %1%;\n"
		    "\n"
		    "  always @*\n"
		    "    begin\n"
		    "      %2% = 1'b0; // default\n"
		    "      case({sel1, sel0}) // just an example\n"
		    "        2'b00 : %3% = a;\n"
		    "        2'b01 : %4% = b;\n"
		    "        2'b10 : %5% = c;\n"
		    "        2'b11 : %6% = d;\n"
		    "      endcase\n"
		    "    end\n");
    f % generate_identifier(out[0]) 
      % generate_identifier(out[0]) 
      % generate_identifier(out[0]) 
      % generate_identifier(out[0]) 
      % generate_identifier(out[0]) 
      % generate_identifier(out[0]);
    return f.str();
  }
  else if(logic_class == "demux") {
    boost::format f("  reg a, b, c, d;\n"
		    "\n"
		    "  always @*\n"
		    "    begin\n"
		    "      %1% = 1'b0; // default\n"
		    "      case({sel1, sel0}) // just an example\n"
		    "        2'b00 : a = %2%;\n"
		    "        2'b01 : b = %3%;\n"
		    "        2'b10 : c = %4%;\n"
		    "        2'b11 : d = %5%;\n"
		    "      endcase\n"
		    "    end\n");
    f % generate_identifier(in[0]) 
      % generate_identifier(in[0]) 
      % generate_identifier(in[0]) 
      % generate_identifier(in[0]) 
      % generate_identifier(in[0]);
    return f.str();
  }

  return
    "/*\n"
    " * Please implement behaviour.\n"
    " */\n";
}


std::string VerilogCodeTemplateGenerator::generate_identifier(std::string const& name, 
							      std::string const& prefix) const {
  std::string identifier = prefix;

  bool first_char = true;
  BOOST_FOREACH(char c, name) {
    if(c == '/' || c == '!') identifier.append("not");
    else if(first_char && !isalpha(c)) {
      //identifier.append("entity_");
      identifier.push_back(c);
    }
    else if(isalnum(c)) identifier.push_back(c);
    else identifier.push_back('_');

    first_char = false;
  }
  return identifier;
}


