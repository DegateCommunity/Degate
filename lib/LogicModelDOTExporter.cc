/*

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

#include <degate.h>
#include "LogicModelDOTExporter.h"
#include "DOTAttributes.h"
#include "FileSystem.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

#include <string>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <list>
#include <tr1/memory>

using namespace std;
using namespace degate;

#define MAX_NODES 7

void LogicModelDOTExporter::export_data(std::string const& filename, LogicModel_shptr lmodel)
  throw( InvalidPathException, InvalidPointerException, std::runtime_error ) {

  if(lmodel == NULL) throw InvalidPointerException("Logic model pointer is NULL.");

  /* Calculate a scaling, so that we can use pixel coordinates from the logic model
     as dot coordinates.

  scaling = (
	     21.0 // cm  -- the width of an A4 paper
	      / 2.54 // cm/inch
	     * (double)get_dots_per_inch() // dots/inch
	     ) / (double)(lmodel->get_width());

  */

  debug(TM, "scaling: %f", scaling);
  string basename(get_basename(filename));
  std::ostringstream stm;
  stm << "time neato -v -Tsvg"
    //<< " -Gdpi=" << get_dots_per_inch()
      << " -o " << basename
      << ".svg " << basename << ".dot";


  add_header_line("");
  add_header_line("This is a logic model export.");
  add_header_line("");
  add_header_line("You can generate an image of this graph with:");
  add_header_line(stm.str());
  add_header_line("");

  add_graph_setting("");

  try {

    // iterate over nets
    if(properties[ENABLE_EDGES]) {
      for(LogicModel::net_collection::iterator net_iter = lmodel->nets_begin();
	  net_iter != lmodel->nets_end(); ++net_iter) {
	Net_shptr net = (*net_iter).second;
	if(net->size() < MAX_NODES)
	  add_net(net);
      }
    }

    // iterate over logic model objects
    for(LogicModel::object_collection::iterator iter = lmodel->objects_begin();
	iter != lmodel->objects_end(); ++iter) {

      PlacedLogicModelObject_shptr o = (*iter).second;

      if(Gate_shptr gate = std::tr1::dynamic_pointer_cast<Gate>(o)) {
	// check if the gate should be rendered
	//if(accept_gate_for_output)
	add_gate(gate);
      }

      else if(properties[ENABLE_VIAS]) {
	if(Via_shptr via = std::tr1::dynamic_pointer_cast<Via>(o))
	  add_via(via);
      }

      /*
	else if(Wire_shptr wire = std::tr1::dynamic_pointer_cast<Wire>(o))
	  add_wire(wires_elem, wire, layer_pos);

      */
    }

    dump_to_file(filename);

  }
  catch(const std::exception& ex) {
    std::cout << "Exception caught: " << ex.what() << std::endl;
    throw;
  }

}

std::string LogicModelDOTExporter::oid_to_str(std::string const& prefix, object_id_t oid) {
  std::ostringstream stm;
  stm << prefix << oid_rewriter->get_new_object_id(oid);
  return stm.str();
}

void LogicModelDOTExporter::add_net(Net_shptr net)
  throw(std::runtime_error) {

  string net_name(oid_to_str("N", net->get_object_id()));

  DOTAttributes attrs;
  //attrs.add("label", net_name);
  attrs.add("shape", "point");

  add_node(net_name, attrs.get_string());

}

std::string LogicModelDOTExporter::add_implicit_net(Net_shptr net) {

  object_id_t new_oid = net->get_object_id();
  string net_name(oid_to_str("N", new_oid));

  if(implicit_net_counter.find(new_oid) == implicit_net_counter.end())
    implicit_net_counter[new_oid] = 1;
  else
    implicit_net_counter[new_oid]++;


  DOTAttributes attrs;
  attrs.add("label", net_name);
  attrs.add("shape", "box");
  attrs.add("color", "red");

  std::ostringstream stm;
  stm << net_name << "_" << implicit_net_counter[new_oid];


  add_node(stm.str(), attrs.get_string());
  return stm.str();

}


void LogicModelDOTExporter::add_gate(Gate_shptr gate)
  throw(std::runtime_error) {

  string node_name(oid_to_str("G", gate->get_object_id()));

  std::ostringstream stm;
  stm << (gate->has_name() ? gate->get_name() : node_name);

  if(get_property(ENABLE_TEMPLATE_NAMES) && gate->has_template()) {
    const GateTemplate_shptr tmpl = gate->get_gate_template();
    stm << "\\n" << tmpl->get_name();
  }

  string gate_name(stm.str());

  DOTAttributes attrs;
  attrs.add("label", gate_name);
  attrs.add("shape", "component");

  if(get_fontsize() > 0)
    attrs.add_number<unsigned int>("fontsize", get_fontsize());

  if(get_penwidth() > 0)
    attrs.add_number<unsigned int>("penwidth", get_penwidth());

  if(properties[PRESERVE_GATE_POSITIONS]) {
    attrs.add_number<unsigned int>("height", lround(scaling*gate->get_height()));
    attrs.add_number<unsigned int>("width", lround(scaling*gate->get_width()));
  }

  /* Use the placement on the chip as hint for graphviz. Graphviz
     might benefit from this.
  */
  attrs.add_position(lround(scaling*gate->get_center_x()),
		     lround(scaling*gate->get_center_y()),
		     properties[PRESERVE_GATE_POSITIONS]);


  if(properties[ENABLE_COLORS] &&
     (MASK_R(gate->get_fill_color()) != 0 ||
      MASK_G(gate->get_fill_color()) != 0 ||
      MASK_B(gate->get_fill_color()) != 0 ))
    attrs.add("color", to_color_string(gate->get_fill_color()));

  if(properties[PRESERVE_GATE_POSITIONS])
    attrs.add("fixedsize", "true");

  add_node(node_name, attrs.get_string());



  if(properties[ENABLE_EDGES]) {
    for(Gate::port_iterator piter = gate->ports_begin();
	piter != gate->ports_end(); ++piter) {
      GatePort_shptr gate_port = *piter;

      add_connection(gate_port->get_net(), node_name,
		     gate_port->get_template_port()->get_name());


    }
  }


}


void LogicModelDOTExporter::add_connection(Net_shptr net,
					   std::string const& src_name,
					   std::string const& edge_name) {

  string net_name(oid_to_str("N", net->get_object_id()));

  DOTAttributes edge_attrs;
  edge_attrs.add("taillabel", edge_name);

  if(net->size() < MAX_NODES)
    add_edge(src_name, net_name,  edge_attrs.get_string());
  else {
    string implicit_net_name = add_implicit_net(net);
    edge_attrs.add("color", "red");
    add_edge(src_name, implicit_net_name, edge_attrs.get_string());
  }

}


void LogicModelDOTExporter::add_via(Via_shptr via) {

  string via_name(oid_to_str("V", via->get_object_id()));

  DOTAttributes attrs;
  attrs.add("label", via_name);
  attrs.add("shape", "box");

  if(get_fontsize() > 0)
    attrs.add_number<unsigned int>("fontsize", get_fontsize());
  if(get_penwidth() > 0)
    attrs.add_number<unsigned int>("penwidth", get_penwidth());

  add_node(via_name, attrs.get_string());

  if(properties[ENABLE_EDGES])
    add_connection(via->get_net(), via_name, "");
}
