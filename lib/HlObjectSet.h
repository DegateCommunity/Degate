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

#ifndef __HLOBJECTSET_H__
#define __HLOBJECTSET_H__

#include <degate.h>
#include <set>
#include <list>
#include <tr1/memory>
#include <ObjectSet.h>

namespace degate {

  /**
   * This class represents a collection of highlighted objects.
   */
  class HlObjectSet : public ObjectSet {

  private:
    void highlight_adjacent_objects(ConnectedLogicModelObject_shptr o,
				    LogicModel_shptr lmodel);
    void unhighlight_adjacent_objects();

    void highlight(PlacedLogicModelObject::HIGHLIGHTING_STATE state);

  private:
    std::list<ConnectedLogicModelObject_shptr> adjacent_objects;

  public:
    void clear();
    void add(degate::PlacedLogicModelObject_shptr object);
    void add(degate::PlacedLogicModelObject_shptr object,
	     LogicModel_shptr lmodel);
    void remove(degate::PlacedLogicModelObject_shptr object);
  };
}

#endif
