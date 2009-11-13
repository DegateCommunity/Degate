/* -*-c++-*-
 
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

#ifndef __PROJECTIMPORTERTEST_H__
#define __PROJECTIMPORTERTEST_H__

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>
#include "ProjectImporter.h"

class ProjectImporterTest : public CPPUNIT_NS :: TestFixture {
	
  CPPUNIT_TEST_SUITE(ProjectImporterTest);
  
  CPPUNIT_TEST (test_import);
  CPPUNIT_TEST (test_import_all);
  CPPUNIT_TEST (test_import_all_new_format);
  CPPUNIT_TEST (test_get_object_at);
  
  CPPUNIT_TEST_SUITE_END ();
	
public:
  void setUp (void);
  void tearDown (void);
  
protected:
  void test_import(void);
  void test_import_all(void);
  void test_import_all_new_format(void);
  void test_get_object_at(void);

};

#endif
