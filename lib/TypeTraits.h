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

#ifndef __TYPETRAITS_H__
#define __TYPETRAITS_H__

#include <PixelPolicies.h>

namespace degate {

  /**
   * A pointer trait for the generic type T.
   * @todo use boost version instead.
   */
  
  template<typename T>
  struct is_pointer {
    static const bool value = false;
  };
  
  /**
   * A pointer trait for the generic type T *.
   * @todo use boost version instead.
   */
  
  template<typename T> 
  struct is_pointer<T*> {
    static const bool value = true;
  }; 
  
  /**
   * A pointer trait for the special shared pointer of type T.
   * @todo use boost version instead.
   */
  
  template<typename T> 
  struct is_pointer<std::tr1::shared_ptr<T> > {
    static const bool value = true;
  }; 


  /**
   * Method parameter type trait for pointer and shared pointer.
   */

  template<typename T, bool b> 
  struct call_trait { 
    typedef T param_type;
  };

  /**
   * Method parameter type trait for normal object that should be passed via reference.
   */

  template<typename T>
  struct call_trait<T, false> { 
    typedef T& param_type;
  };


  /**
   * Type trait for multi channel images.
   */

  template<class PixelType>
  struct is_single_channel_image {
    static const bool value = true;
  };

  /**
   * Type trait for single channel images.
   */

  template<>
  struct is_single_channel_image<rgba_pixel_t> {
    static const bool value = false;
  };


}

#endif
