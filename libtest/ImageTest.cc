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


#include "ImageTest.h"
#include "Image.h"
#include "ImageReaderFactory.h"
#include "TIFFReader.h"
#include "TIFFWriter.h"
#include "TileImage.h"
#include "ImageReaderBase.h"
#include "ImageManipulation.h"

#include "globals.h"
#include <stdlib.h>

CPPUNIT_TEST_SUITE_REGISTRATION (ImageTest);

using namespace std;
using namespace degate;

void ImageTest::setUp(void) { 
}

void ImageTest::tearDown(void) {
}

void ImageTest::test_rgba_in_mem(void) {

  Image< PixelPolicy_RGBA,
    StoragePolicy_Memory > img(100, 100);

  img.set_pixel(80, 80, MERGE_CHANNELS(23, 24, 25, 26));
  rgba_pixel_t p = img.get_pixel(80, 80);
  
  CPPUNIT_ASSERT(MASK_R(p) == 23);
  CPPUNIT_ASSERT(MASK_G(p) == 24);
  CPPUNIT_ASSERT(MASK_B(p) == 25);
  CPPUNIT_ASSERT(MASK_A(p) == 26);
}

void ImageTest::test_rgba_in_temp_file(void) {
	
  // create a temp image
  Image<PixelPolicy_RGBA,
    StoragePolicy_TempFile> img(100, 100);

  img.set_pixel(80, 80, MERGE_CHANNELS(23, 24, 25, 26));
  rgba_pixel_t p = img.get_pixel(80, 80);
  
  CPPUNIT_ASSERT(MASK_R(p) == 23);
  CPPUNIT_ASSERT(MASK_G(p) == 24);
  CPPUNIT_ASSERT(MASK_B(p) == 25);
  CPPUNIT_ASSERT(MASK_A(p) == 26);
}

void ImageTest::test_type_traits(void) {
  CPPUNIT_ASSERT(degate::is_pointer<TileImage_RGBA>::value == false);
  CPPUNIT_ASSERT(degate::is_pointer<TileImage_RGBA *>::value == true);
  CPPUNIT_ASSERT(degate::is_pointer<TileImage_RGBA_shptr>::value == true);
}

TileImage_RGBA_shptr ImageTest::read_image(std::shared_ptr<degate::ImageReaderBase
			   <degate::TileImage_RGBA> > reader, 
			   unsigned int tile_size_exp) {
  clock_t start_time = clock();

  TileImage_RGBA_shptr img(new TileImage_RGBA(reader->get_width(), 
					      reader->get_height(),
					      tile_size_exp));

  CPPUNIT_ASSERT(img->get_directory().size() > 0);

  bool state = reader->get_image(img);

  double elapsed_time = (double)(clock() - start_time) / CLOCKS_PER_SEC;
  cout << "Elapsed time for reading the image with a tile size of " 
       << (1 << tile_size_exp)
       << "x"
       << (1 << tile_size_exp)
       <<" is: " 
       << elapsed_time << " seconds." << std::endl;
  
  CPPUNIT_ASSERT(state == true);

  // at least after reading data there should be a temp directory
  CPPUNIT_ASSERT(is_directory(img->get_directory()));

  return img;
}



void ImageTest::test_image_reader(void) {
  
  std::string imagefile("libtest/testfiles/testfile.tif");

  // create the image reader factory
  ImageReaderFactory<TileImage_RGBA> ir_factory;

  // check if the factory has at least one reader
  file_format_collection const& file_formats = 
    ir_factory.get_file_formats();

  CPPUNIT_ASSERT(file_formats.size() > 0);

  // get a tiff reader from the factory
  std::shared_ptr<ImageReaderBase<TileImage_RGBA> > tiff_reader = 
    ir_factory.get_reader(imagefile);


  // check filename
  CPPUNIT_ASSERT(tiff_reader->get_filename() == imagefile);

  bool read_result = tiff_reader->read();
  CPPUNIT_ASSERT(read_result == true);

  std::cout << std::endl
	    << "The image to read has size "
	    << tiff_reader->get_width() << "x" << tiff_reader->get_height() 
	    << "." << std::endl;

  CPPUNIT_ASSERT(tiff_reader->get_width() > 0);
  CPPUNIT_ASSERT(tiff_reader->get_height() > 0);


  read_image(tiff_reader, 8 /* tiles of size 256x256 */);

  TileImage_RGBA_shptr img = read_image(tiff_reader, 
					10 /* tiles of size 1024x1024 */);

  std::string tiff_out("degate_image_test.tif");
  if(file_exists(tiff_out)) remove_file(tiff_out);
  CPPUNIT_ASSERT(file_exists(tiff_out) == false);
		       
  TIFFWriter<TileImage_RGBA> tiff_writer(img->get_width(), img->get_height(), tiff_out);
  CPPUNIT_ASSERT(tiff_writer.write_image(img) == true);

  CPPUNIT_ASSERT(file_exists(tiff_out) == true);
}

void ImageTest::test_convert_pixel(void) {

  gs_byte_pixel_t br = convert_pixel<gs_byte_pixel_t, rgba_pixel_t>(0xdeadbeef);
  CPPUNIT_ASSERT(RGBA_TO_GS_BY_VAL(0xdeadbeef) == br);

  gs_byte_pixel_t bd = convert_pixel<gs_byte_pixel_t, gs_double_pixel_t>(4.0);
  CPPUNIT_ASSERT(4 == bd);



  gs_double_pixel_t dr = convert_pixel<gs_double_pixel_t, rgba_pixel_t>(0xdeadbeef);
  CPPUNIT_ASSERT(RGBA_TO_GS_BY_VAL(0xdeadbeef) == dr);

  gs_double_pixel_t db = convert_pixel<gs_double_pixel_t, gs_byte_pixel_t>(23);
  CPPUNIT_ASSERT(23.0 == db);



  rgba_pixel_t rb = convert_pixel<rgba_pixel_t, gs_byte_pixel_t>(0xa0);
  CPPUNIT_ASSERT(MERGE_CHANNELS(0xa0, 0xa0, 0xa0, 255) == rb);

  rgba_pixel_t rd = convert_pixel<rgba_pixel_t, gs_double_pixel_t>(4.0);
  CPPUNIT_ASSERT(MERGE_CHANNELS(4, 4, 4, 255) == rd);
}

void ImageTest::test_copy_pixel(void) {

  TileImage_RGBA_shptr img1(new TileImage_RGBA(10, 10));
  TileImage_GS_BYTE_shptr img2(new TileImage_GS_BYTE(10, 10));

  get_pixel_as<gs_byte_pixel_t, TileImage_RGBA>(img1, 5, 5);

  img1->get_pixel_as<gs_byte_pixel_t>(5, 5);
}
