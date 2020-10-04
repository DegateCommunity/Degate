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

#ifndef __JPEGREADER_H__
#define __JPEGREADER_H__

#include <list>
#include <memory>
#include <QImageReader>

#include "StoragePolicies.h"
#include "Core/Image/ImageReaderBase.h"

namespace degate
{
    /**
     * The JPEGReader parses jpeg images.
     */
    template <class ImageType>
    class JPEGReader : public ImageReaderBase<ImageType>
    {
    private:

        QImage* image = nullptr;
        int depth;

    public:

        using ImageReaderBase<ImageType>::get_filename;
        using ImageReaderBase<ImageType>::set_width;
        using ImageReaderBase<ImageType>::set_height;
        using ImageReaderBase<ImageType>::get_width;
        using ImageReaderBase<ImageType>::get_height;

        JPEGReader(std::string const& filename) :
            ImageReaderBase<ImageType>(filename),
            image(nullptr)
        {
        }

        ~JPEGReader()
        {
            if (image != nullptr) delete image;
        }

        bool read();

        bool get_image(std::shared_ptr<ImageType>);
    };

    template <class ImageType>
    bool JPEGReader<ImageType>::read()
    {
        QImageReader reader(get_filename().c_str());
        QSize size = reader.size();
        if (!size.isValid())
        {
            debug(TM, "can't read size of %s\n", get_filename().c_str());
            return false;
        }

        set_width(size.width());
        set_height(size.height());

        assert(reader.imageFormat() == QImage::Format_ARGB32 || reader.imageFormat() == QImage::Format_RGB32);

        image = new QImage(size.width(), size.height(), reader.imageFormat());

        debug(TM, "Reading image with size: %d x %d", get_width(), get_height());

        if (!reader.read(image))
        {
            debug(TM, "can't read %s\n", get_filename().c_str());
            return false;
        }
        depth = image->depth();

        debug(TM, "Image read\n");

        return true;
    }

    template <class ImageType>
    bool JPEGReader<ImageType>::get_image(std::shared_ptr<ImageType> img)
    {
        if (img == nullptr) return false;
        if (image == nullptr) return false;

        for (unsigned int y = 0; y < get_height(); y++)
        {
            for (unsigned int x = 0; x < get_width(); x++)
            {
                QRgb rgb = image->pixel(x, y);
                img->set_pixel(x, y, MERGE_CHANNELS(qRed(rgb), qGreen(rgb), qBlue(rgb), 0xff));
            }
        }

        return true;
    }
}

#endif
