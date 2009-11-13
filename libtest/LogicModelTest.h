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

#ifndef __LOGICMODELTEST_H__
#define __LOGICMODELTEST_H__

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>
#include "QuadTree.h"

class LogicModelTest : public CPPUNIT_NS::TestFixture {
	
  CPPUNIT_TEST_SUITE(LogicModelTest);

  CPPUNIT_TEST (test_compile_check);
  CPPUNIT_TEST (test_casts);
  CPPUNIT_TEST (test_casts_shared_ptr);

  CPPUNIT_TEST (test_add_layer);
  CPPUNIT_TEST (test_add_and_retrieve_placed_lmo);
  CPPUNIT_TEST (test_add_and_retrieve_wire);

  CPPUNIT_TEST_SUITE_END ();
	
public:

	void setUp (void);
	void tearDown (void);


protected:
  void test_compile_check(void);
  void test_casts(void);
  void test_casts_shared_ptr(void);
  void test_add_layer(void);
  void test_add_and_retrieve_placed_lmo(void);
  void test_add_and_retrieve_wire(void);

};

#endif
