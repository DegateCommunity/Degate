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

#ifndef __DRCNET_H__
#define __DRCNET_H__

#include <boost/foreach.hpp>
#include <tr1/memory>
#include <list>
#include <LogicModel.h>
#include <DRCBase.h>

namespace degate {


  /**
   * Design Rule Checks that detects unusual net configurations.
   * These cases are checked:
   * - A net connects gate ports, but these ports are all of the
   *   type. E.g. only inports or only outports are connected.
   * - Corresponding gate templates have an undefined port direction.

   * It is possible, that the port direction is still undefined, because
   * the user forgot to define it. This could be checked with a dedicated DRC
   * for gate template ports, but in order to simplify it, we generate a
   * violation entry here. The drawback is, that it will generate more
   * entries than necessary, because the template port direction is implicitly
   * checked multiple times.
   *
   */

  class DRCNet : public DRCBase {

  public:

    DRCNet();

    void run(LogicModel_shptr lmodel);

  private:

    void check_net(LogicModel_shptr lmodel, Net_shptr net);

  };

}

#endif
