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

  class HlObjectSet : public ObjectSet {

  public: 
    void clear();
    void highlight(bool state = true);
    void add(degate::PlacedLogicModelObject_shptr object);
    void remove(degate::PlacedLogicModelObject_shptr object);
  };
}

#endif
