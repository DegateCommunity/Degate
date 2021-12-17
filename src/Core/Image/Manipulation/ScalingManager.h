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

#ifndef __SCALINGMANAGER_H__
#define __SCALINGMANAGER_H__

#include "Core/Image/Image.h"

#include <map>
#include <cassert>
#include <algorithm>

namespace degate
{
    /**
     * The ScalingManager creates prescaled images for a master image.
     * You can use the ScalingManager only for images of type TileImage.
     * If you want to scale images for yourself, please check out method
     * scale_down().
     *
     * @see ImageManipulation::scale_down()
     */
    template <class ImageType>
    class ScalingManager
    {
    public:

        typedef std::pair<double, std::shared_ptr<ImageType>> image_map_element;
        typedef std::map<double, /* scaling */
                         std::shared_ptr<ImageType>> image_map;

    private:
        std::string base_directory;

        image_map images;

        unsigned int min_size;

        ProjectType project_type;

    private:

        unsigned long get_nearest_power_of_two(unsigned int value)
        {
            unsigned int i = 1;

            if (value == 0) return 1;
            for (;;)
            {
                if (value == 1) return i;
                else if (value == 3) return i * 4;
                value >>= 1;
                i *= 2;
            }
        }


    public:

        typedef std::list<double> zoom_step_list;

        /**
         * Create a new ScalingManager object for an image.
         * @param img The background image.
         * @param base_directory A path where all files can be stored. You
         *  can use the path of the master image for that. Make
         *  sure that the path exist. Not used if attached mode.
         * @param project_type The type of the project (normal or attached).
         * @param min_size Create down scalings until the edge length
         *  is becomes less than \p min_size.
         */
        ScalingManager(std::shared_ptr<ImageType> img,
                       std::string const& base_directory,
                       ProjectType project_type,
                       int min_size = 1024)
        {
            assert(img != nullptr);
            this->base_directory = base_directory;
            images[1] = img;
            this->min_size = min_size;

            this->project_type = project_type;

            assert(file_exists(base_directory));
        }

        /**
         * Destroy a scaling manager. This will destroy images anf their files
         * and directories if the image is not persistent.
         */
        ~ScalingManager()
        {
        }

        /**
         * Get a list of available zoom steps.
         * @return Returns a std::list<double> with the zoom steps. A value larger
         *    than 1 indicates a downscaled version. The non scaled version with
         *    scale factor 1.0 is stored in the list, too.
         */
        const zoom_step_list get_zoom_steps() const
        {
            zoom_step_list steps;

            for (typename image_map::const_iterator iter = images.begin();
                 iter != images.end(); ++iter)
                steps.push_back((*iter).first);

            steps.sort();
            return steps;
        }

        /**
         * Create the scaled images.
         * 
         * Created prescaled images that have the same peristence state as the
         * master image. The files are written into the path, where the
         * master image is stored.
         * 
         * This have no other effect than creating virtually the base image in multi 
         * scale for the attached mode (real loading is dynamic).
         * 
         * @throw InvalidPathException This exception is thrown, if the
         *   \p path (ctor param) doesn't exists.
         */
        void create_scalings()
        {
            // If normal mode, then check if directory exists
            if (!(file_exists(base_directory) && is_directory(base_directory)) && project_type == ProjectType::Normal)
                throw InvalidPathException("The path for prescaled images must exist. But it is not there.");

            std::shared_ptr<ImageType> last_img = images[1];
            unsigned int w = last_img->get_width();
            unsigned int h = last_img->get_height();

            // Create scalings
            for (int i = 2; ((h > min_size) || (w > min_size)) && (i < (1 << 24)); // max 24 scaling levels
                 i *= 2)
            {
                w >>= 1;
                h >>= 1;

                std::string path;

                // Get the path to use
                if (project_type == ProjectType::Normal)
                {
                    // Create a new image directory name.
                    char dir_name[PATH_MAX];
                    snprintf(dir_name, sizeof(dir_name), "scaling_%d.dimg", i);

                    path = join_pathes(images[1]->get_path(), std::string(dir_name));
                }
                else
                    path = images[1]->get_path();

                // Create the scaling if needed
                debug(TM, "create scaled image in %s for scaling factor %d?", path.c_str(), i);
                if (!file_exists(path) && project_type == ProjectType::Normal)
                {
                    debug(TM, "yes");
                    create_directory(path);

                    // Load the base image
                    std::shared_ptr<ImageType> new_img(new ImageType(w, h, path, images[1]->is_persistent(), i));

                    // Scale down
                    scale_down_by_2<ImageType, ImageType>(new_img, last_img);
                    last_img = new_img;
                }
                else
                {
                    debug(TM, "no");

                    // Load the image
                    std::shared_ptr<ImageType> new_img(new ImageType(w, h, path, images[1]->is_persistent(), i));

                    last_img = new_img;
                }

                images[i] = last_img;
            }
        }

        /**
         * Get the image with the nearest scaling value to the requested scaling.
         * @return Returns a std::pair<double, shared_ptr> with the scaling
         *    factor and a shared pointer to the image.
         */
        image_map_element get_image(double request_scaling)
        {
            unsigned int factor;
            if (request_scaling > 1)
            {
                factor = std::min(get_nearest_power_of_two(lrint(request_scaling)),
                                  (unsigned long)lrint(images.rbegin()->first));
                typename image_map::iterator found = images.find(factor);
                assert(found != images.end());
                return *found;
            }

            //debug(TM, "return normal image");
            return image_map_element(1, images[1]);
        }

        /**
         * Get the image list.
         *
         * @return Returns the image list.
         */
        image_map get_images()
        {
            return images;
        }
    };

    /**
     * A typedef for scaling managers that handle background images.
     */
    typedef std::shared_ptr<ScalingManager<BackgroundImage>> ScalingManager_shptr;
}

#endif
