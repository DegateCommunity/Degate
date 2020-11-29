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

#ifndef __IMAGEREADER_H__
#define __IMAGEREADER_H__

#include <list>
#include <memory>
#include <QImage>
#include <QImageReader>

#include "Core/Utils/DegateExceptions.h"
#include "Core/Utils/TypeTraits.h"
#include "Core/Image/StoragePolicies.h"
#include "Core/Image/PixelPolicies.h"
#include "Core/Utils/FileSystem.h"

namespace degate
{
    /**
     * Image reader class.
     */
    template <class ImageType>
    class ImageReader
    {
    private:

        QImageReader* image_reader = nullptr;
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
        ImageReader(std::string const& filename) :
            filename(filename),
            width(0),
            height(0)
        {
        }

        /**
         * The destructor.
         */
        virtual ~ImageReader()
        {
            if (image_reader != nullptr)
                delete image_reader;
        }

        /**
         * Get the filename.
         */
        std::string get_filename() const { return filename; }

        /**
         * Read the image meta data.
         *
         * @return The function returns true, if the image meta date were read. Otherwise false is returned.
         */
        bool read()
        {
            image_reader = new QImageReader(get_filename().c_str());

            // If the image is a multi-page/multi-res, we take the page with the biggest resolution.
            if (image_reader->imageCount() > 1)
            {
                QSize best_size{0, 0};
                int best_image_number = 0;
                for (int i = 0; i < image_reader->imageCount(); i++)
                {
                    if (best_size.width() < image_reader->size().width() || best_size.height() < image_reader->size().height())
                    {
                        best_size = image_reader->size();
                        best_image_number = image_reader->currentImageNumber();
                    }

                    image_reader->jumpToNextImage();
                }

                image_reader->jumpToImage(best_image_number);
            }

            QSize size = image_reader->size();
            if (!size.isValid())
            {
                debug(TM, "can't read size of %s\n", get_filename().c_str());
                return false;
            }

            width = static_cast<unsigned int>(size.width());
            height = static_cast<unsigned int>(size.height());

            return true;
        }

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
        bool get_image(std::shared_ptr<ImageType> img)
        {
            if (image_reader == nullptr) return false;
            if (img == nullptr) return false;

            if (img->get_width() < width || img->get_height() < height)
                return false;

            debug(TM, "Reading image with size: %d x %d", get_width(), get_height());

            QImage image;
            if (!image_reader->read(&image))
            {
                debug(TM, "can't read %s\n", get_filename().c_str());
                return false;
            }

            // Base conversion.
            //image.convertTo(QImage::Format_ARGB32);

            debug(TM, "Image read\n");

            // Fill image (can be long with big images).
            static QRgb rgb;
            for (unsigned int y = 0; y < img->get_height(); y++)
            {
                for (unsigned int x = 0; x < img->get_width(); x++)
                {
                    rgb = image.pixel(x, y);
                    img->set_pixel(x, y, MERGE_CHANNELS(qRed(rgb), qGreen(rgb), qBlue(rgb), qAlpha(rgb)));
                }
            }

            return true;
        }
    };
}

#endif
