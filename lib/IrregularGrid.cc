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

#include "IrregularGrid.h"

#include <assert.h>
#include <stdlib.h>

using namespace degate;

int IrregularGrid::snap_to_grid(int pos) const {
	
	if(pos <= grid_offsets.front()) return grid_offsets.front();
	else if(pos >= grid_offsets.back()) return grid_offsets.back();
    else {
		
		int last = grid_offsets.front();
		for(grid_iter it = grid_offsets.begin(); it != grid_offsets.end(); ++it) {
			int current = *it;
			if(current < pos) last = current;
			else if(current >= pos) {
				int d1 = abs(pos - last);
				int d2 = abs(current - pos);
				return d1 < d2 ? last : current;
			}
		}
		
		// should not reach this line
		assert(1 == 0);
    }
	
}
