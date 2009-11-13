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
#include "Net.h"
#include "Layer.h"
#include "LogicModel.h"
#include "PlacedLogicModelObject.h"

using namespace degate;

PlacedLogicModelObject::PlacedLogicModelObject() : select_state(false) {
}

PlacedLogicModelObject::~PlacedLogicModelObject() {
}

bool PlacedLogicModelObject::is_selected() const { 
  return select_state; 
}

void PlacedLogicModelObject::set_selected(bool select_state) {
  this->select_state = select_state;
}


void PlacedLogicModelObject::set_layer(std::tr1::shared_ptr<Layer> layer) {
  this->layer = layer;
}

std::tr1::shared_ptr<Layer> PlacedLogicModelObject::get_layer() {
  return layer;
}

void PlacedLogicModelObject::notify_shape_change() {

  if(layer != NULL && has_valid_object_id()) {
    layer->notify_shape_change(get_object_id());
  }
}
