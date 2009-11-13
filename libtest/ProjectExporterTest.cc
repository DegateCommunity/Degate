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


#include "ProjectExporterTest.h"
#include "ProjectExporter.h"
#include "ProjectImporter.h"
#include "Project.h"
#include "globals.h"

#include <unistd.h>
#include <sys/param.h>
#include <stdlib.h>
#include <stdexcept>


CPPUNIT_TEST_SUITE_REGISTRATION (ProjectExporterTest);

using namespace std;
using namespace degate;

void ProjectExporterTest::setUp(void) {
}

void ProjectExporterTest::tearDown(void) {
}


void ProjectExporterTest::test_export(void) {

  /*
   * import a project
   */
  ProjectImporter importer;		

  string filename("libtest/testfiles/testproject/project.xml");
  Project_shptr prj(importer.import_all(filename));
	
  CPPUNIT_ASSERT(prj != NULL);

  /*
   * export project data
   */
  ProjectExporter exporter;
  std::string outdir("/tmp");
  exporter.export_all(outdir, prj);

}

