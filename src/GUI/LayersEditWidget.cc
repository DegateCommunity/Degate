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

#include "GUI/LayersEditWidget.h"
#include "Core/ImageHelper.h"
#include "Core/LogicModelHelper.h"

namespace degate
{
	LayerBackgroundSelectionButton::LayerBackgroundSelectionButton(Layer_shptr layer, QWidget* parent) : QPushButton(parent)
	{
		if(layer == NULL)
			change_button_color(false);
		else
			change_button_color(layer->has_background_image());
		
		QObject::connect(this, SIGNAL(clicked()), this, SLOT(on_button_clicked()));
	}

	LayerBackgroundSelectionButton::LayerBackgroundSelectionButton(LayerBackgroundSelectionButton& copy) : QPushButton(copy.parentWidget())
	{
		change_button_color(copy.get_state());

		if(copy.has_new_image())
			new_image = true;
		else
			new_image = false;
		
		image_path = copy.get_image_path();

		QObject::connect(this, SIGNAL(clicked()), this, SLOT(on_button_clicked()));
	}

	LayerBackgroundSelectionButton::~LayerBackgroundSelectionButton()
	{
		
	}

	bool LayerBackgroundSelectionButton::has_new_image()
	{
		return new_image;
	}

	std::string LayerBackgroundSelectionButton::get_image_path()
	{
		return image_path;
	}

	bool LayerBackgroundSelectionButton::get_state()
	{
		return state;
	}

	void LayerBackgroundSelectionButton::on_button_clicked()
	{
		QString res = QFileDialog::getOpenFileName(this, "Select the background image");
		image_path = res.toStdString();

		change_button_color(true);
		new_image = true;
	}

	void LayerBackgroundSelectionButton::change_button_color(bool value)
	{
		if(value)
			setStyleSheet("background-color: rgba(0, 150, 0, 255)");
		else
			setStyleSheet("background-color: rgba(150, 0, 0, 255)");

		state = value;
	}

	LayerTypeSelectionBox::LayerTypeSelectionBox(Layer::LAYER_TYPE type, QWidget* parent) : type(type), QComboBox(parent)
	{
		setSizeAdjustPolicy(AdjustToContents);
		
		addItem("undefined");
		addItem("transistor");
		addItem("logic");
		addItem("metal");
		setCurrentText(from_type(type));
	}

	LayerTypeSelectionBox::LayerTypeSelectionBox(LayerTypeSelectionBox& copy) : QComboBox(copy.parentWidget())
	{
		setSizeAdjustPolicy(AdjustToContents);
		
		addItem("undefined");
		addItem("transistor");
		addItem("logic");
		addItem("metal");
		setCurrentText(from_type(copy.get_layer_type()));

		type = copy.get_layer_type();
	}

	LayerTypeSelectionBox::~LayerTypeSelectionBox()
	{
	}

	Layer::LAYER_TYPE LayerTypeSelectionBox::get_layer_type()
	{
		type = to_type(currentText());
		return type;
	}

	void LayerTypeSelectionBox::set_layer_type(Layer::LAYER_TYPE type)
	{
		this->type = type;
		setCurrentText(from_type(type));
	}

	QString LayerTypeSelectionBox::from_type(Layer::LAYER_TYPE type)
	{
		switch(type)
		{
		case Layer::METAL:
			return "metal";
			break;
		case Layer::TRANSISTOR:
			return "transistor";
			break;
		case Layer::LOGIC:
			return "logic";
			break;
		case Layer::UNDEFINED:
			return "undefined";
			break;
		default:
			return "undefined";
			break;
		}
	}

	Layer::LAYER_TYPE LayerTypeSelectionBox::to_type(QString type)
	{
		if(type == "metal")
			return Layer::METAL;
		else if(type == "transistor")
			return Layer::TRANSISTOR;
		else if(type == "logic")
			return Layer::LOGIC;
		else if(type == "undefined")
			return Layer::UNDEFINED;
		else
			return Layer::UNDEFINED;
	}

	LayersEditWidget::LayersEditWidget(Project_shptr project, QWidget* parent) : QWidget(parent), project(project)
	{
		// Label
		layers_label.setText("Layer config :");

		// List
		layers.setColumnCount(5);
		QStringList list;
		list.append("ID");
		list.append("Enable");
		list.append("Description");
		list.append("Type");
		list.append("Background");
		layers.setHorizontalHeaderLabels(list);

		// Add/remove Buttons
		layers_add_button.setText("Add");
		layers_remove_button.setText("Remove");
		layers_add_remove_buttons_layout.addWidget(&layers_add_button);
		layers_add_remove_buttons_layout.addWidget(&layers_remove_button);

		// Up/Down buttons
		layers_up_buttons.setText("Up");
		layers_down_buttons.setText("Down");
		layers_move_buttons_layout.addWidget(&layers_up_buttons);
		layers_move_buttons_layout.addWidget(&layers_down_buttons);

		// Layout
		layout.addWidget(&layers_label, 0, 0);
		layout.addWidget(&layers, 0, 1);
		layout.addLayout(&layers_add_remove_buttons_layout, 1, 1);
		layout.addLayout(&layers_move_buttons_layout, 2, 1);

		setLayout(&layout);

		// Initialize the layer list
		for(LogicModel::layer_collection::iterator iter = project->get_logic_model()->layers_begin(); iter != project->get_logic_model()->layers_end(); ++iter)
		{
			Layer_shptr layer = *iter;

			if(layer == NULL)
				continue;

			layers.insertRow(layers.rowCount());

			// Id
			QTableWidgetItem* id_item = new QTableWidgetItem(QString::number(layer->get_layer_id()));
			id_item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
			layers.setItem(layers.rowCount() - 1, 0, id_item);

			// Enabled
			QTableWidgetItem* enabled = new QTableWidgetItem();
			enabled->setCheckState(layer->is_enabled() ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
			layers.setItem(layers.rowCount() - 1, 1, enabled);

			// Description
			layers.setItem(layers.rowCount() - 1, 2, new QTableWidgetItem(QString::fromStdString(layer->get_description())));

			// Type
			LayerTypeSelectionBox* cb = new LayerTypeSelectionBox(layer->get_layer_type(), this);
			layers.setCellWidget(layers.rowCount() - 1, 3, cb);

			// Background
			LayerBackgroundSelectionButton* bb = new LayerBackgroundSelectionButton(layer, this);
			layers.setCellWidget(layers.rowCount() - 1, 4, bb);
		}

		layers.resizeColumnsToContents();
		layers.resizeRowsToContents();

		QObject::connect(&layers_add_button, SIGNAL(clicked()), this, SLOT(on_layer_add()));
		QObject::connect(&layers_remove_button, SIGNAL(clicked()), this, SLOT(on_layer_remove()));
		QObject::connect(&layers_up_buttons, SIGNAL(clicked()), this, SLOT(on_layer_up()));
		QObject::connect(&layers_down_buttons, SIGNAL(clicked()), this, SLOT(on_layer_down()));
	}

	LayersEditWidget::~LayersEditWidget()
	{
	}

	void LayersEditWidget::on_layer_add()
	{
		layers.insertRow(layers.rowCount());

		// Id
		QTableWidgetItem* id_item = new QTableWidgetItem("?");
		id_item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
		layers.setItem(layers.rowCount() - 1, 0, id_item);

		// Enabled
		QTableWidgetItem* enabled = new QTableWidgetItem();
		enabled->setCheckState(Qt::CheckState::Checked);
		layers.setItem(layers.rowCount() - 1, 1, enabled);

		// Description
		layers.setItem(layers.rowCount() - 1, 2, new QTableWidgetItem(""));

		// Type
		LayerTypeSelectionBox* cb = new LayerTypeSelectionBox(Layer::UNDEFINED, this);
		layers.setCellWidget(layers.rowCount() - 1, 3, cb);

		// Background
		LayerBackgroundSelectionButton* bb = new LayerBackgroundSelectionButton(NULL, this);
		layers.setCellWidget(layers.rowCount() - 1, 4, bb);

		layers.selectRow(layers.rowCount() - 1);
	}

	void LayersEditWidget::on_layer_remove()
	{
		QItemSelectionModel* select = layers.selectionModel();

		if(!select->hasSelection())
			return;

		QModelIndexList index = select->selectedRows();

		for(auto sel = index.begin(); sel != index.end(); ++sel)
		{
			if(sel->isValid())
				layers.removeRow(sel->row());
		}
	}

	void LayersEditWidget::validate()
	{
		LogicModel::layer_collection layer_collection;

		for(unsigned int i = 0; i < layers.rowCount(); i++)
		{
			Layer_shptr layer;
			
			// Id
			QString text_id = layers.item(i, 0)->text();
			if(text_id == '?')
			{
				// New layer
				layer = Layer_shptr(new Layer(BoundingBox(project->get_logic_model()->get_width(), project->get_logic_model()->get_height())));
				layer->set_layer_id(project->get_logic_model()->get_new_layer_id());
			}
			else
			{
				// Old layer
				layer_id_t id = text_id.toInt();
				layer = project->get_logic_model()->get_layer_by_id(id);
			}

			// Enabled
			layer->set_enabled(layers.item(i, 1)->checkState() == Qt::CheckState::Checked ? true : false);
			
			// Description
			layer->set_description(layers.item(i, 2)->text().toStdString());

			// Type
			layer->set_layer_type(static_cast<LayerTypeSelectionBox*>(layers.cellWidget(i, 3))->get_layer_type());

			// Image
			LayerBackgroundSelectionButton* background = static_cast<LayerBackgroundSelectionButton*>(layers.cellWidget(i, 4));
			if(background->has_new_image())
				load_background_image(layer, project->get_project_directory(), background->get_image_path());

			// Position
			layer->set_layer_pos(i);
			
			layer_collection.push_back(layer);
		}

		project->get_logic_model()->set_layers(layer_collection);
	}

	void LayersEditWidget::on_layer_up()
	{
		QItemSelectionModel* select = layers.selectionModel();

		if(!select->hasSelection())
			return;

		QModelIndexList index = select->selectedRows();

		for(auto sel = index.begin(); sel != index.end(); ++sel)
		{
			move_row(sel->row(), UP);
		}
	}

	void LayersEditWidget::on_layer_down()
	{
		QItemSelectionModel* select = layers.selectionModel();

		if(!select->hasSelection())
			return;

		QModelIndexList index = select->selectedRows();

		for(auto sel = index.begin(); sel != index.end(); ++sel)
		{
			move_row(sel->row(), DOWN);
		}
	}

	void LayersEditWidget::move_row(unsigned row_index, RowMoveDirection direction)
	{
		LayersEditRow source;
		LayersEditRow destination;

		if(direction == UP)
		{
			if(row_index == 0)
				return;

			// Get source
			source.id = layers.takeItem(row_index, 0);
			source.enabled = layers.takeItem(row_index, 1);
			source.description = layers.takeItem(row_index, 2);
			source.type = new LayerTypeSelectionBox(*static_cast<LayerTypeSelectionBox*>(layers.cellWidget(row_index, 3)));
			source.background = new LayerBackgroundSelectionButton(*static_cast<LayerBackgroundSelectionButton*>(layers.cellWidget(row_index, 4)));

			// Get destination
			destination.id = layers.takeItem(row_index - 1, 0);
			destination.enabled = layers.takeItem(row_index - 1, 1);
			destination.description = layers.takeItem(row_index - 1, 2);
			destination.type = new LayerTypeSelectionBox(*static_cast<LayerTypeSelectionBox*>(layers.cellWidget(row_index - 1, 3)));
			destination.background = new LayerBackgroundSelectionButton(*static_cast<LayerBackgroundSelectionButton*>(layers.cellWidget(row_index - 1, 4)));

			
			// Set new source
			layers.setItem(row_index - 1, 0, source.id);
			layers.setItem(row_index - 1, 1, source.enabled);
			layers.setItem(row_index - 1, 2, source.description);
			layers.setCellWidget(row_index - 1, 3, source.type);
			layers.setCellWidget(row_index - 1, 4, source.background);

			// Set new destination
			layers.setItem(row_index, 0, destination.id);
			layers.setItem(row_index, 1, destination.enabled);
			layers.setItem(row_index, 2, destination.description);
			layers.setCellWidget(row_index, 3, destination.type);
			layers.setCellWidget(row_index, 4, destination.background);

			layers.selectRow(row_index - 1);
		}
		else
		{
			if(row_index == layers.rowCount() - 1)
				return;

			// Get source
			source.id = layers.takeItem(row_index, 0);
			source.enabled = layers.takeItem(row_index, 1);
			source.description = layers.takeItem(row_index, 2);
			source.type = new LayerTypeSelectionBox(*static_cast<LayerTypeSelectionBox*>(layers.cellWidget(row_index, 3)));
			source.background = new LayerBackgroundSelectionButton(*static_cast<LayerBackgroundSelectionButton*>(layers.cellWidget(row_index, 4)));

			// Get destination
			destination.id = layers.takeItem(row_index + 1, 0);
			destination.enabled = layers.takeItem(row_index + 1, 1);
			destination.description = layers.takeItem(row_index + 1, 2);
			destination.type = new LayerTypeSelectionBox(*static_cast<LayerTypeSelectionBox*>(layers.cellWidget(row_index + 1, 3)));
			destination.background = new LayerBackgroundSelectionButton(*static_cast<LayerBackgroundSelectionButton*>(layers.cellWidget(row_index + 1, 4)));

			
			// Set new source
			layers.setItem(row_index + 1, 0, source.id);
			layers.setItem(row_index + 1, 1, source.enabled);
			layers.setItem(row_index + 1, 2, source.description);
			layers.setCellWidget(row_index + 1, 3, source.type);
			layers.setCellWidget(row_index + 1, 4, source.background);

			// Set new destination
			layers.setItem(row_index, 0, destination.id);
			layers.setItem(row_index, 1, destination.enabled);
			layers.setItem(row_index, 2, destination.description);
			layers.setCellWidget(row_index, 3, destination.type);
			layers.setCellWidget(row_index, 4, destination.background);

			layers.selectRow(row_index + 1);
		}
	}
}
