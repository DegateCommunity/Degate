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

#ifndef __GATELIBRARY_H__
#define __GATELIBRARY_H__

#include <degate.h>
#include <set>
#include <map>
#include <sstream>

namespace degate {

  /**
   * This class represents a library of template cells.
   *
   * @todo There is no way to deal with gate libraries, which
   *   have different image sizes for the gate templates then the
   *   current project. There should be a way to scale the template
   *   images, but it is unknown how to derive the scaling factor without pain.
   */
  class GateLibrary {

  public:
    
    typedef std::map<object_id_t, GateTemplate_shptr> gate_lib_collection_t;
    typedef gate_lib_collection_t::iterator template_iterator;
    
  private:
    
    gate_lib_collection_t templates;
    
  public:
    
    /** 
     * Constructor for the gate library.
     */

    GateLibrary();
    
    /**
     * The dtor.
     */

    virtual ~GateLibrary();
    
    /**
     * Remove a template from the gate library.
     */

    void remove_template(GateTemplate_shptr gate_template);
    
    /**
     * Add a template to the library.
     * @exception InvalidObjectIDException This exception is thrown if the
     *   template has no object ID.
     * @exception InvalidPointerException 
     */

    void add_template(GateTemplate_shptr gate_template) 
      throw(InvalidObjectIDException, InvalidPointerException);

    /**
     * Get a gate template from the library
     * @exception CollectionLookupException This exception is thrown if
     *  there is no gate template that has ID \p id.
     * @exception InvalidObjectIDException This exception is thrown if the
     *   object ID is invalid.
     * @return Returns a shared pointer to the template. The
     *   pointer value is NULL, if a template with the \p id
     *   was not found.
     */

    GateTemplate_shptr get_template(object_id_t id) 
      throw(InvalidObjectIDException, CollectionLookupException);


    /**
     * Lookup a template port in the gate library.
     * Check all templates in the library for a template port
     */

    GateTemplatePort_shptr get_template_port(object_id_t port_id) 
      throw(CollectionLookupException);

    
    /** 
     * Get an iterator in order to iterate over gate templates.
     */

    template_iterator begin();
    
    /**
     * Get the end marker for the iteration.
     */

    template_iterator end();

    /**
     * print the gate library.
     */

    void print(std::ostream & os);

  };

  

}

#endif

