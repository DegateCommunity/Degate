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

#include "PortPlacementDialog.h"

namespace degate
{
	PortPlacementDialog::PortPlacementDialog(QWidget* parent, Project_shptr project, GateTemplate_shptr gate, GateTemplatePort_shptr port) : QDialog(parent), 
																																			 gate(gate), 
																																			 port(port), 
																																			 project(project), 
																																			 placement(project, gate, port, this)
	{
		resize(gate->get_width() + 300, gate->get_height() + 300);
		placement.resize(gate->get_width(), gate->get_height());
		next_layer_button.setText("Next layer");
		previous_layer_button.setText("Previous layer");
		validate_button.setText("Ok");

		layout.addWidget(&placement);
		layout.addWidget(&next_layer_button);
		layout.addWidget(&previous_layer_button);
		layout.addWidget(&validate_button);

		setLayout(&layout);

		QObject::connect(&next_layer_button, SIGNAL(clicked()), &placement, SLOT(next_layer()));
		QObject::connect(&previous_layer_button, SIGNAL(clicked()), &placement, SLOT(previous_layer()));
		QObject::connect(&validate_button, SIGNAL(clicked()), this, SLOT(on_validation()));
	}

	PortPlacementDialog::~PortPlacementDialog()
	{

	}

	void PortPlacementDialog::on_validation()
	{
		project->get_logic_model()->update_ports(gate);

		close();
	}
}
