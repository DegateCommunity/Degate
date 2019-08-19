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

#ifndef __LAYEREDITWIDGET_H__
#define __LAYEREDITWIDGET_H__

#include "Project.h"
#include "Layer.h"

#include <QWidget>
#include <QLabel>
#include <QTableWidget>
#include <QGridLayout>
#include <QPushButton>
#include <QComboBox>
#include <QFileDialog>

namespace degate
{

	/**
	 * Layer background file selection, on click open a selection window and hold the image path. It show the state, red if no background set, green otherwise.
	 */
	class LayerBackgroundSelectionButton : public QPushButton
	{
		Q_OBJECT
		
	public:
		/**
		 * Specify the concerned layer to set the state of the button (if the layer already have a background image so the state will be true == green color).
		 */
		LayerBackgroundSelectionButton(Layer_shptr layer, QWidget* parent);
		LayerBackgroundSelectionButton(LayerBackgroundSelectionButton& copy);
		~LayerBackgroundSelectionButton();

		/**
		 * Was the button clicked and a new image selected.
		 */
		bool has_new_image();

		/**
		 * Get the image path, empty if no new image selected.
		 * @see has_new_image()
		 */
		std::string get_image_path();

		/**
		 * Get the state of the button (true if green, false if red).
		 */
		bool get_state();

	private slots:
		void on_button_clicked();
		void change_button_color(bool value);

	private:
		std::string image_path;
		bool new_image = false;
		bool state;
		
	};

	/**
	 * Layer type combo box.
	 */
	class LayerTypeSelectionBox : public QComboBox
	{
		Q_OBJECT
		
	public:
		LayerTypeSelectionBox(Layer::LAYER_TYPE type, QWidget* parent);
		LayerTypeSelectionBox(LayerTypeSelectionBox& copy);
		~LayerTypeSelectionBox();

		Layer::LAYER_TYPE get_layer_type();
		void set_layer_type(Layer::LAYER_TYPE type);
		
	private:
		QString from_type(Layer::LAYER_TYPE type);
		Layer::LAYER_TYPE to_type(QString type);
		
		Layer::LAYER_TYPE type;
		
	};

	/**
	 * Describe a row with all his elements.
	 */
	struct LayersEditRow
	{
		QTableWidgetItem* id;
		QTableWidgetItem* enabled;
		QTableWidgetItem* description;
		LayerTypeSelectionBox* type;
		LayerBackgroundSelectionButton* background;
	};

	enum RowMoveDirection
	{
		UP = 0,
		DOWN = 1
	};
	
	class LayersEditWidget : public QWidget
	{
		Q_OBJECT
		
	public:
		LayersEditWidget(Project_shptr project, QWidget* parent);
		~LayersEditWidget();
		
	public slots:
		void on_layer_add();
		void on_layer_remove();
		void on_layer_up();
		void on_layer_down();

		/**
		 * Validate all changes and apply them.
		 */
		void validate();
		
	private:
		void move_row(unsigned row_indice, RowMoveDirection direction);

		Project_shptr project;
		QGridLayout layout;

		// Layers
		QLabel layers_label;
		QTableWidget layers;
		QHBoxLayout layers_add_remove_buttons_layout;
		QPushButton layers_add_button;
		QPushButton layers_remove_button;
		QHBoxLayout layers_move_buttons_layout;
		QPushButton layers_up_buttons;
		QPushButton layers_down_buttons;
	};
}

#endif