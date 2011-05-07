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

#ifndef __STATISTICS_H__
#define __STATISTICS_H__

#include <vector>
#include <algorithm>

namespace degate {

  /**
   * Calculate the median of a vector.
   * @exception DegateRuntimeException This exception is thrown if
   *   vector's size is 0.
   */
  template<typename T>
  inline T median(std::vector<T> & v) {

    if(v.empty())
      throw DegateRuntimeException("Error in median(): The vector is empty.");

    std::sort(v.begin(), v.end());

    unsigned int center = v.size()/2;

    if(v.size() % 2 == 0) {
      return (v[center - 1] + v[center + 1]) / 2;
    }
    else
      return v[center];
  }


  /**
   * Calculate the average of a vector.
   * @exception DegateRuntimeException This exception is thrown if
   *   vector's size is 0.
   */
  template<typename T>
  inline T average(std::vector<T> const& v) {

    if(v.empty())
      throw DegateRuntimeException("Error in average(): The vector is empty.");

    double _sum = 0;

    for(typename std::vector<T>::const_iterator iter = v.begin();
	iter != v.end(); ++iter)
      _sum += *iter;

    return _sum / (double)v.size();
  }


  /**
   * Calculate the standard deviation of a vector.
   * @exception DegateRuntimeException This exception is thrown if
   *   vector's size is 0.
   */
  template<typename T>
  inline T standard_deviation(std::vector<T> const& v) {

    if(v.empty())
      throw DegateRuntimeException("Error in standard_deviation(): The vector is empty.");

    T avg = average<T>(v);
    double sum = 0;
    for(typename std::vector<T>::const_iterator iter = v.begin(); iter != v.end(); ++iter)
      sum += pow(avg - *iter, 2);

    return sqrt(sum);
  }


  
}



#endif
