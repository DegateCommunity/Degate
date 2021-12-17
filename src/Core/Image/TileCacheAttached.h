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

#ifndef __TILECACHEATTACHED_H__
#define __TILECACHEATTACHED_H__

#include "Core/Configuration.h"
#include "Core/Image/GlobalTileCache.h"
#include "Core/Utils/FileSystem.h"
#include "Core/Utils/MemoryMap.h"

#include <QtConcurrent/QtConcurrent>
#include <QImageReader>
#include <cmath>

namespace degate
{
    /**
     * @class TileCacheAttached
     * @brief Tile cache for attached/linked images.
     */
    template<class PixelPolicy>
    class TileCacheAttached : public TileCache<PixelPolicy>
    {
        friend class GlobalTileCache<PixelPolicy>;

    public:

        /**
         * Create a new Tile cache with attached/linked image.
         * 
         * @param path : the path to the image.
         * @param tile_width_exp : the width (and height) for image tiles. This
         *      value is specified as an exponent to the base 2. This means for
         *      example that if you want to use a width of 1024 pixel, you have
         *      to give a value of 10, because 2^10 is 1024.
         * @param scale : the scale to apply when loading the image (e.g. scale = 2
         *      will load the image with final size of width/2 and height/2). 
         *      @see ScalingManager.
         */
        inline TileCacheAttached(std::string path, unsigned int tile_width_exp, unsigned int scale = 1)
            : TileCache<PixelPolicy>(path, tile_width_exp, scale),
              tile_size(1 << tile_width_exp),
              path(path)
        {
            QImageReader reader(path.c_str());

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
                debug(TM, "can't read size of %s\n", path.c_str());
                return;
            }

            // Scaled size conversion
            auto h = static_cast<unsigned int>(round(log(scale) / log(2)));
            scaled_size = QSize{size.width() >> h, size.height() >> h};

            // Create loading tile
            loading_tile = std::make_shared<MemoryMap<typename PixelPolicy::pixel_type>>(tile_size, tile_size);
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
            // Create a file name from tile number
            char filename[PATH_MAX];
            snprintf(filename, sizeof(filename), "%d_%d.dat", x, y);

            // If filename/object is not in cache, load the tile
            typename TileCache<PixelPolicy>::cache_type::const_iterator iter = TileCache<PixelPolicy>::cache.find(filename);

            // If the tile was not found in the cache, then load it
            if (iter == TileCache<PixelPolicy>::cache.end())
            {
                GlobalTileCache<PixelPolicy>& gtc = GlobalTileCache<PixelPolicy>::get_instance();

                // Allocate memory (global tile cache)
                bool ok = gtc.request_cache_memory(this, TileCache<PixelPolicy>::get_image_size());
                assert(ok == true);
                struct timespec now{};
                GET_CLOCK(now);

                // If update_current, then update and set a black loading tile
                if (update_current)
                {
                    TileCache<PixelPolicy>::cache[filename] = std::make_pair(loading_tile, now);;
                    TileCache<PixelPolicy>::current_tile = loading_tile;
                    TileCache<PixelPolicy>::curr_tile_num_x = x;
                    TileCache<PixelPolicy>::curr_tile_num_y = y;
                }

                // Run in another thread the loading phase of the new tile
                QtConcurrent::run([=]()
                {
                    auto data = std::make_pair(load(x, y), now);
                    TileCache<PixelPolicy>::mutex.lock();
                    TileCache<PixelPolicy>::cache[filename] = data;
                    TileCache<PixelPolicy>::mutex.unlock();
                });

                #ifdef TILECACHE_DEBUG
                gtc.print_table();
                #endif

                // Show the loading tile while waiting for the next update to try to load the real tile image
                return;
            }

            // Get current time
            struct timespec now{};
            GET_CLOCK(now);

            // Update entry
            auto entry = TileCache<PixelPolicy>::cache[filename];
            entry.second = now;

            if (update_current)
            {
                TileCache<PixelPolicy>::current_tile = entry.first;
                TileCache<PixelPolicy>::curr_tile_num_x = x;
                TileCache<PixelPolicy>::curr_tile_num_y = y;
            }
        }

    protected:

        /**
         * Load a tile.
         */
        inline
        std::shared_ptr<MemoryMap<typename PixelPolicy::pixel_type>>
        load(unsigned int x, unsigned int y)
        {
            debug(TM, "load(%d, %d)", x, y);

            // Prepare sizes
            QSize reading_size{static_cast<int>(tile_size), static_cast<int>(tile_size)};
            QSize read_size{static_cast<int>(x) * static_cast<int>(tile_size), static_cast<int>(y) * static_cast<int>(tile_size)};

            // Check width
            if (reading_size.width() + read_size.width() > scaled_size.width())
                reading_size.setWidth(scaled_size.width() - read_size.width());

            // Check height
            if (reading_size.height() + read_size.height() > scaled_size.height())
                reading_size.setHeight(scaled_size.height() - read_size.height());

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

            debug(TM, "finished load(%d, %d)", x, y);

            return mem;
        }

    private:

        QSize size;
        QSize scaled_size;
        unsigned int tile_size;
        std::string path;
        int best_image_number = -1;

        std::shared_ptr<MemoryMap<typename PixelPolicy::pixel_type>> loading_tile;
    };
} // namespace degate

#endif
