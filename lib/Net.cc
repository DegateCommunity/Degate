/*

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

#include "globals.h"
#include "LogicModelObjectBase.h"
#include "Net.h"
#include "Layer.h"
#include "LogicModel.h"
#include "ConnectedLogicModelObject.h"
#include "degate_exceptions.h"

using namespace degate;

Net::Net() {
}

Net::~Net() {
}

Net::connection_iterator Net::begin() {
  return connections.begin();
}

Net::connection_iterator Net::end() {
  return connections.end();
}

void Net::remove_object(object_id_t oid) {

  if(oid == 0)
    throw InvalidObjectIDException("The object that has to be "
				   "removed from the net has no object ID.");

  connection_iterator i = connections.find(oid);
  if(i != connections.end()) {
    connections.erase(i);
  }
  else
    throw CollectionLookupException("Can't remove object from the the net, "
				    "because it is not in the net.");
}

void Net::remove_object(ConnectedLogicModelObject_shptr o) {
  remove_object(o->get_object_id());
}


void Net::add_object(object_id_t oid) {
  if(oid == 0)
    throw InvalidObjectIDException("The object that has to be "
				   "added to the net has no object ID.");
  else
    connections.insert(oid);
}

void Net::add_object(ConnectedLogicModelObject_shptr o) {
  add_object(o->get_object_id());
}



unsigned int Net::size() const {
  return connections.size();
}

const std::string Net::get_descriptive_identifier() const {
  boost::format fmter("Net %1%");
  fmter % get_object_id();;
  return fmter.str();
}
