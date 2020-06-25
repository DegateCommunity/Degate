/* -*-c++-*-

  This file is part of the IC reverse engineering tool degate.

  Copyright 2008, 2009, 2010 by Martin Schobert

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

#include <Core/Image/Manipulation/ScalingManager.h>
#include <Core/Image/Image.h>
#include <Core/Image/TIFFReader.h>

#include "catch.hpp"

using namespace degate;

TEST_CASE("Test scaling manager", "[ScalingManager]")
{
    std::string image_file("tests_files/test_file.tif");

    // create an image
    TIFFReader<BackgroundImage> tiff_reader(image_file);
    bool read_result = tiff_reader.read();
    REQUIRE(read_result == true);

    std::string img_dir(create_temp_directory());

    BackgroundImage_shptr img(new BackgroundImage(tiff_reader.get_width(),
                                                       tiff_reader.get_height(),
                                                       img_dir,
                                                       true));

    bool state = tiff_reader.get_image(img);
    REQUIRE(state == true);

    ScalingManager<BackgroundImage> sm(img, img->get_directory(), 256);
    sm.create_scalings();
}