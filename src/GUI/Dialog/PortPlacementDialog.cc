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

#include "PortPlacementDialog.h"

namespace degate
{
    PortPlacementDialog::PortPlacementDialog(QWidget* parent,
                                             const Project_shptr& project,
                                             const GateTemplate_shptr& gate,
                                             const GateTemplatePort_shptr& port)
            : QDialog(parent), gate(gate), port(port), project(project), placement(this, project, gate, port)
	{
	    setWindowTitle(tr("Place/move the port"));

		resize(gate->get_width() + 300, gate->get_height() + 300);
		placement.resize(gate->get_width(), gate->get_height());
		next_layer_button.setText(tr("Next layer"));
		previous_layer_button.setText(tr("Previous layer"));
		validate_button.setText(tr("Ok"));
		cancel_button.setText(tr("Cancel"));

		layers_buttons_layout.addWidget(&next_layer_button);
		layers_buttons_layout.addWidget(&previous_layer_button);
		
		quit_buttons_layout.addWidget(&validate_button);
		quit_buttons_layout.addWidget(&cancel_button);

		layout.addWidget(&placement);
		layout.addLayout(&layers_buttons_layout);
		layout.addLayout(&quit_buttons_layout);

		setLayout(&layout);

		QObject::connect(&next_layer_button, SIGNAL(clicked()), &placement, SLOT(next_layer()));
		QObject::connect(&previous_layer_button, SIGNAL(clicked()), &placement, SLOT(previous_layer()));
		QObject::connect(&validate_button, SIGNAL(clicked()), this, SLOT(on_validation()));
		QObject::connect(&cancel_button, SIGNAL(clicked()), this, SLOT(close()));
	}

	PortPlacementDialog::~PortPlacementDialog()
	{
		placement.doneCurrent();
	}

	void PortPlacementDialog::on_validation()
	{
		port->set_point(placement.get_new_port_position());
		project->get_logic_model()->update_ports(gate);

		close();
	}
}
