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

#ifndef __IMAGETEST_H__
#define __IMAGETEST_H__

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>
#include <memory>

#include <Image.h>
#include <ImageReaderFactory.h>

class ImageTest : public CPPUNIT_NS :: TestFixture {
  
  CPPUNIT_TEST_SUITE(ImageTest);
  
  CPPUNIT_TEST (test_rgba_in_mem);
  CPPUNIT_TEST (test_rgba_in_temp_file);
  
  CPPUNIT_TEST (test_type_traits);
  CPPUNIT_TEST (test_image_reader);
  CPPUNIT_TEST (test_convert_pixel);
  CPPUNIT_TEST (test_copy_pixel);
  
  CPPUNIT_TEST_SUITE_END ();
  
public:
  void setUp (void);
  void tearDown (void);
  
protected:
  void test_rgba_in_mem(void);
  void test_rgba_in_temp_file(void);
  void test_type_traits(void);
  void test_image_reader(void);
  void test_convert_pixel(void);
  void test_copy_pixel(void);
  
  
  
private:
  degate::TileImage_RGBA_shptr read_image(std::shared_ptr<
					  degate::ImageReaderBase<degate::TileImage_RGBA> > reader, 
					  unsigned int tile_size_exp);
  
};

#endif
