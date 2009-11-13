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


#include <ProjectImporterTest.h>
#include <Project.h>
#include <globals.h>
#include <LogicModelHelper.h>

#include <unistd.h>
#include <sys/param.h>
#include <stdlib.h>
#include <stdexcept>


CPPUNIT_TEST_SUITE_REGISTRATION (ProjectImporterTest);

using namespace std;
using namespace degate;

void ProjectImporterTest::setUp(void) {
}

void ProjectImporterTest::tearDown(void) {
}


void ProjectImporterTest::test_import(void) {

  ProjectImporter importer;		

  string filename("libtest/testfiles/testproject/project.xml");
  Project_shptr prj(importer.import(filename));

  CPPUNIT_ASSERT(prj != NULL);

  LogicModel_shptr lmodel = prj->get_logic_model();
  CPPUNIT_ASSERT(lmodel != NULL);

  Layer_shptr layer = get_first_logic_layer(lmodel);
  CPPUNIT_ASSERT(layer != NULL);
  
}

void ProjectImporterTest::test_import_all(void) {

  ProjectImporter importer;		

  string filename("libtest/testfiles/testproject/project.xml");
  Project_shptr prj(importer.import_all(filename));
	
  CPPUNIT_ASSERT(prj != NULL);

  std::cout << "back in test." << std::endl;

  LogicModel_shptr lmodel = prj->get_logic_model();
  CPPUNIT_ASSERT(lmodel != NULL);

  prj->print_all(cout);

  CPPUNIT_ASSERT(lmodel->get_num_layers() == 2);

  Layer_shptr layer = get_first_logic_layer(lmodel);
  CPPUNIT_ASSERT(layer != NULL);
 
}

void ProjectImporterTest::test_import_all_new_format(void) {
  ProjectImporter importer;		

  string filename("libtest/testfiles/testproject_new/project.xml");
  Project_shptr prj(importer.import_all(filename));

  LogicModel_shptr lmodel = prj->get_logic_model();
  CPPUNIT_ASSERT(lmodel != NULL);

  Layer_shptr layer = get_first_logic_layer(lmodel);
  CPPUNIT_ASSERT(layer != NULL);

  GateLibrary_shptr gate_lib = lmodel->get_gate_library();
  CPPUNIT_ASSERT(gate_lib != NULL);

  for(GateLibrary::template_iterator iter = gate_lib->begin();
      iter != gate_lib->end(); ++iter) {

    GateTemplate_shptr gate_tmpl((*iter).second);
    CPPUNIT_ASSERT(gate_tmpl != NULL);

    int i = 0;
    for(GateTemplate::image_iterator img_iter = gate_tmpl->images_begin();
	img_iter != gate_tmpl->images_end(); ++img_iter, i++) {
      
      Layer::LAYER_TYPE layer_type = (*img_iter).first;
      GateTemplateImage_shptr img = (*img_iter).second;    
      CPPUNIT_ASSERT(img != NULL);
    }
    CPPUNIT_ASSERT(i > 0);
  }
  
}

void ProjectImporterTest::test_get_object_at(void) {
  ProjectImporter importer;		

  string filename("libtest/testfiles/testproject/project.xml");
  Project_shptr prj(importer.import_all(filename));


  LogicModel_shptr lmodel = prj->get_logic_model();
  Layer_shptr layer = lmodel->get_current_layer();
  debug(TM, "try to run get_object_at_position");
  PlacedLogicModelObject_shptr plo = layer->get_object_at_position(889, 225);

  CPPUNIT_ASSERT(plo->get_name() == "02.19");
}
