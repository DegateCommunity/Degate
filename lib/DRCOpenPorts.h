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

#ifndef __DRCOPENPORTS_H__
#define __DRCOPENPORTS_H__

#include <DRCBase.h>
#include <LogicModel.h>

namespace degate {

  /**
   * Design Rule Checks that detects open ports, that means if 
   * a port is electrically unconnected.
   */

  class DRCOpenPorts : public DRCBase {

  public:

    DRCOpenPorts();

    void run(LogicModel_shptr lmodel);
  
  };

}

#endif
