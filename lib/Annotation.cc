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

#include "globals.h"
#include "Annotation.h"
#include <boost/format.hpp>

using namespace degate;

Annotation::Annotation(int _min_x, int _max_x, int _min_y, int _max_y, 
		       class_id_t _class_id) : 
  Rectangle(_min_x, _max_x, _min_y, _max_y), class_id(_class_id) { 

}

Annotation::Annotation(BoundingBox const& bbox, class_id_t _class_id) :
  Rectangle(bbox.get_min_x(), bbox.get_max_x(),
	    bbox.get_min_y(), bbox.get_max_y()), 
  class_id(_class_id) {
}

Annotation::~Annotation() {
}

Annotation::class_id_t Annotation::get_class_id() const { 
  return class_id; 
}

void Annotation::set_class_id(Annotation::class_id_t _class_id) { 
  class_id = _class_id; 
}

const std::string Annotation::get_descriptive_identifier() const {
  boost::format fmter("%1% (id=%2%,class=%3%)");
  fmter % get_object_id() % get_name() % get_class_id();
  return fmter.str();
}

const std::string Annotation::get_object_type_name() const {
  return std::string("Annotation");
}

void Annotation::print(std::ostream & os, int n_tabs) const {

  os
    << gen_tabs(n_tabs) << "Annotation name  : " << get_name() << std::endl
    << gen_tabs(n_tabs) << "Description      : " << get_description() << std::endl
    << gen_tabs(n_tabs) << "Object ID        : " << get_object_id() << std::endl
    << gen_tabs(n_tabs) << "Bounding box     : " << Rectangle::get_bounding_box().to_string() << std::endl
    << gen_tabs(n_tabs) << "Annotation class : " << get_class_id() << std::endl
    << std::endl;

  os << std::endl;

}

Annotation::parameter_set_type::const_iterator Annotation::parameters_begin() const {
  return parameters.begin();
}

Annotation::parameter_set_type::const_iterator Annotation::parameters_end() const {
  return parameters.end();
}
