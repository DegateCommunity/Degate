/**
 * This file is part of the IC reverse engineering tool Degate.
 *
 * Copyright 2008, 2009, 2010 by Martin Schobert
 * Copyright 2019-2024 Dorian Bachelot
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

#ifndef __CORE_UTILS_ITERATOR_H__
#define __CORE_UTILS_ITERATOR_H__

#include <cstddef>

namespace degate
{
    /**
     *  @brief  Common %iterator class.
     *
     *  This class does nothing but define nested typedefs.  %Iterator classes
     *  can inherit from this class to save some work.  The typedefs are then
     *  used in specializations and overloading.
     *
     *  In particular, there are no default implementations of requirements
     *  such as @c operator++ and the like.  (How could there be?)
     */
    template<typename _Category,
             typename _Tp,
             typename _Distance = std::ptrdiff_t,
             typename _Pointer = _Tp*,
             typename _Reference = _Tp&>
    struct iterator
    {
        /// One of the @link iterator_tags tag types@endlink.
        typedef _Category iterator_category;
        /// The type "pointed to" by the iterator.
        typedef _Tp value_type;
        /// Distance between iterators is represented as this type.
        typedef _Distance difference_type;
        /// This type represents a pointer-to-value_type.
        typedef _Pointer pointer;
        /// This type represents a reference-to-value_type.
        typedef _Reference reference;
    };
} // namespace degate

#endif
