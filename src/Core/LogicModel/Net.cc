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
#include "Core/LogicModel/Layer.h"
#include "Core/LogicModel/LogicModel.h"
#include "Core/LogicModel/ConnectedLogicModelObject.h"
#include "Core/Utils/DegateExceptions.h"

using namespace degate;

Net::Net()
{
}

Net::~Net()
{
}

DeepCopyable_shptr Net::clone_shallow() const
{
    auto clone = std::make_shared<Net>();
    return clone;
}

void Net::clone_deep_into(DeepCopyable_shptr dest, oldnew_t* oldnew) const
{
    auto clone = std::dynamic_pointer_cast<Net>(dest);
    clone->connections = connections;
    LogicModelObjectBase::clone_deep_into(dest, oldnew);
}

Net::connection_iterator Net::begin()
{
    return connections.begin();
}

Net::connection_iterator Net::end()
{
    return connections.end();
}

void Net::remove_object(object_id_t oid)
{
    if (oid == 0)
        throw InvalidObjectIDException("The object that has to be "
            "removed from the net has no object ID.");

    connection_iterator i = connections.find(oid);
    if (i != connections.end())
    {
        connections.erase(i);
    }
    else
        throw CollectionLookupException("Can't remove object from the the net, "
            "because it is not in the net.");
}

void Net::remove_object(ConnectedLogicModelObject_shptr o)
{
    remove_object(o->get_object_id());
}


void Net::add_object(object_id_t oid)
{
    if (oid == 0)
        throw InvalidObjectIDException("The object that has to be "
            "added to the net has no object ID.");
    else
        connections.insert(oid);
}

void Net::add_object(ConnectedLogicModelObject_shptr o)
{
    add_object(o->get_object_id());
}


unsigned int Net::size() const
{
    return static_cast<unsigned int>(connections.size());
}

const std::string Net::get_descriptive_identifier() const
{
    return QString("%1 %2").arg(tr("Net")).arg(get_object_id()).toStdString();
}
