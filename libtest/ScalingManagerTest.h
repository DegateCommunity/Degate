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

#ifndef __SCALINGMANAGERTEST_H__
#define __SCALINGMANAGERTEST_H__

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>
#include <tr1/memory>

#include <Image.h>
#include <TIFFReader.h>

class ScalingManagerTest : public CPPUNIT_NS :: TestFixture {
  
  CPPUNIT_TEST_SUITE(ScalingManagerTest);
  
  CPPUNIT_TEST (test_scaling_manager_shptrimg);
  
  CPPUNIT_TEST_SUITE_END ();
  
public:
  void setUp (void);
  void tearDown (void);
  
protected:

  void test_scaling_manager_shptrimg(void);
  
};

#endif
