/* -*-c++-*-
 
 This file is part of the IC reverse engineering tool degate.
 
 Copyright 2008, 2009 by Martin Schobert
 
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

#ifndef __EXPORTER_H__
#define __EXPORTER_H__

#include "globals.h"
#include "Image.h"

#include <stdexcept>
#include <sstream>

namespace degate {

  /**
   * This is the  base class for exporter classes.
   */
  class Exporter {
    
  protected:

    /**
     * Convert a number type to a human readable string.
     */
    template<typename T> std::string number_to_string(T num) {
      std::ostringstream stm;
      stm << num;
      return stm.str();
    }

    /**
     * Convert a RGBA color value into the common format of "#%2x%2x%2x%2x".
     */
    std::string to_color_string(color_t col) const {
      char buf[100];
      snprintf(buf, sizeof(buf), "#%02X%02X%02X%02X",
	       MASK_R(col),
	       MASK_G(col),
	       MASK_B(col),
	       MASK_A(col));
      return std::string(buf);
    }
    
  public:

    /**
     * The ctor for an exporter object.
     */
    Exporter() {};

    /**
     * The dtor for an exporter object.
     */
    virtual ~Exporter() {};
  };
  
}

#endif
