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

#ifndef __RECTANGLE_H__
#define __RECTANGLE_H__

#include "Core/Primitive/BoundingBox.h"
#include "Core/Primitive/DeepCopyable.h"
#include "Core/Primitive/AbstractShape.h"

namespace degate
{
	class Rectangle : public AbstractShape, public DeepCopyableBase
	{
	private:
        float min_x, max_x, min_y, max_y;

		BoundingBox bounding_box;

		void calculate_bounding_box();

	public:

		Rectangle();

		Rectangle(float min_x, float max_x, float min_y, float max_y);
		Rectangle(const Rectangle&);

		virtual ~Rectangle();

		void clone_deep_into(DeepCopyable_shptr destination, oldnew_t* oldnew) const;

		virtual bool in_shape(float x, float y, float max_distance = 0) const;
		virtual bool in_bounding_box(BoundingBox const& bbox) const;
		virtual BoundingBox const& get_bounding_box() const;

		virtual bool operator==(const Rectangle& other) const;
		virtual bool operator!=(const Rectangle& other) const;

		virtual bool intersects(Rectangle const& rect) const;
		virtual bool complete_within(Rectangle const& rect) const;

		virtual float get_width() const;
		virtual float get_height() const;

		virtual float get_min_x() const;
		virtual float get_max_x() const;
		virtual float get_min_y() const;
		virtual float get_max_y() const;

		virtual float get_center_x() const;
		virtual float get_center_y() const;

		virtual void set_min_x(float min_x);
		virtual void set_min_y(float min_y);
		virtual void set_max_x(float max_x);
		virtual void set_max_y(float max_y);

		virtual void set_position(float min_x, float max_x, float min_y, float max_y);

		virtual void shift_x(float delta_x);
		virtual void shift_y(float delta_y);
	};
}

#endif
