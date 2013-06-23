/* -*-c++-*-

 This file is part of the IC reverse engineering tool degate.

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

#ifndef __BINARYLINEDETECTION_H__
#define __BINARYLINEDETECTION_H__

#include <Image.h>
#include <IPPipe.h>

namespace degate {

  class BinaryLineDetection {

  private:

		IPPipe pipe;

		unsigned int min_x, max_x, min_y, max_y;
		unsigned int wire_diameter;
		unsigned int median_filter_width;

		unsigned int blur_kernel_size, border;
		double sigma;
		bool has_path;

		TileImage_GS_DOUBLE_shptr grayImage;
		TileImage_GS_DOUBLE_shptr binImage;
		TileImage_GS_DOUBLE_shptr regionImage;

		std::string directory; // path for storing debug images

	private:

	public:

		void setup_pipe();
		
		unsigned int get_width() const;
		unsigned int get_height() const;
		unsigned int get_border() const;

		std::string get_directory() const;

		bool has_directory() const;

		void set_directory(std::string const& path);
		
		TileImage_GS_DOUBLE_shptr gs_to_binary(TileImage_GS_DOUBLE_shptr gray);
		TileImage_GS_DOUBLE_shptr binary_to_region(TileImage_GS_DOUBLE_shptr binary);

	public:

		BinaryLineDetection(unsigned int min_x, unsigned int max_x,
										unsigned int min_y, unsigned int max_y,
										unsigned int wire_diameter,
										unsigned int median_filter_width = 3,
										unsigned int blur_kernel_size = 10,
										double sigma = 0.5);

		~BinaryLineDetection();

		TileImage_GS_DOUBLE_shptr run(ImageBase_shptr img_in,
										TileImage_GS_DOUBLE_shptr probability_map,
										std::string const& directory);

  };

}

#endif
