/* -*-c++-*-

 This file is part of the IC reverse engineering tool degate.

 Copyright 2008, 2009, 2010 by Martin Schobert
 Copyright 2012 Robert Nitsch

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

PlacedLogicModelObject::PlacedLogicModelObject() : highlight_state(HLIGHTSTATE_NOT) {
}

PlacedLogicModelObject::~PlacedLogicModelObject() {
}

void PlacedLogicModelObject::cloneDeepInto(DeepCopyable_shptr destination, oldnew_t *oldnew) const {
  ColoredObject::cloneDeepInto(destination, oldnew);
  LogicModelObjectBase::cloneDeepInto(destination, oldnew);
  
  auto clone = std::dynamic_pointer_cast<PlacedLogicModelObject>(destination);
  assert(clone.get () != 0);
  clone->highlight_state = highlight_state;
  clone->layer = std::dynamic_pointer_cast<Layer>(layer->cloneDeep(oldnew));
}

PlacedLogicModelObject::HIGHLIGHTING_STATE PlacedLogicModelObject::get_highlighted() const {
  return highlight_state;
}

bool PlacedLogicModelObject::is_highlighted() const {
  return highlight_state != PlacedLogicModelObject::HLIGHTSTATE_NOT;
}

void PlacedLogicModelObject::set_highlighted(PlacedLogicModelObject::HIGHLIGHTING_STATE state) {
  highlight_state = state;
}


void PlacedLogicModelObject::set_layer(std::shared_ptr<Layer> layer) {
  this->layer = layer;
}

std::shared_ptr<Layer> PlacedLogicModelObject::get_layer() {
  return layer;
}

void PlacedLogicModelObject::notify_shape_change() {

  if(layer != NULL && has_valid_object_id()) {
    layer->notify_shape_change(get_object_id());
  }
}
