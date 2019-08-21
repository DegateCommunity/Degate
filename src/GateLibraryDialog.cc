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

#include "GateLibraryDialog.h"

namespace degate
{
	GateLibraryDialog::GateLibraryDialog(Project_shptr project, QWidget* parent) : QDialog(parent), list(project, parent), project(project)
	{
		setWindowTitle("Gate library");
		resize(300, 400);
		
		edit_gate_button.setText("Edit");
		validate_button.setText("Ok");
		buttons_layout.addWidget(&edit_gate_button);
		buttons_layout.addWidget(&validate_button);

		layout.addWidget(&list);
		layout.addLayout(&buttons_layout);
		
		setLayout(&layout);

		QObject::connect(&edit_gate_button, SIGNAL(clicked()), this, SLOT(on_edit_gate_button()));
		QObject::connect(&validate_button, SIGNAL(clicked()), this, SLOT(close()));
	}

	GateLibraryDialog::~GateLibraryDialog()
	{
		
	}

	void GateLibraryDialog::on_edit_gate_button()
	{
		if(!list.has_selection())
			return;

		std::vector<GateTemplate_shptr> selected_gates = list.get_selected_gates();

		for(auto& e : selected_gates)
		{
			GateEditDialog dialog(this, e, project);
			dialog.exec();
		}

		list.update_list();
	}
}
