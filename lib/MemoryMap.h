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

#ifndef __MEMORYMAP_H__
#define __MEMORYMAP_H__

#include "globals.h"
#include <string>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <assert.h>
#include <limits.h>
#include <math.h>
#include <tr1/memory>

namespace degate {

  enum MAP_STORAGE_TYPE {
    MAP_STORAGE_TYPE_MEM = 0,
    MAP_STORAGE_TYPE_PERSISTENT_FILE = 1,
    MAP_STORAGE_TYPE_TEMP_FILE = 2,
  };


  /*
   * Storage for data objects, that is mapped from files into memory.
   *
   * You should not use this class directly.
   *
   * @todo: This class is a bit messy. Parts of the code might be moved
   *    into the storage policy classes.
   */
  template<typename T>
  class MemoryMap /* XXX : public MemoryMapBase */ {
		
  private:
    unsigned int width, height;
    MAP_STORAGE_TYPE storage_type;
	
    T * mem;
    std::string filename;
    int fd;
    size_t filesize;

  private:
    ret_t alloc_memory();
    ret_t map_temp_file(std::string const & filename_pattern);
    ret_t map_file(std::string const & filename);
    
    ret_t map_file_by_fd();
    
    void * get_void_ptr(unsigned int x, unsigned int y) const;

    bool is_temp_file() const { 
      return storage_type == MAP_STORAGE_TYPE_TEMP_FILE; 
    }

    bool is_persistent_file() const { 
      return storage_type == MAP_STORAGE_TYPE_PERSISTENT_FILE; 
    }

    bool is_mem() const { 
      return storage_type == MAP_STORAGE_TYPE_MEM; 
    }
    
  public:

	
    MemoryMap(unsigned int width, unsigned int height);
    MemoryMap(unsigned int width, unsigned int height, 
	      MAP_STORAGE_TYPE mode, std::string const & file_to_map);

    //MemoryMap(const MemoryMap &mm);
    ~MemoryMap();
		
    int get_width() const { return width; }
    int get_height() const { return height; }
	
	
    void clear();
    void clear_area(unsigned int min_x, unsigned int min_y, 
		    unsigned int width, unsigned int height);
	
    inline void set(unsigned int x, unsigned int y, T new_val);
    inline T get(unsigned int x, unsigned int y) const;

    void raw_copy(void * buf) const;

    std::string const& get_filename() const { return filename; }
	
    //ret_t deactivate_mapping();
    //ret_t reactivate_mapping();
  };

  template <typename T>
  MemoryMap<T>::MemoryMap(unsigned int _width, unsigned int _height) : 
    width(_width), height(_height),
    storage_type(MAP_STORAGE_TYPE_MEM),
    mem(NULL),
    fd(-1),
    filesize(0) {
    
    ret_t ret = alloc_memory();
    assert(ret == RET_OK);
  }

  template <typename T>
  MemoryMap<T>::MemoryMap(unsigned int _width, unsigned int _height, 
			  MAP_STORAGE_TYPE mode, std::string const & file_to_map) : 
    width(_width), height(_height),
    storage_type(mode),
    mem(NULL),
    filename(file_to_map),
    fd(-1),
    filesize(0) {
    
    assert(mode == MAP_STORAGE_TYPE_PERSISTENT_FILE ||
	   mode == MAP_STORAGE_TYPE_TEMP_FILE);
    
    ret_t ret;
		
    if(mode == MAP_STORAGE_TYPE_TEMP_FILE) {
      ret = map_temp_file(file_to_map);
      if(RET_IS_NOT_OK(ret)) 
	debug(TM, "Can't open a temp file with pattern %s", file_to_map.c_str());

      assert(RET_IS_OK(ret));
    }
    else if(mode == MAP_STORAGE_TYPE_PERSISTENT_FILE) {
      ret = map_file(file_to_map);
      if(RET_IS_NOT_OK(ret)) debug(TM, "Can't open file %s as persistent file", file_to_map.c_str());			
      assert(RET_IS_OK(ret));
    }
  }


  template <typename T>
  MemoryMap<T>::~MemoryMap() {
    
    switch(storage_type) {
    case MAP_STORAGE_TYPE_MEM:
      if(mem != NULL) free(mem);
      mem = NULL;
      break;
    case MAP_STORAGE_TYPE_PERSISTENT_FILE:
    case MAP_STORAGE_TYPE_TEMP_FILE:
      if(mem != NULL) {
	if(msync(mem, filesize, MS_SYNC) == -1) {
	  perror("msync() failed");
	}
	
	if(munmap(mem, filesize) == -1) {
	  perror("munmap failed");
	}
	
	mem = NULL;
      }
      
      close(fd);
      
      if(is_temp_file()) {
	if(unlink(filename.c_str()) == -1) {
	  debug(TM, "Can't unlink temp file");
	}
      }
      break;
    }
    
  }

  template <typename T>
  ret_t MemoryMap<T>::alloc_memory() {
    
    /* If it is not null, it would indicates,
       that there is already any allocation. */
    assert(mem == NULL); 

    mem = (T *) malloc(width * height * sizeof(T));
    assert(mem != NULL);
    if(mem == NULL) return RET_MALLOC_FAILED;
    memset(mem, 0, width * height * sizeof(T));
    return RET_OK;
  }
  
  /**
   * Clear map data.
   */
  
  template <typename T>
  void MemoryMap<T>::clear() {
    assert(mem != NULL);
    if(mem != NULL) memset(mem, 0, width * height * sizeof(T));
  }
  
  
  /** 
   * Clear an area given by start point and width and height
   */
  template <typename T>
  void MemoryMap<T>::clear_area( unsigned int min_x, unsigned int min_y, 
				 unsigned int width, unsigned int height) {
    assert(mem != NULL);
    
    if(mem != NULL) {
      unsigned int x, y;
      for(y = min_y; y < min_y + height; y++)
	memset(get_void_ptr(x, y), 0, width * sizeof(T));
    }
  }
  
  
  /**
   * Create a temp file and use it as storage for the map data.
   * @param filename_pattern The parameter file is a string that specifies the
   *    temp file pattern, e.g. "/tmp/temp.XXXXXXX"
   */
  template <typename T>
  ret_t MemoryMap<T>::map_temp_file(std::string const& filename_pattern) {
    ret_t ret;
    
    assert(is_temp_file());
    
    char * tmp_filename = strdup(filename_pattern.c_str());
    
    fd = mkstemp(tmp_filename);
    
    debug(TM, "temp file: [%s]\n", tmp_filename);
    if(fd == -1) {
      debug(TM, "mkstemp() failed");
      ret = RET_ERR;
    }
    else {
      ret = map_file_by_fd();
      filename = std::string(tmp_filename);
    }
    
    // cleanup
    free(tmp_filename);
    
    return ret;
  }
  

  /**
   * Use storage in file as storage for memory map
   */
  template <typename T>
  ret_t MemoryMap<T>::map_file(std::string const& filename) {
    
    assert(is_persistent_file());
    
    if((fd = open(filename.c_str(), O_RDWR | O_CREAT, 0600)) == -1) {
      debug(TM, "can't open file: %s", filename.c_str());
      return RET_ERR;
    }
    
    return map_file_by_fd();
  }
  
  template <typename T>
  ret_t MemoryMap<T>::map_file_by_fd() {
    
    assert(fd != -1);
    if(fd == -1) {
      debug(TM, "error: invalid file handle");
      return RET_ERR;
    }
    else { 
      // get file size
      filesize = lseek(fd, 0, SEEK_END);
      if(filesize < width * height * sizeof(T)) {
	filesize = width * height * sizeof(T);
	lseek(fd, filesize - 1, SEEK_SET);
	if(write(fd, "\0", 1) != 1) {
	  debug(TM, "can't open file: %s", filename.c_str());
	  return RET_ERR;
	}
      }
      
      // map the file into memory
      if((mem = (T *) mmap(NULL, filesize,
			   PROT_READ | PROT_WRITE, 
			   MAP_FILE | MAP_SHARED, fd, 0)) == (void *)(-1)) {
	debug(TM, "mmap failed for %s", filename.c_str());
	close(fd);
	return RET_ERR;
      }
      
      return RET_OK;
    }
  }
  

  /**
   * On 32 bit architectures it might be neccessary to temporarily unmap data files.
   * This function should be used to unmap the data file from address space.
   * @see reactivate_mapping()
   * @todo Remove this method.
   */
  /*  template <typename T>
  ret_t MemoryMap<T>::deactivate_mapping() {
    ret_t ret = RET_OK;
    
    if(is_mem() ) return RET_ERR;
    //assert(mem != NULL);
    
    if(mem != NULL) {
      
      if(msync(mem, filesize, MS_SYNC) == -1) {
	debug(TM, "msync() failed");
	ret = RET_ERR;
      }
      
      if(munmap(mem, filesize) == -1) {
	debug(TM, "munmap failed");
	ret = RET_ERR;
      }
      
      mem = NULL;
    }
    else 
      ret = RET_ERR;
    return ret;
    }*/
  
  /**
   * On 32 bit architectures it might be neccessary to temporarily unmap data files.
   * This function should be used to map the data file again into address space.
   * @see gr_deactivate_mapping()
   * @todo Remove this method.
   */
  /*  template <typename T>
  ret_t MemoryMap<T>::reactivate_mapping() {
    
    if(fd == 0) {
      debug(TM, "invalid file handle");
      return RET_ERR;
    }
    if(is_mem()) return RET_ERR;
    
    if(mem == NULL) {
      if((mem = (T *) mmap(NULL, filesize,
			   PROT_READ | PROT_WRITE, 
			   MAP_FILE | MAP_SHARED, fd, 0)) == (void *)(-1)) {
	return RET_ERR;
      }
      
      return RET_OK;
    }
    else {
      return RET_ERR;
    }
    }*/

  template <typename T>
  void MemoryMap<T>::raw_copy(void * buf) const {
    assert(mem != NULL);
    memcpy(buf, mem, width * height * sizeof(T));
  }

  
  template <typename T>
  void * MemoryMap<T>::get_void_ptr(unsigned int x, unsigned int y) const {
    if(x + y < width * height)
      return mem + (y * width + x);	
    else {
      debug(TM, "error: out of bounds x=%d, y=%d", x, y);
      assert(1 == 0);
      return NULL;
    }
  }
  
  template <typename T>
  inline void MemoryMap<T>::set(unsigned int x, unsigned int y, T new_val) {
    mem[y * width + x] = new_val;
    /*
    if(x + y * width < width * height)
      mem[y * width + x] = new_val;
    else {
      debug(TM, "error: out of bounds x=%d, y=%d / width=%d, height=%d", x, y, width, height);
      assert(1 == 0);
    }
    */
  }
  
  template <typename T>
  inline T MemoryMap<T>::get(unsigned int x, unsigned int y) const {
    return mem[y * width + x];
    /*
    if(x + y * width < width * height)
      return mem[y * width + x];
    else {
      debug(TM, "error: out of bounds x=%d, y=%d / width=%d, height=%d", x, y, width, height);
      assert(1 == 0);
      return NULL;
    }
    */
  }
  
}

#endif
