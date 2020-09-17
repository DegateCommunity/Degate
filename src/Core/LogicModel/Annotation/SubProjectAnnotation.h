/**
 * This file is part of the IC reverse engineering tool Degate.
 *
 * Copyright 2008, 2009, 2010 by Martin Schobert
 * Copyright 2019-2020 Dorian Bachelot
 *
 * Degate is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * Degate is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with degate. If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef __SUBPROJECTANNOTATION_H__
#define __SUBPROJECTANNOTATION_H__

#include "Globals.h"
#include "Core/LogicModel/LogicModelObjectBase.h"
#include "Core/LogicModel/Layer.h"
#include "Core/LogicModel/PlacedLogicModelObject.h"

#include "Core/Primitive/Rectangle.h"
#include "Annotation.h"

#include <set>

namespace degate
{
	/**
	 * An annotation for subprojects.
	 */
	class SubProjectAnnotation : public Annotation
	{
	public:

		/**
		 * Create a new annotation.
		 */
		SubProjectAnnotation(float min_x, float max_x, float min_y, float max_y,
		                     std::string const& subproject_path);


		/**
		 * Create a new annotation.
		 */
		SubProjectAnnotation(BoundingBox const& bbox, std::string const& subproject_path);

		void set_path(std::string const& subproject_path);

		std::string get_path() const;
	};

	typedef std::shared_ptr<SubProjectAnnotation> SubProjectAnnotation_shptr;
}
#endif
