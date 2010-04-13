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


#include "LookupSubcircuitTest.h"
#include <ProjectImporter.h>

#include "globals.h"
#include <stdlib.h>


CPPUNIT_TEST_SUITE_REGISTRATION (LookupSubcircuitTest);

using namespace degate;

void LookupSubcircuitTest::setUp(void) { 
}

void LookupSubcircuitTest::tearDown(void) {
}

void LookupSubcircuitTest::test(void) {

  ProjectImporter importer;

  std::string filename("/opt/degate-data/new/SC14421CVF_dect/subproject_1/project.xml");
  Project_shptr prj(importer.import_all(filename));


  LogicModel_shptr lmodel = prj->get_logic_model();
  assert(lmodel != NULL);

  LookupSubcircuit lsc(lmodel);
  lsc.search();
}
