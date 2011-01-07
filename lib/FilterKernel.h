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

#ifndef __FILTERKERNEL_H__
#define __FILTERKERNEL_H__

#include <vector>
#include <tr1/memory>
#include <iostream>
#include <math.h>
#include <boost/format.hpp>

namespace degate {

  /**
   * The class FilterKernel implemements a container for 2D filter kernel data.
   */

  class FilterKernel {

  private:
    unsigned int columns, rows;
    std::vector<double> data;

  public:

    FilterKernel(unsigned int _columns, unsigned int _rows) :
      columns(_columns),
      rows(_rows),
      data(_columns * _rows) {
    }

    virtual ~FilterKernel() {}

    inline unsigned int get_columns() const {
      return columns;
    }

    inline unsigned int get_rows() const {
      return rows;
    }

    inline unsigned int get_center_row() const {
      return rows >> 1;
    }

    inline unsigned int get_center_column() const {
      return columns >> 1;
    }

    inline double get(int column, int row) const {
      return data[row * columns + column];
    }

    inline void set(int column, int row, double val) {
      data[row * columns + column] = val;
    }

    void print() const {
      unsigned int x, y;
      for(y = 0; y < columns; y++) {
	for(x = 0; x < rows; x++) {
	  boost::format f("%5.10f ");
	  f % get(x, y);
	  std::cout << f.str();
	}
	std::cout << std::endl;
      }
    }
  };

  typedef std::tr1::shared_ptr<FilterKernel> FilterKernel_shptr;

  class SobelXOperator : public FilterKernel {
  public:
    SobelXOperator() : FilterKernel(3, 3) {
      set(0, 0, 1);
      set(0, 1, 2);
      set(0, 2, 1);

      set(1, 0, 0);
      set(1, 1, 0);
      set(1, 2, 0);

      set(2, 0, -1);
      set(2, 1, -2);
      set(2, 2, -1);
    }
    virtual ~SobelXOperator() {}
  };

  typedef std::tr1::shared_ptr<SobelXOperator> SobelXOperator_shptr;

  class SobelYOperator : public FilterKernel {
  public:
    SobelYOperator() : FilterKernel(3, 3) {
      set(0, 0, 1);
      set(0, 1, 0);
      set(0, 2, -1);

      set(1, 0, 2);
      set(1, 1, 0);
      set(1, 2, -2);

      set(2, 0, 1);
      set(2, 1, 0);
      set(2, 2, -1);
    }
    virtual ~SobelYOperator() {}
  };

  typedef std::tr1::shared_ptr<SobelYOperator> SobelYOperator_shptr;


  class SobelOperator : public FilterKernel {
  public:
    SobelOperator() : FilterKernel(3, 3) {
      set(0, 0, 0);
      set(0, 1, 1);
      set(0, 2, 2);

      set(1, 0, -1);
      set(1, 1, 0);
      set(1, 2, 1);

      set(2, 0, -2);
      set(2, 1, -1);
      set(2, 2, 0);
    }
    virtual ~SobelOperator() {}
  };

  typedef std::tr1::shared_ptr<SobelOperator> SobelOperator_shptr;

  class GaussianBlur : public FilterKernel {
  public:
    GaussianBlur(unsigned int width, unsigned int height, double sigma = 1.4) :
      FilterKernel(width, height) {
      unsigned int x, y;

      for(y = 0; y < get_rows(); y++) {
	for(x = 0; x < get_columns(); x++) {

	  double _x = (double)x - (double)get_center_column();
	  double _y = (double)y - (double)get_center_row();

	  double v = 1.0/(2.0 * M_PI * pow(sigma, 2)) * exp(-(pow(_x, 2) + pow(_y, 2)) / (2*pow(sigma,2)));

	  set(x, y, v);
	}

      }
    }

    virtual ~GaussianBlur() {}
  };

  /**
   * Implements a Laplacian of Gaussian.
   */
  class LoG : public FilterKernel {
  public:
    LoG(unsigned int width, unsigned int height, double sigma = 1.4) : FilterKernel(width, height) {
      unsigned int x, y;

      for(y = 0; y < get_rows(); y++) {
	for(x = 0; x < get_columns(); x++) {

	  double _x = (double)x - (double)get_center_column();
	  double _y = (double)y - (double)get_center_row();

	  double v =
	    -1.0/(M_PI * pow(sigma, 4)) *
	    (1.0 - (pow(_x, 2) + pow(_y, 2)) / (2.0*pow(sigma, 2))) *
	    exp(-(pow(_x, 2) + pow(_y, 2)) / (2.0*pow(sigma,2)));

	  set(x, y, v);
	}

      }
    }

   virtual ~LoG() {}
  };

  typedef std::tr1::shared_ptr<LoG> LoG_shptr;

}

#endif
