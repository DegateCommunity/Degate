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

#ifndef __PORTCOLORMANAGER_H__
#define __PORTCOLORMANAGER_H__

#include <map>
#include <memory>

namespace degate {

  /**
   * The PortColorManager manages color definitions based on common port names.
   *
   * In degate you can define colors, which are used for the rendering. To
   * make gate ports of the same type drawn in the same color, you can store a
   * color specification in the PortColorManager.

   */

  class PortColorManager {

  public:
    typedef std::map<std::string /* port name */, std::pair<color_t, color_t> > port_color_collection;

  private:

    port_color_collection port_color_map;


  public:

    /**
     * Construct a new port color manager.
     */
    PortColorManager() {}

    /**
     * Destroy the port color mananger.
     */
    ~PortColorManager() {};

    /**
     * Add a new color definition.
     */
    void set_color(std::string port_name, color_t frame_color_def, color_t fill_color_def) {
      port_color_map[port_name] = std::pair<color_t, color_t>(frame_color_def, fill_color_def);
    }

    /**
     * Remove a color definition.
     */
    void remove_color(std::string port_name) {
      port_color_map.erase(port_name);
    }

    /**
     * Get a fill color definition.
     * @exception CollectionLookupException
     */
    color_t get_fill_color(std::string const & port_name) const {
      const port_color_collection::const_iterator iter = port_color_map.find(port_name);
      if(iter != port_color_map.end()) {
	return (*iter).second.second;
      }
      else throw CollectionLookupException("Can't lookup fill color.");
    }

    /**
     * Get a frame color definition.
     * @exception CollectionLookupException
     */
    color_t get_frame_color(std::string const & port_name) const {
      const port_color_collection::const_iterator iter = port_color_map.find(port_name);
      if(iter != port_color_map.end()) {
	return (*iter).second.first;
      }
      else throw CollectionLookupException("Can't lookup frame color.");
    }

    /**
     * Check if there exists a frame and fill color definition for a port name.
     */
    bool has_color_definition(std::string const & port_name) const {
      const port_color_collection::const_iterator iter = port_color_map.find(port_name);
      return (iter != port_color_map.end());
    }


    /**
     * Get an iterator to iterate over port color definitions.
     * @see end()
     */
    port_color_collection::iterator begin() {
      return port_color_map.begin();
    }

    /**
     * Get an end iterator.
     * @see begin()
     */
    port_color_collection::iterator end() {
      return port_color_map.end();
    }

  };

  /**
   * Typedef for a shared pointer on PortColorManager objects.
   */
  typedef std::shared_ptr<PortColorManager> PortColorManager_shptr;
}

#endif
