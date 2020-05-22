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

#ifndef __PREFERENCESEDITOR_H__
#define __PREFERENCESEDITOR_H__

#include "GUI/Preferences/PreferencesPage/PreferencesPage.h"
#include "GUI/Preferences/PreferencesPage/AppearancePreferencesPage.h"
#include "PreferencesHandler.h"

#include <QDialog>
#include <QPushButton>
#include <QMessageBox>
#include <QListWidget>
#include <QStackedWidget>
#include <QScrollArea>

namespace degate
{

	/**
	 * @class PreferencesEditor
	 * @brief Dialog to edit preferences.
	 *
	 * @see QDialog
	 */
	class PreferencesEditor : public QDialog
	{
		Q_OBJECT
		
	public:

		/**
		 * Create the preferences editor dialog.
		 *
		 * @param parent : the parent of the dialog.
		 */
		PreferencesEditor(QWidget* parent);
		~PreferencesEditor();

	public slots:
		/**
		 * Save changes.
		 */
		void validate();

		/**
		 * Change page.
		 */
        void change_page(QListWidgetItem* current, QListWidgetItem* previous);

	private:
		QVBoxLayout layout;
		Preferences preferences;
        QHBoxLayout content_layout;
        QScrollArea scroll_area;

        // Pages list
        QListWidget pages_list;
        QVBoxLayout preferences_pages_layout;
        QStackedWidget preferences_pages;

		// Validation buttons
		QHBoxLayout buttons_layout;
		QPushButton save_button;
		QPushButton cancel_button;
		
		// Pages
		std::map<QString, PreferencesPage*> pages;
	};
}

#endif