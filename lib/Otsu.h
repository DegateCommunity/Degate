/* -*-c++-*-

 This file is part of the IC reverse engineering tool degate.

 Copyright 2013 by Taekgwan Kim 

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

#ifndef __OTSU_H__
#define __OTSU_H__

#include <Image.h>

namespace degate {

  /**
     Otsu's algorithm for calculating a threshold for image binarysation:
     https://en.wikipedia.org/wiki/Otsu's_method
   */
  class Otsu {

  private:

    double otsu_threshold;

  public:

    Otsu();
    ~Otsu();

    double get_otsu_threshold();

    void run(TileImage_GS_DOUBLE_shptr gray);
  };
}

#endif
