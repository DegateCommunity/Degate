/* -*-c++-*-

 This file is part of the IC reverse engineering tool degate.

 Copyright 2008, 2009, 2010 by Martin Schobert
 Copyright 2013 by Taekgwan Kim

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

#include <BinaryLineDetection.h>
#include <Otsu.h>
#include <IPCopy.h>
#include <IPMedianFilter.h>
#include <IPNormalize.h>
#include <IPConvolve.h>
#include <IPThresholding.h>

#include <ImageManipulation.h>
#include <IPImageWriter.h>

#include <RegionList.h>

using namespace degate;

BinaryLineDetection::BinaryLineDetection(unsigned int _min_x, unsigned int _max_x,
					unsigned int _min_y, unsigned int _max_y,
					unsigned int _wire_diameter,
					unsigned int _median_filter_width,
					unsigned int _blur_kernel_size,
					double _sigma) :
  min_x(_min_x),
  max_x(_max_x),
  min_y(_min_y),
  max_y(_max_y),
  wire_diameter(_wire_diameter),
  median_filter_width(_median_filter_width),
  blur_kernel_size(_blur_kernel_size),
  border(_blur_kernel_size >> 1),
  sigma(_sigma),
  has_path(false) {

    setup_pipe();

  }

BinaryLineDetection::~BinaryLineDetection() {}

TileImage_GS_DOUBLE_shptr BinaryLineDetection::run(ImageBase_shptr img_in, 
						TileImage_GS_DOUBLE_shptr probability_map, 
						std::string const& directory) {

  set_directory(directory);
  grayImage = std::dynamic_pointer_cast<TileImage_GS_DOUBLE>(pipe.run(img_in));
  save_normalized_image<TileImage_GS_DOUBLE>("/tmp/gray.tif", grayImage);

  TileImage_GS_DOUBLE_shptr binOtsu, binMean1_0, binMean1_1, binMean1_2;
  binOtsu = gs_to_binary(grayImage);
  binMean1_0 = gs_by_mean(grayImage, 1.0);
  binMean1_1 = gs_by_mean(grayImage, 1.1);
  binMean1_2 = gs_by_mean(grayImage, 1.2);
  //save_normalized_image<TileImage_GS_DOUBLE>("/tmp/mean.tif", meanImage);

  RegionList RL_Otsu, RL_Mean1_0, RL_Mean1_1, RL_Mean1_2;
  RL_Otsu = binary_to_region(binOtsu);
  RL_Mean1_0 = binary_to_region(binMean1_0);
  RL_Mean1_1 = binary_to_region(binMean1_1);
  RL_Mean1_2 = binary_to_region(binMean1_2);

  // XXX better use a real temp dir as this allows race conditions
  save_normalized_image<TileImage_GS_DOUBLE>("/tmp/gridOtsu.tif", RL_Otsu.get_unfixed_grid_binary(wire_diameter));
  save_normalized_image<TileImage_GS_DOUBLE>("/tmp/gridMean1.0.tif", RL_Mean1_0.get_unfixed_grid_binary(wire_diameter));
  save_normalized_image<TileImage_GS_DOUBLE>("/tmp/gridMean1.1.tif", RL_Mean1_1.get_unfixed_grid_binary(wire_diameter));
  save_normalized_image<TileImage_GS_DOUBLE>("/tmp/gridMean1.2.tif", RL_Mean1_2.get_unfixed_grid_binary(wire_diameter));

  RL_Otsu.application_grid(wire_diameter);
  RL_Mean1_0.application_grid(wire_diameter);
  RL_Mean1_1.application_grid(wire_diameter);
  RL_Mean1_2.application_grid(wire_diameter);

  save_normalized_image<TileImage_GS_DOUBLE>("/tmp/reducedOtsu.tif", RL_Otsu.get_binary());
  save_normalized_image<TileImage_GS_DOUBLE>("/tmp/reducedMean1.0.tif", RL_Mean1_0.get_binary());
  save_normalized_image<TileImage_GS_DOUBLE>("/tmp/reducedMean1.1.tif", RL_Mean1_1.get_binary());
  save_normalized_image<TileImage_GS_DOUBLE>("/tmp/reducedMean1.2.tif", RL_Mean1_2.get_binary());

  //save_normalized_image<TileImage_GS_DOUBLE>("/tmp/bin.tif", binImage);
  //binImage = region.get_binary();
  //debug(TM, "%d", region.get_count());
  //region.save_region();
  return binOtsu;

}

void BinaryLineDetection::setup_pipe() {

  debug(TM, "will extract background image (%d, %d) (%d, %d)", min_x, min_y, max_x, max_y);
  std::shared_ptr<IPCopy<TileImage_RGBA, TileImage_GS_DOUBLE> > copy_rgba_to_gs
    (new IPCopy<TileImage_RGBA, TileImage_GS_DOUBLE>(min_x, max_x, min_y, max_y));

  pipe.add(copy_rgba_to_gs);

  if(median_filter_width > 0) {
    std::shared_ptr<IPMedianFilter<TileImage_GS_DOUBLE, TileImage_GS_DOUBLE> > median_filter
      (new IPMedianFilter<TileImage_GS_DOUBLE, TileImage_GS_DOUBLE>(median_filter_width));

    pipe.add(median_filter);
  }

  std::shared_ptr<IPNormalize<TileImage_GS_DOUBLE, TileImage_GS_DOUBLE> > normalizer
    (new IPNormalize<TileImage_GS_DOUBLE, TileImage_GS_DOUBLE>(0, 255) );

  pipe.add(normalizer);

  if(blur_kernel_size > 0) {
    std::shared_ptr<GaussianBlur>
      GaussianB(new GaussianBlur(blur_kernel_size, blur_kernel_size, sigma));

    GaussianB->print();
    std::shared_ptr<IPConvolve<TileImage_GS_DOUBLE, TileImage_GS_DOUBLE> > gaussian_blur
      (new IPConvolve<TileImage_GS_DOUBLE, TileImage_GS_DOUBLE>(GaussianB) );

    pipe.add(gaussian_blur);
  }

}

unsigned int BinaryLineDetection::get_width() const {
  return max_x - min_x;
}

unsigned int BinaryLineDetection::get_height() const {
  return max_y - min_y;
}

unsigned int BinaryLineDetection::get_border() const {
  return border;
}

std::string BinaryLineDetection::get_directory() const {
  return directory;
}

bool BinaryLineDetection::has_directory() const {
  return has_path;
}

void BinaryLineDetection::set_directory(std::string const& path) {
  directory = path;
  has_path = true;
}

TileImage_GS_DOUBLE_shptr BinaryLineDetection::gs_to_binary(TileImage_GS_DOUBLE_shptr gray) {

  Otsu o;
  TileImage_GS_DOUBLE_shptr binary_image(new TileImage_GS_DOUBLE(get_width(), get_height()));
  double otsu_threshold;
 
  o.run(gray);
  otsu_threshold = o.get_otsu_threshold();
  debug(TM, "\t\t%lf", otsu_threshold); 

  for(unsigned int y = border; y < get_height() - border- 1; y++) {
    for(unsigned int x = border; x < get_width() - border- 1; x++) {
      if(gray->get_pixel(x, y) < otsu_threshold)
	binary_image->set_pixel(x, y, 0);
      else
	binary_image->set_pixel(x, y, 1);
    }
  }

  return binary_image;

}

TileImage_GS_DOUBLE_shptr BinaryLineDetection::gs_by_mean(TileImage_GS_DOUBLE_shptr gray, double scale) {

  double threshold, sum = 0.0;
  TileImage_GS_DOUBLE_shptr mean_image(new TileImage_GS_DOUBLE(get_width(), get_height()));

  for(unsigned int y = border; y < get_height() - border - 1; y++) {
    for(unsigned int x = border; x < get_width() - border - 1; x++) {
      sum += gray->get_pixel(x, y);
    }
  }

  threshold = sum / ((get_height() - 2 * border) * (get_width() - 2 * border));
  threshold *= scale;

  for(unsigned int y = border; y < get_height() - border - 1; y++) {
    for(unsigned int x = border; x < get_width() - border - 1; x++) {
      if(gray->get_pixel(x, y) >= threshold)
	mean_image->set_pixel(x, y, 1);
      else
	mean_image->set_pixel(x, y, 0);
    }
  }

  return mean_image;

}
TileImage_GS_DOUBLE_shptr BinaryLineDetection::binary_to_edge(TileImage_GS_DOUBLE_shptr binary) {

  unsigned int tmp_y, tmp_x_start, tmp_x_end;
  TileImage_GS_DOUBLE_shptr region(new TileImage_GS_DOUBLE(get_width(), get_height()));

  for(unsigned int y = 0; y < get_height() - 0 - 1; y++) {
    for(unsigned int x = 0; x < get_width() - 0 - 1; x++) {
      if(binary->get_pixel(x, y) == 0) continue;
      else if(binary->get_pixel(x, y) == 1) {
	tmp_y = y;
	tmp_x_start = x;
	while(binary->get_pixel(++x, y) == 1) {
	  if(x == get_width() - 0) break;
	}
	tmp_x_end = x - 1;
      }
      region->set_pixel(tmp_x_start, tmp_y, 1);
      region->set_pixel(tmp_x_end, tmp_y, 1);
    }
  }

  return region;

}

RegionList BinaryLineDetection::binary_to_region(TileImage_GS_DOUBLE_shptr binary) {

  unsigned int tmp_y, tmp_x_start, tmp_x_end;
  RegionList testList(get_width(), get_height());

  for(unsigned int y = 0; y < get_height(); y++) {
    for(unsigned int x = 0; x < get_width(); x++) {
      if(binary->get_pixel(x, y) == 0) continue;
      else if(binary->get_pixel(x, y) == 1) {
	tmp_y = y;
	tmp_x_start = x;
	while(binary->get_pixel(++x, y) == 1) {
	  if(x == get_width() - 0) break;
	}
	tmp_x_end = x - 1;
      }
      testList.set_region(tmp_y, tmp_x_start, tmp_x_end);
    }
  }
debug(TM, "end of making regions");
  //testList.save_region();
  //testList.print_region();
  //testList.free_all_region();
  return testList;

}

void BinaryLineDetection::draw_grid(TileImage_GS_DOUBLE_shptr & binary) {

  for(unsigned int y = 0; y < get_height(); y=y + wire_diameter) {
    for(unsigned int x = 0; x < get_width(); x=x + wire_diameter) {
      binary->set_pixel(x, y, 0);
    }
  }
}
