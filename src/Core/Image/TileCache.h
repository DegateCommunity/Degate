/**
 * This file is part of the IC reverse engineering tool Degate.
 *
 * Copyright 2008, 2009, 2010 by Martin Schobert
 * Copyright 2021 Dorian Bachelot
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

#ifndef __TILECACHE_H__
#define __TILECACHE_H__

#include "Core/Image/TileImage.h"
#include "Core/Image/TileCacheBase.h"
#include "Core/Image/GlobalTileCache.h"
#include "GUI/Workspace/WorkspaceNotifier.h"
#include "Core/Utils/FileSystem.h"
#include "Core/Utils/MemoryMap.h"

#include <QtConcurrent/QtConcurrent>
#include <QImageReader>
#include <cmath>

namespace degate
{
    /**
     * @enum TileLoadingType
     * @brief Defines the different tile loading types.
     */
    enum class TileLoadingType
    {
        Sync,
        Async
    };

    /**
     * @class TileCache
     * @brief Tile cache class.
     * 
     * It supports two types of tile loading depending on the type of the image.
     * If it's in Degate's internal format, then it will use memory mapping from
     * file. Otherwise, it will dynamically load tiles in memory.
     * This is the main point of difference between Attached and Normal project modes.
     */
    template<class PixelPolicy>
    class TileCache : public TileCacheBase
    {
        friend class GlobalTileCache<PixelPolicy>;

    public:

        /**
         * Create a new tile cache.
         * 
         * It supports two types of tile loading depending on the type of the image.
         * If it's in Degate's internal format, then it will use memory mapping from
         * file. Otherwise, it will dynamically load tiles in memory.
         * This is the main point of difference between Attached and Normal project modes.
         * 
         * @param path : the path to the image (can be to a file or a path for example).
         * @param tile_width_exp : the width (and height) for image tiles. This
         *      value is specified as an exponent to the base 2. This means for
         *      example that if you want to use a width of 1024 pixel, you have
         *      to give a value of 10, because 2^10 is 1024.
         * @param scale : the scale to apply when loading the image (e.g. scale = 2
         *      will load the image with final size of width/2 and height/2). 
         *      @see ScalingManager.
         * @param loading_type : the loading type to use when loading a new tile.
         *      If using Degate's image format, only sync is supported.
         * @param notification_list : the list of workspace notification(s) to notify
         *      after a new loading finished. This is done only if async loading type.
         */
        inline TileCache(std::string path, 
                         unsigned int tile_width_exp, 
                         unsigned int scale, 
                         TileLoadingType loading_type, 
                         const WorkspaceNotificationList& notification_list)
            : path(std::move(path)),
              tile_width_exp(tile_width_exp),
              scale(scale),
              loading_type(loading_type),
              notification_list(notification_list),
              tile_size(1 << tile_width_exp)
        {
            // Check if Degate's image format
            QImageReader reader(this->path.c_str());
            if (reader.canRead() == false)
            {
                degate_image_format = true;
                return;
            }

            // If the image is a multi-page/multi-res, we take the page with the biggest resolution.
            if (reader.imageCount() > 1)
            {
                QSize best_size{0, 0};
                for (int i = 0; i < reader.imageCount(); i++)
                {
                    if (best_size.width() < reader.size().width() || best_size.height() < reader.size().height())
                    {
                        best_size = reader.size();
                        best_image_number = reader.currentImageNumber();
                    }

                    reader.jumpToNextImage();
                }

                reader.jumpToImage(best_image_number);
            }

            // Size
            size = reader.size();
            if (!size.isValid())
            {
                debug(TM, "Can't read size of %s.\n", this->path.c_str());
                return;
            }

            // Scaled size conversion
            auto h = static_cast<unsigned int>(round(log(scale) / log(2)));
            scaled_size = QSize{size.width() >> h, size.height() >> h};

            // Create loading tile
            loading_tile = std::make_shared<MemoryMap<typename PixelPolicy::pixel_type>>(tile_size, tile_size);
        }

        /**
         * Release memory on destroy.
         */
        inline ~TileCache()
        {
            std::lock_guard<std::mutex> lock(mtx);

            // Delete and clear watchers
            for (auto* watcher : watchers)
                delete watcher;
            watchers.clear();

            // Release memory
            release_memory();
        }

        /**
         * Cleanup the cache by removing the oldest entry.
         */
        inline void cleanup_cache() override
        {
            if (cache.size() == 0) return;

            std::lock_guard<std::mutex> lock(mtx);

            // Initialize a clock to store the oldest
            struct timespec oldest_clock_val;
            GET_CLOCK(oldest_clock_val);

            auto oldest = cache.begin();

            // Search for oldest entry
            for (auto iter = cache.begin();
                 iter != cache.end(); ++iter)
            {
                struct timespec clock_val = (*iter).second.second;
                if (clock_val < oldest_clock_val)
                {
                    oldest_clock_val.tv_sec = clock_val.tv_sec;
                    oldest_clock_val.tv_nsec = clock_val.tv_nsec;
                    oldest = iter;
                }
            }

            assert(oldest != cache.end());

            // Release memory
            (*oldest).second.first.reset(); // explicit reset of smart pointer

            // Clean the cache entry
            cache.erase(oldest);

#ifdef TILECACHE_DEBUG
            debug(TM, "local cache: %d entries after remove\n", cache.size());
#endif

            // Update the global tile cache (release the virtual memory)
            GlobalTileCache<PixelPolicy>& gtc = GlobalTileCache<PixelPolicy>::get_instance();
            gtc.release_cache_memory(this, get_image_size());
        }

        /**
         * Release all the memory.
         */
        inline void release_memory()
        {
            if (cache.size() > 0)
            {
                std::lock_guard<std::mutex> lock(mtx);

                // Release the global tile cache (by removing all the used virtual memory by this)
                GlobalTileCache<PixelPolicy>& gtc = GlobalTileCache<PixelPolicy>::get_instance();
                gtc.release_cache_memory(this, cache.size() * get_image_size());

                // Release the memory
                current_tile.reset();
                cache.clear();
            }
        }

        /**
         * Print this cache info.
         */
        inline void print() const override
        {
            for (typename cache_type::const_iterator iter = cache.begin();
                 iter != cache.end(); ++iter)
            {
                std::cout << "\t+ "
                          << path << "/"
                          << (*iter).first << " "
                          << (*iter).second.second.tv_sec
                          << "/"
                          << (*iter).second.second.tv_nsec
                          << std::endl;
            }
        }

        /**
         * Cache all tile around a rect (in a radius).
         * 
         * @param min_x : lower left x coordinate of the rect.
         * @param max_x : upper right x cooredinate of the rect.
         * @param min_y : lower left y coordinate of the rect.
         * @param max_y : upper right y coordinate of the rect.
         * @param max_size_x : max possible x coordinate for the rect.
         * @param max_size_y : max possible y coordinate for the rect.
         * @param radius : radius around the rect to cache (in number of tile unit).
         */
        inline void cache_around(unsigned int min_x,
                                 unsigned int max_x,
                                 unsigned int min_y,
                                 unsigned int max_y,
                                 unsigned int max_size_x,
                                 unsigned int max_size_y,
                                 unsigned int radius)
        {
            unsigned int tile_num_min_x = min_x >> tile_width_exp;
            unsigned int tile_num_max_x = max_x >> tile_width_exp;
            unsigned int tile_num_min_y = min_y >> tile_width_exp;
            unsigned int tile_num_max_y = max_y >> tile_width_exp;

            unsigned int tile_num_max_size_x = max_size_x >> tile_width_exp;
            unsigned int tile_num_max_size_y = max_size_y >> tile_width_exp;

            unsigned int cache_min_x = radius > tile_num_min_x ? 0 : tile_num_min_x - radius;
            unsigned int cache_max_x = radius + tile_num_max_x > tile_num_max_size_x ? tile_num_max_size_x : tile_num_max_x + radius;
            unsigned int cache_min_y = radius > tile_num_min_y ? 0 : tile_num_min_y - radius;
            unsigned int cache_max_y = radius + tile_num_max_y > tile_num_max_size_y ? tile_num_max_size_y : tile_num_max_y + radius;

            if (static_cast<uint_fast64_t>(cache_max_x - cache_min_x) *
                        static_cast<uint_fast64_t>(cache_max_y - cache_min_y) *
                        static_cast<uint_fast64_t>(get_image_size()) *
                        static_cast<uint_fast64_t>(sizeof(typename PixelPolicy::pixel_type)) > GlobalTileCache<PixelPolicy>::get_instance().get_max_cache_memory())
            {
                debug(TM, "Cache too small to cache around");

                return;
            }

            for (unsigned int y = cache_min_y; y <= cache_max_y; y++)
            {
                for (unsigned int x = cache_min_x; x <= cache_max_x; x++)
                {
                    //if (y >= tile_num_min_y && y <= tile_num_max_y && x >= tile_num_min_x && x <= tile_num_max_x)
                    //continue;

                    load_tile(x, y);
                }
            }
        }

        /**
         * Get a tile. If the tile is not in the cache, the tile is loaded.
         *
         * @param x Absolut pixel coordinate.
         * @param y Absolut pixel coordinate.
         * @return Returns a shared pointer to a MemoryMap object.
         */
        std::shared_ptr<MemoryMap<typename PixelPolicy::pixel_type>>
        inline get_tile(unsigned int x, unsigned int y)
        {
            unsigned int tile_num_x = x >> tile_width_exp;
            unsigned int tile_num_y = y >> tile_width_exp;

            if (!(current_tile != nullptr && tile_num_x == curr_tile_num_x && tile_num_y == curr_tile_num_y) || current_tile_is_loading)
            {
                load_tile(tile_num_x, tile_num_y, true);
            }

            return current_tile;
        }

        /**
         * Load a new tile and update the cache.
         * 
         * @param x : the x index of the tile (not the real coordinate).
         * @param y : the y index of the tile (not the real coordinate).
         * @param update_current : if true, will update the current_tile pointer, otherwise not.
         */
        inline void load_tile(unsigned int x, unsigned int y, bool update_current = false)
        {
            std::lock_guard<std::mutex> lock(mtx);

            // Check if tile is included in the base image
            // Otherwise return loading tile
            if (!is_included(x, y))
            {
                if (update_current)
                {
                    current_tile = loading_tile;
                    curr_tile_num_x = x;
                    curr_tile_num_y = y;
                    current_tile_is_loading = false;
                }

                return;
            }

            // Get time
            struct timespec now{};
            GET_CLOCK(now);

            // Create a file name from tile number
            auto filename = QString("%1_%2.dat").arg(x).arg(y).toStdString();

            // If filename/object is not in cache, load the tile
            typename cache_type::const_iterator iter = cache.find(filename);

            // If the tile was not found in the cache, then load it
            if (iter == cache.end())
            {
                GlobalTileCache<PixelPolicy>& gtc = GlobalTileCache<PixelPolicy>::get_instance();

                // Allocate memory (global tile cache)
                bool ok = gtc.request_cache_memory(this, get_image_size());
                assert(ok == true);

                if (degate_image_format == false)
                {
                    // Check loading type
                    if (loading_type == TileLoadingType::Async)
                    {
                        // If update_current, then update and set a black loading tile
                        if (update_current)
                        {
                            cache[filename] = std::make_pair(loading_tile, now);
                            current_tile = loading_tile;
                            curr_tile_num_x = x;
                            curr_tile_num_y = y;
                            current_tile_is_loading = true;
                        }

                        // Run in another thread the loading phase of the new tile
                        load_async(x, y, now, filename);

                        // Show the loading tile while waiting for the next update to try to load the real tile image
                        return;
                    }
                    else
                    {
                        // If sync
                        auto temp = load(x, y, tile_size, scaled_size, path, best_image_number);

                        // Prevent overflow
                        if (temp == nullptr)
                            temp = loading_tile;

                        // Update cache
                        cache[filename] = std::make_pair(temp, now);
                    }
                }
                else
                {
                    // Async loading not supported for degate image format (memory map)
                    cache[filename] = std::make_pair(load_degate_image_format(filename), now);
                }

                #ifdef TILECACHE_DEBUG
                gtc.print_table();
                #endif
            }

            // Update entry
            auto entry = cache[filename];
            entry.second = now;

            if (update_current)
            {
                current_tile = entry.first;
                curr_tile_num_x = x;
                curr_tile_num_y = y;
                current_tile_is_loading = false;
            }
        }

    protected:

        /**
         * Get image size in bytes.
         */
        inline uint_fast64_t get_image_size() const
        {
            return sizeof(typename PixelPolicy::pixel_type) * (uint_fast64_t(1) << tile_width_exp) * (uint_fast64_t(1) << tile_width_exp);
        }

        /**
         * Send all notifications regarding the notification list.
         */
        inline void notify()
        {
            for (auto notification : notification_list)
                WorkspaceNotifier::get_instance().notify(notification.first, notification.second);
        }

        /**
         * Load a tile (attached mode/on degate's image format).
         * 
         * @param tile_x : the tile first coordinate (first index).
         * @param tile_y : the tile second coordinate (second index).
         * 
         * @return Returns the new memory map (here, for attached mod, just memory).
         */
        inline
        static
        std::shared_ptr<MemoryMap<typename PixelPolicy::pixel_type>> load(unsigned int tile_x,
                                                                          unsigned int tile_y,
                                                                          unsigned int tile_size,
                                                                          QSize scaled_size,
                                                                          std::string path,
                                                                          int best_image_number)
        {
            // Prepare sizes
            QSize reading_size{static_cast<int>(tile_size), static_cast<int>(tile_size)};
            const QSize read_size{static_cast<int>(tile_x) * static_cast<int>(tile_size), static_cast<int>(tile_y) * static_cast<int>(tile_size)};

            // Check width
            if (reading_size.width() + read_size.width() > scaled_size.width())
                reading_size.setWidth(scaled_size.width() - read_size.width());

            // Check height
            if (reading_size.height() + read_size.height() > scaled_size.height())
                reading_size.setHeight(scaled_size.height() - read_size.height());

            // Check overflow
            if (reading_size.width() <= 0 || reading_size.height() <= 0)
                return nullptr;

            // Create reader
            QImageReader current_reader(path.c_str());

            // Create reading rect
            QRect rect(read_size.width(), read_size.height(), reading_size.width(), reading_size.height());

            // If the image is a multi-page/multi-res, we take the page with the biggest resolution
            if (current_reader.imageCount() > 1)
                current_reader.jumpToImage(best_image_number);

            // Scaled read
            current_reader.setScaledSize(scaled_size);
            current_reader.setScaledClipRect(rect);

            // Set reader reading rect
            QImage img = current_reader.read();
            if (img.isNull())
            {
                debug(TM, "can't read image file when loading a new tile\n");
            }

            // Convert to good format
            if (img.format() != QImage::Format_ARGB32 && img.format() != QImage::Format_RGB32)
            {
                img = img.convertToFormat(QImage::Format_ARGB32);
            }

            // Get data
            const auto *rgb_data = reinterpret_cast<const QRgb*>(&img.constBits()[0]);

            // Create memory map
            auto mem = std::make_shared<MemoryMap<typename PixelPolicy::pixel_type>>(tile_size, tile_size);

            // Prevent overflows
            unsigned int max_x = tile_size > static_cast<unsigned int>(reading_size.width()) ?
                                static_cast<unsigned int>(reading_size.width()) : tile_size;
            unsigned int max_y = tile_size > static_cast<unsigned int>(reading_size.height()) ?
                                static_cast<unsigned int>(reading_size.height()) : tile_size;

            // Fill data
            QRgb rgb;
            for (unsigned int y = 0; y < max_y; y++)
            {
                for (unsigned int x = 0; x < max_x; x++)
                {
                    rgb = rgb_data[y * reading_size.width() + x];
                    mem->set(x, y, MERGE_CHANNELS(qRed(rgb), qGreen(rgb), qBlue(rgb), qAlpha(rgb)));
                }
            }

            return mem;
        }

        /**
         * Load image in degate internal format.
         * 
         * @param filename : just the name of the file to load. The filename is
         *     relative to the path.
         * 
         * @return Returns a memory map (mapped to the corresponding tile file).
         */
        inline
        std::shared_ptr<MemoryMap<typename PixelPolicy::pixel_type>>
        load_degate_image_format(std::string const& filename)
        {
            std::shared_ptr<MemoryMap<typename PixelPolicy::pixel_type>> mem(
                    new MemoryMap<typename PixelPolicy::pixel_type>(
                            uint_fast64_t(1) << tile_width_exp,
                            uint_fast64_t(1) << tile_width_exp,
                            MAP_STORAGE_TYPE_PERSISTENT_FILE,
                            join_pathes(path, filename)));

            return mem;
        }

        /**
         * Check if the tile(x,y) is included in the base image (don't work for degate image format).
         * 
         * @param tile_x : the tile first coordinate (first index).
         * @param tile_y : the tile second coordinate (second index).
         */
        inline bool is_included(unsigned int tile_x, unsigned int tile_y)
        {
            // For historic reasons, Degate's image format will take the project
            // size as image size (even if the real size is less). Also, the 
            // project size is equal to the biggest image size, so no overflow
            // possible. Therefore, always return true in this case.
            if (degate_image_format)
                return true;

            // Check width
            if (static_cast<int>(tile_x) * static_cast<int>(tile_size) >= scaled_size.width())
                return false;

            // Check height
            if (static_cast<int>(tile_y) * static_cast<int>(tile_size) >= scaled_size.height())
                return false;

            return true;
        }

        /**
         * Run load() async, take into account this Tile Cache possible destruction before getting the result.
         */
        inline void load_async(unsigned int x, unsigned int y, struct timespec now, std::string filename)
        {
            // Run the load() function (static) async
            auto future = QtConcurrent::run([=](){
                return load(x, y, tile_size, scaled_size, path, best_image_number);
            });

            // Create a new watcher and add it to the list of watchers
            watchers.push_back(new QFutureWatcher<std::shared_ptr<MemoryMap<typename PixelPolicy::pixel_type>>>(nullptr));
            auto* watcher = watchers.back();

            // Called when loading finished and if the watcher object is still valid (not destroyed).
            QObject::connect(watcher, &QFutureWatcher<std::shared_ptr<MemoryMap<typename PixelPolicy::pixel_type>>>::finished, [=]() {
                // Get the load result (if this lambda was called, then watcher is valid/not destroyed)
                auto temp = watcher->future().result();

                // Prevent overflow
                if (temp == nullptr)
                    temp = loading_tile;

                // Convert to cache type
                auto data = std::make_pair(temp, now);

                // Register the new entry and send notifications (if this lambda was called, then this is valid/not destroyed)
                cache[filename] = data;
                notify();

                // Remove and delete watcher
                watchers.erase(std::remove(watchers.begin(), watchers.end(), watcher), watchers.end());
                delete watcher;
            });

            watcher->setFuture(future);
        }

    private:
        const std::string path;
        const unsigned int tile_width_exp;

        // Cache types.
        typedef std::shared_ptr<MemoryMap<typename PixelPolicy::pixel_type>> MemoryMap_shptr;
        typedef std::map<std::string, // filename
                         std::pair<MemoryMap_shptr, struct timespec>>
                cache_type;

        cache_type cache;

        // Used for caching the working tile.
        MemoryMap_shptr current_tile;
        unsigned curr_tile_num_x = 0;
        unsigned curr_tile_num_y = 0;
        bool current_tile_is_loading = false;

        unsigned int scale;

        std::mutex mtx;

        TileLoadingType loading_type;
        WorkspaceNotificationVector notification_list;

        QSize size;
        QSize scaled_size;
        unsigned int tile_size;
        int best_image_number = -1;
        bool degate_image_format = false;

        std::shared_ptr<MemoryMap<typename PixelPolicy::pixel_type>> loading_tile;

        std::vector<QFutureWatcher<std::shared_ptr<MemoryMap<typename PixelPolicy::pixel_type>>>*> watchers;
    };
} // namespace degate

#endif //__TILECACHE_H__
