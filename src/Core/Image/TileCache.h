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

#ifndef __TILECACHE_H__
#define __TILECACHE_H__

#include "Core/Utils/MemoryMap.h"
#include "Core/Utils/FileSystem.h"
#include "Core/Configuration.h"

#include <string>
#include <map>
#include <memory>
#include <ctime>
#include <utility> // for make_pair
#include <iostream>
#include <iomanip>

#include <chrono>

#include <mutex>

/**
 * Minimum size (in Mb) of the cache.
 */
#define MINIMUM_CACHE_SIZE uint_fast64_t(256)

static void get_clock(struct timespec* ts)
{
    assert(ts != nullptr);

    std::chrono::steady_clock::time_point tp = std::chrono::steady_clock::now();

    auto secs = std::chrono::time_point_cast<std::chrono::seconds>(tp);
    auto ns = std::chrono::time_point_cast<std::chrono::nanoseconds>(tp) - std::chrono::time_point_cast<std::chrono::
        nanoseconds>(secs);

    ts->tv_sec = secs.time_since_epoch().count();
    ts->tv_nsec = static_cast<long>(ns.count());
}

#define GET_CLOCK(dst_variable) \
      get_clock(&dst_variable);


// #define TILECACHE_DEBUG

/**
 * Overloaded comparison operator for timespec-structs.
 * @return Returns true, if \p a is completely before \p b. Else
 *   false is returned.
 */
static bool operator<(struct timespec const& a, struct timespec const& b)
{
    if (a.tv_sec < b.tv_sec) return true;
    else if (a.tv_sec == b.tv_sec && a.tv_nsec < b.tv_nsec) return true;
    else return false;
}

namespace degate
{
    class TileCacheBase
    {
    public:
        virtual void cleanup_cache() = 0;
        virtual void print() const = 0;
    };

    class GlobalTileCache : public SingletonBase<GlobalTileCache>
    {
        friend class SingletonBase<GlobalTileCache>;

    private:

        uint_fast64_t max_cache_memory;
        uint_fast64_t allocated_memory;

        typedef std::pair<struct timespec, uint_fast64_t> cache_entry_t;
        typedef std::map<TileCacheBase *, cache_entry_t> cache_t;

        cache_t cache;

    private:

        GlobalTileCache() : allocated_memory(0)
        {
            if (Configuration::get_max_tile_cache_size() < MINIMUM_CACHE_SIZE)
                max_cache_memory = MINIMUM_CACHE_SIZE * uint_fast64_t(1024) * uint_fast64_t(1024);
            else
                max_cache_memory = Configuration::get_max_tile_cache_size() * uint_fast64_t(1024) * uint_fast64_t(1024);
        }

        void remove_oldest()
        {
            struct timespec now;
            GET_CLOCK(now);

            TileCacheBase* oldest = nullptr;

            for (cache_t::iterator iter = cache.begin(); iter != cache.end(); ++iter)
            {
                cache_entry_t& entry = iter->second;
                if (entry.first < now)
                {
                    now.tv_sec = entry.first.tv_sec;
                    now.tv_nsec = entry.first.tv_nsec;
                    oldest = iter->first;
                }
            }

            if (oldest)
            {
#ifdef TILECACHE_DEBUG
    debug(TM, "Will call cleanup on %p", oldest);
#endif
                oldest->cleanup_cache();
            }
            else
            {
#ifdef TILECACHE_DEBUG
    debug(TM, "there is nothing to free.");
    print_table();
#endif
            }
        }

    public:

        void print_table() const
        {
            std::cout << "Global Image Tile Cache:\n"
                << "Used memory : " << allocated_memory << " bytes\n"
                << "Max memory  : " << max_cache_memory << " bytes\n\n"
                << "Holder           | Last access (sec,nsec)    | Amount of memory\n"
                << "-----------------+---------------------------+------------------------------------\n";

            for (cache_t::const_iterator iter = cache.begin(); iter != cache.end(); ++iter)
            {
                cache_entry_t const& entry = iter->second;
                std::cout << std::setw(16) << std::hex << static_cast<void*>(iter->first);
                std::cout << " | ";
                std::cout << std::setw(12) << entry.first.tv_sec;
                std::cout << ".";
                std::cout << std::setw(12) << entry.first.tv_nsec;
                std::cout << " | ";
                std::cout << entry.second / (1024 * 1024) << " M (" << entry.second << " bytes)\n";
                iter->first->print();
            }
            std::cout << "\n";
        }

        bool request_cache_memory(TileCacheBase* requestor, uint_fast64_t amount)
        {
#ifdef TILECACHE_DEBUG
      debug(TM, "Local cache %p requests %d bytes.", requestor, amount);
#endif
            while (allocated_memory + amount > max_cache_memory)
            {
#ifdef TILECACHE_DEBUG
    debug(TM, "Try to free memory");
#endif
                remove_oldest();
            }

            if (allocated_memory + amount <= max_cache_memory)
            {
                struct timespec now;
                GET_CLOCK(now);

                cache_t::iterator found = cache.find(requestor);
                if (found == cache.end())
                {
                    cache[requestor] = std::make_pair(now, amount);
                }
                else
                {
                    cache_entry_t& entry = found->second;
                    entry.first.tv_sec = now.tv_sec;
                    entry.first.tv_nsec = now.tv_nsec;
                    entry.second += amount;
                }

                allocated_memory += amount;
#ifdef TILECACHE_DEBUG
    print_table();
#endif
                return true;
            }

            debug(TM, "Can't free memory.");

            print_table();
            return false;
        }

        void release_cache_memory(TileCacheBase* requestor, uint_fast64_t amount)
        {
#ifdef TILECACHE_DEBUG
      debug(TM, "Local cache %p releases %d bytes.", requestor, amount);
#endif

            cache_t::iterator found = cache.find(requestor);

            if (found == cache.end())
            {
                debug(TM, "Unknown memory should be released.");
                print_table();
                assert(1==0);
            }
            else
            {
                cache_entry_t& entry = found->second;

                if (entry.second >= amount)
                {
                    entry.second -= amount;
                    assert(allocated_memory >= amount);
                    if (allocated_memory >= amount) allocated_memory -= amount;
                    else
                    {
                        debug(TM, "More mem to release than available.");
                        print_table();
                        assert(1==0);
                    }
                }
                else
                {
                    print_table();
                    assert(entry.second >= amount); // will break
                }

                if (entry.second == 0)
                {
#ifdef TILECACHE_DEBUG
      debug(TM, "Memory completely released. Remove entry from global cache.");
#endif
                    cache.erase(found);
                }
            }
        }

        inline uint_fast64_t get_max_cache_memory() const
        {
            return max_cache_memory;
        }

        inline uint_fast64_t get_allocated_memory() const
        {
            return allocated_memory;
        }

        inline bool is_full(uint_fast64_t amount) const
        {
            return allocated_memory + amount > max_cache_memory;
        }
    };


    /**
     * The TileCache class handles caching of image tiles.
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
    class TileCache : public TileCacheBase
    {
        friend class GlobalTileCache;

    private:

        typedef std::shared_ptr<MemoryMap<typename PixelPolicy::pixel_type>> MemoryMap_shptr;
        typedef std::map<std::string, // filename
                         std::pair<MemoryMap_shptr, struct timespec>> cache_type;

        const std::string directory;
        const unsigned int tile_width_exp;
        const bool persistent;

        cache_type cache;

        // Used for caching the working tile.
        mutable MemoryMap_shptr current_tile;
        mutable unsigned curr_tile_num_x;
        mutable unsigned curr_tile_num_y;

        std::mutex mutex;


    public:

        /**
         * Create a TileCache object.
         * @param directory The directory where all the tiles are for a TileImage.
         * @param tile_width_exp
         * @param persistent
         */
        TileCache(std::string const& directory,
                  unsigned int tile_width_exp,
                  bool persistent,
                  unsigned int min_cache_tiles = 4) :
            directory(directory),
            tile_width_exp(tile_width_exp),
            persistent(persistent)
        {
        }

        /**
         * Destroy a TileCache object.
         */
        ~TileCache()
        {
            release_memory();
        }

        void release_memory()
        {
            if (cache.size() > 0)
            {
                std::lock_guard<std::mutex> lock(mutex);

                GlobalTileCache& gtc = GlobalTileCache::get_instance();
                gtc.release_cache_memory(this, cache.size() * get_image_size());
                current_tile.reset();
                cache.clear();
            }
        }

        void print() const override
        {
            for (typename cache_type::const_iterator iter = cache.begin();
                 iter != cache.end(); ++iter)
            {
                std::cout << "\t+ "
                    << directory << "/"
                    << (*iter).first << " "
                    << (*iter).second.second.tv_sec
                    << "/"
                    << (*iter).second.second.tv_nsec
                    << std::endl;
            }
        }

        inline void cache_around(unsigned int min_x,
                                 unsigned int max_x,
                                 unsigned int min_y,
                                 unsigned int max_y,
                                 unsigned int max_size_x,
                                 unsigned int max_size_y,
                                 unsigned int radius)
        {
            unsigned int tile_num_min_x = min_x >> tile_width_exp;
            unsigned int tile_num_max_x = max_x >> tile_width_exp;
            unsigned int tile_num_min_y = min_y >> tile_width_exp;
            unsigned int tile_num_max_y = max_y >> tile_width_exp;

            unsigned int tile_num_max_size_x = max_size_x >> tile_width_exp;
            unsigned int tile_num_max_size_y = max_size_y >> tile_width_exp;

            unsigned int cache_min_x = radius > tile_num_min_x ? 0 : tile_num_min_x - radius;
            unsigned int cache_max_x = radius + tile_num_max_x > tile_num_max_size_x ? tile_num_max_size_x : tile_num_max_x + radius;
            unsigned int cache_min_y = radius > tile_num_min_y ? 0 : tile_num_min_y - radius;
            unsigned int cache_max_y = radius + tile_num_max_y > tile_num_max_size_y ? tile_num_max_size_y : tile_num_max_y + radius;

            if (static_cast<uint_fast64_t>(cache_max_x - cache_min_x) *
                static_cast<uint_fast64_t>(cache_max_y - cache_min_y) *
                static_cast<uint_fast64_t>(get_image_size()) *
                static_cast<uint_fast64_t>(sizeof(typename PixelPolicy::pixel_type)) > GlobalTileCache::get_instance().get_max_cache_memory())
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

        inline void load_tile(unsigned int x, unsigned int y, bool update_current = false)
        {
            std::lock_guard<std::mutex> lock(mutex);

            // create a file name from tile number
            char filename[PATH_MAX];
            snprintf(filename, sizeof(filename), "%d_%d.dat", x, y);

            // if filename/ object is not in cache, load the tile
            typename cache_type::const_iterator iter = cache.find(filename);

            if (iter == cache.end())
            {
                GlobalTileCache& gtc = GlobalTileCache::get_instance();

                bool ok = gtc.request_cache_memory(this, get_image_size());
                assert(ok == true);
                struct timespec now{};
                GET_CLOCK(now);

                cache[filename] = std::make_pair(load(filename), now);

                //debug(TM, "Cache size : %d/%d", gtc.get_allocated_memory(), gtc.get_max_cache_memory());

                #ifdef TILECACHE_DEBUG
                gtc.print_table();
                #endif
            }

            // Get current time
            struct timespec now{};
            GET_CLOCK(now);

            // Update entry
            auto entry = cache[filename];
            entry.second = now;

            if (update_current)
            {
                current_tile = entry.first;
                curr_tile_num_x = x;
                curr_tile_num_y = y;
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

            if (!(current_tile != nullptr &&
                tile_num_x == curr_tile_num_x &&
                tile_num_y == curr_tile_num_y))
            {
                load_tile(tile_num_x, tile_num_y, true);
            }

            return current_tile;
        }

    protected:

        /**
         * Remove the oldest entry from the cache.
         */
        void cleanup_cache() override
        {
            if (cache.size() == 0) return;

            struct timespec oldest_clock_val;
            GET_CLOCK(oldest_clock_val);

            typename cache_type::iterator oldest = cache.begin();

            for (typename cache_type::iterator iter = cache.begin();
                 iter != cache.end(); ++iter)
            {
                struct timespec clock_val = (*iter).second.second;
                if (clock_val < oldest_clock_val)
                {
                    oldest_clock_val.tv_sec = clock_val.tv_sec;
                    oldest_clock_val.tv_nsec = clock_val.tv_nsec;
                    oldest = iter;
                }
            }

            assert(oldest != cache.end());
            (*oldest).second.first.reset(); // explicit reset of smart pointer
            cache.erase(oldest);
#ifdef TILECACHE_DEBUG
      debug(TM, "local cache: %d entries after remove\n", cache.size());
#endif
            GlobalTileCache& gtc = GlobalTileCache::get_instance();
            gtc.release_cache_memory(this, get_image_size());
        }


    private:

        /**
         * Get image size in bytes.
         */
        uint_fast64_t get_image_size() const
        {
            return sizeof(typename PixelPolicy::pixel_type) * (uint_fast64_t(1) << tile_width_exp) * (uint_fast64_t(1) << tile_width_exp);
        }

        /**
         * Load a tile from an image file.
         * @param filename Just the name of the file to load. The filename is
         *     relative to the \p directory.
         */
        std::shared_ptr<MemoryMap<typename PixelPolicy::pixel_type>>
        load(std::string const& filename) const
        {
            //debug(TM, "directory: [%s] file: [%s]", directory.c_str(), filename.c_str());
            MemoryMap_shptr mem(new MemoryMap<typename PixelPolicy::pixel_type>
                (uint_fast64_t(1) << tile_width_exp,
                 uint_fast64_t(1) << tile_width_exp,
                 MAP_STORAGE_TYPE_PERSISTENT_FILE,
                 join_pathes(directory, filename)));

            return mem;
        }
    }; // end of class TileCache
}

#endif
