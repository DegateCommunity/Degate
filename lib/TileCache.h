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

#ifndef __TILECACHE_H__
#define __TILECACHE_H__

#include "MemoryMap.h"
#include "FileSystem.h"

#include <string>
#include <map>
#include <tr1/memory>
#include <ctime>
#include <utility> // for make_pair

namespace degate {

  class TileCacheBase {
  public:
    virtual void cleanup_cache() = 0;
  };

  class GlobalTileCache : public SingletonBase<GlobalTileCache> {

    friend class SingletonBase<GlobalTileCache>;

  private:

    const static size_t max_cache_memory = 128 * 1024 * 1024;
    size_t allocated_memory;

    typedef std::pair<clock_t, size_t> cache_entry_t;
    typedef std::map<TileCacheBase *, cache_entry_t> cache_t;

    cache_t cache;

  private:
    
    GlobalTileCache() : allocated_memory(0) {
    }

    void remove_oldest() {
      clock_t now = clock();
      TileCacheBase * oldest = NULL;

      for(cache_t::iterator iter = cache.begin(); iter != cache.end(); ++iter) {
	cache_entry_t & entry = iter->second;
	if(entry.first < now) {
	  now = clock();
	  oldest = iter->first;
	}
      }

      if(oldest) oldest->cleanup_cache();
      else {
	debug(TM, "there is nothing to free.");
      }
    }

  public:

    bool request_cache_memory(TileCacheBase * requestor, size_t amount) {

      debug(TM, "Local cache %p requests %d bytes.", requestor, amount);

      while(allocated_memory + amount > max_cache_memory) {
	debug(TM, "Try to free memory");
	remove_oldest();
      }

      if(allocated_memory + amount < max_cache_memory) {

	cache_t::iterator found = cache.find(requestor);
	if(found == cache.end()) {
	  cache[requestor] = std::make_pair(amount, clock());
	}
	else {
	  cache_entry_t & entry = found->second;
	  entry.second += amount;
	  entry.first = clock();
	}

	allocated_memory += amount;

	return true;
      }
      return false;
    }

    void release_cache_memory(TileCacheBase * requestor, size_t amount) {
      
      debug(TM, "Local cache %p releases %d bytes.", requestor, amount);

      cache_t::iterator found = cache.find(requestor);
      if(found != cache.end()) {
	cache_entry_t & entry = found->second;

	assert(entry.second >= amount);
	if(entry.second >= amount) entry.second -= amount;

	if(entry.second == 0) {
	  debug(TM, "Memory completely released. Remove entry from global cache.");
	  cache.erase(found);
	}
      }
     
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
   * @todo Extent the TileCache and let it use a global tile cache. Then it will be easier
   *   to control memory consumtion. You will be abe to set a global maximum, how much
   *   memory can be used for caching.
   */

  template<class PixelPolicy>
  class TileCache : public TileCacheBase {

    friend class GlobalTileCache;

  private:

    typedef std::tr1::shared_ptr<MemoryMap<typename PixelPolicy::pixel_type> > MemoryMap_shptr;
    typedef std::map< std::string, // filename
		      std::pair<MemoryMap_shptr, clock_t> > cache_type;

    const std::string directory;
    const unsigned int tile_width_exp;
    const bool persistent;

    cache_type cache;

    // Used for caching the working tile.
    mutable MemoryMap_shptr current_tile;
    mutable unsigned curr_tile_num_x;
    mutable unsigned curr_tile_num_y;

    
  public:
    
    /**
     * Create a TileCache object.
     * @param _directory The directory where all the tiles are for a TileImage.
     * @param _tile_width_exp
     * @param _persistent 
     */

    TileCache(std::string const& _directory, 
	      unsigned int _tile_width_exp,
	      bool _persistent,
	      unsigned int _min_cache_tiles = 4) : 
      directory(_directory),
      tile_width_exp(_tile_width_exp),
      persistent(_persistent) {}

    /**
     * Destroy a TileCache object.
     */

    ~TileCache() {
      GlobalTileCache & gtc = GlobalTileCache::get_instance();
      gtc.release_cache_memory(this, cache.size() * get_image_size());
    }

    

    /**
     * Get a tile. If the tile is not in the cache, the tile is loaded.
     *
     * @param x Absolut pixel coordinate.
     * @param y Absolut pixel coordinate.
     * @return Returns a shared pointer to a MemoryMap object.
     */

    std::tr1::shared_ptr<MemoryMap<typename PixelPolicy::pixel_type> > 
    inline get_tile(unsigned int x, unsigned int y) {

      unsigned int tile_num_x = x >> tile_width_exp;
      unsigned int tile_num_y = y >> tile_width_exp;

      if(!(current_tile != NULL &&
	   tile_num_x == curr_tile_num_x &&
	   tile_num_y == curr_tile_num_y)) {

	// create a file name from tile number
	char filename[PATH_MAX];
	snprintf(filename, sizeof(filename), "%d_%d.dat", tile_num_x, tile_num_y);
	//debug(TM, "filename is: [%s]", filename);

	// if filename/ object is not in cache, load the tile
	typename cache_type::const_iterator iter = cache.find(filename);

	if(iter == cache.end()) {
	  //cleanup_cache();
	  GlobalTileCache & gtc = GlobalTileCache::get_instance();
	  gtc.request_cache_memory(this, get_image_size());
	  cache[filename] = std::make_pair(load(filename), clock());
	}

	current_tile = cache[filename].first;
	curr_tile_num_x = tile_num_x;
	curr_tile_num_y = tile_num_y;

      }

      return current_tile;
    }

  protected:

    /**
     * Remove the oldest entry from the cache.
     */
    void cleanup_cache() {
	
	clock_t oldest_clock_val = clock();
	typename cache_type::iterator oldest = cache.begin();

	for(typename cache_type::iterator iter = cache.begin();
	    iter != cache.end(); ++iter) {
	  clock_t clock_val = (*iter).second.second;
	  if(clock_val < oldest_clock_val) {
	    oldest_clock_val = clock_val;
	    oldest = iter;
	  }
	}

	cache.erase(oldest);

	GlobalTileCache & gtc = GlobalTileCache::get_instance();
	gtc.release_cache_memory(this, get_image_size());

    }
    

  private:

    /**
     * Get image size in bytes.
     */
    size_t get_image_size() const {
      return sizeof(typename PixelPolicy::pixel_type) * (1<< tile_width_exp) * (1<< tile_width_exp);
    }

    /**
     * Load a tile from an image file.
     * @param filename Just the name of the file to load. The filename is
     *     relative to the \p directory.
     */
    std::tr1::shared_ptr<MemoryMap<typename PixelPolicy::pixel_type> > 
    load(std::string const& filename) const {

      //debug(TM, "directory: [%s] file: [%s]", directory.c_str(), filename.c_str());
      MemoryMap_shptr mem(new MemoryMap<typename PixelPolicy::pixel_type>
			  (1 << tile_width_exp,
			   1 << tile_width_exp,
			   MAP_STORAGE_TYPE_PERSISTENT_FILE,
			   join_pathes(directory, filename)));

      return mem;
    }


  }; // end of class TileCache

}

#endif
