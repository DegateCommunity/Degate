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

#ifndef __NET_H__
#define __NET_H__

#include <set>
#include <tr1/memory>


#include "globals.h"


namespace degate {

  
  /**
   * The net class represents an electrical potential that is shared
   * between electrically adjacent objects.
   *
   * Why do methods in class Net work with object ID instead of
   * shared pointers? There is an automatism. A ConnectedLogicModelObject
   * adds itself to a net, if you set the net for the ConnectedLogicModelObject.
   * And it removes itself from a Net object, if it's destructor is called.
   * The problem is, that the ConnectedLogicModelObject itself only
   * has a \p this pointer. An object can't have a shared pointer to itself.
   * One could work with normal pointers, but this would somehow circumvent
   * shared pointer approach in libdegate. So we use loosely coupled
   * object IDs.
   *
   * @see ConnectedLogicModelObject::set_net()
   * @see ConnectedLogicModelObject::remove_net()
   */
  class Net : public LogicModelObjectBase {
    
    friend class ConnectedLogicModelObject;

  private:
    
    std::set<object_id_t> connections;
    
  protected:

    /**
     * Add an object of type ConnectedLogicModelObject to the net.
     * It is silently ignored, if the object is already referenced
     * from the net.
     * @exception InvalidObjectIDException This exception is thrown
     *  if the object has an invalid object ID.
     */

    virtual void add_object(ConnectedLogicModelObject_shptr o) 
      throw(InvalidObjectIDException);


    /**
     * Add an object to the net.
     * @see add_object()
     */
    virtual void add_object(object_id_t oid) throw(InvalidObjectIDException);
  

    /**
     * Remove an object from a net.
     * @exception CollectionLookupException Indicates that the object
     *  is not referenced from the net.
     * @exception InvalidObjectIDException As in add_object().
     * @see add_object()
     */

    virtual void remove_object(ConnectedLogicModelObject_shptr o) 
      throw(CollectionLookupException, InvalidObjectIDException);

    /**
     * Remove object.
     * @see remove_object()
     */

    virtual void remove_object(object_id_t o) 
      throw(CollectionLookupException, InvalidObjectIDException);

    
  public:
    
    typedef std::set<object_id_t>::iterator connection_iterator;
    typedef std::set<object_id_t>::iterator iterator;
    typedef std::set<object_id_t>::const_iterator const_iterator;
    
    /**
     * Construct a new net.
     */
    Net();
    
    /**
     * Destroy a net.
     * @see LogicModel::remove_net()
     */
    virtual ~Net();
    
    /**
     * Get an iterator to iterate over all objects that are electrically connected with this net.
     * Be careful with iterator invalidation!
     */
    virtual connection_iterator begin();
    
    /**
     * Get an end marker.
     */
    virtual connection_iterator end();
    
	
    /**
     * Get the number of objects that are connected with this net.
     */

    virtual unsigned int size() const;
  };

}

#endif
