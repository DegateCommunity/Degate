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

#ifndef __IMAGE_H__
#define __IMAGE_H__


#ifdef WORDS_BIGENDIAN
#define MASK_R(r) (r >> 24)
#define MASK_G(g) ((g >> 16) & 0xff)
#define MASK_B(b) ((b >> 8) & 0xff)
#define MASK_A(a) (a & 0xff)
#define MERGE_CHANNELS(r,g,b,a) ((r << 24) | (g <<16) | (b << 8) | a)
#else
#define MASK_R(r) (r & 0xff)
#define MASK_G(g) ((g >> 8) & 0xff)
#define MASK_B(b) ((b >> 16) & 0xff)
#define MASK_A(a) (a >> 24)
#define MERGE_CHANNELS(r,g,b,a) ((a << 24) | (b <<16) | (g << 8) | r)
#endif

// 75 * 255 + 147 * 255 + 35 * 255 = 65535
#define RGBA_TO_GS_BY_PTR(pix_ptr) RGBA_TO_GS_BY_VAL(*pix_ptr)
#define RGBA_TO_GS_BY_VAL(pix) (((75 * MASK_R(pix)) + (147 * MASK_G(pix)) + (35 * MASK_B(pix))) >> 8)


#include "Globals.h"
#include "PixelPolicies.h"
#include "StoragePolicies.h"
#include "TileImage.h"
#include "Core/Utils/FileSystem.h"
#include "Core/Configuration.h"
#include "Core/Utils/TypeTraits.h"
#include "TypeConstraints.h"
#include "Core/Image/Manipulation/ImageManipulation.h"


namespace degate
{
    class ImageBase
    {
    private:

        BoundingBox bounding_box;

    public:

        ImageBase(unsigned int width, unsigned int height) :
            bounding_box(static_cast<float>(width), static_cast<float>(height))
        {
        }

        virtual ~ImageBase()
        {
        }

        /**
         * Get the width of an image.
         */
        inline unsigned int get_width() const
        {
            return static_cast<unsigned int>(bounding_box.get_width());
        }

        /**
         * Get the height of an image.
         */
        inline unsigned int get_height() const
        {
            return static_cast<unsigned int>(bounding_box.get_height());
        }

        /**
         * Get the bounding box that represents the image.
         */
        inline BoundingBox const& get_bounding_box() const
        {
            return bounding_box;
        }
    };


    /**
     * The generic templated class for image objects.
     *
     * This class can be used with StoragePolicy_Memory and
     * StoragePolicy_TempFile.
     *
     * Every image constructor should have a two parameter
     * version. This is necessary for image processing plugins,
     * because plugins have to create temporary images. The two
     * parameter constructor should then make reasonable default
     * assumtions about storage. Temporary images are really always
     * temporary. This means that we can create empty images in
     * degate's temp directory. The constructor should only get
     * the width and height of an image.
     */
    template <class PixelPolicy,
              template <class _PixelPolicy> class StoragePolicy>
    class Image : public ImageBase,
                  public StoragePolicy<PixelPolicy>
    {
    public:

        /**
         * The constructor.
         */
        Image(unsigned int width, unsigned int height) :
            ImageBase(width, height),
            StoragePolicy<PixelPolicy>(width, height)
        {
        }

        /**
         * The destructor.
         */
        virtual ~Image()
        {
        }


        /**
         * Get pixel with conversion.
         */
        template <typename PixelTypeDst>
        inline PixelTypeDst get_pixel_as(unsigned int x, unsigned int y)
        {
            return convert_pixel<PixelTypeDst, typename PixelPolicy::pixel_type>(this->get_pixel(x, y));
        }

        /**
         * Set pixel with conversion.
         */
        template <typename PixelTypeSrc>
        inline void set_pixel_as(unsigned int x, unsigned int y, PixelTypeSrc p)
        {
            this->set_pixel(x, y,
                            convert_pixel<typename PixelPolicy::pixel_type, PixelTypeSrc>(p));
        }
    };

    typedef std::shared_ptr<ImageBase> ImageBase_shptr;

    /**
     * Partial template specialization for the storage policy StoragePolicy_PersistentFile.
     */
    template <class PixelPolicy>
    class Image<PixelPolicy, StoragePolicy_PersistentFile> :
        public ImageBase,
        public StoragePolicy_PersistentFile<PixelPolicy>
    {
    public:

        Image(unsigned int width,
              unsigned int height,
              std::string const& filename) :
            ImageBase(width, height),
            StoragePolicy_PersistentFile<PixelPolicy>(width,
                                                      height,
                                                      filename)
        {
        }

        virtual ~Image()
        {
        }


        /**
         * Get pixel with conversion.
         */
        template <typename PixelTypeDst>
        inline PixelTypeDst get_pixel_as(unsigned int x, unsigned int y)
        {
            return convert_pixel<PixelTypeDst, typename PixelPolicy::pixel_type>(this->get_pixel(x, y));
        }

        /**
         * Set pixel with conversion.
         */
        template <typename PixelTypeSrc>
        inline void set_pixel_as(unsigned int x, unsigned int y, PixelTypeSrc p)
        {
            this->set_pixel(x, y,
                            convert_pixel<typename PixelPolicy::pixel_type, PixelTypeSrc>(p));
        }
    };


    /**
     * Partial template specialization for the storage policy StoragePolicy_Tile.
     */
    template <class PixelPolicy>
    class Image<PixelPolicy, StoragePolicy_Tile> :
        public ImageBase,
        public StoragePolicy_Tile<PixelPolicy>
    {
    public:

        /**
         * Constructor for temporary virtual images.
         * 
         * It supports two types of tile loading depending on the type of the image.
         * This is the main point of difference between Attached and Normal project modes.
         * @see TileCache.
         * 
         * @param width : the width of the image.
         * @param height : the height of the image.
         * @param scale : the scale to apply when loading the image (e.g. scale = 2
         *      will load the image with final size of width/2 and height/2). 
         *      @see ScalingManager.
         * @param tile_width_exp : the width (and height) for image tiles. This
         *      value is specified as an exponent to the base 2. This means for
         *      example that if you want to use a width of 1024 pixel, you have
         *      to give a value of 10, because 2^10 is 1024.
         * @param loading_type : the loading type to use when loading a new tile.
         * @param notification_list : the list of workspace notification(s) to notify
         *      after a new loading finished. This is done only if async loading type.
         */
        Image(unsigned int width,
              unsigned int height,
              unsigned int scale = 1,
              unsigned int tile_width_exp = 10,
              TileLoadingType loading_type = TileLoadingType::Sync, 
              const WorkspaceNotificationList& notification_list = {}) :
            ImageBase(width, height),
            StoragePolicy_Tile<PixelPolicy>(width, height,
                                            create_temp_directory(),
                                            false,
                                            scale,
                                            tile_width_exp,
                                            loading_type,
                                            notification_list)
        {
        }

        /**
         * Constructor for persistent virtual images.
         * 
         * It supports two types of tile loading depending on the type of the image.
         * This is the main point of difference between Attached and Normal project modes.
         * @see TileCache.
         * 
         * @param width : the width of the image.
         * @param height : the height of the image.
         * @param path : the path of the image, can be a directory (store mode) or
         *      a file path (attached mode). @see TileCache.
         * @param persistent : if true, will persist on the disk.
         * @param scale : the scale to apply when loading the image (e.g. scale = 2
         *      will load the image with final size of width/2 and height/2). 
         *      @see ScalingManager.
         * @param tile_width_exp : the width (and height) for image tiles. This
         *      value is specified as an exponent to the base 2. This means for
         *      example that if you want to use a width of 1024 pixel, you have
         *      to give a value of 10, because 2^10 is 1024.
         * @param loading_type : the loading type to use when loading a new tile.
         * @param notification_list : the list of workspace notification(s) to notify
         *      after a new loading finished. This is done only if async loading type.
         */
        Image(unsigned int width,
              unsigned int height,
              std::string const& path,
              bool persistent = true,
              unsigned int scale = 1,
              unsigned int tile_width_exp = 10,
              TileLoadingType loading_type = TileLoadingType::Sync,
              const WorkspaceNotificationList& notification_list = {}) :
            ImageBase(width, height),
            StoragePolicy_Tile<PixelPolicy>(width, height,
                                            path,
                                            persistent,
                                            scale,
                                            tile_width_exp,
                                            loading_type,
                                            notification_list)
        {
        }

        /**
         * The dtor.
         */
        virtual ~Image()
        {
        }

        /**
         * Get pixel with conversion.
         */
        template <typename PixelTypeDst>
        inline PixelTypeDst get_pixel_as(unsigned int x, unsigned int y)
        {
            return convert_pixel<PixelTypeDst, typename PixelPolicy::pixel_type>(this->get_pixel(x, y));
        }

        /**
         * Set pixel with conversion.
         */
        template <typename PixelTypeSrc>
        inline void set_pixel_as(unsigned int x, unsigned int y, PixelTypeSrc p)
        {
            this->set_pixel(x, y,
                            convert_pixel<typename PixelPolicy::pixel_type, PixelTypeSrc>(p));
        }
    };


    /**
     * Typedefs for common types of virtual images.
     */

    typedef Image<PixelPolicy_RGBA, StoragePolicy_Tile> TileImage_RGBA;
    typedef Image<PixelPolicy_GS_DOUBLE, StoragePolicy_Tile> TileImage_GS_DOUBLE;
    typedef Image<PixelPolicy_GS_BYTE, StoragePolicy_Tile> TileImage_GS_BYTE;

    typedef std::shared_ptr<TileImage_RGBA> TileImage_RGBA_shptr;
    typedef std::shared_ptr<TileImage_GS_DOUBLE> TileImage_GS_DOUBLE_shptr;
    typedef std::shared_ptr<TileImage_GS_BYTE> TileImage_GS_BYTE_shptr;


    typedef Image<PixelPolicy_RGBA, StoragePolicy_Tile> BackgroundImage;
    typedef std::shared_ptr<BackgroundImage> BackgroundImage_shptr;


    typedef Image<PixelPolicy_RGBA, StoragePolicy_TempFile> TempImage_RGBA;
    typedef Image<PixelPolicy_GS_DOUBLE, StoragePolicy_TempFile> TempImage_GS_DOUBLE;
    typedef Image<PixelPolicy_GS_BYTE, StoragePolicy_TempFile> TempImage_GS_BYTE;

    typedef std::shared_ptr<TempImage_RGBA> TempImage_RGBA_shptr;
    typedef std::shared_ptr<TempImage_GS_DOUBLE> TempImage_GS_DOUBLE_shptr;
    typedef std::shared_ptr<TempImage_GS_BYTE> TempImage_GS_BYTE_shptr;


    typedef Image<PixelPolicy_RGBA, StoragePolicy_PersistentFile> PersistentImage_RGBA;
    typedef std::shared_ptr<PersistentImage_RGBA> PersistentImage_RGBA_shptr;


    typedef Image<PixelPolicy_RGBA, StoragePolicy_Memory> MemoryImage;
    typedef std::shared_ptr<MemoryImage> MemoryImage_shptr;


    typedef Image<PixelPolicy_GS_BYTE, StoragePolicy_Memory> MemoryImage_GS_BYTE;
    typedef Image<PixelPolicy_GS_DOUBLE, StoragePolicy_Memory> MemoryImage_GS_DOUBLE;
    typedef Image<PixelPolicy_RGBA, StoragePolicy_Memory> MemoryImage_RGBA;
    typedef std::shared_ptr<MemoryImage_GS_BYTE> MemoryImage_GS_BYTE_shptr;
    typedef std::shared_ptr<MemoryImage_GS_DOUBLE> MemoryImage_GS_DOUBLE_shptr;
    typedef std::shared_ptr<MemoryImage_RGBA> MemoryImage_shptr;

    typedef MemoryImage RendererImage;
    typedef MemoryImage_shptr RendererImage_shptr;

    // typedef for the image format in which we store template images within memory
    typedef MemoryImage GateTemplateImage;
    typedef MemoryImage_shptr GateTemplateImage_shptr;
}

#endif
