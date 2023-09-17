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

#define _CRT_SECURE_NO_WARNINGS 1

#include "Core/Configuration.h"
#include "GUI/Preferences/PreferencesHandler.h"

using namespace degate;

Configuration::Configuration() = default;

uint_fast64_t Configuration::get_max_tile_cache_size()
{
    return static_cast<uint_fast64_t>(PREFERENCES_HANDLER.get_preferences().cache_size);
}

unsigned int Configuration::get_max_concurrent_thread_count()
{
    const auto& pref = PREFERENCES_HANDLER.get_preferences();

    // Check default value (0 = use ideal thread count)
    if (pref.max_concurrent_thread_count == 0) 
        return QThread::idealThreadCount();

    return pref.max_concurrent_thread_count;
}