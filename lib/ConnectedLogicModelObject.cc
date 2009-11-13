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

#include "globals.h"
#include "LogicModelObjectBase.h"
#include "Layer.h"
#include "PlacedLogicModelObject.h"
#include "Net.h"
#include "ConnectedLogicModelObject.h"

using namespace degate;

ConnectedLogicModelObject::ConnectedLogicModelObject() {
}

ConnectedLogicModelObject:: ~ConnectedLogicModelObject() {
  remove_net();
};

void ConnectedLogicModelObject::set_net(Net_shptr net) {
  if(this->net != NULL) {
    this->net->remove_object(get_object_id());
  }
  this->net = net; 
  this->net->add_object(get_object_id());
}

void ConnectedLogicModelObject::remove_net() {
  if(net != NULL) {
    net->remove_object(get_object_id());
    net.reset();
  }
}


Net_shptr ConnectedLogicModelObject::get_net()  { 
  return net; 
}
