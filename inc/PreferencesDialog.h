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

#ifndef __PREFERENCESDIALOG_H__
#define __PREFERENCESDIALOG_H__

#include "ThemePickerWidget.h"
#include "PreferencesHandler.h"

#include <QDialog>
#include <QPushButton>
#include <QMessageBox>

namespace degate
{

	/**
	 * @class PreferencesDialog
	 * @brief Dialog to edit preferences.
	 *
	 * @see QDialog
	 */
	class PreferencesDialog : public QDialog
	{
		Q_OBJECT
		
	public:

		/**
		 * Create the preferences edit dialog.
		 *
		 * @param parent : the parent of the dialog.
		 */
		PreferencesDialog(QWidget* parent);
		~PreferencesDialog();

	public slots:
		/**
		 * Save changes on the preferences.
		 */
		void validate();

	private:
		QVBoxLayout layout;

		// Validation buttons
		QHBoxLayout validation_buttons_layout;
		QPushButton save_button;
		QPushButton cancel_button;
		
		// Theme
		ThemePickerWidget theme;
	};
}

#endif