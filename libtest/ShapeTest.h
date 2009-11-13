/* -*-c++-*-
 
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

#ifndef __SHAPETEST_H__
#define __SHAPETEST_H__

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>
#include "BoundingBox.h"

class ShapeTest : public CPPUNIT_NS :: TestFixture {

  CPPUNIT_TEST_SUITE(ShapeTest);

  CPPUNIT_TEST (test_line_empty_bbox);
  CPPUNIT_TEST (test_point_empty_bbox);
  CPPUNIT_TEST (test_circle_empty_bbox);
  CPPUNIT_TEST (test_rectangle_empty_bbox);


  CPPUNIT_TEST (test_in_shape_rectangle);
  CPPUNIT_TEST (test_in_shape_point);
  CPPUNIT_TEST (test_in_shape_circle);
  CPPUNIT_TEST (test_in_shape_line);


  CPPUNIT_TEST_SUITE_END ();

 public:
  void setUp (void);
  void tearDown (void);

 protected:
  void test_line_empty_bbox(void);
  void test_point_empty_bbox(void);
  void test_circle_empty_bbox(void);
  void test_rectangle_empty_bbox(void);

  void test_in_shape_rectangle();
  void test_in_shape_circle();
  void test_in_shape_point();
  void test_in_shape_line();

 private:
  degate::BoundingBox empty_bbox;
};

#endif
