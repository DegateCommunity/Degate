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
#include "BoundingBox.h"
#include <algorithm>
#include <boost/format.hpp>

using namespace degate;

BoundingBox::BoundingBox() {
  max_y = min_y = max_x = min_x = 0;
}

BoundingBox::BoundingBox(int min_x, int max_x, int min_y, int max_y) {
  this->min_x = std::min(min_x, max_x);
  this->max_x = std::max(min_x, max_x);
  this->min_y = std::min(min_y, max_y);
  this->max_y = std::max(min_y, max_y);
}

BoundingBox::BoundingBox(int width, int height) {
  this->min_x = 0;
  this->max_x = width;
  this->min_y = 0;
  this->max_y = height;
}

BoundingBox::BoundingBox(const BoundingBox& o) {
  this->min_x = o.min_x;
  this->max_x = o.max_x;
  this->min_y = o.min_y;
  this->max_y = o.max_y;
}

BoundingBox::~BoundingBox() {
}

BoundingBox const& BoundingBox::get_bounding_box() const {
  return *this;
}

bool BoundingBox::in_shape(int x, int y) const {
  return (x >= min_x && x <= max_x && y >= min_y && y <= max_y) ? true : false;
}

bool BoundingBox::operator==(const BoundingBox& other) const {
    return (min_x == other.min_x &&
            max_x == other.max_x &&
            min_y == other.min_y &&
            max_y == other.max_y);
}

bool BoundingBox::operator!=(const BoundingBox& other) const {
    return !(*this == other);
}


bool BoundingBox::in_bounding_box(BoundingBox const& bbox) const {
  return (min_x >= bbox.min_x &&
	  max_x <= bbox.max_x &&
	  min_y >= bbox.min_y &&
	  max_y <= bbox.max_y);
}

bool BoundingBox::intersects(BoundingBox const & rect) const {

  return !( rect.min_x > max_x ||
	    rect.max_x < min_x ||
	    rect.min_y > max_y ||
	    rect.max_y < min_y);
}


/**
 * Check, if rectangle rect is complete within rectangle represented by this.
 */

bool BoundingBox::complete_within(BoundingBox const & rect) const {

  return (min_x <= rect.min_x &&
	  max_x >= rect.max_x &&
	  min_y <= rect.min_y &&
	  max_y >= rect.max_y);
}

unsigned int BoundingBox::get_width() const {
  return max_x - min_x;
}

unsigned int BoundingBox::get_height() const {
  return max_y - min_y;
}

int BoundingBox::get_min_x() const {
  return min_x;
}

int BoundingBox::get_max_x() const {
  return max_x;
}

int BoundingBox::get_min_y() const {
  return min_y;
}

int BoundingBox::get_max_y() const {
  return max_y;
}

void BoundingBox::set_min_x(int min_x) {
  this->min_x = std::min(min_x, max_x);
  this->max_x = std::max(min_x, max_x);
}

void BoundingBox::set_min_y(int min_y) {
  this->min_y = std::min(min_y, max_y);
  this->max_y = std::max(min_y, max_y);
}

void BoundingBox::set_max_x(int max_x) {
  this->min_x = std::min(min_x, max_x);
  this->max_x = std::max(min_x, max_x);
}

void BoundingBox::set_max_y(int max_y) {
  this->min_y = std::min(min_y, max_y);
  this->max_y = std::max(min_y, max_y);
}

void BoundingBox::shift_y(int delta_y) {
  min_y += delta_y;
  max_y += delta_y;
}

void BoundingBox::shift_x(int delta_x) {
  min_x += delta_x;
  max_x += delta_x;
}

void BoundingBox::shift(int delta_x, int delta_y) {
  shift_x(delta_x);
  shift_y(delta_y);
}

int BoundingBox::get_center_x() const {
  return min_x + get_width() / 2;
}

int BoundingBox::get_center_y() const {
  return min_y + get_height() / 2;
}

void BoundingBox::set(int min_x, int max_x, int min_y, int max_y) {
  this->min_x = std::min(min_x, max_x);
  this->max_x = std::max(min_x, max_x);
  this->min_y = std::min(min_y, max_y);
  this->max_y = std::max(min_y, max_y);
}

void BoundingBox::print(std::ostream & os, int n_tabs) const {
  os << gen_tabs(n_tabs) << to_string();
}

std::string BoundingBox::to_string() const {
  boost::format f("x = %1% .. %2% / y = %3% ... %4%");
  f % min_x % max_x % min_y % max_y;
  return f.str();
}
