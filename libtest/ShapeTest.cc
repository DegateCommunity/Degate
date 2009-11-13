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


#include "Shape.h"
#include "ShapeTest.h"

#include "globals.h"
#include <stdlib.h>

#include "Line.h"
#include "Circle.h"
#include "Point.h"
#include "Rectangle.h"

CPPUNIT_TEST_SUITE_REGISTRATION (ShapeTest);

using namespace degate;

void ShapeTest::setUp(void) { 
}

void ShapeTest::tearDown(void) {
}

void ShapeTest::test_line_empty_bbox(void) {
	Line l;
	CPPUNIT_ASSERT(l.get_bounding_box() == empty_bbox);
}

void ShapeTest::test_circle_empty_bbox(void) {
	Circle c;
	CPPUNIT_ASSERT(c.get_bounding_box() == empty_bbox);
}

void ShapeTest::test_point_empty_bbox(void) {
	Point p;
	CPPUNIT_ASSERT(p.get_bounding_box() == empty_bbox);
}

void ShapeTest::test_rectangle_empty_bbox(void) {
	Rectangle r;
	CPPUNIT_ASSERT(r.get_bounding_box() == empty_bbox);
}


void ShapeTest::test_in_shape_rectangle() {
	
	BoundingBox b(10, 90, 10, 90);
	Rectangle r(10, 90, 10, 90);
	r.get_bounding_box().print();
	b.print();
	CPPUNIT_ASSERT(r.get_bounding_box() == b);
	
	CPPUNIT_ASSERT(r.in_shape(50, 50) == true);
	CPPUNIT_ASSERT(r.in_shape(10, 10) == true);
	CPPUNIT_ASSERT(r.in_shape(90, 90) == true);
	
	CPPUNIT_ASSERT(r.in_shape(50, 9) == false);
	
	r.shift(10, 10);
	CPPUNIT_ASSERT(r.in_shape(10, 10) == false);
	CPPUNIT_ASSERT(r.in_shape(100, 100) == true );
}

void ShapeTest::test_in_shape_point() {
	Point p(10, 10);
	
	CPPUNIT_ASSERT(p.in_shape(0, 0) == false);
	CPPUNIT_ASSERT(p.in_shape(10, 10) == true);
	
	p.shift(10, 10);
	CPPUNIT_ASSERT(p.in_shape(10, 10) == false);
	CPPUNIT_ASSERT(p.in_shape(20, 20) == true);
}

void ShapeTest::test_in_shape_circle() {
	Circle c(10, 10, 5);
	
	CPPUNIT_ASSERT(c.in_shape(8, 8) == true);
	CPPUNIT_ASSERT(c.in_shape(100, 100) == false);
	
	c.shift(10, 10);
	CPPUNIT_ASSERT(c.in_shape(18, 18) == true);
	
}

void ShapeTest::test_in_shape_line() {
	const unsigned int diameter = 2;
	
	Line l(10, 10, 90, 90, diameter);
	CPPUNIT_ASSERT(l.is_vertical() == false);
	CPPUNIT_ASSERT(l.is_horizontal() == false);
	
	CPPUNIT_ASSERT(l.in_shape(10, 10) == true);
	CPPUNIT_ASSERT(l.in_shape(9, 9) == false);
	CPPUNIT_ASSERT(l.in_shape(91, 91) == false);
	
	l.shift(10, 10);
	CPPUNIT_ASSERT(l.in_shape(0, 0) == false);
	CPPUNIT_ASSERT(l.in_shape(10, 10) == false);
	CPPUNIT_ASSERT(l.in_shape(100, 100) == true);
	
	Line h(10, 10, 100, 10, diameter);
	CPPUNIT_ASSERT(h.is_horizontal());

	BoundingBox b = h.get_bounding_box();
	CPPUNIT_ASSERT(b.get_height() == diameter);
	CPPUNIT_ASSERT(b.get_width() == 90);
	
	CPPUNIT_ASSERT(h.in_shape(50, 10) == true);
	CPPUNIT_ASSERT(h.in_shape(50, 11) == true);
	CPPUNIT_ASSERT(h.in_shape(50, 8) == false);
	CPPUNIT_ASSERT(h.in_shape(5, 10) == false);
	
	Line v(10, 10, 10, 100, diameter);
	b = v.get_bounding_box();
	CPPUNIT_ASSERT(b.get_height() == 90);
	CPPUNIT_ASSERT(b.get_width() == diameter);

	CPPUNIT_ASSERT(v.is_vertical());
	CPPUNIT_ASSERT(v.in_shape(10, 10) == true);
	CPPUNIT_ASSERT(v.in_shape(10, 50) == true);
	CPPUNIT_ASSERT(v.in_shape(10, 100) == true);

	CPPUNIT_ASSERT(v.in_shape(10, 5) == false);
	CPPUNIT_ASSERT(v.in_shape(10, 110) == false);

	CPPUNIT_ASSERT(v.in_shape(50, 50) == false);

	
}

