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

#include "Core/Project.h"
#include "Core/Layer.h"

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
	 * @class LayerBackgroundSelectionButton
	 * @brief Layer background file selection, on click open a selection window and hold the image path.
	 *
	 * It show the state, red if no background set, green otherwise.
	 *
	 * @see QPushButton
	 */
	class LayerBackgroundSelectionButton : public QPushButton
	{
		Q_OBJECT
		
	public:
		/**
		 * Specify the concerned layer to set the state of the button (if the layer already have a background image so the state will be true == green color).
		 *
		 * @param layer : the concerned layer.
		 * @param parent : the parent of the button.
		 */
		LayerBackgroundSelectionButton(Layer_shptr layer, QWidget* parent);

		/**
		 * Create a layer background selection button from another one (copy).
		 *
		 * @param copy : the other layer background selection button to copy.
		 */
		LayerBackgroundSelectionButton(LayerBackgroundSelectionButton& copy);
		~LayerBackgroundSelectionButton();

		/**
		 * Was the button clicked and a new image selected.
		 *
		 * @return Return true if a new image had been selected.
		 */
		bool has_new_image();

		/**
		 * Get the image path, empty if no new image selected.
		 * @see has_new_image()
		 *
		 * @return Return the new image path (can be empty if no new image).
		 */
		std::string get_image_path();

		/**
		 * Get the state of the button.
		 *
		 * @return Return true if green (image set), false if red (not image set).
		 */
		bool get_state();

	private slots:

		/**
		 * The function is called when the button is clicked.
		 * It will open a image selection window to select the new background.
		 */
		void on_button_clicked();

		/**
		 * Change the button color.
		 *
		 * @param value : if true the color will turn green, if falser the color will turn red.
		 */
		void change_button_color(bool value);

	private:
		std::string image_path;
		bool new_image = false;
		bool state;
		
	};

	/**
	 * @class LayerTypeSelectionBox
	 * @brief Combo box to select the type of a layer.
	 *
	 * @see QComboBox
	 */
	class LayerTypeSelectionBox : public QComboBox
	{
		Q_OBJECT
		
	public:

		/**
		 * Create the layer type selection box.
		 *
		 * @param type : the layer type.
		 * @param parent : the parent of the combo box.
		 */
		LayerTypeSelectionBox(Layer::LAYER_TYPE type, QWidget* parent);

		/**
		 * Create the layer type selection box from another one (copy).
		 *
		 * @param copy : the other box to copy.
		 */
		LayerTypeSelectionBox(LayerTypeSelectionBox& copy);
		~LayerTypeSelectionBox();

		/**
		 * Get the layer type.
		 *
		 * @return Return the layer type of the attached layer.
		 */
		Layer::LAYER_TYPE get_layer_type();

		/**
		 * Set the layer type.
		 *
		 * @param type : the new layer type.
		 */
		void set_layer_type(Layer::LAYER_TYPE type);
		
	private:
		/**
		 * Convert a layer type enum to a QString.
		 *
		 * @param type : the type of the layer to convert.
		 *
		 * @return Return the converted layer type.
		 */
		QString from_type(Layer::LAYER_TYPE type);

		/**
		 * Convert a layer type string to the enum type.
		 *
		 * @param type : the type of the layer to convert.
		 *
		 * @return Return the converted layer type.
		 */
		Layer::LAYER_TYPE to_type(QString type);
		
		Layer::LAYER_TYPE type;
		
	};

	/**
	 * @struct LayersEditRow
	 * @brief Describe a row of the layer edit widget with all his elements.
	 */
	struct LayersEditRow
	{
		QTableWidgetItem* id; /*!< The item representing the id of the layer. */
		QTableWidgetItem* enabled; /*!< The item representing the state of the layer. */
		QTableWidgetItem* description; /*!< The item representing the description of the layer. */
		LayerTypeSelectionBox* type; /*!< The box representing the type of the layer. */
		LayerBackgroundSelectionButton* background; /*!< The background selection button of the layer. */
	};

	/**
	 * @enum RowMoveDirection
	 * @brief Describe the rown move direction.
	 */
	enum RowMoveDirection
	{
		UP = 0, /*!< Move the row up. */
		DOWN = 1 /*!< Move the row down. */
	};

	/**
	 * @class LayersEditWidget
	 * @brief Widget to edit all layers of a project.
	 *
	 * @warning The list is ordered !
	 *
	 * @see QWidget
	 */
	class LayersEditWidget : public QWidget
	{
		Q_OBJECT
		
	public:
		
		/**
		 * Create a layers edit widget.
		 *
		 * @param project : the current active project.
		 * @param parent : the parent of the widget.
		 */
		LayersEditWidget(Project_shptr project, QWidget* parent);
		~LayersEditWidget();
		
	public slots:
		/**
		 * Add a new layer to the list.
		 */
		void on_layer_add();

		/**
		 * Remove a layer of the list.
		 */
		void on_layer_remove();

		/**
		 * Move a layer up (the list is ordered).
		 */
		void on_layer_up();

		/**
		 * Move a layer down (the list is ordered).
		 */
		void on_layer_down();

		/**
		 * Validate all changes and apply them.
		 */
		void validate();
		
	private:
		/**
		 * Move the row.
		 *
		 * @param row_index : the index of the row to move.
		 * @param direction : the direction to move the row (@see RowMoveDirection).
		 */
		void move_row(unsigned row_index, RowMoveDirection direction);

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