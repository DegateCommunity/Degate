/*
 
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

#include "Point.h"

using namespace degate;

Point::Point() {
  x = y = 0;
}

Point::Point(int x, int y) {
  this->x = x;
  this->y = y;
}

bool Point::operator==(const Point& other) const {
    return (x == other.x && y == other.y);
}

bool Point::operator!=(const Point& other) const {
    return !(*this == other);
}

int Point::get_x() const {
  return x;
}

int Point::get_y() const {
  return y;
}

void Point::set_x(int x) {
  this->x = x;
}

void Point::set_y(int y) {
  this->y = y;
}


void Point::shift_y(int delta_y) {
  y += delta_y;
}

void Point::shift_x(int delta_x) {
  x += delta_x;
}


