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

#include "globals.h"

#include "Circle.h"
#include "BoundingBox.h"
#include <math.h>

using namespace degate;

Circle::Circle() {
  x = y = diameter = 0;
  calculate_bounding_box();
}

Circle::Circle(int x, int y, unsigned int diameter) {
  this->x = x;
  this->y = y;
  this->diameter = diameter;
  calculate_bounding_box();
}

void Circle::cloneDeepInto(DeepCopyable_shptr dest, oldnew_t *oldnew) const {
  auto clone = std::dynamic_pointer_cast<Circle>(dest);
  clone->x = x;
  clone->y = y;
  clone->diameter = diameter;
  clone->bounding_box = bounding_box;
}

bool Circle::in_shape(int x, int y, int max_distance) const {
  int delta_x = this->x - x;
  int delta_y = this->y - y;
  return sqrt((double)(delta_x * delta_x + delta_y * delta_y)) <= diameter + max_distance;
}

bool Circle::in_bounding_box(BoundingBox const& bbox) const {
  return bounding_box.in_bounding_box(bbox);
}

BoundingBox const& Circle::get_bounding_box() const {
  return bounding_box;
}


bool Circle::operator==(const Circle& other) const {
    return (x == other.x && y == other.y && diameter == other.diameter);
}

bool Circle::operator!=(const Circle& other) const {
    return !(*this == other);
}

int Circle::get_x() const {
  return x;
}

int Circle::get_y() const {
  return y;
}


unsigned int Circle::get_diameter() const {
  return diameter;
}

void Circle::set_x(int x) {
  this->x = x;
  calculate_bounding_box();
}

void Circle::set_y(int y) {
  this->y = y;
  calculate_bounding_box();
}

void Circle::set_diameter(unsigned int diameter) {
  this->diameter = diameter;
  calculate_bounding_box();
}

void Circle::shift_y(int delta_y) {
  y += delta_y;
  calculate_bounding_box();
}

void Circle::shift_x(int delta_x) {
  x += delta_x;
  calculate_bounding_box();
}


void Circle::calculate_bounding_box() {
  unsigned int radius = diameter / 2;
  bounding_box = BoundingBox(x - radius, x + radius,
			     y - radius, y + radius);

}
