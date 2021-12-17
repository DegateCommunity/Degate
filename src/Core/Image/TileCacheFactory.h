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

#ifndef __TILECACHEFACTORY_H__
#define __TILECACHEFACTORY_H__

#include "Core/Image/TileCacheAttached.h"
#include "Core/Image/TileCacheStore.h"
#include "Core/Primitive/SingletonBase.h"

namespace degate
{
    /**
     * @class TileCacheFactory
     * @brief Create appropriete tile cache regarding the type of the current loaded project.
     */
    class TileCacheFactory : public SingletonBase<TileCacheFactory>
    {
    public:
        
        /**
         * Generate a new appropriete tile cache.
         * 
         * @param path : is either the image path or the path with saved image in Degate tile image format.
         * @param tile_width_exp : the width (and height) for image tiles. This
         *      value is specified as an exponent to the base 2. This means for
         *      example that if you want to use a width of 1024 pixel, you have
         *      to give a value of 10, because 2^10 is 1024.
         * @param scale : the scale to apply when loading the image (e.g. scale = 2
         *      will load the image with final size of width/2 and height/2). 
         *      @see ScalingManager.
         */
        template<typename PixelPolicy>
        inline std::shared_ptr<TileCache<PixelPolicy>> generate(std::string path,
                                                                unsigned int tile_width_exp,
                                                                unsigned int scale = 1)
        {
            switch (project_type)
            {
                case ProjectType::Normal:
                    return std::make_shared<TileCacheStore<PixelPolicy>>(path, tile_width_exp, scale);
                    break;
                case ProjectType::Attached:
                    return std::make_shared<TileCacheAttached<PixelPolicy>>(path, tile_width_exp, scale);
                    break;
                default:
                    return std::make_shared<TileCacheStore<PixelPolicy>>(path, tile_width_exp);
                    break;
            }
        }

        /**
         * Set the project type to use when creating new tile cache.
         * 
         * If the project type change, make sure to destroy all existing
         * tile cache and recreate them.
         */
        inline void set_project_type(ProjectType project_type)
        {
            this->project_type = project_type;
        }

        /**
         * Get the current project type used to generate new tile cache.
         */
        inline ProjectType get_project_type()
        {
            return project_type;
        }

    private:
        ProjectType project_type = ProjectType::Normal;
    };
} // namespace degate

#endif //__TILECACHEFACTORY_H__
