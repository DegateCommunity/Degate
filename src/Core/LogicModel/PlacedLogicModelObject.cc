/**
 * This file is part of the IC reverse engineering tool Degate.
 *
 * Copyright 2008, 2009, 2010 by Martin Schobert
 * Copyright 2012 Robert Nitsch
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

#include "Globals.h"
#include "Core/LogicModel/LogicModelObjectBase.h"
#include "Net.h"
#include "Layer.h"
#include "Core/LogicModel/LogicModel.h"
#include "PlacedLogicModelObject.h"

using namespace degate;

PlacedLogicModelObject::PlacedLogicModelObject() : highlight_state(HLIGHTSTATE_NOT)
{
}

PlacedLogicModelObject::~PlacedLogicModelObject()
{
}

void PlacedLogicModelObject::clone_deep_into(DeepCopyable_shptr destination, oldnew_t* oldnew) const
{
    ColoredObject::clone_deep_into(destination, oldnew);
    LogicModelObjectBase::clone_deep_into(destination, oldnew);

	auto clone = std::dynamic_pointer_cast<PlacedLogicModelObject>(destination);
	assert(clone.get () != nullptr);
	clone->highlight_state = highlight_state;
	clone->layer = std::dynamic_pointer_cast<Layer>(layer.lock()->clone_deep(oldnew));
}

PlacedLogicModelObject::HIGHLIGHTING_STATE PlacedLogicModelObject::get_highlighted() const
{
	return highlight_state;
}

bool PlacedLogicModelObject::is_highlighted() const
{
	return highlight_state != PlacedLogicModelObject::HLIGHTSTATE_NOT;
}

void PlacedLogicModelObject::set_highlighted(PlacedLogicModelObject::HIGHLIGHTING_STATE state)
{
	highlight_state = state;
}


void PlacedLogicModelObject::set_layer(std::shared_ptr<Layer> layer)
{
	this->layer = layer;
}

std::shared_ptr<Layer> PlacedLogicModelObject::get_layer()
{
	return layer.lock();
}

void PlacedLogicModelObject::notify_shape_change()
{
	if (layer.lock() != nullptr && has_valid_object_id())
	{
		layer.lock()->notify_shape_change(get_object_id());
	}
}
