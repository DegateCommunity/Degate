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

#include "Line.h"
#include "globals.h"
#include <math.h>

using namespace degate;

Line::Line() : 
  from_x(0),
  from_y(0),
  to_x(0),
  to_y(0),
  diameter(0),
  d_x(0),
  d_y(0) {
  calculate_bounding_box();
}

Line::Line(int _from_x, int _from_y, int _to_x, int _to_y, unsigned int _diameter) : 
  from_x(_from_x),
  from_y(_from_y),
  to_x(_to_x),
  to_y(_to_y),
  diameter(_diameter),
  d_x(_to_x - _from_x),
  d_y(_to_y - _from_y) {
  calculate_bounding_box();
}

bool Line::in_shape(int x, int y) const {
	
  /*
    How to check if a point is on a line:
    y = m*x + n
    m = dy / dx
    n = y0 - m*x0
    y' = m*x' + n
    
    |y' - y| < epsilon?
  */
  
  /*
    Check if it is a vertical line (dy ~~ 0). If it is true, the bounding box
    describes the line. The same applies to horiontal lines.
  */
  
  if(is_vertical() || is_horizontal()) {
    return 
      from_x < x && x < to_x &&
      from_y < y && y < to_y;
  }
  else {
    
    // check if x is outside the x-range
    if(x < from_x || x > to_x) 
      return false;
    
    double m = d_y / d_x;
    double n = (double)from_y - m * (double)from_x;
    double y_dash = m * (double) x + n;
    
    if(fabs(y_dash - y) <= diameter / 2)
      return true;
    else
      return false;
  }
}

bool Line::in_bounding_box(BoundingBox const& bbox) const {
  return bounding_box.in_bounding_box(bbox);
}


BoundingBox const& Line::get_bounding_box() const {
  return bounding_box;
}

bool Line::is_vertical() const {
  return to_x - from_x == 0;
}

bool Line::is_horizontal() const {
  return to_y - from_y == 0;
}

void Line::set_diameter(unsigned int diameter) {
  this->diameter = diameter;
  calculate_bounding_box();
}

unsigned int Line::get_diameter() const {
  return diameter;
}

int Line::get_from_x() const {
  return from_x;
}

int Line::get_from_y() const {
  return from_y;
}

int Line::get_to_x() const {
  return to_x;
}

int Line::get_to_y() const {
  return to_y;
}

void Line::set_from_x(int from_x) {
  this->from_x = from_x;
  d_x = to_x - from_x;
  calculate_bounding_box();
}

void Line::set_to_x(int to_x) {
  this->to_x = to_x;
  d_x = to_x - from_x;
  calculate_bounding_box();
}

void Line::set_from_y(int from_y) {
  this->from_y = from_y;
  d_y = to_y - from_y;
  calculate_bounding_box();
}

void Line::set_to_y(int to_y) {
  this->to_y = to_y;
  d_y = to_y - from_y;
  calculate_bounding_box();
}

void Line::shift_y(int delta_y) {
  from_y += delta_y;
  to_y += delta_y;
  calculate_bounding_box();
}

void Line::shift_x(int delta_x) {
  from_x += delta_x;
  to_x += delta_x;
  calculate_bounding_box();
}


void Line::calculate_bounding_box() {
  bounding_box = BoundingBox(from_x, to_x, from_y, to_y);	     
}


unsigned int Line::get_length() const {
  return ((labs(from_x - to_x) << 1) + (labs(from_y - to_y) << 1)) >> 1;
}

Point Line::get_p1() const {
  return Point(from_x, from_y);
}

Point Line::get_p2() const {
  return Point(to_x, to_y);
}

void Line::set_p1(Point const& p) {
  set_from_x(p.get_x());
  set_from_y(p.get_y());
}

void Line::set_p2(Point const& p) {
  set_to_x(p.get_x());
  set_to_y(p.get_y());
}
