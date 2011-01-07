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

#ifndef __MEMORYMAPBASE_H__
#define __MEMORYMAPBASE_H__

#include "globals.h"
#include <string>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <assert.h>
#include <limits.h>
#include <math.h>

namespace degate {

template<typename T>
class MemoryMapBase {

protected:
	unsigned int width, height;

public:

	MemoryMapBase(unsigned int width, unsigned int height);
	virtual ~MemoryMapBase();

	virtual int get_width() const { return width; }
	virtual int get_height() const { return height; }


	virtual void clear();
	virtual void clear_area(unsigned int min_x, unsigned int min_y,
							unsigned int width, unsigned int height);

	virtual void set(unsigned int x, unsigned int y, T new_val);
	virtual T get(unsigned int x, unsigned int y) const;

};

}

#endif
