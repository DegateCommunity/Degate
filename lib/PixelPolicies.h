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

#ifndef __PIXELPOLICIES_H__
#define __PIXELPOLICIES_H__

#include "globals.h"
#include "MemoryMap.h"

namespace degate {

  enum IMAGE_TYPE {
    IMAGE_TYPE_GS_BYTE = 1,
    IMAGE_TYPE_GS_DOUBLE = 2,
    IMAGE_TYPE_RGBA = 3
  };

  typedef uint8_t gs_byte_pixel_t;
  typedef double gs_double_pixel_t;
  typedef uint32_t rgba_pixel_t;

  /* -------------------------------------------------------------------------- *
   * pixel type policies
   * -------------------------------------------------------------------------- */
   

  /**
   * Base class for image policies.
   */ 
  class PixelPolicy_Base {
  };
  
  /* 
   * Image policy for RGBA images. Each channel is a 8 bit value. 
   */

  class PixelPolicy_RGBA : public PixelPolicy_Base {
  protected:
    static const IMAGE_TYPE image_type = IMAGE_TYPE_RGBA;
    
  public:
    typedef rgba_pixel_t pixel_type;
    static bool is_multi_channel() { return true; }

  };
  
  /** 
   * Represents a greyscale image pixel policy. Each pixel value is a double. 
   */

  class PixelPolicy_GS_DOUBLE : public PixelPolicy_Base {
  protected:
    static const IMAGE_TYPE image_type = IMAGE_TYPE_GS_DOUBLE;
  public:
    typedef gs_double_pixel_t pixel_type;
    static bool is_single_channel() { return true; }
  };
  
  /** 
   * Represents a greyscale image pixel policy. Each pixel value is a 8 bit value. 
   */

  class PixelPolicy_GS_BYTE : public PixelPolicy_Base {
  protected:
    static const IMAGE_TYPE image_type = IMAGE_TYPE_GS_BYTE;
  public:
    typedef gs_byte_pixel_t pixel_type;
    static bool is_single_channel() { return true; }
  };

}

#endif
