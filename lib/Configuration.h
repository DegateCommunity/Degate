/* -*-c++-*-

 This file is part of the IC reverse engineering tool degate.

 Copyright 2008, 2009, 2010 by Martin Schobert

 Degate is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 any later version.

 Degate is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with degate. If not, see <http://www.gnu.org/licenses/>.

*/

#ifndef __CONFIGURATION_H__
#define __CONFIGURATION_H__

#include <globals.h>
#include <SingletonBase.h>

namespace degate {

  /**
   * Get the temp directory.
   * This is a shortcut for
   * \p (Configuration::get_instance()).get_temp_directory() .
   */

  std::string get_temp_directory();


  class Configuration : public SingletonBase<Configuration> {

    friend class SingletonBase<Configuration>;

  private:

    Configuration();

  public:

    /**
     * Get the cache size for image tiles in MB.
     * @return If the environment variable DEGATE_CACHE_SIZE is set,
     *   its value. Else the default cache size is returned.
     *   That is 256 MB.
     */
    size_t get_max_tile_cache_size() const;


    /**
     * Get the URI address pattern for the collaboration server.
     * It is a pattern, because it holds a placeholder for the channel ID.
     * This channel ID identifies a shared project. The pattern is a
     * format string for boost::format.
     * @return If the environment variable DEGATE_SERVER_URI_PATTERN is set,
     *   its value. Else the default URI pattern is returned.
     */

    std::string get_servers_uri_pattern() const;

  };

}

#endif
