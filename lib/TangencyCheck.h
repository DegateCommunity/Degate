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

#ifndef __TANGENCYCHECK_H__
#define __TANGENCYCHECK_H__

#include <degate.h>
#include <globals.h>
#include <Circle.h>
#include <Line.h>
#include <Rectangle.h>
#include <PlacedLogicModelObject.h>

namespace degate {

  /**
   * Check if two objects are tangent. It is assumed that both
   * objects are on the same layer.
   * @return Returns whether the objects are touching each other.
   */
  bool check_object_tangency(PlacedLogicModelObject_shptr o1,
			     PlacedLogicModelObject_shptr o2);


  bool check_object_tangency(Circle_shptr o1,
			     Circle_shptr o2);
  
  
  bool check_object_tangency(Line_shptr o1,
			     Line_shptr o2);
  
  bool check_object_tangency(Rectangle_shptr o1,
			     Rectangle_shptr o2);
  
  bool check_object_tangency(Circle_shptr o1,
			     Line_shptr o2);
  
  
  bool check_object_tangency(Circle_shptr o1,
			     Rectangle_shptr o2);
  
  bool check_object_tangency(Line_shptr o1,
			     Rectangle_shptr o2);






}

#endif
