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

#ifndef __GATELIBRARYDIALOG_H__
#define __GATELIBRARYDIALOG_H__

#include "GateTemplateListWidget.h"
#include "GateEditDialog.h"

#include <QDialog>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>

namespace degate
{

	/**
	 * @class GateLibraryDialog
	 * @brief Dialog to show all gates of a project.
	 *
	 * It will show all gates template of the current active project.
	 * It have a button to edit them individually.
	 *
	 * @see QDialog
	 */
	class GateLibraryDialog : public QDialog
	{
		Q_OBJECT
		
	public:

		/**
		 * Create the dialog, to show it call the exec function.
		 *
		 * @param project : the current active project.
		 * @param parent : the parent of the dialog.
		 *
		 * @see QDialog
		 */
		GateLibraryDialog(Project_shptr project, QWidget* parent);
		~GateLibraryDialog();

	public slots:
		/**
		 * Run a gate edit dialog on all the selected gates template (selected rows).
		 */
		void on_edit_gate_button();

	private:
		Project_shptr project;
		QVBoxLayout layout;

		QHBoxLayout buttons_layout;
		GateTemplateListWidget list;
		QPushButton edit_gate_button;
		QPushButton validate_button;
		
	};
}

#endif