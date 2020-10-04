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

#ifndef __HLOBJECTSET_H__
#define __HLOBJECTSET_H__

#include <set>
#include <list>
#include <map>
#include <memory>

#include "Core/LogicModel/ObjectSet.h"

namespace degate
{
    /**
     * This class represents a collection of highlighted objects.
     */
    class HlObjectSet : public ObjectSet
    {
    private:
        typedef std::map<ConnectedLogicModelObject_shptr,
                         std::list<ConnectedLogicModelObject_shptr>> adjacent_objects_t;

        adjacent_objects_t adjacent_objects;

        std::function<void(PlacedLogicModelObject_shptr)> object_update_function;

    private:
        void highlight_adjacent_objects(ConnectedLogicModelObject_shptr o,
                                        LogicModel_shptr lmodel);

        void unhighlight_adjacent_objects(adjacent_objects_t::mapped_type& list);

        void highlight(PlacedLogicModelObject::HIGHLIGHTING_STATE state);

    public:

        /**
         * Set a function that will be called every time an object state changed (like the highlight state).
         *
         * @param object_update_function : the function to call.
         */
        void set_object_update_function(std::function<void(PlacedLogicModelObject_shptr)> object_update_function);

        void clear();
        void add(degate::PlacedLogicModelObject_shptr object);
        void add(degate::PlacedLogicModelObject_shptr object,
                 LogicModel_shptr lmodel);
        void remove(degate::PlacedLogicModelObject_shptr object);
    };
}

#endif
