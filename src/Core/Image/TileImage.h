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

#ifndef __TILEIMAGE_H__
#define __TILEIMAGE_H__

#include "Core/Utils/FileSystem.h"
#include "Core/Image/TileCache.h"
#include "Globals.h"
#include "PixelPolicies.h"
#include "StoragePolicies.h"

namespace degate
{
    /**
     * Storage policy for image objects that consists of tiles.
     *
     * This implementation uses a TileCache.
     */
    template <class PixelPolicy>
    class StoragePolicy_Tile : public StoragePolicy_Base<PixelPolicy>
    {
    public:

        typedef std::shared_ptr<MemoryMap<typename PixelPolicy::pixel_type>> MemoryMap_shptr;

    private:

        // Do the underlying file resources have to be cleaned up on destruction?
        const bool persistent;

        // Exponent and a bitmask to calculate tile numbers.
        const unsigned int tile_width_exp;
        const unsigned int offset_bitmask;

        // The place where we store the image data.
        const std::string path;

        // A helper class to load tiles.
        std::shared_ptr<TileCache<PixelPolicy>> tile_cache;

        unsigned int tiles_number;

        unsigned int width;
        unsigned int height;

    private:


        /**
         * Get the minimum width or height of an tile based image, that
         * it at least requested_size pixel width / height.
         * @param requested_size The minimum size.
         * @param tile_width_exp he width (and height) for image tiles. This
         *      value is specified as an exponent to the base 2. This means for
         *      example that if you want to use a width of 1024 pixel, you have
         *      to give a value of 10, because 2^10 is 1024.
         * @return
         */
        unsigned int calc_real_size(unsigned int requested_size,
                                    unsigned int tile_width_exp) const
        {
            // we can't use the get_tile_size() method here, because we use
            // this method during the base class constructor call.
            unsigned int tile_size = (1 << tile_width_exp);
            unsigned int remainder = requested_size % tile_size;
            if (remainder == 0) return requested_size;
            else return requested_size - remainder + tile_size;
        }

    public:

        /**
         * The constructor for a tile based image. The constructed image has at
         * least the size specified via the width and height parameter.
         * Because the image is splitted into equisized tiles the constructed
         * image might be larger than the requested size.
         * 
         * It supports two types of tile loading depending on the type of the image.
         * This is the main point of difference between Attached and Normal project modes.
         * @see TileCache.
         *
         * @param width The minimum width of the image.
         * @param height The minimum height of the image.
         * @param path Can be a path with all the tile image (store mode)
         *      or also directly an image path (attached mode).
         * @param persistent This boolean value indicates whether the image files
         *      are removed on object destruction.
         * @param tile_width_exp The width (and height) for image tiles. This
         *      value is specified as an exponent to the base 2. This means for
         *      example that if you want to use a width of 1024 pixel, you have
         *      to give a value of 10, because 2^10 is 1024.
         * @param loading_type : the loading type to use when loading a new tile.
         * @param notification_list : the list of workspace notification(s) to notify
         *      after a new loading finished. This is done only if async loading type.
         */
        StoragePolicy_Tile(unsigned int width,
                           unsigned int height,
                           std::string const& path,
                           bool persistent = false,
                           unsigned int scale = 1,
                           unsigned int tile_width_exp = 10,
                           TileLoadingType loading_type = TileLoadingType::Sync, 
                           const WorkspaceNotificationList& notification_list = {})
            : persistent(persistent),
              tile_width_exp(tile_width_exp),
              offset_bitmask((1 << tile_width_exp) - 1),
              path(path),
              tile_cache(std::make_shared<TileCache<PixelPolicy>>(path, tile_width_exp, scale, loading_type, notification_list)),
              width(width),
              height(height)
        {
            if (!file_exists(path))
                create_directory(path);

            double temp_tile_size = 1 << tile_width_exp;
            tiles_number = static_cast<unsigned>(ceil(static_cast<double>(width) / temp_tile_size)) * static_cast<unsigned>(ceil(static_cast<double>(height) / temp_tile_size));
        }

        /**
         * The destructor.
         */
        virtual ~StoragePolicy_Tile()
        {
            tile_cache->release_memory();

            if (persistent == false && is_directory(path))
                remove_directory(path);
        }

        inline unsigned int get_tiles_number() const
        {
            return tiles_number;
        }

        /**
         * Get the width / height of a single tile. The size is a power of two.
         */
        inline unsigned int get_tile_size() const
        {
            return (1 << tile_width_exp);
        }

        inline unsigned int get_tile_width_exp() const
        {
            return tile_width_exp;
        }

        /**
         * Either where images are stored or the image path.
         */
        std::string get_path() const { return path; }

        /**
         * Check if the image is persistent.
         */
        bool is_persistent() const { return persistent; }

        inline typename PixelPolicy::pixel_type get_pixel(unsigned int x, unsigned int y) const;

        inline void set_pixel(unsigned int x, unsigned int y, typename PixelPolicy::pixel_type new_val);

        /**
         * Copy the raw data from an image tile that has its upper left corner at x,y into a buffer.
         */
        void raw_copy(void* dst_buf, unsigned int src_x, unsigned int src_y) const
        {
            MemoryMap_shptr mem = tile_cache->get_tile(src_x, src_y);
            mem->raw_copy(dst_buf);
        }

        /**
         * Get raw data from an image tile that has its upper left corner at x,y into a buffer (can be null).
         * @return Returns data.
         */
        void* data(unsigned int src_x, unsigned int src_y)
        {
            return tile_cache->get_tile(src_x, src_y)->data();
        }

        /**
         * Cache the tile around a rectangle.
         *
         * @param min_x : The minimum x coordinate of the rectangle.
         * @param max_x : The maximum x coordinate of the rectangle.
         * @param min_y : The minimum y coordinate of the rectangle.
         * @param max_y : The maximum y coordinate of the rectangle.
         * @param radius : The radius around the rectangle where to cache tiles.
         *
         */
        void cache(unsigned int min_x,
                   unsigned int max_x,
                   unsigned int min_y,
                   unsigned int max_y,
                   unsigned int radius)
        {
            tile_cache->cache_around(min_x, max_x, min_y, max_y, width, height, radius);
        }

        /**
         * Release the cache memory.
         */
        void release_memory()
        {
            tile_cache->release_memory();
        }
    };

    template <class PixelPolicy>
    inline typename PixelPolicy::pixel_type
    StoragePolicy_Tile<PixelPolicy>::get_pixel(unsigned int x,
                                               unsigned int y) const
    {
        MemoryMap_shptr mem = tile_cache->get_tile(x, y);
        return mem->get(x & offset_bitmask, y & offset_bitmask);
    }

    template <class PixelPolicy>
    inline void
    StoragePolicy_Tile<PixelPolicy>::set_pixel(unsigned int x, unsigned int y,
                                               typename PixelPolicy::pixel_type new_val)
    {
        MemoryMap_shptr mem = tile_cache->get_tile(x, y);
        mem->set(x & offset_bitmask, y & offset_bitmask, new_val);
    }
}


#endif
