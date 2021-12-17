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

#ifndef __CONFIGURATION_H__
#define __CONFIGURATION_H__

#include "Globals.h"
#include "Core/Primitive/SingletonBase.h"

namespace degate
{
    /**
     * @class Configuration
     * @brief Singleton class to store important parameters needed everywhere.
     */
    class Configuration : public SingletonBase<Configuration>
    {

        friend class SingletonBase<Configuration>;

    private:
        Configuration();

    public:
        /**
         * Get the cache size for image tiles in MB.
         * @return Returns the maximum cache size (in Mb) from the preferences.
         */
        static uint_fast64_t get_max_tile_cache_size();
    };

} // namespace degate

#endif
