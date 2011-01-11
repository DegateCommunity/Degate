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

#include <DRCNet.h>

using namespace degate;

DRCNet::DRCNet() :
  DRCBase("net", "Check for unusual net configs.", DRC_ERROR) {
}

void DRCNet::run(LogicModel_shptr lmodel) {
  clear_drc_violations();

  if(lmodel == NULL) return;

  // iterate over nets
  for(LogicModel::net_collection::iterator net_iter = lmodel->nets_begin();
      net_iter != lmodel->nets_end(); ++net_iter) {

    check_net(lmodel, (*net_iter).second);
  }

}

void DRCNet::check_net(LogicModel_shptr lmodel, Net_shptr net) {

  unsigned int
    in_ports = 0,
    out_ports = 0;

  // iterate over all objects from a net
  for(Net::connection_iterator c_iter = net->begin();
      c_iter != net->end(); ++c_iter) {

    object_id_t oid = *c_iter;

    PlacedLogicModelObject_shptr plo = lmodel->get_object(oid);

    if(GatePort_shptr gate_port = std::tr1::dynamic_pointer_cast<GatePort>(plo)) {

      assert(gate_port->has_template_port() == true); // can't happen

      if(gate_port->has_template_port()) {

	GateTemplatePort_shptr tmpl_port = gate_port->get_template_port();
	if(tmpl_port->is_inport()) in_ports++;
	else if(tmpl_port->is_outport()) out_ports++;
	else {
	  boost::format f("For the corresponding gate template port of %1% the port "
			  "direction is undefined.");
	  f % gate_port->get_descriptive_identifier();
	  add_drc_violation(DRCViolation_shptr(new DRCViolation(gate_port, f.str(),
								get_drc_class_name())));

	}

      }

    }
  }

  if((in_ports > 0 && out_ports == 0) ||
     (in_ports == 0 && out_ports > 0)) {

    for(Net::connection_iterator c_iter = net->begin();
	c_iter != net->end(); ++c_iter) {

      object_id_t oid = *c_iter;
      PlacedLogicModelObject_shptr plo = lmodel->get_object(oid);
      if(GatePort_shptr gate_port = std::tr1::dynamic_pointer_cast<GatePort>(plo)) {

	GateTemplatePort_shptr tmpl_port = gate_port->get_template_port();
	std::string error_msg;

	if(in_ports > 0) {
	  boost::format f("In-Port %1% is not feeded. It is only connected "
			  "with %2% other in-ports.");
	  f % gate_port->get_descriptive_identifier() % (in_ports - 1);
	  error_msg = f.str();
	}
	else {
	  boost::format f("Out-Port %1% is connected with other out-ports.");
	  f % gate_port->get_descriptive_identifier();
	  error_msg = f.str();
	}

	add_drc_violation(DRCViolation_shptr(new DRCViolation(gate_port, error_msg,
							      get_drc_class_name())));
      }
    }
  }
}
