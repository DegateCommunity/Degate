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

#ifndef __LAYEREDITDIALOG_H__
#define __LAYEREDITDIALOG_H__

#include "LayersEditWidget.h"

#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>

namespace degate
{
	class LayersEditDialog : public QDialog
	{
		Q_OBJECT
		
	public:
		LayersEditDialog(Project_shptr project, QWidget* parent);
		~LayersEditDialog();

	public slots:
		/**
		 * Validate all changes and apply them.
		 */
		void validate();

	private:
		Project_shptr project;
		QVBoxLayout layout;

		LayersEditWidget layers;
		QHBoxLayout buttons_layout;
		QPushButton validate_button;
		QPushButton cancel_button;
		
	};
}

#endif