/* -*-c++-*-
 
 This file is part of the IC reverse engineering tool degate.
 
 Copyright 2008, 2009 by Martin Schobert
 
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

#ifndef __LOOKUPSUBCIRCUIT_H__
#define __LOOKUPSUBCIRCUIT_H__

#include <set>
#include <tr1/memory>
#include <string>

#include <degate.h>
#include <LogicModelHelper.h>

#include <boost/foreach.hpp>

namespace degate {

  
  class LookupSubcircuit {
    
  private:
    
    LogicModel_shptr lmodel;
    std::list<Gate_shptr> openlist;

  protected:

    virtual void initialize_openlist() {
      // initialize openlist
      for(LogicModel::gate_collection::const_iterator iter = lmodel->gates_begin();
	  iter != lmodel->gates_end(); ++iter) {
	Gate_shptr gate = iter->second;
	assert(gate != NULL);

	if(is_logic_class(gate, "flipflop") ||
	   is_logic_class(gate, "xor") ||
	   is_logic_class(gate, "xnor")) openlist.push_back(gate);
      }
    }

  public:
    
   
    /**
     * Construct a new net.
     */

    LookupSubcircuit(LogicModel_shptr _lmodel) : lmodel(_lmodel) {
    }
    
    /**
     * Destroy.
     */
    virtual ~LookupSubcircuit() {}

    virtual void search() {

      initialize_openlist();


      Gate_shptr g = openlist.front();
      openlist.pop_front();

      trace_recursive(g);

      BOOST_FOREACH(Gate_shptr gate, openlist) {

	   
      }
    }

    trace_recursive(Gate_shptr start_gate, std::set<Gate_shptr> & closed_list) {
      std::cout << "Gate " << start_gate->get_descriptive_identifier() << " connected with: " << std::endl;

      std::set<Gate_shptr> other_gates;

      BOOST_FOREACH(Gate_shptr g, 
		    filter_connected_gates(start_gate, GateTemplatePort::PORT_TYPE_OUT, "Q",
					   "flipflop", GateTemplatePort::PORT_TYPE_IN, "D"))
	if(closed_list.find(g) != closed_list.end()) {
	  other_gates.insert(g);
	  closed_list.insert(g);
	}

      BOOST_FOREACH(Gate_shptr g, 
		    filter_connected_gates(start_gate, GateTemplatePort::PORT_TYPE_IN, "D",
					   "flipflop", GateTemplatePort::PORT_TYPE_OUT, "Q"))
	if(closed_list.find(g) != closed_list.end()) {
	  other_gates.insert(g);
	  closed_list.insert(g);
	}

      BOOST_FOREACH(Gate_shptr g2, other_gates) {
	std::cout << "\tGate " << g2->get_descriptive_identifier() << std::endl;

	trace_recursive(g2, closed_list);
      }
    }

    
    /**
     * Check if there is a port on \p gate of type \p src_port_type
     * that shares a net with another gate of type \p logic_class.
     * 
     */
    std::set<Gate_shptr> filter_connected_gates(Gate_shptr gate, 
						GateTemplatePort::PORT_TYPE src_port_type,
						std::string const& src_port_name,
						std::string const& logic_class, 
						GateTemplatePort::PORT_TYPE dst_port_type,
						std::string const& dst_port_name) const {
      
      std::set<Gate_shptr> connected;

      for(Gate::port_iterator iter = gate->ports_begin();
	  iter != gate->ports_end(); ++iter) {

	GatePort_shptr gport = *iter;
	if(get_port_type(gport) == src_port_type) {
	  Net_shptr net = gport->get_net();
	  if(net != NULL) {
	    BOOST_FOREACH(object_id_t oid, *net) {
	      if(oid != gport->get_object_id()) {
		PlacedLogicModelObject_shptr plmo = lmodel->get_object(oid);
		assert(plmo != NULL);
		if(GatePort_shptr other_port = std::tr1::dynamic_pointer_cast<GatePort>(plmo)) {
		  Gate_shptr other_gate = other_port->get_gate();

		  if(is_logic_class(other_gate, logic_class) &&
		     get_port_type(other_port) == dst_port_type) {



		    if((src_port_name.empty() && dst_port_name.empty()) ||
		       (src_port_name.empty() && dst_port_name == get_template_port_name(other_port)) ||
		       (dst_port_name.empty() && src_port_name == get_template_port_name(gport)) ||
		       (src_port_name == get_template_port_name(gport) && 
			dst_port_name == get_template_port_name(other_port)))
		      connected.insert(other_gate);
		  }
		}
	      }
	      
	    }
	  }
	}
      }
      return connected;
    }

  };

}

#endif
