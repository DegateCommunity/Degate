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
#include <QSpinBox.h>
#include <QLabel>
#include <QLayout>
#include <QPushButton>
#include <QLineEdit>

namespace degate
{
	class NewProjectDialog : public QDialog
	{
		Q_OBJECT

	public:
		NewProjectDialog(QWidget* parent);
		~NewProjectDialog();

		std::string get_project_name();
		unsigned get_height();
		unsigned get_width();
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