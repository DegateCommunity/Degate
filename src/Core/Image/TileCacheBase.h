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

#ifndef __TILECACHEBASE_H__
#define __TILECACHEBASE_H__

#include <chrono>
#include <utility>

/**
 * Minimum size (in Mb) of the cache.
 */
#define MINIMUM_CACHE_SIZE uint_fast64_t(256)

static void get_clock(struct timespec* ts)
{
    assert(ts != nullptr);

    std::chrono::steady_clock::time_point tp = std::chrono::steady_clock::now();

    auto secs = std::chrono::time_point_cast<std::chrono::seconds>(tp);
    auto ns = std::chrono::time_point_cast<std::chrono::nanoseconds>(tp) -
              std::chrono::time_point_cast<std::chrono::nanoseconds>(secs);

    ts->tv_sec = secs.time_since_epoch().count();
    ts->tv_nsec = static_cast<long>(ns.count());
}

#define GET_CLOCK(dst_variable) get_clock(&dst_variable);

/**
 * Overloaded comparison operator for timespec-structs.
 * @return Returns true, if \p a is completely before \p b. Else
 *   false is returned.
 */
static bool operator<(struct timespec const& a, struct timespec const& b)
{
    if (a.tv_sec < b.tv_sec)
        return true;
    else if (a.tv_sec == b.tv_sec && a.tv_nsec < b.tv_nsec)
        return true;
    else
        return false;
}

namespace degate
{
    class TileCacheBase
    {
    public:
        virtual void cleanup_cache() = 0;
        virtual void print() const = 0;
    };
}

#endif
