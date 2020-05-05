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

#include "GUI/NewProjectDialog.h"

namespace degate
{
	NewProjectDialog::NewProjectDialog(QWidget* parent) : QDialog(parent)
	{
		project_label.setText("Project name :");
		layer_label.setText("Layer count :");
		width_label.setText("Project width :");
		height_label.setText("Project height :");

		layer_count.setMinimum(0);
		layer_count.setMaximum(50);

		width.setMinimum(0);
		width.setMaximum(1000000);

		height.setMinimum(0);
		height.setMaximum(1000000);

		validate_button.setText("Validate");

		main_layout.addWidget(&project_label);
		main_layout.addWidget(&project_name);
		main_layout.addWidget(&layer_label);
		main_layout.addWidget(&layer_count);
		main_layout.addWidget(&width_label);
		main_layout.addWidget(&width);
		main_layout.addWidget(&height_label);
		main_layout.addWidget(&height);
		main_layout.addWidget(&validate_button);

		QObject::connect(&validate_button, SIGNAL(pressed()), this, SLOT(accept()));

		setLayout(&main_layout);
	}

	NewProjectDialog::~NewProjectDialog()
	{

	}

	std::string NewProjectDialog::get_project_name()
	{
		return project_name.text().toStdString();
	}

	unsigned NewProjectDialog::get_height()
	{
		return height.value();
	}

	unsigned NewProjectDialog::get_width()
	{
		return width.value();
	}

	unsigned NewProjectDialog::get_layer_count()
	{
		return layer_count.value();
	}
}
