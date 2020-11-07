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

#ifndef __IMAGEHELPER_H__
#define __IMAGEHELPER_H__

#include "Core/Image/ImageWriterBase.h"
#include "Core/Image/TIFFWriter.h"
#include "Core/Image/PixelPolicies.h"
#include "Core/Image/StoragePolicies.h"
#include "Core/Image/Image.h"
#include "Core/Image/ImageReader.h"

#include <set>
#include <boost/foreach.hpp>
#include <QImageReader>

namespace degate
{
    /**
     * Load an image in a common image format, such as tiff.
     * @exception InvalidPathException Thrown if, path does not exists.
     * @exception DegateRuntimeException This exception is thrown, if there is
     *   no matching image importer or if the import failed.
     */
    template <typename ImageType>
    std::shared_ptr<ImageType> load_image(std::string const& path)
    {
        if (!file_exists(path))
        {
            boost::format fmter("Error in load_image(): file %1% does not exist.");
            fmter % path;
            throw InvalidPathException(fmter.str());
        }
        else
        {
            boost::format fmter("Error in load_image(): The image file %1% cannot be loaded.");
            fmter % path;

            ImageReader<ImageType> reader(path);
            if (!reader.read())
            {
                debug(TM, "Failed to read meta data of image.");
                throw DegateRuntimeException(fmter.str());
            }

            // Create output image.
            auto output_image = std::make_shared<ImageType>(reader.get_width(), reader.get_height());

            if (!reader.get_image(output_image))
            {
                debug(TM, "Failed to read image.");
                throw DegateRuntimeException(fmter.str());
            }

            return output_image;
        }
    }

    /**
     * Load an image in a common image format, such as tiff, into an existing degate image.
     * @exception InvalidPointerException This exception is thrown, if parameter \p img represents an invalid pointer.
     */
    template <typename ImageType>
    void load_image(std::string const& path, std::shared_ptr<ImageType> img)
    {
        if (img == nullptr) throw InvalidPointerException("invalid image pointer");
        std::shared_ptr<ImageType> i = load_image<ImageType>(path);
        copy_image<ImageType, ImageType>(img, i);
    }


    /**
     * Store an image in a common file format.
     * Only the tiff file format is supported.
     * @todo We should use a factory for writer objects.
     * @exception DegateRuntimeException This exception is thrown, if the writer failed to save the image.
     * @exception InvalidPointerException This exception is thrown, if parameter \p img represents an invalid pointer.
     */
    template <typename ImageType>
    void save_image(std::string const& path, std::shared_ptr<ImageType> img)
    {
        if (img == nullptr) throw InvalidPointerException("invalid image pointer");
        TIFFWriter<ImageType> tiff_writer(img->get_width(),
                                          img->get_height(), path);
        if (tiff_writer.write_image(img) != true)
        {
            boost::format fmter("Error in save_image(): Cannot write file %1%.");
            fmter % path;
            throw DegateRuntimeException(fmter.str());
        }
    }

    /**
     * Save a part of an image.
     * @exception InvalidPointerException This exception is thrown, if parameter \p img represents an invalid pointer.
     */
    template <typename ImageType>
    void save_part_of_image(std::string const& path,
                            std::shared_ptr<ImageType> img,
                            BoundingBox const& bounding_box)
    {
        if (img == nullptr) throw InvalidPointerException("invalid image pointer");
        std::shared_ptr<ImageType> part(new ImageType(bounding_box.get_width(),
                                                      bounding_box.get_height()));

        extract_partial_image(part, img, bounding_box);
        save_image(path, part);
    }

    /**
     * Normalize a single channel image and store it in a common file format.
     * Only the tiff file format is supported.
     * @exception InvalidPointerException This exception is thrown, if parameter \p img represents an invalid pointer.
     */
    template <typename ImageType>
    void save_normalized_image(std::string const& path, std::shared_ptr<ImageType> img)
    {
        if (img == nullptr) throw InvalidPointerException("invalid image pointer");
        std::shared_ptr<ImageType> normalized_img(new ImageType(img->get_width(),
                                                                img->get_height()));

        normalize<ImageType, ImageType>(normalized_img, img, 0, 255);

        save_image<ImageType>(path, normalized_img);
    }


    /**
     * Merge a set of images by averaging them.
     * @exception DegateRuntimeException This exception is thrown, if images differ in size.
     * @return If image collection \p images contains elements, this function returns a
     *   valid merged image. If the collection is empty, a nullptr pointer is returned.
     */
    template <typename ImageType>
    std::shared_ptr<ImageType> merge_images(std::list<std::shared_ptr<ImageType>> const& images)
    {
        std::shared_ptr<ImageType> new_img;
        if (images.empty()) return new_img;

        const std::shared_ptr<ImageType> img = images.front();

        unsigned int w = img->get_width(), h = img->get_height();
        std::vector<double> i_tmp(4 * w * h);

        BOOST_FOREACH(const std::shared_ptr<ImageType> i, images)
        {
            // verify that all images have the same dimensions
            if (w != i->get_width() || h != i->get_height())
                throw DegateRuntimeException("merge_images() failed, because images differ in size.");

            for (unsigned int y = 0; y < h; y++)
                for (unsigned int x = 0; x < w; x++)
                {
                    color_t pix = i->template get_pixel_as<color_t>(x, y);
                    unsigned int offs = 4 * (y * w + x);
                    i_tmp[offs] += MASK_R(pix);
                    i_tmp[offs + 1] += MASK_G(pix);
                    i_tmp[offs + 2] += MASK_B(pix);
                    i_tmp[offs + 3] += MASK_A(pix);
                }
        }

        const double elems = images.size();

        new_img = std::shared_ptr<ImageType>(new GateTemplateImage(w, h));
        assert(new_img != nullptr);

        for (unsigned int y = 0; y < h; y++)
            for (unsigned int x = 0; x < w; x++)
            {
                unsigned int offs = 4 * (y * w + x);
                color_t pix = MERGE_CHANNELS(lround(i_tmp[offs] / elems),
                                             lround(i_tmp[offs+1] / elems),
                                             lround(i_tmp[offs+2] / elems),
                                             lround(i_tmp[offs+3] / elems));
                new_img->set_pixel(x, y, pix);
            }

        return new_img;
    }
}

#endif
