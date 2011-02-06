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

#ifndef __RCVCONTAINER_H__
#define __RCVCONTAINER_H__

#include <boost/foreach.hpp>
#include <tr1/memory>
#include <list>
#include <RCBase.h>

namespace degate {

  // forward declaration
  class RCViolation;
  typedef std::tr1::shared_ptr<RCViolation> RCViolation_shptr;

  /**
   * Representation for a container type, which holds a list
   * of Rule Check Violations.
   */
  class RCVContainer {
  public:
    typedef std::list<RCViolation_shptr> container_type;
    typedef container_type::iterator iterator;
    typedef container_type::const_iterator const_iterator;

  private:
    container_type violations;

  public:
    /**
     * The ctor.
     */
    RCVContainer();

    /**
     * The dtor.
     */
    ~RCVContainer();

    /**
     * Add a RC violation to the container.
     */
    void push_back(RCViolation_shptr rcv);
    
    /**
     * Get an iterator to the start of the list.
     */
    iterator begin();

    /**
     * Get an iterator to the end of the list.
     */
    iterator end();

    /**
     * Get a const iterator for the start of the list.
     */
    const_iterator begin() const;

    /**
     * Get a const iterator for the end of the list.
     */
    const_iterator end() const;
    
    /**
     * Clear entire list of RC violations.
     */
    void clear();

    /**
     * Find a RC violation in the container.
     */
    iterator find(RCViolation_shptr rcv);

    /**
     * Find a RC violation and return a const iterator.
     */
    const_iterator find(RCViolation_shptr rcv) const;

    /**
     * Get the number of entries.
     */
    size_t size() const;

    /**
     * Check if the container has already stored a specific RC violation.
     * @param rcv The RC violation for which the presence should be checked.
     * @return Returns true, if the RC violation is present in the container.
     */
    bool contains(RCViolation_shptr rcv) const;

    /**
     * Erase a specific RC violation.
     * @param rcv The RC violation, which should be removed.
     * @return Returns true, if the RC violation was found and removed.
     *   Else false is returned.
     */
    bool erase(RCViolation_shptr rcv);

  };

}

#endif
