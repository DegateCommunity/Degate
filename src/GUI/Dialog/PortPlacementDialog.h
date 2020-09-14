/**
 * This file is part of the IC reverse engineering tool Degate.
 *
 * Copyright 2008, 2009, 2010 by Martin Schobert
 * Copyright 2019-2020 Dorian Bachelot
 *
 * Degate is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * Degate is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with degate. If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef __PORTPLACEMENTDIALOG_H__
#define __PORTPLACEMENTDIALOG_H__

#include "GUI/Widget/PortPlacementWidget.h"

#include <QDialog>
#include <QVBoxLayout>

namespace degate
{

	/**
	 * @class PortPlacementDialog
	 * @brief Dialog to place a port template of a gate template.
	 *
	 * @see QDialog
	 */
	class PortPlacementDialog : public QDialog
	{
		Q_OBJECT

	public:

		/**
		 * Create a port placement dialog.
		 *
		 * @param parent : the parent of the dialog.
		 * @param project : the current active project.
		 * @param port : the port template to place.
		 */
		PortPlacementDialog(QWidget* parent, const Project_shptr& project, const GateTemplate_shptr& gate, const GateTemplatePort_shptr& port);
		~PortPlacementDialog() override;

	public slots:
		/**
		 * Save the new position of the port template.
		 */
		void on_validation();

	private:
		Project_shptr project;
		GateTemplate_shptr gate;
		GateTemplatePort_shptr port;

		QVBoxLayout layout;
		QHBoxLayout quit_buttons_layout;
		QHBoxLayout layers_buttons_layout;
		PortPlacementWidget placement;
		QPushButton validate_button;
		QPushButton cancel_button;
		QPushButton next_layer_button;
		QPushButton previous_layer_button;
	};
}

#endif