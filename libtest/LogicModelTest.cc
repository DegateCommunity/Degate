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
#include "LogicModelTest.h"

#include <stdlib.h>
#include "QuadTree.h"
#include "Wire.h"
#include "Via.h"

CPPUNIT_TEST_SUITE_REGISTRATION (LogicModelTest);

using namespace std;
using namespace degate;

void LogicModelTest::setUp(void) {
}

void LogicModelTest::tearDown(void) {
}

void LogicModelTest::test_compile_check(void) {
  //PlacedLogicModelObject placed_object; // compile check: try to instantiate an object
  //Shape shape; // compile check: try to instantiate
}

void LogicModelTest::test_casts(void) {

  Wire * w = new Wire(20, 21, 30, 31, 5);
  CPPUNIT_ASSERT(w != NULL);

  PlacedLogicModelObject * ptr = dynamic_cast<PlacedLogicModelObject *>(w);
  CPPUNIT_ASSERT(ptr != NULL);

  Wire * w2 = dynamic_cast<Wire *>(ptr);
  CPPUNIT_ASSERT(w2 != NULL);

  delete w;

}

void LogicModelTest::test_casts_shared_ptr(void) {

  Wire_shptr w(new Wire(20, 21, 30, 31, 5));
  CPPUNIT_ASSERT(w != NULL);

  PlacedLogicModelObject_shptr ptr = std::dynamic_pointer_cast<PlacedLogicModelObject>(w);
  CPPUNIT_ASSERT(ptr != NULL);

  Wire_shptr w2 = std::dynamic_pointer_cast<Wire>(ptr);
  CPPUNIT_ASSERT(w2 != NULL);
}

void LogicModelTest::test_add_layer(void) {

  LogicModel_shptr lmodel(new LogicModel(100, 100));
  lmodel->add_layer(0);
  lmodel->add_layer(1);
  lmodel->add_layer(2);
  lmodel->add_layer(3);
  lmodel->add_layer(10);
  lmodel->add_layer(100);

  CPPUNIT_ASSERT_THROW(lmodel->add_layer(3), std::logic_error);
  CPPUNIT_ASSERT_THROW(lmodel->add_layer(10), std::logic_error);

  lmodel->remove_layer(3);
}


void LogicModelTest::test_add_and_retrieve_placed_lmo(void) {
  LogicModel_shptr lmodel(new LogicModel(100, 100));
  CPPUNIT_ASSERT(lmodel != NULL);

  PlacedLogicModelObject_shptr plo(new Gate(0, 10, 0, 10));
  CPPUNIT_ASSERT(plo != NULL);

  CPPUNIT_ASSERT(lmodel->objects_begin() == lmodel->objects_end());
  lmodel->add_object(0, plo);
  CPPUNIT_ASSERT(lmodel->objects_begin() != lmodel->objects_end());

  PlacedLogicModelObject_shptr retrieved = lmodel->get_object(plo->get_object_id());
  CPPUNIT_ASSERT(retrieved != NULL);
}

void LogicModelTest::test_add_and_retrieve_wire(void) {
  LogicModel_shptr lmodel(new LogicModel(100, 100));

  Wire_shptr w(new Wire(20, 21, 30, 31, 5));
  CPPUNIT_ASSERT(w->has_valid_object_id() == false);

  CPPUNIT_ASSERT(lmodel->objects_begin() == lmodel->objects_end());
  lmodel->add_object(0, w);
  CPPUNIT_ASSERT(w->has_valid_object_id() == true);
  CPPUNIT_ASSERT(lmodel->objects_begin() != lmodel->objects_end());
  
  object_id_t oid = w->get_object_id();
  CPPUNIT_ASSERT(oid != 0);


  PlacedLogicModelObject_shptr plo = lmodel->get_object(oid);
  CPPUNIT_ASSERT(plo != NULL);

  Wire_shptr retrieved_wire = std::dynamic_pointer_cast<Wire>(plo);
  CPPUNIT_ASSERT(retrieved_wire != NULL);

  for(int j = 0; j < 100; j++) {
    Wire_shptr w2(new Wire(20, 21, 30, 31, 5));

    lmodel->add_object(0, w2);
  }

  // iteration test
  int i = 0;
  for(LogicModel::object_collection::iterator iter = 
	lmodel->objects_begin(); iter != lmodel->objects_end(); ++iter, i++) {

    PlacedLogicModelObject_shptr o = (*iter).second;
    CPPUNIT_ASSERT(o != NULL);
    CPPUNIT_ASSERT(o->has_valid_object_id() == true);
    CPPUNIT_ASSERT((*iter).first == o->get_object_id());
  }
  CPPUNIT_ASSERT(i > 0);
}


