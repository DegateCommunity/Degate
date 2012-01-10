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

#include <MemoryMap.h>
#include <FileSystem.h>
#include <Configuration.h>

#include <string>
#include <map>
#include <tr1/memory>
#include <ctime>
#include <utility> // for make_pair
#include <iostream>
#include <inttypes.h>

#ifdef __APPLE__
  #include <sys/time.h> // for gettimeofday
  #define GET_CLOCK(dst_variable) \
  { \
      struct timeval tv; \
      gettimeofday(&tv, NULL); \
      dst_variable.tv_sec = tv.tv_sec; \
      dst_variable.tv_nsec = tv.tv_usec * 1000; \
  }
#else
  #define GET_CLOCK(dst_variable) \
      clock_gettime(CLOCK_MONOTONIC,  &dst_variable);
#endif

// #define TILECACHE_DEBUG

/**
 * Overloaded comparison operator for timespec-structs.
 * @return Returns true, if \p a is completely before \p b. Else
 *   false is returned.
 */
static bool operator<(struct timespec const & a, struct timespec const & b) {
  if(a.tv_sec < b.tv_sec) return true;
  else if(a.tv_sec == b.tv_sec && a.tv_nsec < b.tv_nsec) return true;
  else return false;
}

namespace degate {


  class TileCacheBase {
  public:
    virtual void cleanup_cache() = 0;
    virtual void print() const = 0;
  };

  class GlobalTileCache : public SingletonBase<GlobalTileCache> {

    friend class SingletonBase<GlobalTileCache>;

  private:

    size_t max_cache_memory;
    size_t allocated_memory;

    typedef std::pair<struct timespec, size_t> cache_entry_t;
    typedef std::map<TileCacheBase *, cache_entry_t> cache_t;

    cache_t cache;

  private:

    GlobalTileCache() : allocated_memory(0) {
      Configuration & conf = Configuration::get_instance();
      max_cache_memory = conf.get_max_tile_cache_size() * 1024 *1024;
    }

    void remove_oldest() {
      struct timespec now;
      GET_CLOCK(now);

      TileCacheBase * oldest = NULL;

      for(cache_t::iterator iter = cache.begin(); iter != cache.end(); ++iter) {
	cache_entry_t & entry = iter->second;
	if(entry.first < now) {
	  now.tv_sec = entry.first.tv_sec;
	  now.tv_nsec = entry.first.tv_nsec;
	  oldest = iter->first;
	}
      }

      if(oldest) {
#ifdef TILECACHE_DEBUG
	debug(TM, "Will call cleanup on %p", oldest);
#endif
	oldest->cleanup_cache();
      }
      else {
#ifdef TILECACHE_DEBUG
	debug(TM, "there is nothing to free.");
	print_table();
#endif
      }
    }

  public:

    void print_table() const {
      printf("Global Image Tile Cache:\n"
	     "Used memory : %llu bytes\n"
	     "Max memory  : %llu bytes\n\n"
	     "Holder           | Last access (sec,nsec)    | Amount of memory\n"
	     "-----------------+---------------------------+------------------------------------\n",
	     (long long unsigned)allocated_memory, (long long unsigned)max_cache_memory);

      for(cache_t::const_iterator iter = cache.begin(); iter != cache.end(); ++iter) {
	cache_entry_t const& entry = iter->second;
	printf("%16p | %12ld.%12ld | %u M (%u bytes)\n",
	       iter->first, entry.first.tv_sec, entry.first.tv_nsec, entry.second/(1024*1024), entry.second);
	iter->first->print();
      }

      printf("\n");
    }

    bool request_cache_memory(TileCacheBase * requestor, size_t amount) {

#ifdef TILECACHE_DEBUG
      debug(TM, "Local cache %p requests %d bytes.", requestor, amount);
#endif
      while(allocated_memory + amount > max_cache_memory) {
#ifdef TILECACHE_DEBUG
	debug(TM, "Try to free memory");
#endif
	remove_oldest();
      }

      if(allocated_memory + amount <= max_cache_memory) {
	struct timespec now;
	GET_CLOCK(now);

	cache_t::iterator found = cache.find(requestor);
	if(found == cache.end()) {
	  cache[requestor] = std::make_pair(now, amount);
	}
	else {
	  cache_entry_t & entry = found->second;
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

    void release_cache_memory(TileCacheBase * requestor, size_t amount) {

#ifdef TILECACHE_DEBUG
      debug(TM, "Local cache %p releases %d bytes.", requestor, amount);
#endif

      cache_t::iterator found = cache.find(requestor);

      if(found == cache.end()) {
	debug(TM, "Unknown memory should be released.");
	print_table();
	assert(1==0);
      }
      else {
	cache_entry_t & entry = found->second;

	if(entry.second >= amount) {
	  entry.second -= amount;
	  assert(allocated_memory >= amount);
	  if(allocated_memory >= amount) allocated_memory -= amount;
	  else {
	    debug(TM, "More mem to release than available.");
	    print_table();
	    assert(1==0);
	  }
	}
	else {
	  print_table();
	  assert(entry.second >= amount); // will break
	}

	if(entry.second == 0) {
#ifdef TILECACHE_DEBUG
	  debug(TM, "Memory completely released. Remove entry from global cache.");
#endif
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
   */

  template<class PixelPolicy>
  class TileCache : public TileCacheBase {

    friend class GlobalTileCache;

  private:

    typedef std::tr1::shared_ptr<MemoryMap<typename PixelPolicy::pixel_type> > MemoryMap_shptr;
    typedef std::map< std::string, // filename
		      std::pair<MemoryMap_shptr, struct timespec> > cache_type;

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
      if(cache.size() > 0) {
	GlobalTileCache & gtc = GlobalTileCache::get_instance();
	gtc.release_cache_memory(this, cache.size() * get_image_size());
      }
    }

    void print() const {
      for(typename cache_type::const_iterator iter = cache.begin();
	  iter != cache.end(); ++iter) {
	std::cout << "\t+ "
		  << directory << "/"
		  << (*iter).first << " "
		  << (*iter).second.second.tv_sec
		  << "/"
		  << (*iter).second.second.tv_nsec
		  << std::endl;
      }
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
	  bool ok = gtc.request_cache_memory(this, get_image_size());
	  assert(ok == true);
	  struct timespec now;
	  GET_CLOCK(now);

	  cache[filename] = std::make_pair(load(filename), now);
#ifdef TILECACHE_DEBUG
	  gtc.print_table();
#endif
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

      if(cache.size() == 0) return;

      struct timespec oldest_clock_val;
      GET_CLOCK(oldest_clock_val);

      typename cache_type::iterator oldest = cache.begin();

      for(typename cache_type::iterator iter = cache.begin();
	  iter != cache.end(); ++iter) {

	struct timespec clock_val = (*iter).second.second;
	if(clock_val < oldest_clock_val) {
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
