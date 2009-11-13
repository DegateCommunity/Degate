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


#include <degate.h>
#include "QuadTree.h"
#include "Wire.h"
#include "Via.h"
#include "LMOinQTreeTest.h"

#include <stdlib.h>

CPPUNIT_TEST_SUITE_REGISTRATION (LMOinQTreeTest);

using namespace std;
using namespace degate;

void LMOinQTreeTest::setUp(void) {
	BoundingBox bbox(0, 100, 0, 100);
	qtree = new QuadTree<PlacedLogicModelObject *>(bbox);
}

void LMOinQTreeTest::tearDown(void) {
	delete qtree;
}

void LMOinQTreeTest::test_casts(void) {
  Wire * w = new Wire(20, 21, 30, 31, 5);
  CPPUNIT_ASSERT(w != NULL);

  PlacedLogicModelObject * ptr = NULL;

  ptr = dynamic_cast<PlacedLogicModelObject *>(w);

  CPPUNIT_ASSERT(ptr != NULL);

  delete w;

}
void LMOinQTreeTest::test_insert_wire_into_qtree(void) {
  test_casts();
	Wire * w = new Wire(20, 21, 30, 31, 5);
	CPPUNIT_ASSERT(w != NULL);
	CPPUNIT_ASSERT(w->get_from_x() == 20);
	CPPUNIT_ASSERT(w->get_from_y() == 21);
	CPPUNIT_ASSERT(w->get_to_x() == 30);
	CPPUNIT_ASSERT(w->get_to_y() == 31);
	CPPUNIT_ASSERT(w->get_diameter() == 5);
	
	w->set_name("wire one");
	w->set_description("description for wire one");
	
	qtree->insert(dynamic_cast<PlacedLogicModelObject*>(w));

	for(region_iterator<PlacedLogicModelObject *> i = qtree->region_iter_begin(); i != qtree->region_iter_end(); ++i) {
		PlacedLogicModelObject * o = *i;

		CPPUNIT_ASSERT(i == qtree->region_iter_begin());

		CPPUNIT_ASSERT(o != NULL);
		CPPUNIT_ASSERT(o->get_name() == "wire one");
		CPPUNIT_ASSERT(o->get_description() == "description for wire one");

		Wire * wxx = dynamic_cast<Wire *>(o);
		CPPUNIT_ASSERT(wxx != NULL);
		CPPUNIT_ASSERT(wxx->get_net() == NULL);

		CPPUNIT_ASSERT(typeid(o) == typeid(PlacedLogicModelObject *));
		
		Wire * w2 = dynamic_cast<Wire *>(o);
		CPPUNIT_ASSERT(w != NULL);
		CPPUNIT_ASSERT(typeid(w2) == typeid(Wire *));
		CPPUNIT_ASSERT(w2->get_name() == "wire one");
		
		CPPUNIT_ASSERT(w2->get_from_x() == 20);
		CPPUNIT_ASSERT(w2->get_from_y() == 21);
		
	}
	
	delete w;
}

void LMOinQTreeTest::test_insert_via_into_qtree(void) {
  test_casts();
  Via * v = new Via(10, 11, 3, Via::DIRECTION_UP);
  CPPUNIT_ASSERT(v != NULL);
  CPPUNIT_ASSERT(v->get_x() == 10);
  CPPUNIT_ASSERT(v->get_y() == 11);
  CPPUNIT_ASSERT(v->get_diameter() == 3);
  CPPUNIT_ASSERT(v->get_direction() == Via::DIRECTION_UP);
  delete v;
}




void LMOinQTreeTest::test_overlapping_objects(void) {

  Gate * g = new Gate(0, 30, 0, 30);
  Via * v = new Via(10, 10, 5, Via::DIRECTION_UP);

  qtree->insert(dynamic_cast<PlacedLogicModelObject*>(g));
  qtree->insert(dynamic_cast<PlacedLogicModelObject*>(v));

  int counter = 0;
  for(region_iterator<PlacedLogicModelObject *> i = qtree->region_iter_begin(10,10,10,10); 
      i != qtree->region_iter_end(); ++i, counter++) {
    PlacedLogicModelObject * o = *i;
    CPPUNIT_ASSERT(dynamic_cast<Gate *>(o) != NULL ||
		   dynamic_cast<Via *>(o) != NULL);
		   

  }
  CPPUNIT_ASSERT(counter == 2);
  delete g;
  delete v;
}
