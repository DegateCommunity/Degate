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

#include "Prerequisites.h"
#include "globals.h"

#include <string>

#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <cstring>
#include <ctime>
#include <sys/types.h>
#include <fcntl.h>
#include <cassert>
#include <climits>
#include <cmath>
#include <memory>
#include <boost/utility.hpp>

#if defined(SYS_WINDOWS)
#define NOMINMAX
#include <Windows.h>
#include <io.h>
#define PATH_MAX MAX_PATH
#elif defined(SYS_UNIX)
#include <sys/mman.h>
#include <unistd.h>
#include <limits.h> // PATH_MAX for UNIX
#else
#error "Unknown architecture"
#endif

namespace degate
{
	enum MAP_STORAGE_TYPE
	{
		MAP_STORAGE_TYPE_MEM = 0,
		MAP_STORAGE_TYPE_PERSISTENT_FILE = 1,
		MAP_STORAGE_TYPE_TEMP_FILE = 2,
	};


	/*
	 * Storage for data objects, that is mapped from files into memory.
	 *
	 * You should not use this class directly.
	 */
	template <typename T>
	class MemoryMap : boost::noncopyable
	{
	private:
		unsigned int width, height;
		MAP_STORAGE_TYPE storage_type;

		std::string filename;
		size_t filesize;
		size_t mem_size;

		#ifdef SYS_WINDOWS
			typedef void* fd;
			void* mem_file;
		#else
			typedef int fd;
		#endif

		fd file;
		T* mem_view;

	private:
		ret_t alloc_memory();
		ret_t map_file(std::string const& filename);

		void* get_void_ptr(unsigned int x, unsigned int y) const;

		bool is_temp_file() const
		{
			return storage_type == MAP_STORAGE_TYPE_TEMP_FILE;
		}

		bool is_persistent_file() const
		{
			return storage_type == MAP_STORAGE_TYPE_PERSISTENT_FILE;
		}

		bool is_mem() const
		{
			return storage_type == MAP_STORAGE_TYPE_MEM;
		}

	public:


		/**
		 * Allocate a heap based memory chunk.
		 * @param width The width of a 2D map.
		 * @param height The height of a 2D map.
		 */
		MemoryMap(unsigned int width, unsigned int height);

		/**
		 * Create a file based memory chunk.
		 * The storage is filebases. The file is mapped into memory.
		 * @param width The width of a 2D map.
		 * @param height The height of a 2D map.
		 * @param mode Is either MAP_STORAGE_TYPE_PERSISTENT_FILE or MAP_STORAGE_TYPE_TEMP_FILE.
		 * @param file_to_map The name of the file, which should be mmap().
		 */
		MemoryMap(unsigned int width, unsigned int height,
				  MAP_STORAGE_TYPE mode, std::string const& file_to_map);

		/**
		 * The destructor.
		 */
		~MemoryMap();

		int get_width() const { return width; }
		int get_height() const { return height; }

		/**
		 * Cear the whole memory map.
		 */
		void clear();

		void clear_area(unsigned int min_x, unsigned int min_y,
						unsigned int width, unsigned int height);

		/**
		 * Set the value of a memory element.
		 */
		inline void set(unsigned int x, unsigned int y, T new_val);

		/**
		 * Get the value of an memory element.
		 */
		inline T get(unsigned int x, unsigned int y) const;

		/**
		 * Copy the whole memory content into a buffer. Make sure that the buffer \p buf
		 * is large enough to hold get_width() * get_height() * sizeof(T) bytes.
		 */
		void raw_copy(void* buf) const;

		/**
		 * Get the name of the mapped file.
		 * @returns Returns a string with the mapped file. If the memory
		 *   chunk is heap and not file based, an empty string is returned.
		 */
		std::string const& get_filename() const { return filename; }
	};

	template <typename T>
	MemoryMap<T>::MemoryMap(unsigned int _width, unsigned int _height) :
		width(_width), height(_height),
		storage_type(MAP_STORAGE_TYPE_MEM),
		filename(),
		filesize(0),
		mem_size(_width * _height * sizeof(T)),
		file(0),
		#ifdef SYS_WINDOWS
		mem_file(NULL),
		#endif
		mem_view(NULL)
	{
		assert(width > 0 && height > 0);

		ret_t ret = alloc_memory();
		assert(ret == RET_OK);
	}

	template <typename T>
	MemoryMap<T>::MemoryMap(unsigned int _width, unsigned int _height,
							MAP_STORAGE_TYPE mode, std::string const& file_to_map) :
		width(_width), height(_height),
		storage_type(mode),
		filename(file_to_map),
		file(0),
		filesize(0),
		mem_size(_width * _height * sizeof(T)),
		#ifdef SYS_WINDOWS
		mem_file(NULL),
		#endif
		mem_view(NULL)
	{
		assert(mode == MAP_STORAGE_TYPE_PERSISTENT_FILE || mode == MAP_STORAGE_TYPE_TEMP_FILE);

		assert(width > 0 && height > 0);

		ret_t ret;

		if (mode == MAP_STORAGE_TYPE_TEMP_FILE)
		{
			// Random filename
			std::string fn = std::string(std::tmpnam(nullptr));
			ret = map_file(fn);
			if (RET_IS_NOT_OK(ret)) debug(TM, "Can't open a temp file with pattern %s", fn.c_str());
			assert(RET_IS_OK(ret));
		}
		else if (mode == MAP_STORAGE_TYPE_PERSISTENT_FILE)
		{
			ret = map_file(file_to_map);
			if (RET_IS_NOT_OK(ret)) debug(TM, "Can't open file %s as persistent file", file_to_map.c_str());
			assert(RET_IS_OK(ret));
		}
	}


	template <typename T>
	MemoryMap<T>::~MemoryMap()
	{
		switch (storage_type)
		{
		case MAP_STORAGE_TYPE_MEM:

			if (mem_view != NULL) free(mem_view);
			mem_view = NULL;

			break;
		case MAP_STORAGE_TYPE_PERSISTENT_FILE:
			
			break;
		case MAP_STORAGE_TYPE_TEMP_FILE:

			if(mem_view)
			{
				#ifdef SYS_WINDOWS
					UnmapViewOfFile(mem_view);
				#else
					msync(mem_view, filesize, MS_SYNC);
					munmap(mem_view, filesize);
				#endif
				mem_view = NULL;
			}

			#ifdef SYS_WINDOWS
				if(mem_file)
				{
					CloseHandle(mem_file);
					mem_file = NULL;
				}
			#endif

			if(file)
			{
				#ifdef SYS_WINDOWS
					CloseHandle(file);
				#else
					close(file);
				#endif
				file = 0;
			}

			filesize = 0;

			if(remove(filename.c_str()) != 0) // For Linux can be unlink
			{
				debug(TM, "can't delete file: %s", filename.c_str());
			}

			break;
		}
	}

	template <typename T>
	ret_t MemoryMap<T>::alloc_memory()
	{
		/* If it is not null, it would indicates,
		   that there is already any allocation. */
		assert(mem_view == NULL);

		assert(is_mem());

		mem_view = (T*)malloc(width * height * sizeof(T));
		assert(mem_view != NULL);
		if (mem_view == NULL) return RET_MALLOC_FAILED;

		memset(mem_view, 0, width * height * sizeof(T));

		return RET_OK;
	}

	/**
	 * Clear map data.
	 */

	template <typename T>
	void MemoryMap<T>::clear()
	{
		assert(mem_view != NULL);
		if (mem_view != NULL) memset(mem_view, 0, width * height * sizeof(T));
	}


	/**
	 * Clear an area given by start point and width and height
	 */
	template <typename T>
	void MemoryMap<T>::clear_area(unsigned int min_x, unsigned int min_y,
								  unsigned int width, unsigned int height)
	{
		assert(mem_view != NULL);

		if (mem_view != NULL)
		{
			unsigned int x, y;
			for (y = min_y; y < min_y + height; y++)
				memset(get_void_ptr(x, y), 0, width * sizeof(T));
		}
	}


	/**
	 * Use storage in file as storage for memory map
	 */
	template <typename T>
	ret_t MemoryMap<T>::map_file(std::string const& filename)
	{
		assert(is_persistent_file() || is_temp_file());

		this->filename = filename;

		#ifdef SYS_WINDOWS

			file = CreateFileA(filename.c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
			if(!file)
			{
				debug(TM, "can't open file: %s", filename.c_str());
				return RET_ERR;
			}

			LARGE_INTEGER res;
			if(!GetFileSizeEx(file, &res))
			{
				debug(TM, "can't get size of file: %s", filename.c_str());
				return RET_ERR;
			}
			filesize = static_cast<size_t>(res.QuadPart);

			if(filesize < mem_size)
			{
				filesize = mem_size;

				DWORD res = SetFilePointer(file, filesize - 1, NULL, FILE_BEGIN);
				if(res == INVALID_SET_FILE_POINTER)
				{
					debug(TM, "can't set file pointer of file: %s", filename.c_str());
					return RET_ERR;
				}

				DWORD dwBytesWritten = 0;
				char str[] = "\0";

				bool write_res = WriteFile(file, str, strlen(str), &dwBytesWritten, NULL);
				if(!write_res)
				{
					debug(TM, "can't write to file: %s", filename.c_str());
					return RET_ERR;
				}
			}

			mem_file = CreateFileMapping(file, NULL, PAGE_READWRITE, 0, 0, NULL);
			if(!mem_file)
			{
				debug(TM, "can't map file: %s", filename.c_str());
				return RET_ERR;
			}

		#else

			file = open(filename.c_str(), O_RDWR | O_CREAT, 0600)
			if(file == -1)
			{
				debug(TM, "can't open file: %s", filename.c_str());
				return RET_ERR;
			}

			struct stat64 inf;
			if(fstat64(file, &inf) < 0)
			{
				debug(TM, "can't get the size of file: %s", filename.c_str());
				return RET_ERR;
			}

			filesize = inf.st_size;

			if (filesize < mem_size)
			{
				filesize = mem_size;
				lseek(file, filesize - 1, SEEK_SET);
				if (write(file, "\0", 1) != 1)
				{
					debug(TM, "can't open file: %s", filename.c_str());
					return RET_ERR;
				}
			}

		#endif

		assert(filesize == mem_size);

		#ifdef SYS_WINDOWS

			mem_view = (T*)MapViewOfFile(mem_file, FILE_MAP_ALL_ACCESS, 0, 0, mem_size);

			if(!mem_view)
			{
				debug(TM, "can't create memory map");
				return RET_ERR;
			}

		#else

			mem_view = (T*)mmap(NULL, filesize, PROT_READ | PROT_WRITE, MAP_FILE | MAP_SHARED, fd, 0);

			if(mem_view == (void*)(-1))
			{
				debug(TM, "can't create memory map");
				return RET_ERR;
			}

		#endif

		return RET_OK;
	}


	template <typename T>
	void MemoryMap<T>::raw_copy(void* buf) const
	{
		assert(mem_view != NULL);
		memcpy(buf, mem_view, width * height * sizeof(T));
	}


	template <typename T>
	void* MemoryMap<T>::get_void_ptr(unsigned int x, unsigned int y) const
	{
		if (x + y < width * height)
			return mem_view + (y * width + x);
		else
		{
			debug(TM, "error: out of bounds x=%d, y=%d", x, y);
			assert(1 == 0);
			return NULL;
		}
	}

	template <typename T>
	inline void MemoryMap<T>::set(unsigned int x, unsigned int y, T new_val)
	{
		if (x >= width || y >= height)
		{
			debug(TM, "error: out of bounds x=%d, y=%d / width=%d, height=%d", x, y, width, height);
		}
		assert(x < width && y < height);
		mem_view[y * width + x] = new_val;
	}

	template <typename T>
	inline T MemoryMap<T>::get(unsigned int x, unsigned int y) const
	{
		if (x >= width || y >= height)
		{
			debug(TM, "error: out of bounds x=%d, y=%d / width=%d, height=%d", x, y, width, height);
		}

		assert(x < width && y < height);
		return mem_view[y * width + x];
	}
}

#endif
