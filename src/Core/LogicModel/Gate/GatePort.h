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

#ifndef __GATEPORT_H__
#define __GATEPORT_H__

#include "Globals.h"

#include "Core/LogicModel/ConnectedLogicModelObject.h"
#include "Core/Primitive/Rectangle.h"
#include "Gate.h"
#include "GateTemplatePort.h"
#include <set>

#include <memory>

namespace degate
{
    /**
     * This class represents a port of a gate.
     * @todo Dispatch get_fill/frame_color() calls to
     *   GateTemplatePort::get_xxx_color(). Make sure, that colors
     *   are updated when changes are made in the PortColorManager.
     */
    class GatePort : public Circle, public ConnectedLogicModelObject
    {
    Q_DECLARE_TR_FUNCTIONS(degate::GatePort)

    private:

        std::weak_ptr<Gate> gate;
        std::shared_ptr<GateTemplatePort> gate_template_port;
        object_id_t template_port_id;

    public:


        explicit GatePort()
        {
        };

        /**
         * Create a gate port and set a "reference" to the the template port.
         *
         * @param gate A shared pointer to the gate, the port is created for.
         * @param gate_template_port A shared pointer to a template port.
         * @param diameter The diameter of the port.
         */
        GatePort(std::shared_ptr<Gate> gate,
                 std::shared_ptr<GateTemplatePort> gate_template_port,
                 unsigned int diameter = 5);

        /**
         * Create a gate port.
         *
         * @param gate A shared pointer to the gate, the port is created for.
         * @param diameter The diameter of the port.
         */
        GatePort(std::shared_ptr<Gate> gate, unsigned int diameter = 5);


        /**
         * The destructor.
         */
        virtual ~GatePort()
        {
        }

        //@{
        DeepCopyable_shptr clone_shallow() const;
        void clone_deep_into(DeepCopyable_shptr destination, oldnew_t* oldnew) const;
        //@}

        /**
         * Set the ID of the template port.
         */
        virtual void set_template_port_type_id(object_id_t template_port_id);

        /**
         * Get the ID of the template port.
         */
        virtual object_id_t get_template_port_type_id() const;

        /**
         * Get the template port.
         */
        virtual GateTemplatePort_shptr get_template_port();

        /**
         * Get the template port.
         */
        virtual const GateTemplatePort_shptr get_template_port() const;

        /**
         * Set the template port.
         */
        virtual void set_template_port(std::shared_ptr<GateTemplatePort> gate_template_port);


        /**
         * Check if there is a template port defined.
         */
        virtual bool has_template_port() const;

        /**
         * Check if a gate port already belongs to a gate.
         * Normally a gate port belongs to a gate.
         */
        virtual bool is_assigned_to_a_gate() const;


        /**
         * Get the gate, this gate port belongs to.
         * @return Returns a shared pointer to a gate. A pointer value
         *   of nullptr indicates, that the gate port is not assigned to a gate.
         * @see has_gate()
         */
        std::shared_ptr<Gate> get_gate();


        /**
         * Get a human readable string that describes the whole
         * logic model object. The string should be unique in order
         * to let the user identify the concrete object. But that
         * is not a must.
         */
        virtual const std::string get_descriptive_identifier() const;

        /**
         * Get a human readable string that names the object type.
         * Here it is "Gate port".
         */
        virtual const std::string get_object_type_name() const;


        /**
         * Print gate port.
         */
        void print(std::ostream& os = std::cout, int n_tabs = 0) const;


        /**
         * Set the absolute position of a port.
         */
        void set_x(float x);

        /**
         * Set the absolute position of a port.
         */
        void set_y(float y);


        void shift_x(float delta_x);
        void shift_y(float delta_y);
        void set_diameter(unsigned int diameter);


        virtual bool in_bounding_box(BoundingBox const& bbox) const
        {
            return Circle::in_bounding_box(bbox);
        }

        virtual BoundingBox const& get_bounding_box() const
        {
            return Circle::get_bounding_box();
        }

        virtual bool in_shape(float x, float y, float max_distance = 0) const
        {
            return Circle::in_shape(x, y, max_distance);
        }
    };
}

#endif
