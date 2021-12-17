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

#ifndef __TILECACHESTORE_H__
#define __TILECACHESTORE_H__

#include "Core/Utils/MemoryMap.h"
#include "Core/Utils/FileSystem.h"
#include "Core/Configuration.h"
#include "Core/Image/TileCache.h"

#include <string>
#include <map>
#include <memory>
#include <ctime>
#include <utility> // for make_pair
#include <iostream>
#include <iomanip>
#include <mutex>

namespace degate
{

    /**
     * The TileCacheStore class handles caching of image tiles.
     *
     * The implementation keeps track how old the cached tile is.
     * If new tiles become loaded, old tiles are removed from the
     * cache. You can control the numer of cached tiles via the
     * constructor parameter \p _min_cache_tiles. The memory
     * requirement is around
     * \p _min_cache_tiles*sizeof(PixelPolicy::pixel_type)*(2^_tile_width_exp)^2 ,
     * where \p sizeof(PixelPolicy::pixel_type) is the size of a pixel.
     */
    template <class PixelPolicy>
    class TileCacheStore : public TileCache<PixelPolicy>
    {

    public:

        /**
         * Create a TileCacheStore object.
         * 
         * @param path : the path of the path where all the tiles are for a TileImage.
         * @param tile_width_exp : the width (and height) for image tiles. This
         *      value is specified as an exponent to the base 2. This means for
         *      example that if you want to use a width of 1024 pixel, you have
         *      to give a value of 10, because 2^10 is 1024.
         * @param scale : the scale to apply when loading the image (e.g. scale = 2
         *      will load the image with final size of width/2 and height/2). 
         *      @see ScalingManager. Not used here since images are already imported/scaled.
         */
        TileCacheStore(std::string path, unsigned int tile_width_exp, unsigned int scale = 1)
            : TileCache<PixelPolicy>(path, tile_width_exp, scale)
        {
        }

        /**
         * Load a new tile and update the cache.
         * 
         * @param x : the x index of the tile (not the real coordinate).
         * @param y : the y index of the tile (not the real coordinate).
         * @param update_current : if true, will update the current_tile pointer, otherwise not.
         */
        inline void load_tile(unsigned int x, unsigned int y, bool update_current = false)
        {
            std::lock_guard<std::mutex> lock(TileCache<PixelPolicy>::mutex);

            // create a file name from tile number
            char filename[PATH_MAX];
            snprintf(filename, sizeof(filename), "%d_%d.dat", x, y);

            // if filename/object is not in cache, load the tile
            typename TileCache<PixelPolicy>::cache_type::const_iterator iter = TileCache<PixelPolicy>::cache.find(filename);

            if (iter == TileCache<PixelPolicy>::cache.end())
            {
                GlobalTileCache<PixelPolicy>& gtc = GlobalTileCache<PixelPolicy>::get_instance();

                bool ok = gtc.request_cache_memory(this, TileCache<PixelPolicy>::get_image_size());
                assert(ok == true);
                struct timespec now{};
                GET_CLOCK(now);

                TileCache<PixelPolicy>::cache[filename] = std::make_pair(load(filename), now);

                //debug(TM, "Cache size : %d/%d", gtc.get_allocated_memory(), gtc.get_max_cache_memory());

                #ifdef TILECACHE_DEBUG
                gtc.print_table();
                #endif
            }

            // Get current time
            struct timespec now{};
            GET_CLOCK(now);

            // Update entry
            auto entry = TileCache<PixelPolicy>::cache[filename];
            entry.second = now;

            if (update_current)
            {
                TileCache<PixelPolicy>::current_tile = entry.first;
                TileCache<PixelPolicy>::curr_tile_num_x = x;
                TileCache<PixelPolicy>::curr_tile_num_y = y;
            }
        }


    private:

        /**
         * Load a tile from an image file.
         * @param filename Just the name of the file to load. The filename is
         *     relative to the \p path.
         */
        std::shared_ptr<MemoryMap<typename PixelPolicy::pixel_type>>
        load(std::string const& filename) const
        {
            std::shared_ptr<MemoryMap<typename PixelPolicy::pixel_type>> mem(
                    new MemoryMap<typename PixelPolicy::pixel_type>(uint_fast64_t(1) << TileCache<PixelPolicy>::tile_width_exp,
                                                                    uint_fast64_t(1) << TileCache<PixelPolicy>::tile_width_exp,
                                                                    MAP_STORAGE_TYPE_PERSISTENT_FILE,
                                                                    join_pathes(TileCache<PixelPolicy>::path, filename)));

            return mem;
        }
    }; // end of class TileCacheStore
}

#endif
