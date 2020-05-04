/* -*-c++-*-

 This file is part of the IC reverse engineering tool degate.

 Copyright 2008, 2009, 2010 by Martin Schobert
 Copyright 2012 Robert Nitsch

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

#ifndef __LINE_H__
#define __LINE_H__

#include <Core/BoundingBox.h>
#include "DeepCopyable.h"
#include <Core/Shape.h>
#include <Core/Point.h>

namespace degate
{
	class Line : public AbstractShape, public DeepCopyableBase
	{
	private:
		float from_x, from_y, to_x, to_y;
		unsigned int diameter;

		double d_x, d_y;

		BoundingBox bounding_box;

	private:

		/**
		 * Recalulate the bounding box of a line.
		 * If the line is either horizontally nor vertically
		 * aligned, the bounding box corresponds exactly to
		 * the area, the line spans. If the line is horizontal
		 * or vertical, the bounding box also depends on the
		 * diameter.
		 */
		void calculate_bounding_box();

	public:

		Line();
		Line(float from_x, float from_y, float to_x, float to_y, unsigned int diameter);

		virtual ~Line()
		{
		}

		void cloneDeepInto(DeepCopyable_shptr destination, oldnew_t* oldnew) const;

		virtual bool in_shape(float x, float y, float max_distance = 0) const;
		virtual bool in_bounding_box(BoundingBox const& bbox) const;
		virtual BoundingBox const& get_bounding_box() const;


		virtual float get_from_x() const;
		virtual float get_to_x() const;
		virtual float get_from_y() const;
		virtual float get_to_y() const;

		virtual void set_from_x(float min_x);
		virtual void set_from_y(float min_y);
		virtual void set_to_x(float max_x);
		virtual void set_to_y(float max_y);

		virtual void shift_x(float delta_x);
		virtual void shift_y(float delta_y);

		virtual unsigned int get_diameter() const;
		virtual void set_diameter(unsigned int diameter);

		virtual bool is_vertical() const;
		virtual bool is_horizontal() const;

		virtual unsigned int get_length() const;

		virtual Point get_p1() const;
		virtual Point get_p2() const;

		virtual void set_p1(Point const& p);
		virtual void set_p2(Point const& p);

		float distance_to_line(const Point &p) const;
	};
}

#endif
