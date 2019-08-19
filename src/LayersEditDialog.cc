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

#include "LayersEditDialog.h"

namespace degate
{
	LayersEditDialog::LayersEditDialog(Project_shptr project, QWidget* parent) : layers(project, parent), QDialog(parent), project(project)
	{
		validate_button.setText("Ok");
		cancel_button.setText("Cancel");

		buttons_layout.addWidget(&validate_button);
		buttons_layout.addWidget(&cancel_button);
		
		layout.addWidget(&layers);
		layout.addLayout(&buttons_layout);

		setLayout(&layout);

		QObject::connect(&validate_button, SIGNAL(clicked()), this, SLOT(validate()));
		QObject::connect(&cancel_button, SIGNAL(clicked()), this, SLOT(close()));
	}

	LayersEditDialog::~LayersEditDialog()
	{
		
	}

	void LayersEditDialog::validate()
	{
		layers.validate();

		close();
	}
}
