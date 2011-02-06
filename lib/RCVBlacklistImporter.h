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

#ifndef __RCVBLACKLISTIMPORTER_H__
#define __RCVBLACKLISTIMPORTER_H__

#include "globals.h"
#include "RCViolation.h"
#include "XMLImporter.h"

#include <stdexcept>

namespace degate {
  
  /**
   * The RCVBlacklistImporter imports a list of RC violations, which should be ignored.
   */

  class RCVBlacklistImporter : public XMLImporter {
  private:

    void parse_list(const xmlpp::Element * const element, RCBase::container_type & blacklist);
    LogicModel_shptr _lmodel;

  public:
    RCVBlacklistImporter(LogicModel_shptr lmodel) : _lmodel(lmodel) {}
    ~RCVBlacklistImporter() {}

    void import_into(std::string const& filename, RCBase::container_type & blacklist);
  };

}
#endif
