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

#include <Core/LogicModel/HlObjectSet.h>
#include <Core/LogicModel/ConnectedLogicModelObject.h>
#include <Core/LogicModel/LogicModel.h>

#include <algorithm>
#include <memory>

using namespace std;
using namespace degate;


void HlObjectSet::set_object_update_function(std::function<void(PlacedLogicModelObject_shptr)> object_update_function)
{
    this->object_update_function = object_update_function;
}

void HlObjectSet::clear()
{
	highlight(PlacedLogicModelObject::HLIGHTSTATE_NOT);

	for(auto & adjacent_object : adjacent_objects)
	{
		unhighlight_adjacent_objects(adjacent_object.second);
	}

	adjacent_objects.clear();
	ObjectSet::clear();
}

void HlObjectSet::highlight(PlacedLogicModelObject::HIGHLIGHTING_STATE state)
{
	for (auto& e : *this)
    {
        e->set_highlighted(state);

        if(object_update_function)
            object_update_function(e);
    }
}


void HlObjectSet::add(std::shared_ptr<PlacedLogicModelObject> object)
{
	ObjectSet::add(object);
	object->set_highlighted(PlacedLogicModelObject::HLIGHTSTATE_DIRECT);

    if(object_update_function)
        object_update_function(object);
}

void HlObjectSet::add(std::shared_ptr<PlacedLogicModelObject> object,
                      LogicModel_shptr lmodel)
{
	add(object);

	if (ConnectedLogicModelObject_shptr o =
		std::dynamic_pointer_cast<ConnectedLogicModelObject>(object))
	{
		// highlight adjacent objects
		highlight_adjacent_objects(o, lmodel);
	}
}


void HlObjectSet::highlight_adjacent_objects(ConnectedLogicModelObject_shptr o,
                                             LogicModel_shptr lmodel)
{
	Net_shptr net = o->get_net();
	if (net == nullptr) return;

	// iterate over net
	for(auto& oid : *net)
	{
		PlacedLogicModelObject_shptr plo = lmodel->get_object(oid);
		ConnectedLogicModelObject_shptr clo =
			std::dynamic_pointer_cast<ConnectedLogicModelObject>(plo);

		assert(clo != nullptr);

		// remember connnected objects in list
		if (o != clo)
		{
			clo->set_highlighted(PlacedLogicModelObject::HLIGHTSTATE_ADJACENT);

            if(object_update_function)
                object_update_function(clo);

			adjacent_objects[o].push_back(clo);
		}
	}
}

void HlObjectSet::unhighlight_adjacent_objects(adjacent_objects_t::mapped_type& list)
{
	// iterate over list
	for(auto& clo : list)
	{
		auto iter = adjacent_objects.find(clo);
		if (iter == adjacent_objects.end())
        {
            clo->set_highlighted(PlacedLogicModelObject::HLIGHTSTATE_NOT);

            if(object_update_function)
                object_update_function(clo);
        }
	}
	list.clear();
}

void HlObjectSet::remove(std::shared_ptr<PlacedLogicModelObject> object)
{
	ObjectSet::remove(object);

	if (ConnectedLogicModelObject_shptr o =
		std::dynamic_pointer_cast<ConnectedLogicModelObject>(object))
	{
		auto iter = adjacent_objects.find(o);
		if (iter != adjacent_objects.end())
		{
			unhighlight_adjacent_objects((*iter).second);
		}
		adjacent_objects.erase(iter);
	}

	object->set_highlighted(PlacedLogicModelObject::HLIGHTSTATE_NOT);

    if(object_update_function)
        object_update_function(object);
}
