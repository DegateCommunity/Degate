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

#ifndef __DESIGNRULECHECKER_H__
#define __DESIGNRULECHECKER_H__

#include <DRCBase.h>
#include <DRCOpenPorts.h>
#include <DRCNet.h>

namespace degate {

  class DesignRuleChecker : public DRCBase {

  private:

    std::list<DRCBase_shptr> checks;

  public:

    DesignRuleChecker() : DRCBase("drc-all", "A collection of all DRCs.") {
      checks.push_back(DRCBase_shptr(new DRCOpenPorts()));
      checks.push_back(DRCBase_shptr(new DRCNet()));
    }

    void run(LogicModel_shptr lmodel) {

      debug(TM, "run DRC");

      clear_drc_violations();

      BOOST_FOREACH(DRCBase_shptr check, checks) {
	std::cout << "DRC: " << check->get_drc_class_name() << std::endl;
	check->run(lmodel);
	BOOST_FOREACH(DRCViolation_shptr violation, check->get_drc_violations()) {
	  add_drc_violation(violation);
	}
      }

      debug(TM, "found %d drc violations.", get_drc_violations().size());
    }
  };

}

#endif
