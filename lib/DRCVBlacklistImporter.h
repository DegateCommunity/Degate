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

#ifndef __DRCVBLACKLISTIMPORTER_H__
#define __DRCVBLACKLISTIMPORTER_H__

#include "globals.h"
#include "DRCViolation.h"
#include "XMLImporter.h"

#include <stdexcept>

namespace degate {
  
  /**
   * The DRCVBlacklistImporter imports a list of DRC violations, which should be ignored.
   */

  class DRCVBlacklistImporter : public XMLImporter {
  private:

    void parse_list(const xmlpp::Element * const element, DRCBase::container_type & blacklist);
    LogicModel_shptr _lmodel;

  public:
    DRCVBlacklistImporter(LogicModel_shptr lmodel) : _lmodel(lmodel) {}
    ~DRCVBlacklistImporter() {}

    void import_into(std::string const& filename, DRCBase::container_type & blacklist);
  };

}
#endif
