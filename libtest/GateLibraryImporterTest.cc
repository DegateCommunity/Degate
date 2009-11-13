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
#include "GateLibraryImporterTest.h"
#include "GateLibrary.h"
#include "globals.h"

#include <unistd.h>
#include <sys/param.h>
#include <stdlib.h>
#include <stdexcept>


CPPUNIT_TEST_SUITE_REGISTRATION (GateLibraryImporterTest);

using namespace std;
using namespace degate;

void GateLibraryImporterTest::setUp(void) {
}

void GateLibraryImporterTest::tearDown(void) {
}


void GateLibraryImporterTest::test_import(void) {

  GateLibraryImporter importer;		

  string filename("libtest/testfiles/testproject/gate_library.xml");
  GateLibrary_shptr glib(importer.import(filename));

  CPPUNIT_ASSERT(glib != NULL);

  // iterator test
  int i = 0;
  for(GateLibrary::template_iterator iter = glib->begin();
      iter != glib->end(); ++iter, i++) {

    GateTemplate_shptr tmpl = (*iter).second;
    CPPUNIT_ASSERT(tmpl != NULL);

    // valid assuptions for the test data
    CPPUNIT_ASSERT(tmpl->get_reference_counter() == 0);
    CPPUNIT_ASSERT(tmpl->get_height() > 0);
    CPPUNIT_ASSERT(tmpl->get_width() > 0);
    CPPUNIT_ASSERT(tmpl->has_valid_object_id() == true);
    
    int j = 0;
    for(GateTemplate::port_iterator piter = tmpl->ports_begin();
	piter != tmpl->ports_end(); ++piter, j++) {
      GateTemplatePort_shptr port = *piter;
      CPPUNIT_ASSERT(port != NULL);
      CPPUNIT_ASSERT(port->has_valid_object_id() == true);
    }
    CPPUNIT_ASSERT(j > 0);

  }

  CPPUNIT_ASSERT(i > 0);
}

