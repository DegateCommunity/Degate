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

#ifndef __SELECTGATETEMPLATEDIALOG_H__
#define __SELECTGATETEMPLATEDIALOG_H__

#include "Core/Project.h"
#include "GateTemplateListWidget.h"

#include <QDialog>
#include <QVBoxLayout>
#include <QPushButton>

namespace degate
{
	/**
	 * @class SelectGateTemplateDialog
	 * @brief Dialog to list all gates template of the logic model and get the selected one.
	 *
	 * @see QDialog
	 */
	class SelectGateTemplateDialog : public QDialog
	{
		Q_OBJECT
		
	public:

		/**
		 * Create the gate template selection dialog.
		 *
		 * @param project : the current active project.
		 * @param parent : the parent of the dialog.
		 */
		SelectGateTemplateDialog(Project_shptr project, QWidget* parent);
		~SelectGateTemplateDialog();

		/**
		 * Get the selected gate template.
		 *
		 * @return Return the selected gate template.
		 */
		GateTemplate_shptr get_selected_gate();
		
	private:
		GateTemplateListWidget list;
		QVBoxLayout layout;
		QPushButton validate_button;
	};
}

#endif