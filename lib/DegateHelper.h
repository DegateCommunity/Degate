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

#ifndef __DEGATEHELPER_H__
#define __DEGATEHELPER_H__

#include <degate.h>

#define LENGTH_TO_MAX(l) (l > 0 ? l - 1 : 0)

namespace degate {

  /**
   * Calculate the next higher power of two for a value \p i with i > 0.
   */
  template<typename IntegerType>
  IntegerType next_power_of_two(IntegerType i) {
    IntegerType rval = 1;
    while(rval < i) rval <<= 1;
    return rval;
  }

  /**
   * Tokenize a string.
   * @param str That is the string to tokenize, e.g.
   *    "hello world. \"foo bar\"". This would result
   *    in three tokens.
   */
  std::vector<std::string> tokenize(std::string const& str);

}

#endif
