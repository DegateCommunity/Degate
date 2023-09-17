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

#ifndef __GLOBALTILECACHE_H__
#define __GLOBALTILECACHE_H__

#include "Core/Image/TileCacheBase.h"
#include "Core/Primitive/SingletonBase.h"

#include <cstddef>
#include <utility>
#include <iostream>
#include <iomanip>

namespace degate
{
    /**
     * @class GlobalTileCache
     * @brief Keep in memory all the used memory at any instant (by tile caches).
     * 
     * This can ask for every single tile cache to release memory if needed.
     * 
     * @warning This is a singleton, only one instance can exists.
     */
    template <class PixelPolicy>
    class GlobalTileCache : public SingletonBase<GlobalTileCache<PixelPolicy>>
    {
        friend class SingletonBase<GlobalTileCache>;

    private:
        uint_fast64_t max_cache_memory;
        uint_fast64_t allocated_memory;

        typedef std::pair<struct timespec, uint_fast64_t> cache_entry_t;
        typedef std::map<TileCacheBase*, cache_entry_t> cache_t;

        cache_t cache;

    private:

        /**
         * Create a new global tile cache object (singleton).
         */
        GlobalTileCache() : allocated_memory(0)
        {
            if (Configuration::get_max_tile_cache_size() < MINIMUM_CACHE_SIZE)
                max_cache_memory = MINIMUM_CACHE_SIZE * uint_fast64_t(1024) * uint_fast64_t(1024);
            else
                max_cache_memory = Configuration::get_max_tile_cache_size() * uint_fast64_t(1024) * uint_fast64_t(1024);
        }

        /**
         * Search for the oldest cache that requested memory and make it release memory.
         */
        void remove_oldest()
        {
            struct timespec now;
            GET_CLOCK(now);

            TileCacheBase* oldest = nullptr;

            // Search for oldest cache
            for (auto iter = cache.begin(); iter != cache.end(); ++iter)
            {
                cache_entry_t& entry = iter->second;
                if (entry.first < now)
                {
                    now.tv_sec = entry.first.tv_sec;
                    now.tv_nsec = entry.first.tv_nsec;
                    oldest = iter->first;
                }
            }

            if (oldest != nullptr)
            {
#ifdef TILECACHE_DEBUG
                debug(TM, "Will call cleanup on %p", oldest);
#endif

                // If found, make the oldest release memory
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

        /**
         * Print a status table about the global tile cache.
         */
        void print_table() const
        {
            std::cout << "Global Image Tile Cache:\n"
                      << "Used memory : " << allocated_memory << " bytes\n"
                      << "Max memory  : " << max_cache_memory << " bytes\n\n"
                      << "Holder           | Last access (sec,nsec)    | Amount of memory\n"
                      << "-----------------+---------------------------+------------------------------------\n";

            for (auto iter = cache.begin(); iter != cache.end(); ++iter)
            {
                cache_entry_t const& entry = iter->second;
                std::cout << std::setw(16) << std::hex << static_cast<void*>(iter->first);
                std::cout << " | ";
                std::cout << std::setw(12) << entry.first.tv_sec;
                std::cout << ".";
                std::cout << std::setw(12) << entry.first.tv_nsec;
                std::cout << " | ";
                std::cout << entry.second / static_cast<unsigned long>(1024 * 1024) << " M (" << entry.second << " bytes)\n";
                iter->first->print();
            }
            std::cout << "\n";
        }

        /**
         * Request memory from the cache (this is virtual).
         * 
         * If too less memory remaining, then will call remove_oldest().
         */
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
                struct timespec now{};
                GET_CLOCK(now);

                auto found = cache.find(requestor);
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

        /**
         * Release memory from the cache (virtual).
         */
        void release_cache_memory(TileCacheBase* requestor, uint_fast64_t amount)
        {
#ifdef TILECACHE_DEBUG
            debug(TM, "Local cache %p releases %d bytes.", requestor, amount);
#endif

            auto found = cache.find(requestor);

            if (found == cache.end())
            {
                debug(TM, "Unknown memory should be released.");
                print_table();
                assert(1 == 0);
            }
            else
            {
                cache_entry_t& entry = found->second;

                if (entry.second >= amount)
                {
                    entry.second -= amount;
                    assert(allocated_memory >= amount);
                    if (allocated_memory >= amount)
                        allocated_memory -= amount;
                    else
                    {
                        debug(TM, "More mem to release than available.");
                        print_table();
                        assert(1 == 0);
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
}

#endif //__GLOBALTILECACHE_H__
