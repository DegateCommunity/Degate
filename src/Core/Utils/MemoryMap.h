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

#ifndef __MEMORYMAP_H__
#define __MEMORYMAP_H__

#include "Globals.h"

#include <QDir>
#include <QFile>
#include <QFileDevice>
#include <QObject>
#include <QTemporaryFile>
#include <memory>
#include <utility>

namespace degate
{
    enum MAP_STORAGE_TYPE
    {
        MAP_STORAGE_TYPE_MEM = 0,
        MAP_STORAGE_TYPE_PERSISTENT_FILE = 1,
        MAP_STORAGE_TYPE_TEMP_FILE = 2,
    };


    /**
     * Storage for data objects, that is mapped from files into memory.
     *
     * You should not use this class directly.
     */
    template<typename T>
    class MemoryMap : QObject
    {
    private:
        unsigned int width, height;
        MAP_STORAGE_TYPE storage_type;

        size_t mem_size;
        QFileDevice* backing_file;
        T* mem_view;

    private:
        Q_DISABLE_COPY(MemoryMap)

        ret_t alloc_memory();
        ret_t map_file(QFile* file);
        void unmap();

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
        MemoryMap(unsigned int width, unsigned int height, MAP_STORAGE_TYPE mode, std::string const& file_to_map);

        /**
         * The destructor.
         */
        ~MemoryMap();

        int get_width() const
        {
            return width;
        }
        int get_height() const
        {
            return height;
        }

        /**
         * Cear the whole memory map.
         */
        void clear();

        void clear_area(unsigned int min_x, unsigned int min_y, unsigned int width, unsigned int height);

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
         * Copy all data from a buffer into the memory map. Make sure that the buffer \p buf
         * is large enough and hold get_width() * get_height() * sizeof(T) bytes.
         */
        void raw_set(void* buf) const;

        /**
         * Get the name of the mapped file.
         * @returns Returns a string with the mapped file. If the memory
         *   chunk is heap and not file based, an empty string is returned.
         */
        std::string const get_filename() const
        {
            return (backing_file != nullptr) ? QDir::toNativeSeparators(backing_file->fileName()).toStdString() :
                                               std::string{};
        }

        /**
         * Get data (can be null).
         * @return Returns data.
         */
        T* data()
        {
            return mem_view;
        };
    };

    template<typename T>
    MemoryMap<T>::MemoryMap(unsigned int width, unsigned int height)
        : width(width),
          height(height),
          storage_type(MAP_STORAGE_TYPE_MEM),
          mem_size(width * height * sizeof(T)),
          mem_view(nullptr)
    {
        assert(width > 0 && height > 0);

        ret_t ret = alloc_memory();
        assert(ret == RET_OK);
    }

    template<typename T>
    MemoryMap<T>::MemoryMap(unsigned int width,
                            unsigned int height,
                            MAP_STORAGE_TYPE mode,
                            std::string const& file_to_map)
        : width(width),
          height(height),
          storage_type(mode),
          mem_size(width * height * sizeof(T)),
          mem_view(nullptr)
    {
        assert(mode == MAP_STORAGE_TYPE_PERSISTENT_FILE || mode == MAP_STORAGE_TYPE_TEMP_FILE);

        assert(width > 0 && height > 0);

        ret_t ret;
        std::unique_ptr<QFile> file;
        if (mode == MAP_STORAGE_TYPE_TEMP_FILE)
        {
            // Random filename
            auto temporary_file = std::make_unique<QTemporaryFile>();
            temporary_file->setAutoRemove(true);
            if (temporary_file->open())
            {
                file = std::move(temporary_file);
            }
        }
        else
        {
            auto persistent_file = std::make_unique<QFile>(QString::fromStdString(file_to_map));
            if (persistent_file->open(QFile::ReadWrite))
            {
                file = std::move(persistent_file);
            }
        }

        if (!file)
        {
            debug(TM,
                  "Can't open a %s file named \"%s\"",
                  mode == MAP_STORAGE_TYPE_TEMP_FILE ? "temporary" : "persistent",
                  mode == MAP_STORAGE_TYPE_TEMP_FILE ? "<temporary>" : file_to_map.c_str());
            return;
        }

        ret = map_file(file.get());
        if (RET_IS_NOT_OK(ret))
        {
            debug(TM, "Can't map memory from %s", file->fileName().toLatin1().constData());
            file->close();
        }
        else
        {
            backing_file = file.release();
        }

        assert(RET_IS_OK(ret));
    }


    template<typename T>
    MemoryMap<T>::~MemoryMap()
    {
        switch (storage_type)
        {
            case MAP_STORAGE_TYPE_MEM:
                delete[] mem_view;
                break;

            case MAP_STORAGE_TYPE_PERSISTENT_FILE:
            case MAP_STORAGE_TYPE_TEMP_FILE:
                unmap();
                break;
        }
    }

    template<typename T>
    void MemoryMap<T>::unmap()
    {
        if (mem_view && backing_file)
        {
            backing_file->unmap(reinterpret_cast<uchar*>(mem_view));
            mem_view = nullptr;
        }

        if (backing_file && backing_file->isOpen())
        {
            backing_file->close();
            delete backing_file;
        }

        backing_file = nullptr;
    }

    template<typename T>
    ret_t MemoryMap<T>::alloc_memory()
    {
        /* If it is not null, it would indicate that there is already an allocation. */
        assert(mem_view == nullptr);

        assert(is_mem());

        mem_view = new (std::nothrow) T[static_cast<std::size_t>(width) * static_cast<std::size_t>(height)]();
        assert(mem_view != nullptr);
        if (mem_view == nullptr)
        {
            return RET_MALLOC_FAILED;
        }

        return RET_OK;
    }

    /**
     * Clear map data.
     */
    template<typename T>
    void MemoryMap<T>::clear()
    {
        assert(mem_view != nullptr);
        if (mem_view != nullptr)
            memset(mem_view, 0, width * height * sizeof(T));
    }


    /**
     * Clear an area given by start point and width and height
     */
    template<typename T>
    void MemoryMap<T>::clear_area(unsigned int min_x, unsigned int min_y, unsigned int width, unsigned int height)
    {
        assert(mem_view != nullptr);

        if (mem_view != nullptr)
        {
            for (auto y = min_y; y < min_y + height; y++)
                memset(get_void_ptr(min_x, y), 0, width * sizeof(T));
        }
    }

    /**
     * Use storage in file as storage for memory map
     */
    template<typename T>
    ret_t MemoryMap<T>::map_file(QFile* file)
    {
        assert(file);
        assert(is_persistent_file() || is_temp_file());

        if (!file->resize(mem_size))
        {
            debug(TM, "cannot resize file %s", file->fileName().toLatin1().constData());
            return RET_ERR;
        }

        mem_view = reinterpret_cast<T*>(file->map(0, file->size()));
        if (mem_view == nullptr)
        {
            debug(TM, "cannot create memory map");
            return RET_ERR;
        }

        return RET_OK;
    }


    template<typename T>
    void MemoryMap<T>::raw_copy(void* buf) const
    {
        assert(mem_view != nullptr);
        memcpy(buf, mem_view, static_cast<std::size_t>(width) * static_cast<std::size_t>(height) * sizeof(T));
    }


    template<typename T>
    void MemoryMap<T>::raw_set(void* buf) const
    {
        assert(mem_view != nullptr);
        memcpy(mem_view, buf, static_cast<std::size_t>(width) * static_cast<std::size_t>(height) * sizeof(T));
    }


    template<typename T>
    void* MemoryMap<T>::get_void_ptr(unsigned int x, unsigned int y) const
    {
        if (x + y < width * height)
            return mem_view + (y * width + x);
        else
        {
            debug(TM, "error: out of bounds x=%d, y=%d", x, y);
            assert(1 == 0);
            return nullptr;
        }
    }

    template<typename T>
    inline void MemoryMap<T>::set(unsigned int x, unsigned int y, T new_val)
    {
        if (x >= width || y >= height)
        {
            debug(TM, "error: out of bounds x=%d, y=%d / width=%d, height=%d", x, y, width, height);
        }
        assert(x < width && y < height);
        mem_view[y * width + x] = new_val;
    }

    template<typename T>
    inline T MemoryMap<T>::get(unsigned int x, unsigned int y) const
    {
        if (x >= width || y >= height)
        {
            debug(TM, "error: out of bounds x=%d, y=%d / width=%d, height=%d", x, y, width, height);
        }

        assert(x < width && y < height);
        return mem_view[y * width + x];
    }
} // namespace degate

#endif
