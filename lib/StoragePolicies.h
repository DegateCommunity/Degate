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

#ifndef __STORAGEPOLICIES_H__
#define __STORAGEPOLICIES_H__

#include "globals.h"
#include "MemoryMap.h"
#include "Configuration.h"
#include "FileSystem.h"
#include "Image.h"

namespace degate {


  /* -------------------------------------------------------------------------- *
   * storage policies
   * -------------------------------------------------------------------------- */

   
  /**
   * Base class for the storage policy of an image.
   * This is basically the same as StoragePolicy_GenericBase, but
   * adds some methods depending on the PixelPolicy specialization.
   */
  template<class PixelPolicy>
  class StoragePolicy_Base  {

  public:
    typedef typename PixelPolicy::pixel_type pixel_type;
    typedef PixelPolicy pixel_policy;

    StoragePolicy_Base() {}

    virtual ~StoragePolicy_Base() {}

    /**
     * Get pixel for "native" pixel types.
     * This method is abstract. If you derive from this class, you should
     * implement it for a concrete StoragePolicy.
     */
    virtual pixel_type get_pixel(unsigned int x, unsigned int y) const = 0;

    /**
     * Set "native" pixel.
     * This method is abstract. If you derive from this class, you should
     * implement it for a concrete StoragePolicy.
     */
    virtual void set_pixel(unsigned int x, unsigned int y, pixel_type new_val) = 0;

  };



  
  /** 
   * Storage policy for image objects that resists in memory. 
   */
  template<class PixelPolicy>
  class StoragePolicy_Memory : public StoragePolicy_Base<PixelPolicy> {

  protected:

    static const MAP_STORAGE_TYPE storage_type = MAP_STORAGE_TYPE_MEM;
    MemoryMap<typename PixelPolicy::pixel_type> memory_map;

  public:

    StoragePolicy_Memory(unsigned int _width, unsigned int _height) :
      memory_map(_width, _height) {
    }

    virtual ~StoragePolicy_Memory() {}

    inline typename PixelPolicy::pixel_type get_pixel(unsigned int x, unsigned int y) const {
      return memory_map.get(x, y);
    }

    inline void set_pixel(unsigned int x, unsigned int y, 
		   typename PixelPolicy::pixel_type new_val) {
      memory_map.set(x, y, new_val);
    }

  };


  /** 
   * Storage policy for image objects that are stored in a file. 
   */

  template<class PixelPolicy>
  class StoragePolicy_File : public StoragePolicy_Base<PixelPolicy> {

  protected:
    
    MemoryMap<typename PixelPolicy::pixel_type> memory_map;
    
  public:
    
    StoragePolicy_File(unsigned int _width, 
		       unsigned int _height, 
		       std::string const& filename,
		       bool persistent = false) :
      memory_map(_width, _height, 
		 persistent == false ? MAP_STORAGE_TYPE_TEMP_FILE : MAP_STORAGE_TYPE_PERSISTENT_FILE, 
		 filename) {
    }

    virtual ~StoragePolicy_File() {}

    inline typename PixelPolicy::pixel_type get_pixel(unsigned int x, 
						       unsigned int y) const {
      return memory_map.get(x, y);
    }

    inline void set_pixel(unsigned int x, unsigned int y, 
			  typename PixelPolicy::pixel_type new_val) {
      memory_map.set(x, y, new_val);
    }

  };


  /** 
   * Storage policy for image objects that are stored in a temporary file. 
   */
  template<class PixelPolicy>
  class StoragePolicy_TempFile : public StoragePolicy_File<PixelPolicy> {

  public:
    StoragePolicy_TempFile(unsigned int _width, 
			   unsigned int _height) :
      StoragePolicy_File<PixelPolicy>(_width, _height,
				      generate_temp_file_pattern(get_temp_directory()),
				      false) {}

    virtual ~StoragePolicy_TempFile() {}
  };

  /**
   * Storage policy for image objects that are stored in a persistent file.
   */
  template<class PixelPolicy>
  class StoragePolicy_PersistentFile : public StoragePolicy_File<PixelPolicy> {

  public:
    
    StoragePolicy_PersistentFile(unsigned int _width, 
				 unsigned int _height,
				 std::string const& filename) :
      StoragePolicy_File<PixelPolicy>(_width, _height, filename, true) {}

    virtual ~StoragePolicy_PersistentFile() {}
  };
  

}

#endif
