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

#include "Wire.h"
#include "Globals.h"
#include "Core/LogicModel/LogicModelObjectBase.h"
#include "Core/LogicModel/Net.h"
#include "Core/LogicModel/Layer.h"
#include "Core/LogicModel/LogicModel.h"
#include "Core/LogicModel/ConnectedLogicModelObject.h"
#include "Core/Utils/DegateExceptions.h"

#include <exception>

using namespace degate;

Wire::Wire(float from_x, float from_y, float to_x, float to_y, unsigned int diameter) :
	Line(from_x, from_y, to_x, to_y, diameter)
{
}

Wire::Wire(Line line) :
    Line(line)
{

}

DeepCopyable_shptr Wire::clone_shallow() const
{
	return std::make_shared<Wire>(get_from_x(), get_from_y(), get_to_x(), get_to_y(), get_diameter());
}

void Wire::clone_deep_into(DeepCopyable_shptr dest, oldnew_t* oldnew) const
{
	auto clone = std::dynamic_pointer_cast<Wire>(dest);
    Line::clone_deep_into(dest, oldnew);
    RemoteObject::clone_deep_into(dest, oldnew);
}

const std::string Wire::get_descriptive_identifier() const
{
	if (has_name())
	{
        return QString("%1 (%2)").arg(QString::fromStdString(get_name())).arg(get_object_id()).toStdString();
	}
	else
	{
        return QString("(%1)").arg(get_object_id()).toStdString();
	}
}

const std::string Wire::get_object_type_name() const
{
	return tr("Wire").toStdString();
}

void Wire::print(std::ostream& os, int n_tabs) const
{
}

object_id_t Wire::push_object_to_server(std::string const& server_url)
{
	/*
	try
	{
		xmlrpc_c::paramList params;
		params.add(xmlrpc_c::value_string("add"));
		params.add(xmlrpc_c::value_string("wire"));

		Layer_shptr layer = get_layer();
		assert(layer != nullptr);
		params.add(xmlrpc_c::value_int(layer->get_layer_id()));

		params.add(xmlrpc_c::value_int(get_from_x()));
		params.add(xmlrpc_c::value_int(get_from_y()));
		params.add(xmlrpc_c::value_int(get_to_x()));
		params.add(xmlrpc_c::value_int(get_to_y()));
		params.add(xmlrpc_c::value_int(get_diameter()));

		int const transaction_id =
			xmlrpc_c::value_int(remote_method_call(server_url, "degate.push", params));

		set_remote_object_id(transaction_id);
		std::cout << "Pushed wire to server. remote id is: " << transaction_id << std::endl;
		return transaction_id;
	}
	catch (std::exception const& e)
	{
		std::cerr << "Client threw error: " << e.what() << std::endl;
		throw XMLRPCException(e.what());
	}
	catch (...)
	{
		std::cerr << "Client threw unexpected error." << std::endl;
		throw XMLRPCException("Client threw unexpected error.");
	}
	*/

	return 0;
}
