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

#ifndef __GATEEDITDIALOG_H__
#define __GATEEDITDIALOG_H__

#include "Project.h"
#include "ColorSelection.h"
#include "PortPlacementDialog.h"

#include <QDialog>
#include <QWidget>
#include <QTabWidget>
#include <QDialogButtonBox>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QColorDialog>
#include <QTableWidget>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>

namespace degate
{
	class GateEditEntityTab : public QWidget
	{
		Q_OBJECT

	public:
		GateEditEntityTab(QWidget* parent, GateTemplate_shptr gate_to_edit, Project_shptr project);
		~GateEditEntityTab();

	public slots:
		/*
		 * Save all changes.
		 */
		void validate();

	private slots:
		void add_port();
		void remove_port();
		void update_ports_list();
		void reset_fill_color();
		void reset_frame_color();
		void on_port_place();

	private:
		GateTemplate_shptr gate = NULL;
		Project_shptr project = NULL;
		QGridLayout layout;

		// Name
		QLabel name_label;
		QLineEdit name;

		// Description
		QLabel description_label;
		QLineEdit description;

		// Logic class
		QLabel logic_class_label;
		QComboBox logic_class;

		// Ports
		QLabel ports_label;
		QTableWidget ports;
		QHBoxLayout ports_buttons_layout;
		QPushButton ports_add_button;
		QPushButton ports_remove_button;
		QPushButton ports_place;

		// Fill color
		QLabel fill_color_label;
		ColorSelectionButton fill_color;
		QHBoxLayout fill_color_layout;
		QPushButton fill_color_reset_button;

		// Frame color
		QLabel frame_color_label;
		ColorSelectionButton frame_color;
		QHBoxLayout frame_color_layout;
		QPushButton frame_color_reset_button;

	};

	class GateEditBehaviourTab : public QWidget
	{
		Q_OBJECT

	public:
		GateEditBehaviourTab(QWidget* parent, GateTemplate_shptr gate_to_edit, Project_shptr project);
		~GateEditBehaviourTab();

	public slots:
		/*
		 * Save all changes.
		 */
		void validate();

	private:
		GateTemplate_shptr gate = NULL;
		Project_shptr project = NULL;

	};

	class GateEditLayoutTab : public QWidget
	{
		Q_OBJECT

	public:
		GateEditLayoutTab(QWidget* parent, GateTemplate_shptr gate_to_edit, Project_shptr project);
		~GateEditLayoutTab();

	public slots:
		/*
		 * Save all changes.
		 */
		void validate();

	private:
		GateTemplate_shptr gate = NULL;
		Project_shptr project = NULL;

	};

	class GateEditDialog : public QDialog
	{
		Q_OBJECT

	public:
		GateEditDialog(QWidget* parent, GateTemplate_shptr gate_to_edit, Project_shptr project);
		~GateEditDialog();

	public slots:
		/*
		 * Save all changes.
		 */
		void validate();

	private:
		GateTemplate_shptr gate = NULL;
		Project_shptr project = NULL;

		QTabWidget tab;
		QDialogButtonBox button_box;
		QVBoxLayout layout;

		GateEditEntityTab entity_tab;
		GateEditBehaviourTab behaviour_tab;
		GateEditLayoutTab layout_tab;
	};
}

#endif