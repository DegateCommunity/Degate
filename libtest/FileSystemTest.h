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

#ifndef __FILESYSTEMTEST_H__
#define __FILESYSTEMTEST_H__

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>
#include "FileSystem.h"

class FileSystemTest : public CPPUNIT_NS :: TestFixture {
  
  CPPUNIT_TEST_SUITE(FileSystemTest);
  
  CPPUNIT_TEST (test_is_directory);
  CPPUNIT_TEST (test_is_file);
  CPPUNIT_TEST (test_file_exists);
  CPPUNIT_TEST (test_get_basedir);
  CPPUNIT_TEST (test_get_suffix);
  CPPUNIT_TEST (test_get_realpath);
  CPPUNIT_TEST (test_read_directory);
  CPPUNIT_TEST (test_create_and_remove_temp_directory);
  CPPUNIT_TEST (test_create_and_remove_directory);
  CPPUNIT_TEST (test_get_filename_from_path);
  CPPUNIT_TEST (test_get_basename);
  CPPUNIT_TEST (test_get_relative_path);
  
  CPPUNIT_TEST_SUITE_END ();
  
public:
  void setUp (void);
  void tearDown (void);
  
protected:
  void test_is_directory(void);
  void test_is_file(void);
  void test_file_exists(void);
  void test_get_basedir(void);
  void test_get_realpath(void);
  void test_get_suffix(void);
  void test_read_directory(void);
  void test_create_and_remove_temp_directory(void);
  void test_create_and_remove_directory(void);
  void test_get_filename_from_path(void);
  void test_get_basename(void);
  void test_get_relative_path(void);
};

#endif
