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

#ifndef __OBJECTSET_H__
#define __OBJECTSET_H__

#include <degate.h>
#include <set>
#include <list>
#include <memory>
#include <boost/foreach.hpp>

namespace degate {

  /**
   * Returns true if the object can be removed from the Logic Model.
   */
  bool is_removable(PlacedLogicModelObject_shptr o);

  /**
   * Check if an object can be electrically interconnected with another object.
   */
  bool is_interconnectable(PlacedLogicModelObject_shptr o);

  template<typename Type>
  bool is_of_object_type(PlacedLogicModelObject_shptr o) {
    return std::dynamic_pointer_cast<Type>(o) != NULL;
  }

  class ObjectSet {

  public:
    typedef std::set<PlacedLogicModelObject_shptr, LMOCompare> object_set_type;
    typedef object_set_type::const_iterator const_iterator;
    typedef object_set_type::iterator iterator;

  private:
    object_set_type objects;

  public:
    virtual ~ObjectSet() {}
    virtual void clear();
    virtual void add(PlacedLogicModelObject_shptr object);
    virtual void remove(PlacedLogicModelObject_shptr object);

    size_t size() const { return objects.size(); }

    const_iterator begin() const { return objects.begin(); }
    const_iterator end() const { return objects.end(); }
    iterator begin() { return objects.begin(); }
    iterator end() { return objects.end(); }

    bool empty() const { return objects.empty(); }

    bool contains(PlacedLogicModelObject_shptr o) const { return objects.find(o) != objects.end(); }
    /**
     * Check if all objects evaluate to true for a check.
     * If there are no objects, false is returned.
     */
    bool check_for_all(bool (*check_function)(PlacedLogicModelObject_shptr)) const {
      if(empty()) return false;

      BOOST_FOREACH(PlacedLogicModelObject_shptr o, objects) {
	if(check_function(o) == false) return false;
      }
      return true;
    }

    template<typename ObjectType>
      std::shared_ptr<ObjectType> get_single_object() const {
      std::shared_ptr<ObjectType> o;

      if(size() == 1) {
	const_iterator it = objects.begin();
	if(o = std::dynamic_pointer_cast<ObjectType>(*it)) return o;
      }
      return o;
    }

  };
}

#endif
