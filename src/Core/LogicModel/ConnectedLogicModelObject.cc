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
#include "Layer.h"
#include "PlacedLogicModelObject.h"
#include "Net.h"
#include "ConnectedLogicModelObject.h"

#include <boost/foreach.hpp>

using namespace degate;

ConnectedLogicModelObject::ConnectedLogicModelObject()
{
}

ConnectedLogicModelObject::~ConnectedLogicModelObject()
{
    remove_net();
};

void ConnectedLogicModelObject::clone_deep_into(DeepCopyable_shptr dest, oldnew_t* oldnew) const
{
    PlacedLogicModelObject::clone_deep_into(dest, oldnew);

    auto clone = std::dynamic_pointer_cast<ConnectedLogicModelObject>(dest);
    if (net.get() != nullptr)
    {
        clone->net = std::dynamic_pointer_cast<Net>(net->clone_deep(oldnew));
    }
}

void ConnectedLogicModelObject::set_net(Net_shptr net)
{
    if (this->net != nullptr)
    {
        this->net->remove_object(get_object_id());
    }
    this->net = net;
    this->net->add_object(get_object_id());
}

void ConnectedLogicModelObject::remove_net()
{
    if (net != nullptr)
    {
        net->remove_object(get_object_id());
        net.reset();
    }
}


Net_shptr ConnectedLogicModelObject::get_net()
{
    return net;
}

bool ConnectedLogicModelObject::is_connected() const
{
    if (net == nullptr) return false;
    if (net->size() >= 2) return true;
    BOOST_FOREACH(object_id_t oid, *net)
    {
        if (oid != get_object_id()) return true;
    }
    return false;
}
