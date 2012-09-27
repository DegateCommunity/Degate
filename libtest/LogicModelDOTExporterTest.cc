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
#include <LogicModelDOTExporterTest.h>
#include <LogicModelDOTExporter.h>
#include <LogicModelImporter.h>
#include <GateLibrary.h>
#include <GateLibraryImporter.h>
#include <FileSystem.h>
#include <globals.h>

#include <unistd.h>
#include <sys/param.h>
#include <stdlib.h>
#include <stdexcept>


CPPUNIT_TEST_SUITE_REGISTRATION (LogicModelDOTExporterTest);

using namespace std;
using namespace degate;

void LogicModelDOTExporterTest::setUp(void) {
}

void LogicModelDOTExporterTest::tearDown(void) {
}


void LogicModelDOTExporterTest::test_export(void) {

  /*
   * parse a library
   */

  GateLibraryImporter gate_library_importer;
  string gl_filename("libtest/testfiles/testproject/gate_library.xml");
  GateLibrary_shptr glib(gate_library_importer.import(gl_filename));
  CPPUNIT_ASSERT(glib != NULL);

  /*
   * parse logic model
   */

  LogicModelImporter lm_importer(10000, 10000, glib);
  string filename("libtest/testfiles/testproject/lmodel.xml");
  LogicModel_shptr lmodel(lm_importer.import(filename));
  CPPUNIT_ASSERT(lmodel != NULL);

  
  /*
   * export logic model as dot files
   */

  LogicModelDOTExporter exporter(std::shared_ptr<ObjectIDRewriter>(new ObjectIDRewriter(true)));

  string out_filename("/tmp/layout_gate_placement.dot");

  exporter.set_property(LogicModelDOTExporter::PRESERVE_GATE_POSITIONS, true);
  exporter.set_property(LogicModelDOTExporter::ENABLE_EDGES, false);
  exporter.set_property(LogicModelDOTExporter::ENABLE_VIAS, false);
  exporter.set_property(LogicModelDOTExporter::ENABLE_WIRES, false);
  exporter.export_data(out_filename, lmodel);

  CPPUNIT_ASSERT(file_exists(out_filename) == true);


  LogicModelDOTExporter exporter2(std::shared_ptr<ObjectIDRewriter>(new ObjectIDRewriter(true)));

  string out_filename2("/tmp/logic_model_graph.dot");

  exporter2.set_property(LogicModelDOTExporter::PRESERVE_GATE_POSITIONS, false);
  exporter2.set_property(LogicModelDOTExporter::ENABLE_EDGES, true);
  exporter2.set_property(LogicModelDOTExporter::ENABLE_VIAS, true);
  exporter2.set_property(LogicModelDOTExporter::ENABLE_WIRES, true);
  exporter2.set_property(LogicModelDOTExporter::ENABLE_TEMPLATE_NAMES, false);
  exporter2.export_data(out_filename2, lmodel);

  CPPUNIT_ASSERT(file_exists(out_filename2) == true);
  
}

