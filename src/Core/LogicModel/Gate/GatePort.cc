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
#include "Core/LogicModel/Net.h"
#include "Core/LogicModel/Layer.h"
#include "Core/LogicModel/LogicModel.h"

#include "GatePort.h"
#include "Gate.h"
#include "GateTemplatePort.h"
#include "Core/Primitive/Circle.h"

#include <boost/format.hpp>
#include <memory>

using namespace degate;

GatePort::GatePort(std::shared_ptr<Gate> gate,
                   std::shared_ptr<GateTemplatePort> gate_template_port,
                   unsigned int diameter) :
    Circle(gate->get_min_x() +
           gate->get_relative_x_position_within_gate(gate_template_port->get_x()),
           gate->get_min_y() +
           gate->get_relative_y_position_within_gate(gate_template_port->get_y()),
           diameter),
    gate(gate),
    gate_template_port(gate_template_port),
    template_port_id(gate_template_port->get_object_id())
{
}


GatePort::GatePort(std::shared_ptr<Gate> gate, unsigned int diameter) :
    Circle(0, 0, diameter),
    gate(gate),
    template_port_id(0)
{
    //set_x(gate->get_min_x());
    //set_y(gate->get_min_y());
}

DeepCopyable_shptr GatePort::clone_shallow() const
{
    auto clone = std::make_shared<GatePort>();
    clone->template_port_id = template_port_id;
    return clone;
}

void GatePort::clone_deep_into(DeepCopyable_shptr dest, oldnew_t* oldnew) const
{
    auto clone = std::dynamic_pointer_cast<GatePort>(dest);

    clone->gate = std::dynamic_pointer_cast<Gate>(gate.lock()->clone_deep(oldnew));
    clone->gate_template_port = std::dynamic_pointer_cast<GateTemplatePort>(gate_template_port->clone_deep(oldnew));

    Circle::clone_deep_into(dest, oldnew);
    ConnectedLogicModelObject::clone_deep_into(dest, oldnew);
}

void GatePort::set_template_port_type_id(object_id_t template_port_id)
{
    this->template_port_id = template_port_id;
}


object_id_t GatePort::get_template_port_type_id() const
{
    return template_port_id;
}

GateTemplatePort_shptr GatePort::get_template_port()
{
    return gate_template_port;
}

const GateTemplatePort_shptr GatePort::get_template_port() const
{
    return gate_template_port;
}

void GatePort::set_template_port(std::shared_ptr<GateTemplatePort> gate_template_port)
{
    this->gate_template_port = gate_template_port;

    /* If the gate port is added to a gate afterwards, this caluclation will
     be ignored. But if the port already belongs to a gate and a a template is
     set afterwards, this calculation is used.
    */
    assert(gate.lock() != nullptr);
    set_x(gate.lock()->get_min_x() +
        gate.lock()->get_relative_x_position_within_gate(gate_template_port->get_x()));
    set_y(gate.lock()->get_min_y() +
        gate.lock()->get_relative_y_position_within_gate(gate_template_port->get_y()));
}


bool GatePort::has_template_port() const
{
    return gate_template_port != nullptr;
}

bool GatePort::is_assigned_to_a_gate() const
{
    return gate.lock() != nullptr;
}


std::shared_ptr<Gate> GatePort::get_gate()
{
    return gate.lock();
}

const std::string GatePort::get_descriptive_identifier() const
{
    if (has_template_port() && is_assigned_to_a_gate() &&
        gate.lock()->has_template())
    {
        if (gate.lock()->has_name())
        {
            return QString("%1 : %2 (%3)").arg(QString::fromStdString(gate.lock()->get_name()))
                                          .arg(QString::fromStdString(gate_template_port->get_name()))
                                          .arg(QString::fromStdString(gate.lock()->get_gate_template()->get_name()))
                                          .toStdString();
        }
        else
        {
            return QString("%1 (%2, %3=%4)").arg(QString::fromStdString(gate_template_port->get_name()))
                                            .arg(QString::fromStdString(gate.lock()->get_gate_template()->get_name()))
                                            .arg(tr("gate"))
                                            .arg(gate.lock()->get_object_id())
                                            .toStdString();
        }
    }
    else
    {
        return QString("(%1)").arg(get_object_id()).toStdString();
    }
}


const std::string GatePort::get_object_type_name() const
{
    return tr("Gate port").toStdString();
}


void GatePort::print(std::ostream& os, int n_tabs) const
{
    const GateTemplatePort_shptr tmpl_port = get_template_port();

    os
        << gen_tabs(n_tabs) << "Gate port name    : " << get_name() << std::endl
        << gen_tabs(n_tabs) << "Object ID         : " << get_object_id() << std::endl
        << gen_tabs(n_tabs) << "Template Port ID  : " <<
        (has_template_port() ? tmpl_port->get_object_id() : 0) << std::endl
        << gen_tabs(n_tabs) << "Diameter          : " << get_diameter() << std::endl
        << gen_tabs(n_tabs) << "Port position     : " << get_x() << " / " << get_y() << std::endl
        << gen_tabs(n_tabs) << "Bounding box      : " << get_bounding_box().to_string() << std::endl

        << std::endl;;
}


void GatePort::set_x(float x)
{
    auto bb = get_bounding_box();
    Circle::set_x(x);
    notify_shape_change(bb);
}

void GatePort::set_y(float y)
{
    auto bb = get_bounding_box();
    Circle::set_y(y);
    notify_shape_change(bb);
}

void GatePort::shift_x(float delta_x)
{
    auto bb = get_bounding_box();
    Circle::shift_x(delta_x);
    notify_shape_change(bb);
}

void GatePort::shift_y(float delta_y)
{
    auto bb = get_bounding_box();
    Circle::shift_y(delta_y);
    notify_shape_change(bb);
}

void GatePort::set_diameter(unsigned int diameter)
{
    auto bb = get_bounding_box();
    Circle::set_diameter(diameter);
    notify_shape_change(bb);
}
