/* -*-c++-*-
 
 This file is part of the IC reverse engineering tool degate.
 
 Copyright 2008, 2009 by Martin Schobert
 
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

#ifndef __TILEIMAGE_H__
#define __TILEIMAGE_H__

#include "globals.h"
#include "PixelPolicies.h"
#include "StoragePolicies.h"
#include "FileSystem.h"
#include "TileCache.h"

namespace degate {

  /** 
   * Storage policy for image objects that consists of tiles.
   *
   * This implementation uses a TileCache.
   */

  template<class PixelPolicy>
  class StoragePolicy_Tile : public StoragePolicy_Base<PixelPolicy> {

  public:

    typedef std::tr1::shared_ptr<MemoryMap<typename PixelPolicy::pixel_type> > MemoryMap_shptr;

  private:

    // Do the underlying file resources have to be cleaned up on destruction?
    const bool persistent;

    // Exponent and a bitmask to calculate tile numbers.
    const unsigned int tile_width_exp;
    const unsigned int offset_bitmask;

    // The place where we store the image data.
    const std::string directory;

    // A helper class to load tiles.
    mutable TileCache<PixelPolicy> tile_cache;

  private:


    /**
     * Get the minimum width or height of an tile based image, that
     * it at least requested_size pixel width / height.
     * @param requested_size The minimum size.
     * @param tile_width_exp The exponent (to base 2) that gives th
     * @return 
     */
    unsigned int calc_real_size(unsigned int requested_size,
				unsigned int tile_width_exp) const {

      // we can't use the get_tile_size() method here, because we use
      // this method during the base class constructor call.
      unsigned int tile_size = (1 << tile_width_exp);      
      unsigned int remainder = requested_size % tile_size;
      if(remainder == 0) return requested_size;
      else return requested_size - remainder + tile_size;
    }

  public:

    /**
     * The constructor for a tile based image. The constructed image has at
     * least the size specified via the width and height parameter.
     * Because the image is splitted into equisized tiles the constructed
     * image might be larger than the requested size.
     *
     * @param _width The minimum width of the image.
     * @param _height The minimum height of the image.
     * @param _directory A tile based image is stored in multiple
     *      files. This directory specifies the place where
     *      the files are stored. If the directory doen't exits, it is created.
     * @param _persistent This boolean value indicates whether the image files
     *      are removed on object destruction.
     * @param _tile_width_exp The width (and height) for image tiles. This
     *      value is specified as an exponent to the base 2. This means for
     *      example that if you want to use a width of 1024 pixel, you have
     *      to give a value of 10, because 2^10 is 1024.
     * @param _max_cache_tiles The number of tiles that should be cached.
     *      The value must be >=1. It is passed to the ctor of TileCache.
     */
    StoragePolicy_Tile(unsigned int _width, unsigned int _height,
		       std::string const& _directory,
		       bool _persistent = false,
		       unsigned int _tile_width_exp = 10,
		       unsigned int _max_cache_tiles = 4) :
      persistent(_persistent),
      tile_width_exp(_tile_width_exp),
      offset_bitmask((1 << _tile_width_exp) - 1),
      directory(_directory),
      tile_cache(_directory, _tile_width_exp, _persistent, _max_cache_tiles) {

      if(!file_exists(_directory)) create_directory(_directory);

    }
    
    /**
     * The destructor.
     */
    virtual ~StoragePolicy_Tile() {
      if(persistent == false) remove_directory(directory);
    }
    

    /**
     * Get the width / height of a single tile. The size is a power of two.
     */

    inline unsigned int get_tile_size() const {
      return (1 << tile_width_exp);
    }


    /**
     * Get the directory, where images are stored.
     */
    std::string get_directory() const { return directory; }

    /**
     * Check if the image is persistent.
     */
    bool is_persistent() const { return persistent; }


    inline typename PixelPolicy::pixel_type get_pixel(unsigned int x, unsigned int y) const;
    
    inline void set_pixel(unsigned int x, unsigned int y, typename PixelPolicy::pixel_type new_val);

    /**
     * Copy the raw data from an image tile that has its upper left corner at x,y into a buffer.
     */
    void raw_copy(void * dst_buf, unsigned int x, unsigned int y) const {
      MemoryMap_shptr mem = tile_cache.get_tile(x, y);
      mem->raw_copy(dst_buf);
    }

  };

  template<class PixelPolicy>
  inline typename PixelPolicy::pixel_type 
  StoragePolicy_Tile<PixelPolicy>::get_pixel(unsigned int x, 
					     unsigned int y) const {
    MemoryMap_shptr mem = tile_cache.get_tile(x, y);
    return mem->get(x & offset_bitmask, y & offset_bitmask);
  }

  template<class PixelPolicy>
  inline void 
  StoragePolicy_Tile<PixelPolicy>::set_pixel(unsigned int x, unsigned int y, 
					     typename PixelPolicy::pixel_type new_val) {

    MemoryMap_shptr mem = tile_cache.get_tile(x, y);
    mem->set(x & offset_bitmask, y & offset_bitmask, new_val);
  }





}

  
  

#endif
