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

#include "Rectangle.h"
#include "globals.h"
#include <iostream>
#include <algorithm>

using namespace degate;

Rectangle::Rectangle() :
  min_x(0),
  max_x(0),
  min_y(0),
  max_y(0) {
  calculate_bounding_box();
}

Rectangle::Rectangle(int min_x, int max_x, int min_y, int max_y) {

  this->min_x = std::min(min_x, max_x);
  this->max_x = std::max(min_x, max_x);
  this->min_y = std::min(min_y, max_y);
  this->max_y = std::max(min_y, max_y);
  calculate_bounding_box();
}

Rectangle::Rectangle(const Rectangle& o) {
  this->min_x = o.min_x;
  this->max_x = o.max_x;
  this->min_y = o.min_y;
  this->max_y = o.max_y;
  calculate_bounding_box();
}

Rectangle::~Rectangle() {
}

bool Rectangle::in_shape(int x, int y) const {
  return (x >= min_x && x <= max_x && y >= min_y && y <= max_y) ? true : false;
}

BoundingBox const& Rectangle::get_bounding_box() const {
  return bounding_box;
}


bool Rectangle::operator==(const Rectangle& other) const {
  return (min_x == other.min_x &&
	  max_x == other.max_x &&
	  min_y == other.min_y &&
	  max_y == other.max_y);
}

bool Rectangle::operator!=(const Rectangle& other) const {
  return !(*this == other);
}

/**
 * Check, if this rectangle is in the bounding box.
 */

bool Rectangle::in_bounding_box(BoundingBox const& bbox) const {
  
  return ( bbox.get_min_x() <= min_x ||
	   bbox.get_max_x() >= max_x ||
	   bbox.get_min_y() <= min_y ||
	   bbox.get_max_y() >= max_y);
}

bool Rectangle::intersects(Rectangle const & rect) const {

  return !( rect.min_x > max_x ||
	    rect.max_x < min_x ||
	    rect.min_y > max_y ||
	    rect.max_y < min_y);
}


/**
 * Check, if rectangle rect is complete within rectangle represented by this.
 */

bool Rectangle::complete_within(Rectangle const & rect) const {

  return (min_x <= rect.min_x &&
	  max_x >= rect.max_x &&
	  min_y <= rect.min_y &&
	  max_y >= rect.max_y);
}

unsigned int Rectangle::get_width() const {
  return max_x - min_x;
}

unsigned int Rectangle::get_height() const {
  return max_y - min_y;
}

int Rectangle::get_min_x() const {
  return min_x;
}

int Rectangle::get_max_x() const {
  return max_x;
}

int Rectangle::get_min_y() const {
  return min_y;
}

int Rectangle::get_max_y() const {
  return max_y;
}

void Rectangle::set_min_x(int min_x) {
  this->min_x = std::min(min_x, max_x);
  this->max_x = std::max(min_x, max_x);
  calculate_bounding_box();
}

void Rectangle::set_min_y(int min_y) {
  this->min_y = std::min(min_y, max_y);
  this->max_y = std::max(min_y, max_y);
  calculate_bounding_box();
}

void Rectangle::set_max_x(int max_x) {
  this->min_x = std::min(min_x, max_x);
  this->max_x = std::max(min_x, max_x);
  calculate_bounding_box();
}

void Rectangle::set_max_y(int max_y) {
  this->min_y = std::min(min_y, max_y);
  this->max_y = std::max(min_y, max_y);
  calculate_bounding_box();
}

void Rectangle::shift_y(int delta_y) {
  min_y += delta_y;
  max_y += delta_y;
  calculate_bounding_box();
}

void Rectangle::shift_x(int delta_x) {
  min_x += delta_x;
  max_x += delta_x;
  calculate_bounding_box();
}


int Rectangle::get_center_x() const {
  return min_x + get_width() / 2;
}

int Rectangle::get_center_y() const {
  return min_y + get_height() / 2;
}

void Rectangle::set_position(int min_x, int max_x, int min_y, int max_y) {
  this->min_x = std::min(min_x, max_x);
  this->max_x = std::max(min_x, max_x);
  this->min_y = std::min(min_y, max_y);
  this->max_y = std::max(min_y, max_y);
  calculate_bounding_box();
}

void Rectangle::calculate_bounding_box() {
  bounding_box = BoundingBox(min_x, max_x, min_y, max_y);	     
}
