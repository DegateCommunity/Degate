/*

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

#include "Line.h"
#include "globals.h"
#include <cmath>
#include <cstdlib>
#include <algorithm>

using namespace degate;

Line::Line() :
	from_x(0),
	from_y(0),
	to_x(0),
	to_y(0),
	diameter(0),
	d_x(0),
	d_y(0)
{
	calculate_bounding_box();
}

Line::Line(float _from_x, float _from_y, float _to_x, float _to_y, unsigned int _diameter) :
	from_x(_from_x),
	from_y(_from_y),
	to_x(_to_x),
	to_y(_to_y),
	diameter(_diameter),
	d_x(_to_x - _from_x),
	d_y(_to_y - _from_y)
{
	calculate_bounding_box();
}

void Line::cloneDeepInto(DeepCopyable_shptr dest, oldnew_t* oldnew) const
{
	auto clone = std::dynamic_pointer_cast<Line>(dest);
	clone->from_x = from_x;
	clone->from_y = from_y;
	clone->to_x = to_x;
	clone->to_y = to_y;
	clone->diameter = diameter;
	clone->d_x = d_x;
	clone->d_y = d_y;
	clone->calculate_bounding_box();
}

float Line::distance_to_line(Point const& p) const
{
    return fabs((to_y - from_y) * p.get_x() - (to_x - from_x) * p.get_y() + to_x * from_y - to_y * from_x) /
           sqrt(pow(to_y - from_y, 2) + pow(to_x - from_x, 2));
}

bool Line::in_shape(float x, float y, float max_distance) const
{

    if (is_vertical() || is_horizontal())
    {
        return bounding_box.in_shape(x, y, max_distance);
    }
    else
    {
        if (distance_to_line(Point(x, y)) <= diameter / 2.0 + max_distance)
            return true;
        else
            return false;
    }
}

bool Line::in_bounding_box(BoundingBox const& bbox) const
{
	return bounding_box.in_bounding_box(bbox);
}


BoundingBox const& Line::get_bounding_box() const
{
	return bounding_box;
}

bool Line::is_vertical() const
{
	return to_x - from_x == 0;
}

bool Line::is_horizontal() const
{
	return to_y - from_y == 0;
}

void Line::set_diameter(unsigned int diameter)
{
	this->diameter = diameter;
	calculate_bounding_box();
}

unsigned int Line::get_diameter() const
{
	return diameter;
}

float Line::get_from_x() const
{
	return from_x;
}

float Line::get_from_y() const
{
	return from_y;
}

float Line::get_to_x() const
{
	return to_x;
}

float Line::get_to_y() const
{
	return to_y;
}

void Line::set_from_x(float from_x)
{
	this->from_x = from_x;
	d_x = to_x - from_x;
	calculate_bounding_box();
}

void Line::set_to_x(float to_x)
{
	this->to_x = to_x;
	d_x = to_x - from_x;
	calculate_bounding_box();
}

void Line::set_from_y(float from_y)
{
	this->from_y = from_y;
	d_y = to_y - from_y;
	calculate_bounding_box();
}

void Line::set_to_y(float to_y)
{
	this->to_y = to_y;
	d_y = to_y - from_y;
	calculate_bounding_box();
}

void Line::shift_y(float delta_y)
{
	from_y += delta_y;
	to_y += delta_y;
	calculate_bounding_box();
}

void Line::shift_x(float delta_x)
{
	from_x += delta_x;
	to_x += delta_x;
	calculate_bounding_box();
}


void Line::calculate_bounding_box()
{
    float radius = diameter / 2.0f;

	if (is_vertical())
		bounding_box = BoundingBox(std::max(from_x - radius, 0.f),
                                   std::max(to_x + radius, 0.f), from_y, to_y);
	else if (is_horizontal())
		bounding_box = BoundingBox(from_x, to_x,
                                   std::max(from_y - radius, 0.f),
                                   std::max(to_y + radius, 0.f));
	else
		bounding_box = BoundingBox(from_x, to_x, from_y, to_y);
}


unsigned int Line::get_length() const
{
	return ((labs(from_x - to_x) << 1) + (labs(from_y - to_y) << 1)) >> 1;
}

Point Line::get_p1() const
{
	return Point(from_x, from_y);
}

Point Line::get_p2() const
{
	return Point(to_x, to_y);
}

void Line::set_p1(Point const& p)
{
	set_from_x(p.get_x());
	set_from_y(p.get_y());
}

void Line::set_p2(Point const& p)
{
	set_to_x(p.get_x());
	set_to_y(p.get_y());
}
