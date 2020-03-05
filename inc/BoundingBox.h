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

#ifndef __BOUNDINGBOX_H__
#define __BOUNDINGBOX_H__

#include <cstdio>
#include "globals.h"
#include <iostream>

namespace degate
{
	class BoundingBox
	{
	private:
        float min_x, max_x, min_y, max_y;

	public:
		BoundingBox();

		BoundingBox(float min_x, float max_x, float min_y, float max_y);
		BoundingBox(float width, float height);
		BoundingBox(const BoundingBox&);

		virtual ~BoundingBox();

		BoundingBox const& get_bounding_box() const;
		bool in_shape(float x, float y, float max_distance = 0) const;

		/**
		 * Check if this bounding box is completly within the bounding box given as parameter.
		 */

		bool in_bounding_box(BoundingBox const& bbox) const;
		bool operator==(const BoundingBox& other) const;
		bool operator!=(const BoundingBox& other) const;

		bool intersects(BoundingBox const& rect) const;
		bool complete_within(BoundingBox const& rect) const;

        float get_width() const;
        float get_height() const;

        float get_min_x() const;
        float get_max_x() const;
        float get_min_y() const;
        float get_max_y() const;

        float get_center_x() const;
        float get_center_y() const;

		void set_min_x(float min_x);
		void set_min_y(float min_y);
		void set_max_x(float max_x);
		void set_max_y(float max_y);

		void set(float min_x, float max_x, float min_y, float max_y);

		void shift_x(float delta_x);
		void shift_y(float delta_y);
		void shift(float delta_x, float delta_y);

		void print(std::ostream& os = std::cout, int n_tabs = 0) const;
		std::string to_string() const;
	};
}
#endif
