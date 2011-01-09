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

#ifndef __AUTONAMEGATES_H__
#define __AUTONAMEGATES_H__

#include <LogicModel.h>

namespace degate {

  /**
   * This class helps to generate names for gates depending on their position.
   *
   * Gates are usually placed in rows or columns. It might be useful to name
   * them by their position. The class AutoNameGates generates names in the
   * format ${row}.${column}. E.g. the gate name "5.23" indicates that this
   * gate is placed in the 5th row and the 23th column.
   *
   */
  class AutoNameGates {

  public:

    enum ORIENTATION {
      ALONG_ROWS = 1,
      ALONG_COLS = 2
    };

    AutoNameGates(LogicModel_shptr lmodel, ORIENTATION orientation);

    void run();

  private:
    void rename_gates(std::vector<unsigned int> const & histogram, 
		      ORIENTATION orientation,
		      std::list<int> const& scan_lines) const;

    void fill_histogram(LogicModel_shptr lmodel,
			std::vector<unsigned int> & histogram, 
			ORIENTATION orientation) const;

    std::list<int> scan_histogram(std::vector<unsigned int> const & histogram) const;

  private:
    LogicModel_shptr _lmodel;
    ORIENTATION _orientation;
    Layer_shptr layer;  

  };

}

#endif
