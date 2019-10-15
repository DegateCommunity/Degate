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

#ifndef __NEWPROJECTWINDOW_H__
#define __NEWPROJECTWINDOW_H__

#include <QDialog>
#include <QSpinBox>
#include <QLabel>
#include <QLayout>
#include <QPushButton>
#include <QLineEdit>

namespace degate
{

	/**
	 * @class NewProjectDialog
	 * @brief Dialog for creating a new project.
	 *
	 * @see QDialog
	 */
	class NewProjectDialog : public QDialog
	{
		Q_OBJECT

	public:

		/**
		 * Create the new project dialog.
		 *
		 * @param parent : the parent of the dialog.
		 */
		NewProjectDialog(QWidget* parent);
		~NewProjectDialog();

		/**
		 * Get the new project name.
		 *
		 * @return Return the new project name.
		 */
		std::string get_project_name();
		unsigned get_height();

		/**
		 * Get the width of the new project.
		 *
		 * @return Return the width of the new project.
		 */
		unsigned get_width();

		/**
		 * Get the new project layer count.
		 *
		 * @return Return the number of layers of the new project.
		 */
		unsigned get_layer_count();

	private:
		QSpinBox height;
		QSpinBox width;
		QSpinBox layer_count;
		QLabel layer_label;
		QLabel width_label;
		QLabel height_label;
		QLabel project_label;
		QVBoxLayout main_layout;
		QPushButton validate_button;
		QLineEdit project_name;
	};
}

#endif