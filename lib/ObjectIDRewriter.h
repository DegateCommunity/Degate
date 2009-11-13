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

#ifndef __OBJECTIDREWRITER_H__
#define __OBJECTIDREWRITER_H__

#include "globals.h"

#include <stdexcept>
#include <sstream>
#include <map>

namespace degate {

/**
 * This class is used to defragment object IDs from a logic model.
 *
 * Each logic model object has a uniquie identifier. The logic model keeps track which
 * object IDs are in use and which are free. Internally there are cases where we want to
 * reorganize the object IDs. The ObjectIDRewriter is used therefore. 
 *
 */

class ObjectIDRewriter {
	
private:

  std::map<object_id_t, object_id_t> table;
  object_id_t oid_counter;

  bool enable_id_rewrite;

public:

  /** 
   * Construct a new Rewriter.
   * @param _enable_id_rewrite If this parameter is set to false, then there is no object ID rewriting.
   */
  ObjectIDRewriter(bool _enable_id_rewrite = true) : 
    oid_counter(2342), 
    enable_id_rewrite(_enable_id_rewrite) {};

  /**
   * The destructor.
   */

  virtual ~ObjectIDRewriter() {};

  /**
   * Get an object ID replacement.
   * If you called the ctor with 'false', then you will get the the same object ID back. This is somehow
   * a pass through mode.
   * @param old_id For this object ID yoy will get a defragmented object ID.
   * @return Returns another object ID.
   */
  object_id_t get_new_object_id(object_id_t old_id) {
    if(!enable_id_rewrite) return old_id;
    else {
      if(table.find(old_id) == table.end()) {
	table[old_id] = oid_counter;
	return oid_counter++;
      }
      else return table[old_id];
    }
  }

};

typedef std::tr1::shared_ptr<ObjectIDRewriter> ObjectIDRewriter_shptr;

}

#endif
