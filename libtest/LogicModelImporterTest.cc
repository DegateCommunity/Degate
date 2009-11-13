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
#include "LogicModelImporterTest.h"
#include "LogicModel.h"
#include "GateLibraryImporter.h"

#include "globals.h"

#include <unistd.h>
#include <sys/param.h>
#include <stdlib.h>
#include <stdexcept>


CPPUNIT_TEST_SUITE_REGISTRATION (LogicModelImporterTest);

using namespace std;
using namespace degate;

void LogicModelImporterTest::setUp(void) {
}

void LogicModelImporterTest::tearDown(void) {
}

void LogicModelImporterTest::test_import(void) {


  GateLibraryImporter gate_library_importer;
  string gl_filename("libtest/testfiles/testproject/gate_library.xml");
  GateLibrary_shptr glib(gate_library_importer.import(gl_filename));
  CPPUNIT_ASSERT(glib != NULL);


  LogicModelImporter lm_importer(10000, 10000, glib);
  string filename("libtest/testfiles/testproject/lmodel.xml");
  LogicModel_shptr lmodel(lm_importer.import(filename));
  CPPUNIT_ASSERT(lmodel != NULL);

  for(LogicModel::object_collection::iterator iter = 
	lmodel->objects_begin(); iter != lmodel->objects_end(); ++iter) {
    PlacedLogicModelObject_shptr o = (*iter).second;

    CPPUNIT_ASSERT(o != NULL);
    CPPUNIT_ASSERT(o->has_valid_object_id());
    CPPUNIT_ASSERT((*iter).first == o->get_object_id());
    CPPUNIT_ASSERT(o->get_layer() != NULL);
  }

  // try to reuse the importer
  LogicModel_shptr lmodel2(lm_importer.import(filename));
  CPPUNIT_ASSERT(lmodel2 != NULL);
}

