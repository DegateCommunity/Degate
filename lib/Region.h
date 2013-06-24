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

#ifndef __REGION_H__
#define __REGION_H__

typedef std::list<struct endPoint> endPoint_list;
typedef std::list<struct regionLine> regionLine_list;

struct endPoint {
  unsigned int x1;
  unsigned int x2;
};

struct regionLine {
  unsigned int y;
  endPoint_list x_next;
};

namespace degate {

  class Region {

    private:

      unsigned int y_top;
      unsigned int y_bottom;
      regionLine_list y_next;

    public:

      Region() : 
      y_bottom(0) {}

      ~Region() {}

  };

}

#endif
