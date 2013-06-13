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


#include <QuadTree.h>
#include <QuadTreeDownIterator.h>
#include <QuadTreeRegionIterator.h>
#include <degate.h>

#include <list>

#include "QuadTreeTest.h"
#include "globals.h"
#include <stdlib.h>

CPPUNIT_TEST_SUITE_REGISTRATION (QuadTreeTest);

using namespace degate;

void QuadTreeTest::setUp(void) {

  const BoundingBox bbox(0, 1000, 0, 1000);
  qt = new QuadTree<PlacedLogicModelObject_shptr>(bbox, 4);
  CPPUNIT_ASSERT(qt != NULL);
  CPPUNIT_ASSERT(qt->is_leave() == true);

}

void QuadTreeTest::tearDown(void) {
  delete qt;
  qt = NULL;
}

void QuadTreeTest::test_insert(void) {
  
  CPPUNIT_ASSERT(qt != NULL);

  Gate_shptr g1(new Gate(10, 20, 10, 20));
  CPPUNIT_ASSERT(RET_IS_OK(qt->insert(g1)));
  CPPUNIT_ASSERT(qt->total_size() == 1);

  Gate_shptr g2(new Gate(90, 100, 90, 100));
  CPPUNIT_ASSERT(RET_IS_OK(qt->insert(g2)));
  CPPUNIT_ASSERT(qt->total_size() == 2);

}


void QuadTreeTest::test_iterator(void) {
  // qt is empty
  CPPUNIT_ASSERT(qt != NULL);

  CPPUNIT_ASSERT(qt->region_iter_begin(0,0,0,0) == qt->region_iter_begin(0,0,0,0));
  CPPUNIT_ASSERT(qt->region_iter_begin(0,0,0,0) == qt->region_iter_begin(0,0,0,1));
  CPPUNIT_ASSERT(qt->region_iter_end() == qt->region_iter_begin(0,0,0,0));
  CPPUNIT_ASSERT(qt->region_iter_end() == qt->region_iter_end());

  Gate_shptr g1(new Gate(10, 20, 10, 20));
  CPPUNIT_ASSERT(RET_IS_OK(qt->insert(g1)));

  Gate_shptr g2(new Gate(90, 100, 90, 100));
  CPPUNIT_ASSERT(RET_IS_OK(qt->insert(g2)));

  Gate_shptr g3(new Gate(190, 200, 190, 200));
  CPPUNIT_ASSERT(RET_IS_OK(qt->insert(g3)));

  Gate_shptr g4(new Gate(500, 510, 500, 510));
  CPPUNIT_ASSERT(RET_IS_OK(qt->insert(g4)));

  Gate_shptr g5(new Gate(600, 610, 600, 610));
  CPPUNIT_ASSERT(RET_IS_OK(qt->insert(g5)));

  Gate_shptr g6(new Gate(700, 710, 700, 710));
  CPPUNIT_ASSERT(RET_IS_OK(qt->insert(g6)));

  Gate_shptr g7(new Gate(800, 810, 800, 810));
  CPPUNIT_ASSERT(RET_IS_OK(qt->insert(g7)));

  Gate_shptr g8(new Gate(900, 910, 900, 910));
  CPPUNIT_ASSERT(RET_IS_OK(qt->insert(g8)));

  CPPUNIT_ASSERT(qt->total_size() == 8);

  qt->print(std::cout, 0, true);

  unsigned int i = 0;
  for(region_iterator<PlacedLogicModelObject_shptr> it = 
	qt->region_iter_begin(480, 620, 480, 620);
      it != qt->region_iter_end();
      ++it, i++) {
    CPPUNIT_ASSERT (it != qt->region_iter_end());
    CPPUNIT_ASSERT(*it != NULL);

    PlacedLogicModelObject_shptr gate = *it;
//    BoundingBox const& bb = gate->get_bounding_box();
    //bb.print();
  }
  if(i != 2) {
    debug(TM, "i = %d", i);
  }
  CPPUNIT_ASSERT(i == 2);


  i = 0;
  for(region_iterator<PlacedLogicModelObject_shptr> it = 
	qt->region_iter_begin();
      it != qt->region_iter_end();
      ++it, i++) {
    CPPUNIT_ASSERT (it != qt->region_iter_end());
    CPPUNIT_ASSERT(*it != NULL);
  }
  if(i != qt->total_size()) {
    debug(TM, "i = %d", i);
  }
  CPPUNIT_ASSERT(i == qt->total_size());
}


/*
void QuadTreeTest::test_iterator_pointer(void) {

CPPUNIT_ASSERT(qt_p != NULL);
CPPUNIT_ASSERT(qt_p->down_iter_begin() == qt_p->down_iter_end());

  for(down_iterator<Rectangle *> it = qt_p->down_iter_begin(); it != qt_p->down_iter_end(); ++it) {
    CPPUNIT_ASSERT(*it != NULL);
  }

  test_insert_pointer();

  unsigned int i = 0;

  //std::cout << std::endl;
  for(down_iterator<Rectangle *> it = qt_p->down_iter_begin();
      it != qt_p->down_iter_end();
      ++it, i++) {
    //std::cout << "node " << i << std::endl;
    CPPUNIT_ASSERT (it != qt_p->down_iter_end());
    CPPUNIT_ASSERT(*it != NULL);
    CPPUNIT_ASSERT ((*it == r1) || (*it == r2));
  }
  //std::cout << std::endl;
  
  CPPUNIT_ASSERT(qt_p->total_size() == i);
  CPPUNIT_ASSERT(i == 2);
}


void QuadTreeTest::test_iterator_compare(void) {
    CPPUNIT_ASSERT(qt_p != NULL);
    test_insert_pointer();
    down_iterator<Rectangle *> it1 = qt_p->down_iter_begin();
    down_iterator<Rectangle *> it2 = qt_p->down_iter_begin();


    CPPUNIT_ASSERT(it1 == it2);
    ++it1;
    CPPUNIT_ASSERT(it1 != it2);
    ++it2;

    CPPUNIT_ASSERT(it1 == it2);

    down_iterator<Rectangle *> it1_end = qt_p->down_iter_end();
    down_iterator<Rectangle *> it2_end = qt_p->down_iter_end();

    CPPUNIT_ASSERT(it1_end == it2_end);

    ++it1;
    CPPUNIT_ASSERT(it1 != it2);
    ++it1;
    CPPUNIT_ASSERT(it1 == it1_end);

    ++it2;
    ++it2;
    CPPUNIT_ASSERT(it2 == it2_end);

    CPPUNIT_ASSERT(qt_p->down_iter_end() == qt_p->down_iter_end());
    CPPUNIT_ASSERT(qt_p->down_iter_begin() == qt_p->down_iter_begin());
    CPPUNIT_ASSERT(qt_p->down_iter_begin() != qt_p->down_iter_end());
}

void QuadTreeTest::test_split(void) {
    CPPUNIT_ASSERT(qt_p != NULL);

    CPPUNIT_ASSERT(qt_p->total_size() == 0);
    test_insert_pointer();

    CPPUNIT_ASSERT(RET_IS_OK(qt_p->insert(r1)));
    CPPUNIT_ASSERT(RET_IS_OK(qt_p->insert(r1)));
    CPPUNIT_ASSERT(qt_p->total_size() == 4);
    CPPUNIT_ASSERT(qt_p->depth() == 1);

    CPPUNIT_ASSERT(RET_IS_OK(qt_p->insert(r1)));
    CPPUNIT_ASSERT(qt_p->total_size() == 5);
    CPPUNIT_ASSERT(qt_p->depth() == 2);

}

void QuadTreeTest::test_remove(void) {
    const unsigned int max_elements = 1000;

    std::list<Rectangle *> r_list;
    for(unsigned int i = 0; i < max_elements; i++) {
        unsigned int min_x = 1+(int) (90.0*rand()/(RAND_MAX+1.0));
        unsigned int min_y = 1+(int) (90.0*rand()/(RAND_MAX+1.0));
        unsigned int w = 1+(int) (10.0*rand()/(RAND_MAX+1.0));
        unsigned int h = 1+(int) (10.0*rand()/(RAND_MAX+1.0));
        Rectangle * r = new Rectangle(min_x, min_x + w, min_y, min_y + h);
        CPPUNIT_ASSERT(r != NULL);
        r_list.push_back(r);

        CPPUNIT_ASSERT(RET_IS_OK(qt_p->insert(r)));
    }

    CPPUNIT_ASSERT(qt_p->total_size() == max_elements);
    CPPUNIT_ASSERT(qt_p->depth() >= 2);

    for(std::list<Rectangle *>::iterator it = r_list.begin(); it != r_list.end(); ++it) {
        CPPUNIT_ASSERT(RET_IS_OK(qt_p->remove(*it)));
        delete *it;
    }

    CPPUNIT_ASSERT(qt_p->total_size() == 0);
    CPPUNIT_ASSERT(qt_p->depth() == 1);
}


void QuadTreeTest::test_region_iterator_simple(void) {

  CPPUNIT_ASSERT(qt_p != NULL);
 
  CPPUNIT_ASSERT(qt_p->region_iter_begin(0,0,0,0) == qt_p->region_iter_begin(0,0,0,0));
  CPPUNIT_ASSERT(qt_p->region_iter_begin(0,0,0,0) == qt_p->region_iter_begin(0,0,0,1));
  CPPUNIT_ASSERT(qt_p->region_iter_end() == qt_p->region_iter_begin(0,0,0,0));
  CPPUNIT_ASSERT(qt_p->region_iter_end() == qt_p->region_iter_end());

  test_insert_pointer();

  region_iterator<Rectangle *> 
    it = qt_p->region_iter_begin(0, qt_p->get_width(), 0, qt_p->get_height()), 
    tmp = qt_p->region_iter_begin(0, qt_p->get_width(), 0, qt_p->get_height()), 
    end = qt_p->region_iter_end();

  CPPUNIT_ASSERT(it == tmp);
  ++it;
  CPPUNIT_ASSERT(it != end);
  ++it;
  CPPUNIT_ASSERT(it == end);
}

void QuadTreeTest::test_region_iterator_complex(void) {

  for( unsigned int per_node = 0; per_node < 5; per_node++) {

    for(unsigned int i = 0; i < 50; i++) {
      std::cout 
	<< "+ testing iterator with " << i
	<< " elements in qtree and \"max\" " << per_node
	<< " elements per node" 
	<< std::endl;
      
      test_region_iterator_complex_helper(i, per_node);
    }
  }
}

void QuadTreeTest::test_region_iterator_complex_helper(unsigned int max_elements_in_tree, 
						       unsigned int max_elemtents_per_node) {

  const BoundingBox bbox(0, 100, 0, 100);
  QuadTree<Rectangle *> * tree = new QuadTree<Rectangle *>(bbox, max_elemtents_per_node);


  std::list<Rectangle *> r_list;
  for(unsigned int i = 0; i < max_elements_in_tree; i++) {
    unsigned int min_x = 1+(int) (90.0*rand()/(RAND_MAX+1.0));
    unsigned int min_y = 1+(int) (90.0*rand()/(RAND_MAX+1.0));
    unsigned int w = 1+(int) (10.0*rand()/(RAND_MAX+1.0));
    unsigned int h = 1+(int) (10.0*rand()/(RAND_MAX+1.0));
    
    Rectangle * r = new Rectangle(min_x, min_x + w, min_y, min_y + h);
    CPPUNIT_ASSERT(r != NULL);
    r_list.push_back(r);
    
    CPPUNIT_ASSERT(RET_IS_OK(tree->insert(r)));
  }


  unsigned int i = 0;

  for(region_iterator<Rectangle *>  it = tree->region_iter_begin(30, 60, 30, 60);
      it != tree->region_iter_end();
      ++it, i++) {
    CPPUNIT_ASSERT (it != tree->region_iter_end());
    CPPUNIT_ASSERT(*it != NULL);
  }
  
  
  for(std::list<Rectangle *>::iterator it = r_list.begin(); it != r_list.end(); ++it) {
    CPPUNIT_ASSERT(RET_IS_OK(tree->remove(*it)));
    delete *it;
  }

  delete tree;
}
*/
