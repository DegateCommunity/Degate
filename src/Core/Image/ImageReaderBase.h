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

#ifndef __IMAGEREADERBASE_H__
#define __IMAGEREADERBASE_H__

#include <list>
#include <memory>
#include "Core/Utils/DegateExceptions.h"

#include "Core/Utils/TypeTraits.h"
#include "Core/Image/StoragePolicies.h"
#include "Core/Image/PixelPolicies.h"
#include "Core/Utils/FileSystem.h"

namespace degate
{
    /**
     * The base class for image readers.
     */
    template <class ImageType>
    class ImageReaderBase
    {
    private:

        std::string filename;
        unsigned int width, height;

    protected:

        /**
         * Set the width of the image.
         * This method should be called by derived image readers to set
         * the image size.
         */
        void set_width(unsigned int width) { this->width = width; }


        /**
         * Set the height of the image.
         * This method should be called by derived image readers to set
         * the image size.
         */
        void set_height(unsigned int height) { this->height = height; }


    public:

        /**
         * Constructor.
         */
        ImageReaderBase(std::string const& filename) :
            filename(filename),
            width(0),
            height(0)
        {
        }

        /**
         * The destructor.
         */
        virtual ~ImageReaderBase()
        {
        }

        /**
         * Get the filename.
         */
        std::string get_filename() const { return filename; }

        /**
         * Read the image ot at least its meta data.
         *
         * If you derive from class ImageReaderBase, you can implement a full image read
         * operation here. But it is also possible to implement just the read of meta data,
         * such as width and height.
         *
         * @return The function returns true, if the image file was read. Else false
         *      is returned. If read() was successful you can
         */
        virtual bool read() = 0;

        /**
         * Get the image width.
         * You have to call read() before.
         * @see read()
         */
        unsigned int get_width() const { return width; }

        /**
         * Get the image height.
         * You have to call read() before.
         * @see read()
         */
        unsigned int get_height() const { return height; }

        /**
         * Read the file content into image.
         */
        virtual bool get_image(std::shared_ptr<ImageType> img) = 0;
    };
}

#endif
