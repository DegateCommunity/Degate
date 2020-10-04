/**
 * This file is part of the IC reverse engineering tool Degate.
 *
 * Copyright 2008, 2009, 2010 by Martin Schobert
 * Copyright 2012 Robert Nitsch
 * Copyright 2019-2020 Dorian Bachelot
 *
 * Degate is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * Degate is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with degate. If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "Globals.h"
#include "Annotation.h"
#include <boost/format.hpp>

using namespace degate;

Annotation::Annotation(float min_x, float max_x, float min_y, float max_y,
                       class_id_t class_id) :
        Rectangle(min_x, max_x, min_y, max_y), class_id(class_id)
{
}

Annotation::Annotation(BoundingBox const& bbox, class_id_t class_id) :
	Rectangle(bbox.get_min_x(), bbox.get_max_x(),
	          bbox.get_min_y(), bbox.get_max_y()),
	class_id(class_id)
{
}

Annotation::~Annotation()
{
}

DeepCopyable_shptr Annotation::clone_shallow() const
{
	auto clone = std::make_shared<Annotation>();
	return clone;
}

void Annotation::clone_deep_into(DeepCopyable_shptr dest, oldnew_t* oldnew) const
{
	auto clone = std::dynamic_pointer_cast<Annotation>(dest);
	clone->class_id = class_id;
	clone->parameters = parameters;
    Rectangle::clone_deep_into(dest, oldnew);
    PlacedLogicModelObject::clone_deep_into(dest, oldnew);
}

Annotation::class_id_t Annotation::get_class_id() const
{
	return class_id;
}

void Annotation::set_class_id(Annotation::class_id_t class_id)
{
	this->class_id = class_id;
}

const std::string Annotation::get_descriptive_identifier() const
{
    return QString("%1 (%2=%3,%4=%5)").arg(QString::fromStdString(get_name()))
                                      .arg(tr("id"))
                                      .arg(get_object_id())
                                      .arg(tr("class"))
                                      .arg(get_class_id())
                                      .toStdString();
}

const std::string Annotation::get_object_type_name() const
{
	return tr("Annotation").toStdString();
}

void Annotation::print(std::ostream& os, int n_tabs) const
{
	os
		<< gen_tabs(n_tabs) << "Annotation name  : " << get_name() << std::endl
		<< gen_tabs(n_tabs) << "Description      : " << get_description() << std::endl
		<< gen_tabs(n_tabs) << "Object ID        : " << get_object_id() << std::endl
		<< gen_tabs(n_tabs) << "Bounding box     : " << Rectangle::get_bounding_box().to_string() << std::endl
		<< gen_tabs(n_tabs) << "Annotation class : " << get_class_id() << std::endl
		<< std::endl;

	os << std::endl;
}

Annotation::parameter_set_type::const_iterator Annotation::parameters_begin() const
{
	return parameters.begin();
}

Annotation::parameter_set_type::const_iterator Annotation::parameters_end() const
{
	return parameters.end();
}
