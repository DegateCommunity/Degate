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

#ifndef __CONNECTEDLOGICMODELOBJECT_H__
#define __CONNECTEDLOGICMODELOBJECT_H__

#include "globals.h"
#include "Net.h"
#include "PlacedLogicModelObject.h"

namespace degate {

  /**
   * Represents a logic model object, that can be electrically connected to other
   * logic model objects.
   */

  class ConnectedLogicModelObject : public PlacedLogicModelObject  {

  private:

    Net_shptr net;

  public: 

    /**
     * Construct an object.
     */
    
    ConnectedLogicModelObject();
    

    /**
     * Destroy object. It will deregister this object from the net.
     * @see remove_net()
     */

    virtual ~ConnectedLogicModelObject();

    /**
     * Set the net for this object. This method will add the object to the net.
     */
    virtual void set_net(Net_shptr net);

    /**
     * Remove the net from this object. It will deregister this object
     * from the net's connection list as well.
     */
    virtual void remove_net();


    /**
     * Get a shared pointer to the net.
     */
    virtual Net_shptr get_net();
  };

}

#endif
