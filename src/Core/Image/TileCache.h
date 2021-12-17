/**
 * This file is part of the IC reverse engineering tool Degate.
 *
 * Copyright 2008, 2009, 2010 by Martin Schobert
 * Copyright 2021 Dorian Bachelot
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

#ifndef __TILECACHE_H__
#define __TILECACHE_H__

#include "Core/Image/TileImage.h"
#include "Core/Image/TileCacheBase.h"
#include "Core/Image/GlobalTileCache.h"

namespace degate
{
    /**
     * @class TileCache
     * @brief Tile cache base class (interface).
     */
    template<class PixelPolicy>
    class TileCache : public TileCacheBase
    {
        friend class GlobalTileCache<PixelPolicy>;

    public:

        /**
         * Create a new tile cache.
         * 
         * @param path : the path to the image (can be to a file or a path for example).
         * @param tile_width_exp : the width (and height) for image tiles. This
         *      value is specified as an exponent to the base 2. This means for
         *      example that if you want to use a width of 1024 pixel, you have
         *      to give a value of 10, because 2^10 is 1024.
         * @param scale : the scale to apply when loading the image (e.g. scale = 2
         *      will load the image with final size of width/2 and height/2). 
         *      @see ScalingManager.
         */
        inline TileCache(std::string path, unsigned int tile_width_exp, unsigned int scale)
            : path(std::move(path)),
              tile_width_exp(tile_width_exp),
              scale(scale)
        {
        }

        /**
         * Release memory on destroy.
         */
        inline ~TileCache()
        {
            release_memory();
        }

        /**
         * Cleanup the cache by removing the oldest entry.
         */
        inline void cleanup_cache() override
        {
            if (TileCache<PixelPolicy>::cache.size() == 0) return;

            // Initialize a clock to store the oldest
            struct timespec oldest_clock_val;
            GET_CLOCK(oldest_clock_val);

            auto oldest = TileCache<PixelPolicy>::cache.begin();

            // Search for oldest entry
            for (auto iter = TileCache<PixelPolicy>::cache.begin();
                 iter != TileCache<PixelPolicy>::cache.end(); ++iter)
            {
                struct timespec clock_val = (*iter).second.second;
                if (clock_val < oldest_clock_val)
                {
                    oldest_clock_val.tv_sec = clock_val.tv_sec;
                    oldest_clock_val.tv_nsec = clock_val.tv_nsec;
                    oldest = iter;
                }
            }

            assert(oldest != TileCache<PixelPolicy>::cache.end());

            // Release memory
            (*oldest).second.first.reset(); // explicit reset of smart pointer

            // Clean the cache entry
            TileCache<PixelPolicy>::cache.erase(oldest);

#ifdef TILECACHE_DEBUG
            debug(TM, "local cache: %d entries after remove\n", TileCache<PixelPolicy>::cache.size());
#endif

            // Update the global tile cache (release the virtual memory)
            GlobalTileCache<PixelPolicy>& gtc = GlobalTileCache<PixelPolicy>::get_instance();
            gtc.release_cache_memory(this, TileCache<PixelPolicy>::get_image_size());
        }

        /**
         * Release all the memory.
         */
        inline virtual void release_memory()
        {
            if (TileCache<PixelPolicy>::cache.size() > 0)
            {
                std::lock_guard<std::mutex> lock(mutex);

                // Release the global tile cache (by removing all the used virtual memory by this)
                GlobalTileCache<PixelPolicy>& gtc = GlobalTileCache<PixelPolicy>::get_instance();
                gtc.release_cache_memory(this, TileCache<PixelPolicy>::cache.size() * TileCache<PixelPolicy>::get_image_size());

                // Release the memory
                TileCache<PixelPolicy>::current_tile.reset();
                TileCache<PixelPolicy>::cache.clear();
            }
        }

        /**
         * Print this cache info.
         */
        inline void print() const override
        {
            for (typename TileCache<PixelPolicy>::cache_type::const_iterator iter = TileCache<PixelPolicy>::cache.begin();
                 iter != TileCache<PixelPolicy>::cache.end(); ++iter)
            {
                std::cout << "\t+ "
                          << TileCache<PixelPolicy>::path << "/"
                          << (*iter).first << " "
                          << (*iter).second.second.tv_sec
                          << "/"
                          << (*iter).second.second.tv_nsec
                          << std::endl;
            }
        }

        /**
         * Cache all tile around a rect (in a radius).
         * 
         * @param min_x : lower left x coordinate of the rect.
         * @param max_x : upper right x cooredinate of the rect.
         * @param min_y : lower left y coordinate of the rect.
         * @param max_y : upper right y coordinate of the rect.
         * @param max_size_x : max possible x coordinate for the rect.
         * @param max_size_y : max possible y coordinate for the rect.
         * @param radius : radius around the rect to cache (in number of tile unit).
         */
        inline virtual void cache_around(unsigned int min_x,
                                         unsigned int max_x,
                                         unsigned int min_y,
                                         unsigned int max_y,
                                         unsigned int max_size_x,
                                         unsigned int max_size_y,
                                         unsigned int radius)
        {
            unsigned int tile_num_min_x = min_x >> TileCache<PixelPolicy>::tile_width_exp;
            unsigned int tile_num_max_x = max_x >> TileCache<PixelPolicy>::tile_width_exp;
            unsigned int tile_num_min_y = min_y >> TileCache<PixelPolicy>::tile_width_exp;
            unsigned int tile_num_max_y = max_y >> TileCache<PixelPolicy>::tile_width_exp;

            unsigned int tile_num_max_size_x = max_size_x >> TileCache<PixelPolicy>::tile_width_exp;
            unsigned int tile_num_max_size_y = max_size_y >> TileCache<PixelPolicy>::tile_width_exp;

            unsigned int cache_min_x = radius > tile_num_min_x ? 0 : tile_num_min_x - radius;
            unsigned int cache_max_x = radius + tile_num_max_x > tile_num_max_size_x ? tile_num_max_size_x : tile_num_max_x + radius;
            unsigned int cache_min_y = radius > tile_num_min_y ? 0 : tile_num_min_y - radius;
            unsigned int cache_max_y = radius + tile_num_max_y > tile_num_max_size_y ? tile_num_max_size_y : tile_num_max_y + radius;

            if (static_cast<uint_fast64_t>(cache_max_x - cache_min_x) *
                        static_cast<uint_fast64_t>(cache_max_y - cache_min_y) *
                        static_cast<uint_fast64_t>(TileCache<PixelPolicy>::get_image_size()) *
                        static_cast<uint_fast64_t>(sizeof(typename PixelPolicy::pixel_type)) > GlobalTileCache<PixelPolicy>::get_instance().get_max_cache_memory())
            {
                debug(TM, "Cache too small to cache around");

                return;
            }

            for (unsigned int y = cache_min_y; y <= cache_max_y; y++)
            {
                for (unsigned int x = cache_min_x; x <= cache_max_x; x++)
                {
                    //if (y >= tile_num_min_y && y <= tile_num_max_y && x >= tile_num_min_x && x <= tile_num_max_x)
                    //continue;

                    load_tile(x, y);
                }
            }
        }

        /**
         * Get a tile. If the tile is not in the cache, the tile is loaded.
         *
         * @param x Absolut pixel coordinate.
         * @param y Absolut pixel coordinate.
         * @return Returns a shared pointer to a MemoryMap object.
         */
        std::shared_ptr<MemoryMap<typename PixelPolicy::pixel_type>>
        inline get_tile(unsigned int x, unsigned int y)
        {
            unsigned int tile_num_x = x >> tile_width_exp;
            unsigned int tile_num_y = y >> tile_width_exp;

            // This is an optimisation, but don't fit well for attached mode
            // TODO: When notifier for new loaded tile (attached mode) readd this
            //if (!(current_tile != nullptr && tile_num_x == curr_tile_num_x && tile_num_y == curr_tile_num_y))
            //{
                load_tile(tile_num_x, tile_num_y, true);
            //}

            return current_tile;
        }

        /**
         * Load a new tile and update the cache.
         * 
         * @param x : the x index of the tile (not the real coordinate).
         * @param y : the y index of the tile (not the real coordinate).
         * @param update_current : if true, will update the current_tile pointer, otherwise not.
         */
        virtual void load_tile(unsigned int x, unsigned int y, bool update_current = false) = 0;

    protected:

        /**
         * Get image size in bytes.
         */
        uint_fast64_t get_image_size() const
        {
            return sizeof(typename PixelPolicy::pixel_type) * (uint_fast64_t(1) << tile_width_exp) * (uint_fast64_t(1) << tile_width_exp);
        }

    protected:
        const std::string path;
        const unsigned int tile_width_exp;

        // Cache types.
        typedef std::shared_ptr<MemoryMap<typename PixelPolicy::pixel_type>> MemoryMap_shptr;
        typedef std::map<std::string, // filename
                         std::pair<MemoryMap_shptr, struct timespec>>
                cache_type;

        cache_type cache;

        // Used for caching the working tile.
        mutable MemoryMap_shptr current_tile;
        mutable unsigned curr_tile_num_x = 0;
        mutable unsigned curr_tile_num_y = 0;

        unsigned int scale;

        std::mutex mutex;
    };
} // namespace degate

#endif //__TILECACHE_H__
