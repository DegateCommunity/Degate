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


#include "FileSystemTest.h"
#include "FileSystem.h"
#include "globals.h"

#include <unistd.h>
#include <sys/param.h>
#include <stdlib.h>

#include <stdexcept>
#include <list>
#include <string>

CPPUNIT_TEST_SUITE_REGISTRATION (FileSystemTest);

using namespace std;
using namespace degate;

void FileSystemTest::setUp(void) {
}

void FileSystemTest::tearDown(void) {
}


void FileSystemTest::test_is_directory(void) {
  
  CPPUNIT_ASSERT(is_directory("/reallynonexistentdirectory") == false);
  CPPUNIT_ASSERT(is_directory("/tmp/") == true);
  CPPUNIT_ASSERT(is_directory("/tmp") == true);
  CPPUNIT_ASSERT(is_directory("/etc/hosts") == false);
}

void FileSystemTest::test_is_file(void) {

  CPPUNIT_ASSERT(is_file("/reallynonexistentdirectory") == false);
  CPPUNIT_ASSERT(is_file("/etc/hosts") == true);
  CPPUNIT_ASSERT(is_file("/etc/") == false);
  CPPUNIT_ASSERT(is_file("/etc") == false);
}

void FileSystemTest::test_file_exists(void) {

  CPPUNIT_ASSERT(file_exists("/reallynonexistentdirectory") == false);
  CPPUNIT_ASSERT(file_exists("/etc/hosts") == true);
  CPPUNIT_ASSERT(file_exists("/etc/") == true);
}

void FileSystemTest::test_get_realpath(void) {
  CPPUNIT_ASSERT(get_realpath("/usr") == "/usr");
  CPPUNIT_ASSERT(get_realpath("/usr/") == "/usr");
}

void FileSystemTest::test_get_basedir(void) {

  std::string expectedResult(get_realpath("/etc")); // on os x /etc is a symlink

  CPPUNIT_ASSERT(expectedResult.compare( get_basedir("/etc/hosts")) == 0);
  CPPUNIT_ASSERT(expectedResult.compare( get_basedir("/etc/")) == 0);
  CPPUNIT_ASSERT(expectedResult.compare( get_basedir("/etc")) == 0);

  CPPUNIT_ASSERT(get_basedir("/nonex/istent") == "/nonex");
  CPPUNIT_ASSERT(get_basedir("/nonexistent") == "/");

}

void FileSystemTest::test_get_suffix(void) {
  std::string suffix_dat(get_file_suffix("test.dat"));

  if(suffix_dat.compare("dat") != 0) {
    cout << "Error: the suffix for test.dat is: [" 
	 << suffix_dat << "] but should be just 'dat'" 
	 << std::endl;
  }

  CPPUNIT_ASSERT(suffix_dat.compare("dat") == 0);

  std::string suffix_none(get_file_suffix("testdat"));
  CPPUNIT_ASSERT(suffix_none.size() == 0);

  std::string suffix_not_really(get_file_suffix("testdat."));
  CPPUNIT_ASSERT(suffix_not_really.size() == 0);

  std::string suffix_not_really2(get_file_suffix("testdat......."));
  CPPUNIT_ASSERT(suffix_not_really2.size() == 0);
}

void FileSystemTest::test_read_directory(void) {
  list<string> files = read_directory("/etc");
  CPPUNIT_ASSERT(files.empty() == false); // for a test this assumption is ok

  for(list<string>::const_iterator iter = files.begin();
      iter != files.end(); ++iter) {
    CPPUNIT_ASSERT(*iter != "..");
    CPPUNIT_ASSERT(*iter != ".");
  }
				      

  CPPUNIT_ASSERT_THROW(read_directory("/reallynonexistentdirectory"), FileSystemException);
}

void FileSystemTest::test_create_and_remove_temp_directory(void) {
  string dir = create_temp_directory();

  CPPUNIT_ASSERT(file_exists(dir) == true);
  remove_directory(dir);
  CPPUNIT_ASSERT(file_exists(dir) == false);
}

void FileSystemTest::test_create_and_remove_directory(void) {

  string my_dir("/tmp/test-dir-for-degate-unit-tests");

  CPPUNIT_ASSERT(file_exists(my_dir) == false);

  create_directory(my_dir);

  CPPUNIT_ASSERT(file_exists(my_dir) == true);

  remove_directory(my_dir);

  CPPUNIT_ASSERT(file_exists(my_dir) == false);
}

void FileSystemTest::test_get_filename_from_path(void) {

  CPPUNIT_ASSERT(get_filename_from_path("") == "");
  CPPUNIT_ASSERT(get_filename_from_path("/etc/hosts") == "hosts");
  CPPUNIT_ASSERT(get_filename_from_path("/etc/resolv.conf") == "resolv.conf");
}

void FileSystemTest::test_get_basename(void) {
  CPPUNIT_ASSERT(get_basename("") == "");
  CPPUNIT_ASSERT(get_basename("/etc/hosts") == "hosts");
  CPPUNIT_ASSERT(get_basename("/etc/resolv.conf") == "resolv");
  CPPUNIT_ASSERT(get_basename("/etc/resolv.") == "resolv");
  CPPUNIT_ASSERT(get_basename("/etc/.conf") == "");
  CPPUNIT_ASSERT(get_basename("/etc/resolv...conf") == "resolv..");

  CPPUNIT_ASSERT(get_basename("resolv.conf") == "resolv");
  CPPUNIT_ASSERT(get_basename(".conf") == "");

}

void FileSystemTest::test_get_relative_path(void) {

  
  CPPUNIT_ASSERT(get_relative_path("/", "/") == "");
  CPPUNIT_ASSERT(get_relative_path("/usr", "/usr/") == "");
  CPPUNIT_ASSERT(get_relative_path("/usr/", "/usr") == "");

  CPPUNIT_ASSERT(get_relative_path("/usr/include", "/usr/") == "./include");
  CPPUNIT_ASSERT(get_relative_path("/usr/include/", "/usr/") == "./include");
  CPPUNIT_ASSERT(get_relative_path("/usr/include", "/usr/") == "./include");

  CPPUNIT_ASSERT(get_relative_path("/usr/", "/usr/include") == "..");
}
