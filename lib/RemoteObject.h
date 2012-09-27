/* -*-c++-*-

 This file is part of the IC reverse engineering tool degate.

 Copyright 2008, 2009, 2010 by Martin Schobert
 Copyright 2012 Robert Nitsch

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

#ifndef __REMOTEOBJECT_H__
#define __REMOTEOBJECT_H__

#include "DeepCopyable.h"

#include <cstdlib>
#include <string>
#include <iostream>
#include <assert.h>

namespace degate {

  class RemoteObject : public DeepCopyableBase {

  private:

    object_id_t remote_oid;

  protected:

    virtual object_id_t push_object_to_server(std::string const& server_url) = 0;

  public:

    RemoteObject() : remote_oid(0) {
    }

    virtual ~RemoteObject() {
    }

    void cloneDeepInto(DeepCopyable_shptr dest, oldnew_t *oldnew) const {
      auto clone = std::dynamic_pointer_cast<RemoteObject>(dest);
      clone->remote_oid = remote_oid;
    }

    virtual bool has_remote_object_id() const {
      return remote_oid != 0;
    }

    virtual object_id_t get_remote_object_id() const {
      return remote_oid;
    }

    virtual void set_remote_object_id(object_id_t oid) {
      remote_oid = oid;
    }


    virtual object_id_t push(std::string const& server_url) {
      if(remote_oid == 0) {
	debug(TM, "RemoteObject::push(): push object to server.");
	return push_object_to_server(server_url);
      }
      else {
	debug(TM, "RemoteObject::push(): object is already pushed to server.");
	return 0;
      }
    }

  };


}

#endif
