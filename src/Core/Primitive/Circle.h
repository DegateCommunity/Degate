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

#ifndef __CIRCLE_H__
#define __CIRCLE_H__

#include "Core/Primitive/AbstractShape.h"
#include "Core/Primitive/DeepCopyable.h"
#include "Core/Primitive/BoundingBox.h"

namespace degate
{
	class Circle : public AbstractShape, public DeepCopyableBase
	{
	private:
        float x, y;
		unsigned int diameter = 5;

		BoundingBox bounding_box;

		void calculate_bounding_box();

	public:

		Circle();
		Circle(float x, float y, unsigned int diameter);

		virtual ~Circle()
		{
		}

		void clone_deep_into(DeepCopyable_shptr destination, oldnew_t* oldnew) const;

		virtual bool in_shape(float x, float y, float max_distance = 0) const;

		virtual bool in_bounding_box(BoundingBox const& bbox) const;
		virtual BoundingBox const& get_bounding_box() const;

		virtual bool operator==(const Circle& other) const;
		virtual bool operator!=(const Circle& other) const;

		virtual float get_x() const;
		virtual float get_y() const;
		virtual unsigned int get_diameter() const;

		virtual void set_x(float x);
		virtual void set_y(float y);
		virtual void set_diameter(unsigned int diameter);

		virtual void shift_x(float delta_x);
		virtual void shift_y(float delta_y);
	};
}

#endif
