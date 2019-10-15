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

	/**
	 * @class GateEditEntityTab
	 * @brief The entity tab of the gate edit dialog.
	 *
	 * This tab allow the edition of all the properties of the gate template.
	 *
	 * @see GateEditDialog
	 */
	class GateEditEntityTab : public QWidget
	{
		Q_OBJECT

	public:
		
		/**
		 * Create the entity tab.
		 *
		 * @param parent : the parent of the tab.
		 * @param gate : the gate template to edit.
		 * @param project : the current active project.
		 */
		GateEditEntityTab(QWidget* parent, GateTemplate_shptr gate, Project_shptr project);
		~GateEditEntityTab();

	public slots:
		/**
		 * Save all changes.
		 */
		void validate();

	private slots:

		/**
		 * Add a port to the gate template (can't cancel this action).
		 */
		void add_port();

		/**
		 * Remove a port of the gate template (can't cancel this action).
		 */
		void remove_port();

		/**
		 * Update the ports list.
		 */
		void update_ports_list();

		/**
		 * Reset the fill color.
		 */
		void reset_fill_color();

		/**
		 * Reset the frame color (outline).
		 */
		void reset_frame_color();

		/**
		 * Open a new port placement window for the gate template.
		 */
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

	/**
	 * @class GateEditBehaviourTab
	 * @brief The behaviour tab of the gate edit dialog.
	 *
	 * This tab allow the definition of the behaviour of the gate (VHDL/Verilog).
	 *
	 * @see GateEditDialog
	 */
	class GateEditBehaviourTab : public QWidget
	{
		Q_OBJECT

	public:

		/**
		 * Create the behaviour tab.
		 *
		 * @param parent : the parent of the tab.
		 * @param gate : the gate template to edit.
		 * @param project : the current active project.
		 */
		GateEditBehaviourTab(QWidget* parent, GateTemplate_shptr gate, Project_shptr project);
		~GateEditBehaviourTab();

	public slots:
		/**
		 * Save all changes.
		 */
		void validate();

	private:
		GateTemplate_shptr gate = NULL;
		Project_shptr project = NULL;

	};


	/**
	 * @class GateEditLayoutTab
	 * @brief The layout tab of the gate edit dialog.
	 *
	 * This tab show the available images of the gate template on different layers.
	 *
	 * @see GateEditDialog
	 */
	class GateEditLayoutTab : public QWidget
	{
		Q_OBJECT

	public:

		/**
		 * Create the layout tab.
		 *
		 * @param parent : the parent of the tab.
		 * @param gate : the gate template to edit.
		 * @param project : the current active project.
		 */
		GateEditLayoutTab(QWidget* parent, GateTemplate_shptr gate, Project_shptr project);
		~GateEditLayoutTab();

	public slots:
		/**
		 * Save all changes.
		 */
		void validate();

	private:
		GateTemplate_shptr gate = NULL;
		Project_shptr project = NULL;

		QVBoxLayout layout;

		// Metal layer
		QHBoxLayout metal_layout;
		QLabel metal_label;
		ImageRenderer* metal = NULL;

		// Logic layer
		QHBoxLayout logic_layout;
		QLabel logic_label;
		ImageRenderer* logic = NULL;

		// Transistor layer
		QHBoxLayout transistor_layout;
		QLabel transistor_label;
		ImageRenderer* transistor = NULL;

	};

	/**
	 * @class GateEditDialog
	 * @brief Dialog to edit a gate template.
	 *
	 * This window is composed of three tabs (@see GateEditEntityTab, @see GateEditBehaviourTab and @see GateEditLayoutTab).
	 *
	 * @see QDialog
	 */
	class GateEditDialog : public QDialog
	{
		Q_OBJECT

	public:

		/**
		 * Create a gate template edit dialog, to show it call the exec function.
		 *
		 * @param parent : the parent of the dialog.
		 * @param gate : the gate template to edit.
		 * @param project : the current active project.
		 *
		 * @see QDialog
		 */
		GateEditDialog(QWidget* parent, GateTemplate_shptr gate, Project_shptr project);
		~GateEditDialog();

	public slots:
		/**
		 * Save all changes.
		 */
		virtual void validate();

	protected:
		QDialogButtonBox button_box;
		QVBoxLayout layout;
		Project_shptr project = NULL;

	private:
		GateTemplate_shptr gate = NULL;

		QTabWidget tab;

		GateEditEntityTab entity_tab;
		GateEditBehaviourTab behaviour_tab;
		GateEditLayoutTab layout_tab;
	};

	/**
	 * @class GateInstanceEditDialog
	 * @brief Dialog to edit a gate instance (the gate orientation and his attached gate template).
	 *
	 * This window is composed of three tabs (@see GateEditEntityTab, @see GateEditBehaviourTab and @see GateEditLayoutTab) and an orientation selector.
	 *
	 * @see QDialog
	 */
	class GateInstanceEditDialog : public GateEditDialog
	{
		Q_OBJECT

	public:

		/**
		 * Create a gate instance edit dialog, to show it call the exec function.
		 *
		 * @param parent : the parent of the dialog.
		 * @param gate : the gate to edit.
		 * @param project : the current active project.
		 *
		 * @see QDialog
		 */
		GateInstanceEditDialog(QWidget* parent, Gate_shptr gate, Project_shptr project);
		~GateInstanceEditDialog();

	public slots:
		/**
		 * Save all changes.
		 */
		void validate();

	private:
		Gate_shptr gate = NULL;

		QHBoxLayout orientation_layout;
		QLabel orientation_label;
		QComboBox orientation;
	};
}

#endif