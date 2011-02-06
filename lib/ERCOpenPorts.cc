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

#include <RCBase.h>
#include <ERCOpenPorts.h>

using namespace degate;

ERCOpenPorts::ERCOpenPorts() :
  RCBase("open_port", "Check for unconnected ports.", RC_ERROR) {
}

void ERCOpenPorts::run(LogicModel_shptr lmodel) {
  clear_rc_violations();

  if(lmodel == NULL) return;

  // iterate over Gates
  debug(TM, "\tRC: iterate over gates.");

  for(LogicModel::gate_collection::iterator g_iter = lmodel->gates_begin();
      g_iter != lmodel->gates_end(); ++g_iter) {

    Gate_shptr gate = g_iter->second;
    
    for(Gate::port_const_iterator p_iter = gate->ports_begin();
	p_iter != gate->ports_end(); ++p_iter) {
      
      GatePort_shptr port = *p_iter;
      assert(port != NULL);
      
      Net_shptr net = port->get_net();
      if(net == NULL || net->size() <= 1) {
	
	boost::format f("Port %1% is unconnected.");
	f % port->get_descriptive_identifier();

	debug(TM, "\tRC: found a vioation.");
	add_rc_violation(RCViolation_shptr(new RCViolation(port, f.str(),
							   get_rc_class_name())));
      }
    }
  }
}

