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

#include "Circle.h"
#include "BoundingBox.h"
#include <cmath>

using namespace degate;

Circle::Circle()
{
	x = y = 0.f;
	diameter = 0;
	calculate_bounding_box();
}

Circle::Circle(float x, float y, unsigned int diameter)
{
	this->x = x;
	this->y = y;
	this->diameter = diameter;
	calculate_bounding_box();
}

void Circle::clone_deep_into(DeepCopyable_shptr dest, oldnew_t* oldnew) const
{
	auto clone = std::dynamic_pointer_cast<Circle>(dest);
	clone->x = x;
	clone->y = y;
	clone->diameter = diameter;
	clone->bounding_box = bounding_box;
}

bool Circle::in_shape(float x, float y, float max_distance) const
{
	float delta_x = this->x - x;
	float delta_y = this->y - y;
	return sqrt(delta_x * delta_x + delta_y * delta_y) <= static_cast<float>(diameter) + max_distance;
}

bool Circle::in_bounding_box(BoundingBox const& bbox) const
{
	return bounding_box.in_bounding_box(bbox);
}

BoundingBox const& Circle::get_bounding_box() const
{
	return bounding_box;
}


bool Circle::operator==(const Circle& other) const
{
	return (x == other.x && y == other.y && diameter == other.diameter);
}

bool Circle::operator!=(const Circle& other) const
{
	return !(*this == other);
}

float Circle::get_x() const
{
	return x;
}

float Circle::get_y() const
{
	return y;
}


unsigned int Circle::get_diameter() const
{
	return diameter;
}

void Circle::set_x(float x)
{
	this->x = x;
	calculate_bounding_box();
}

void Circle::set_y(float y)
{
	this->y = y;
	calculate_bounding_box();
}

void Circle::set_diameter(unsigned int diameter)
{
	this->diameter = diameter;
	calculate_bounding_box();
}

void Circle::shift_y(float delta_y)
{
	y += delta_y;
	calculate_bounding_box();
}

void Circle::shift_x(float delta_x)
{
	x += delta_x;
	calculate_bounding_box();
}


void Circle::calculate_bounding_box()
{
	unsigned int radius = diameter / 2;
	bounding_box = BoundingBox(x - radius, x + radius,
	                           y - radius, y + radius);
}
