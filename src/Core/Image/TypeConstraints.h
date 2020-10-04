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

#ifndef __TYPECONSTRAINTS_H__
#define __TYPECONSTRAINTS_H__

#include "Core/Image/Image.h"
#include "Core/Image/PixelPolicies.h"
#include "Core/Image/StoragePolicies.h"

namespace degate
{
    /**
     * A dummy method to get rid of compiler warnings.
     */
    template <class T>
    inline void ignore_unused_variable_warning(T const&)
    {
    }

    /**
     * Compile time check for single channel image types.
     */
    template <class ImageType>
    struct assert_is_single_channel_image
    {
        static void constraints(ImageType img)
        {
            typedef typename ImageType::pixel_policy PolicyType;
            PolicyType::is_single_channel();
        }

        assert_is_single_channel_image()
        {
            void (*p)(ImageType) = constraints;
            ignore_unused_variable_warning(p);
        }
    };

    /**
     * Compile time check for multi channel image types.
     */
    template <class ImageType>
    struct assert_is_multi_channel_image
    {
        static void constraints(ImageType img)
        {
            typedef typename ImageType::pixel_policy PolicyType;
            PolicyType::is_multi_channel();
        }

        assert_is_multi_channel_image()
        {
            void (*p)(ImageType) = constraints;
            ignore_unused_variable_warning(p);
        }
    };
}

#endif
