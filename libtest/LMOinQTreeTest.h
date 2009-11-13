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

#ifndef __LMOINQTREETEST_H__
#define __LMOINQTREETEST_H__

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>
#include "QuadTree.h"

class LMOinQTreeTest : public CPPUNIT_NS::TestFixture {
	
  CPPUNIT_TEST_SUITE(LMOinQTreeTest);

  CPPUNIT_TEST (test_casts);
  CPPUNIT_TEST (test_insert_wire_into_qtree);
  CPPUNIT_TEST (test_insert_via_into_qtree);
  CPPUNIT_TEST (test_overlapping_objects);
  CPPUNIT_TEST_SUITE_END ();
	
public:

  void setUp (void);
  void tearDown (void);


protected:
  void test_casts(void);
  void test_insert_wire_into_qtree(void);
  void test_insert_via_into_qtree(void);
  void test_overlapping_objects(void);

  degate::QuadTree<degate::PlacedLogicModelObject *> * qtree;
	
};

#endif
