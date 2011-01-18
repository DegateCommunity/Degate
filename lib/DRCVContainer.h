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

#ifndef __DRCVCONTAINER_H__
#define __DRCVCONTAINER_H__

#include <boost/foreach.hpp>
#include <tr1/memory>
#include <list>
#include <DRCBase.h>

namespace degate {

  // forward declaration
  class DRCViolation;
  typedef std::tr1::shared_ptr<DRCViolation> DRCViolation_shptr;

  /**
   * Representation for a container type, which holds a list
   * of Design Rule Check Violations.
   */
  class DRCVContainer {
  public:
    typedef std::list<DRCViolation_shptr> container_type;
    typedef container_type::iterator iterator;
    typedef container_type::const_iterator const_iterator;

  private:
    container_type violations;

  public:
    /**
     * The ctor.
     */
    DRCVContainer();

    /**
     * The dtor.
     */
    ~DRCVContainer();

    /**
     * Add a DRC violation to the container.
     */
    void push_back(DRCViolation_shptr drcv);
    
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
     * Clear entire list of DRC violations.
     */
    void clear();

    /**
     * Find a DRC violation in the container.
     */
    iterator find(DRCViolation_shptr drcv);

    /**
     * Find a DRC violation and return a const iterator.
     */
    const_iterator find(DRCViolation_shptr drcv) const;

    /**
     * Get the number of entries.
     */
    size_t size() const;

    /**
     * Check if the container has already stored a specific DRC violation.
     * @param drcv The DRC violation for which the presence should be checked.
     * @return Returns true, if the DRC violation is present in the container.
     */
    bool contains(DRCViolation_shptr drcv) const;

    /**
     * Erase a specific DRC violation.
     * @param drcv The DRC violation, which should be removed.
     * @return Returns true, if the DRC violation was found and removed.
     *   Else false is returned.
     */
    bool erase(DRCViolation_shptr drcv);

  };

}

#endif
