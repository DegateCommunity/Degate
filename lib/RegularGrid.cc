/*
 
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

#include "RegularGrid.h"
#include <math.h>

using namespace degate;

void RegularGrid::precalc_steps() {
	grid_offsets.clear();
	if(distance > 0) {
		for(double i = min; i < max; i += distance) {
			grid_offsets.push_back(lround(i));
		}
		grid_offsets.sort();
	}
}

int RegularGrid::snap_to_grid(int pos) const {
			
	if(pos <= min) return min; 
	else if(pos >= max) return max;
    else {
		if(distance == 0) return pos;
		
		unsigned int grid_coord_x_lo = (unsigned int)((pos - min) / distance);
		unsigned int grid_coord_x_hi = grid_coord_x_lo + 1;
		
		if( (grid_coord_x_hi * distance + min - pos) < (pos - (grid_coord_x_lo * distance + min)))       
			return (int)(grid_coord_x_hi * distance + min);
		else  return (int)(grid_coord_x_lo * distance + min);
    }
	
}
