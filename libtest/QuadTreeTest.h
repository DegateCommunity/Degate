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

#ifndef __QUADTREETEST_H__
#define __QUADTREETEST_H__

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>
#include "QuadTree.h"

class QuadTreeTest : public CPPUNIT_NS::TestFixture {

  CPPUNIT_TEST_SUITE(QuadTreeTest);

  CPPUNIT_TEST (test_insert);
  CPPUNIT_TEST (test_iterator);

  /*  CPPUNIT_TEST (test_iterator_compare);
      CPPUNIT_TEST (test_iterator_pointer);
  CPPUNIT_TEST (test_split);
  CPPUNIT_TEST (test_remove);
  CPPUNIT_TEST (test_region_iterator_simple);
  CPPUNIT_TEST (test_region_iterator_complex);
  */
  CPPUNIT_TEST_SUITE_END ();

 public:
  void setUp (void);
  void tearDown (void);

 protected:

  void test_insert(void);
  void test_iterator(void);

  /*
  void test_iterator_compare(void);

  void test_split(void);
  void test_remove(void);


  void test_region_iterator_simple(void);
  void test_region_iterator_complex(void);
  void test_region_iterator_complex_helper(unsigned int max_elements_in_tree, 
					   unsigned int max_elemtents_per_node);

  */
 private:
  degate::QuadTree<degate::PlacedLogicModelObject_shptr> * qt;
};

#endif
