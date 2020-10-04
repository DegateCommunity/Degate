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

#include "EMarker.h"
//#include <XmlRpc.h>
#include <boost/format.hpp>

using namespace degate;

EMarker::EMarker(float x, float y, diameter_t diameter, bool is_module_port) :
	Circle(x, y, diameter), module_port(is_module_port)
{
}

EMarker::~EMarker()
{
}

DeepCopyable_shptr EMarker::clone_shallow() const
{
	auto clone = std::make_shared<EMarker>();
	return clone;
}

void EMarker::clone_deep_into(DeepCopyable_shptr dest, oldnew_t* oldnew) const
{
	auto clone = std::dynamic_pointer_cast<EMarker>(dest);
    Circle::clone_deep_into(clone, oldnew);
    ConnectedLogicModelObject::clone_deep_into(clone, oldnew);
    RemoteObject::clone_deep_into(clone, oldnew);
}

const std::string EMarker::get_descriptive_identifier() const
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

const std::string EMarker::get_object_type_name() const
{
	return tr("EMarker").toStdString();
}


void EMarker::print(std::ostream& os, int n_tabs) const
{
	os
		<< gen_tabs(n_tabs) << "EMarker name      : " << get_name() << std::endl
		<< gen_tabs(n_tabs) << "Object ID         : " << get_object_id() << std::endl
		<< gen_tabs(n_tabs) << "EMarker position  : " << get_x() << " / " << get_y() << std::endl
		<< gen_tabs(n_tabs) << "Bounding box      : " << Circle::get_bounding_box().to_string() << std::endl
		<< std::endl;;
}

void EMarker::shift_x(float delta_x)
{
	Circle::shift_x(delta_x);
	notify_shape_change();
}

void EMarker::shift_y(float delta_y)
{
	Circle::shift_y(delta_y);
	notify_shape_change();
}

void EMarker::set_x(float x)
{
	Circle::set_x(x);
	notify_shape_change();
}

void EMarker::set_y(float y)
{
	Circle::set_y(y);
	notify_shape_change();
}

void EMarker::set_diameter(unsigned int diameter)
{
	Circle::set_diameter(diameter);
	notify_shape_change();
}


object_id_t EMarker::push_object_to_server(std::string const& server_url)
{
	/*
	try
	{
		xmlrpc_c::paramList params;
		params.add(xmlrpc_c::value_string("add"));
		params.add(xmlrpc_c::value_string("emarker"));

		Layer_shptr layer = get_layer();
		assert(layer != nullptr);
		params.add(xmlrpc_c::value_int(layer->get_layer_id()));

		params.add(xmlrpc_c::value_int(get_x()));
		params.add(xmlrpc_c::value_int(get_y()));
		params.add(xmlrpc_c::value_int(get_diameter()));

		int const transaction_id =
			xmlrpc_c::value_int(remote_method_call(server_url, "degate.push", params));

		set_remote_object_id(transaction_id);

		std::cout << "Pushed via to server. remote id is: " << transaction_id << std::endl;
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
