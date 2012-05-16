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


#include "globals.h"
#include "LogicModelObjectBase.h"
#include "Layer.h"
#include "PlacedLogicModelObject.h"

using namespace degate;

LogicModelObjectBase::LogicModelObjectBase(object_id_t oid) :
  object_id(oid) {
}

LogicModelObjectBase::LogicModelObjectBase(std::string const& object_name,
					   std::string const& object_description) :
  object_id(0),
  name(object_name),
  description(object_description) {
}

LogicModelObjectBase::LogicModelObjectBase(object_id_t oid,
					   std::string const& object_name,
					   std::string const& object_description) :
  object_id(oid),
  name(object_name),
  description(object_description) {
}

LogicModelObjectBase::~LogicModelObjectBase() {
}

void LogicModelObjectBase::set_name(std::string const& name) {
  this->name = name;
}

void LogicModelObjectBase::set_description(std::string const & description) {
  this->description = description;
}

std::string const & LogicModelObjectBase::get_name() const {
  return name;
}

std::string const & LogicModelObjectBase::get_description() const {
  return description;
}

bool LogicModelObjectBase::has_name() const {
  return !name.empty();
}

bool LogicModelObjectBase::has_description() const {
  return !description.empty();
}


void LogicModelObjectBase::set_object_id(object_id_t oid) {
  object_id = oid;
}

object_id_t LogicModelObjectBase::get_object_id() const {
  return object_id;
}

bool LogicModelObjectBase::has_valid_object_id() const {
  return object_id != 0;
}


const std::string LogicModelObjectBase::get_descriptive_identifier() const {
  // should be dispatched. In debug mode we break here.
  assert(1 == 0);

  boost::format fmter("Generic object %1%");
  fmter % get_object_id();;
  return fmter.str();
}

const std::string LogicModelObjectBase::get_object_type_name() const {
  return std::string("Generic object");
}



bool LMOCompare::operator() (const LogicModelObjectBase &a, const LogicModelObjectBase &b) const {
  return a.get_object_id() < b.get_object_id();
}

bool LMOCompare::operator() (const LogicModelObjectBase_shptr &a, const LogicModelObjectBase_shptr &b) const {
  return this->operator()(*a, *b);
}