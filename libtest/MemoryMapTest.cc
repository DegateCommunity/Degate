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


#include "MemoryMap.h"
#include "MemoryMapTest.h"

#include "globals.h"
#include <stdlib.h>

CPPUNIT_TEST_SUITE_REGISTRATION (MemoryMapTest);

using namespace std;
using namespace degate;

void MemoryMapTest::setUp(void) { 
}

void MemoryMapTest::tearDown(void) {
}

void MemoryMapTest::test_mem(void) {
	MemoryMap<int> mm(100, 100);
	//CPPUNIT_ASSERT(mm.is_mem());

	CPPUNIT_ASSERT(mm.get_width() == 100);
	CPPUNIT_ASSERT(mm.get_height() == 100);
	
	mm.set(10, 10, 99);
	CPPUNIT_ASSERT(mm.get(10, 10) == 99);
}

void MemoryMapTest::test_temp(void) {
	MemoryMap<int> mm(100, 100, MAP_STORAGE_TYPE_TEMP_FILE, "/tmp/tmp.XXXXXXXXX");
	//CPPUNIT_ASSERT(mm.is_temp_file());

	CPPUNIT_ASSERT(mm.get_width() == 100);
	CPPUNIT_ASSERT(mm.get_height() == 100);

	mm.set(10, 10, 99);
	CPPUNIT_ASSERT(mm.get(10, 10) == 99);
	
}

void MemoryMapTest::test_persistent(void) {
	string filename("/tmp/.tmp.abcdefg");
	MemoryMap<int> mm(100, 100, MAP_STORAGE_TYPE_PERSISTENT_FILE, filename);
	//CPPUNIT_ASSERT(mm.is_persistent_file());
	CPPUNIT_ASSERT(mm.get_filename() == filename);
	
	CPPUNIT_ASSERT(mm.get_width() == 100);
	CPPUNIT_ASSERT(mm.get_height() == 100);
	
	mm.set(10, 10, 99);
	CPPUNIT_ASSERT(mm.get(10, 10) == 99);
	
	/*
	CPPUNIT_ASSERT(RET_IS_OK(mm.deactivate_mapping()));
	CPPUNIT_ASSERT(RET_IS_NOT_OK(mm.deactivate_mapping()));

	CPPUNIT_ASSERT(RET_IS_OK(mm.reactivate_mapping()));
	CPPUNIT_ASSERT(RET_IS_NOT_OK(mm.reactivate_mapping()));
	*/
	
}

