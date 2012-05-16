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

#ifndef __LOGICMODELOBJECTBASE_H__
#define __LOGICMODELOBJECTBASE_H__

#include "globals.h"
#include <boost/format.hpp>

namespace degate {

  /**
   * The class LogicModelObjectBase is the base class for basically every class
   * that represents a logic model object.
   */
  class LogicModelObjectBase {

  private:

    object_id_t object_id;

    std::string name;
    std::string description;

  public:

    /**
     * Create a new object.
     */

    LogicModelObjectBase(object_id_t oid = 0);

    /**
     * The constructor.
     */

    LogicModelObjectBase(std::string const& object_name,
			 std::string const& object_description);

    /**
     * The constructor.
     */

    LogicModelObjectBase(object_id_t oid,
			 std::string const& object_name,
			 std::string const& object_description);

    /**
     * The dtor.
     */

    virtual ~LogicModelObjectBase();

    /**
     * Set the name for a logic model object. It is up to the user
     * how the object is named. But it should be identifying.
     */

    virtual void set_name(std::string const& name);

    /**
     * Set the description for a logic model object. It is up to the user
     * how the object is described.
     */

    virtual void set_description(std::string const & description);

    /**
     * Get the name for a logic model object.
     */

    virtual std::string const & get_name() const;

    /**
     * Get the description for a logic model object.
     */

    virtual std::string const & get_description() const;

    /**
     * Check if a logic model object has a name.
     */

    virtual bool has_name() const;

    /**
     * Check if a logic model object has a description.
     */

    virtual bool has_description() const;

    /**
     * Set the object ID for a logic model object.
     */

    virtual void set_object_id(object_id_t oid);

    /**
     * Get the object ID for a logic model object.
     */

    virtual object_id_t get_object_id() const;

    /**
     * Check if the logic model object has a valid object ID.
     */

    virtual bool has_valid_object_id() const;


    /**
     * Get a human readable string that describes the whole
     * logic model object. The string should be unique in order
     * to let the user identify the concrete object. But that
     * is not a must.
     */

    virtual const std::string get_descriptive_identifier() const;

    /**
     * Get a human readable string that names the object type,
     * e.g. "Wire" or "Gate port".
     */

    virtual const std::string get_object_type_name() const;

  };

  typedef std::tr1::shared_ptr<LogicModelObjectBase> LogicModelObjectBase_shptr;
  
  class LMOCompare {
  public:
    bool operator() (const LogicModelObjectBase &a, const LogicModelObjectBase &b) const;
    bool operator() (const LogicModelObjectBase_shptr &a, const LogicModelObjectBase_shptr &b) const;
  };
}

#endif
