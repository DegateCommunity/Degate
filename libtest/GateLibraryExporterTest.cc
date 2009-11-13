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
#include "GateLibraryExporterTest.h"
#include "GateLibraryExporter.h"
#include "GateLibraryImporter.h"
#include "GateLibrary.h"
#include "FileSystem.h"

#include <unistd.h>
#include <sys/param.h>
#include <stdlib.h>
#include <stdexcept>


CPPUNIT_TEST_SUITE_REGISTRATION (GateLibraryExporterTest);

using namespace std;
using namespace degate;

void GateLibraryExporterTest::setUp(void) {
}

void GateLibraryExporterTest::tearDown(void) {
}


void GateLibraryExporterTest::test_export(void) {

  /*
   * parse a library
   */
  GateLibraryImporter importer;		

  string filename("libtest/testfiles/testproject/gate_library.xml");
  GateLibrary_shptr glib(importer.import(filename));

  /*
   * export library
   */
  GateLibraryExporter exporter(std::tr1::shared_ptr<ObjectIDRewriter>(new ObjectIDRewriter()));
  string out_filename("/tmp/gate_library.xml");
  exporter.export_data(out_filename, glib);

  CPPUNIT_ASSERT(file_exists(out_filename) == true);

  /*
   * re-parse library
   */
  GateLibraryImporter reimporter;
  GateLibrary_shptr glib2(reimporter.import(out_filename));
  
}

