/**
 * This file is part of the IC reverse engineering tool Degate.
 *
 * Copyright 2008, 2009, 2010 by Martin Schobert
 * Copyright 2019-2020 Dorian Bachelot
 *
 * Degate is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * Degate is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with degate. If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef __IPCOPY_H__
#define __IPCOPY_H__

#include <string>
#include "Core/Image/Processor/ImageProcessorBase.h"

namespace degate
{
	/**
	 * Processor: Copy an image with auto conversion.
	 */
	template <typename ImageTypeIn, typename ImageTypeOut>
	class IPCopy : public ImageProcessorBase
	{
	private:

		unsigned int min_x, max_x, min_y, max_y;
		bool work_on_region;

	public:

		/**
		 * The constructor for processing the whole image.
		 */
		IPCopy() :
			ImageProcessorBase("IPCopy",
			                   "Copy an image with pixel type auto conversion",
			                   false,
			                   typeid(typename ImageTypeIn::pixel_type),
			                   typeid(typename ImageTypeOut::pixel_type)),
			work_on_region(false)
		{
		}

		/**
		 * The constructor for working on an image region.
		 */
		IPCopy(unsigned int min_x, unsigned int max_x, unsigned int min_y, unsigned int max_y) :
			ImageProcessorBase("IPCopy",
			                   "Copy an image with pixel type auto conversion",
			                   false,
			                   typeid(typename ImageTypeIn::pixel_type),
			                   typeid(typename ImageTypeOut::pixel_type)),
			min_x(min_x),
			max_x(max_x),
			min_y(min_y),
			max_y(max_y),
			work_on_region(true)
		{
		}

		/**
		 * The destructor.
		 */
		virtual ~IPCopy()
		{
		}


		virtual ImageBase_shptr run(ImageBase_shptr in)
		{
			assert(in != nullptr);

			std::shared_ptr<ImageTypeIn> img_in = std::dynamic_pointer_cast<ImageTypeIn>(in);

			std::shared_ptr<ImageTypeOut> img_out
			(work_on_region
				 ? new ImageTypeOut(max_x - min_x, max_y - min_y)
				 : new ImageTypeOut(in->get_width(), in->get_height()));

			assert(img_in != nullptr);
			assert(img_out != nullptr);

			std::cout << "Copy image." << std::endl;

			if (work_on_region == true)
			{
				extract_partial_image<ImageTypeOut,
				                      ImageTypeIn>(img_out, img_in,
				                                   min_x, max_x,
				                                   min_y, max_y);
			}
			else
			{
				copy_image<ImageTypeOut, ImageTypeIn>(img_out, img_in);
			}

			return img_out;
		}
	};
}

#endif
