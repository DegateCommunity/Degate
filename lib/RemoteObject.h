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

#ifndef __REMOTEOBJECT_H__
#define __REMOTEOBJECT_H__


#include <cstdlib>
#include <string>
#include <iostream>
#include <assert.h>

namespace degate {

  class RemoteObject {

  private:

    object_id_t remote_oid;
    
  protected:

    virtual void push_object_to_server(std::string const& server_url) = 0;

  public:

    RemoteObject() : remote_oid(0) {
    }

    virtual ~RemoteObject() {
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


    virtual bool push(std::string const& server_url) {
      if(remote_oid == 0) {
	push_object_to_server(server_url);
	return true;
      }
      return false;
    }

  };


}

#endif
