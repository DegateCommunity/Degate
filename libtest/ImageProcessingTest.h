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

#ifndef __IMAGEPROCESSINGTEST_H__
#define __IMAGEPROCESSINGTEST_H__

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>
#include <memory>

#include <degate.h>

class ImageProcessingTest : public CPPUNIT_NS :: TestFixture {
  
  CPPUNIT_TEST_SUITE(ImageProcessingTest);
  
  CPPUNIT_TEST (test_pipe);
  //CPPUNIT_TEST (test_wire_matching);
  //CPPUNIT_TEST (test_background_classification_dect);
  //CPPUNIT_TEST (test_background_classification_legic);
  //CPPUNIT_TEST(test_line_segment_extraction);
  //CPPUNIT_TEST(test_mifare);
  //CPPUNIT_TEST(test_c_unknown);

  CPPUNIT_TEST_SUITE_END ();
  
public:

  void setUp (void);
  void tearDown (void);
  
protected:

  void test_pipe(void);
  void test_wire_matching(void);
  void test_background_classification_dect(void);
  void test_background_classification_legic(void);
  void test_line_segment_extraction(void);
  void test_mifare(void);
  void test_c_unknown(void);
};

#endif
