/**
 * This file is part of the IC reverse engineering tool Degate.
 *
 * Copyright 2008, 2009, 2010 by Martin Schobert
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
#include "Point.h"

#include <boost/format.hpp>
#include <cmath>

using namespace degate;

Point::Point()
{
    x = y = 0;
}

Point::Point(float x, float y)
{
    this->x = x;
    this->y = y;
}

bool Point::operator==(const Point& other) const
{
    return (x == other.x && y == other.y);
}

bool Point::operator!=(const Point& other) const
{
    return !(*this == other);
}

float Point::get_x() const
{
    return x;
}

float Point::get_y() const
{
    return y;
}

void Point::set_x(float x)
{
    this->x = x;
}

void Point::set_y(float y)
{
    this->y = y;
}


void Point::shift_y(float delta_y)
{
    y += delta_y;
}

void Point::shift_x(float delta_x)
{
    x += delta_x;
}


float Point::get_distance(Point const& p) const
{
    return sqrt((p.x - x) * (p.x - x) + (p.y - y) * (p.y - y));
}

std::string Point::to_string() const
{
    boost::format f("point(%1%, %2%)");
    f % x % y;
    return f.str();
}
