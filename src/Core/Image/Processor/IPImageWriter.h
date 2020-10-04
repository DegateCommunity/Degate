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

#ifndef __IPIMAGEWRITER_H__
#define __IPIMAGEWRITER_H__

#include <string>
#include "Core/Image/Processor/ImageProcessorBase.h"
#include "Core/Image/ImageHelper.h"

namespace degate
{
    /**
     * Processor: Write an image to file.
     */
    template <typename ImageType>
    class IPImageWriter : public ImageProcessorBase
    {
    private:
        std::string filename;

    public:

        /**
         * The constructor.
         */
        IPImageWriter(std::string filename) :
            ImageProcessorBase("IPImageWriter",
                               "Write an image.",
                               false,
                               typeid(typename ImageType::pixel_type),
                               typeid(typename ImageType::pixel_type)),
            filename(filename)
        {
        }

        /**
         * The destructor.
         */
        virtual ~IPImageWriter()
        {
        }

        virtual ImageBase_shptr run(ImageBase_shptr in)
        {
            assert(in != nullptr);

            std::shared_ptr<ImageType> img_in = std::dynamic_pointer_cast<ImageType>(in);

            std::shared_ptr<ImageType> img_out(new ImageType(in->get_width(), in->get_height()));

            assert(img_in != nullptr);
            assert(img_out != nullptr);

            std::cout << "writing file: " << filename << std::endl;

            normalize<ImageType, ImageType>(img_out, img_in, 0, 255);

            save_image<ImageType>(filename, img_out);

            return img_in;
        }
    };
}

#endif
