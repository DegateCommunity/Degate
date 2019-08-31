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

#include "PreferencesDialog.h"

namespace degate
{
	PreferencesDialog::PreferencesDialog(QWidget* parent) : QDialog(parent), theme(this, PREFERENCES_HANDLER.get_theme(), PREFERENCES_HANDLER.get_icon_theme())
	{
		layout.addWidget(&theme);

		// Save
		save_button.setText("Save");
		validation_buttons_layout.addWidget(&save_button);
		QObject::connect(&save_button, SIGNAL(clicked()), this, SLOT(validate()));

		// Cancel
		cancel_button.setText("Cancel");
		validation_buttons_layout.addWidget(&cancel_button);
		QObject::connect(&cancel_button, SIGNAL(clicked()), this, SLOT(close()));
		
		layout.addLayout(&validation_buttons_layout);
		
		setLayout(&layout);
	}

	PreferencesDialog::~PreferencesDialog()
	{
	}

	void PreferencesDialog::validate()
	{
		PREFERENCES_HANDLER.set_theme(theme.get_theme());
		PREFERENCES_HANDLER.set_icon_theme(theme.get_icon_theme());

		PREFERENCES_HANDLER.save();
		close();
	}
}
