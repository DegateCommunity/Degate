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

#include "HlObjectSet.h"
#include <algorithm>
#include <tr1/memory>

using namespace std;
using namespace degate;


void HlObjectSet::clear() {
  highlight(PlacedLogicModelObject::HLIGHTSTATE_NOT);
  unhighlight_adjacent_objects();

  ObjectSet::clear();
}

void HlObjectSet::highlight(PlacedLogicModelObject::HIGHLIGHTING_STATE state) {
  for(iterator it = begin(); it != end(); ++it)
    (*it)->set_highlighted(state);
}


void HlObjectSet::add(std::tr1::shared_ptr<PlacedLogicModelObject> object) {
  ObjectSet::add(object);
  object->set_highlighted(PlacedLogicModelObject::HLIGHTSTATE_DIRECT);
}

void HlObjectSet::add(std::tr1::shared_ptr<PlacedLogicModelObject> object,
		      LogicModel_shptr lmodel) {
  add(object);

  if(size() == 1) {
    if(ConnectedLogicModelObject_shptr o =
       std::tr1::dynamic_pointer_cast<ConnectedLogicModelObject>(object) ) {
      // highlight adjacent objects
      highlight_adjacent_objects(o, lmodel);
    }
  }
  else {
    unhighlight_adjacent_objects();
  }
}


void HlObjectSet::highlight_adjacent_objects(ConnectedLogicModelObject_shptr o,
					     LogicModel_shptr lmodel) {
  Net_shptr net = o->get_net();
  if(net == NULL) return;

  // iterate over net
  BOOST_FOREACH(object_id_t oid, *net) {
    PlacedLogicModelObject_shptr plo = lmodel->get_object(oid);
    ConnectedLogicModelObject_shptr clo =
      std::tr1::dynamic_pointer_cast<ConnectedLogicModelObject>(plo);
    assert(clo != NULL);
    // remember connnected objects in list
    if(o != clo) {

      assert(clo->get_highlighted() ==
	     PlacedLogicModelObject::HLIGHTSTATE_NOT);

      clo->set_highlighted(PlacedLogicModelObject::HLIGHTSTATE_ADJACENT);

      adjacent_objects.push_back(clo);
    }
  }
}

void HlObjectSet::unhighlight_adjacent_objects() {
  // iterate over list
  BOOST_FOREACH(ConnectedLogicModelObject_shptr clo, adjacent_objects) {

    clo->set_highlighted(PlacedLogicModelObject::HLIGHTSTATE_NOT);
  }
  adjacent_objects.clear();
}

void HlObjectSet::remove(std::tr1::shared_ptr<PlacedLogicModelObject> object) {
  ObjectSet::remove(object);
  object->set_highlighted(PlacedLogicModelObject::HLIGHTSTATE_NOT);

  /* If there is only a single object highlighted, we must call
     unhighlight. If adjacent objects are not highlighted, calling
     unhighlight will not harm and even might fix invalid situations.
  */
  unhighlight_adjacent_objects();
}
