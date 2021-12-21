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

#include "LayersEditWidget.h"

#include "Core/Image/ImageHelper.h"
#include "Core/LogicModel/LogicModelHelper.h"
#include "GUI/Dialog/ProgressDialog.h"
#include "GUI/Preferences/ThemeManager.h"

#include <memory>
#include <QtWidgets/QMessageBox>
#include <QHeaderView>

namespace degate
{
    LayerBackgroundSelectionButton::LayerBackgroundSelectionButton(QWidget* parent, const Layer_shptr& layer)
            : QPushButton(parent)
    {
        if (layer == nullptr)
            change_button_color(false);
        else
            change_button_color(layer->has_background_image());

        QObject::connect(this, SIGNAL(clicked()), this, SLOT(on_button_clicked()));
    }

    LayerBackgroundSelectionButton::LayerBackgroundSelectionButton(LayerBackgroundSelectionButton& copy)
            : QPushButton(copy.parentWidget())
    {
        change_button_color(copy.get_state());

        if (copy.has_new_image())
            new_image = true;
        else
            new_image = false;

        image_path = copy.get_image_path();

        QObject::connect(this, SIGNAL(clicked()), this, SLOT(on_button_clicked()));
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
        QString res = QFileDialog::getOpenFileName(this, tr("Select the background image"));

        if (QImageReader::imageFormat(res).isEmpty())
        {
            QMessageBox::warning(this, tr("Invalid image"), tr("Wrong image type."));
            return;
        }

        image_path = res.toStdString();

        change_button_color(true);
        new_image = true;
    }

    void LayerBackgroundSelectionButton::change_button_color(bool value)
    {
        if (value)
        {
            setStyleSheet("background-color: rgba(0, 150, 0, 255)");
            setText(tr("Image defined"));
        }
        else
        {
            setStyleSheet("background-color: rgba(150, 0, 0, 255)");
            setText(tr("No Image defined"));
        }

        state = value;
    }

    LayerTypeSelectionBox::LayerTypeSelectionBox(Layer::LAYER_TYPE type, QWidget* parent)
            : QComboBox(parent), type(type)
    {
        types[Layer::UNDEFINED]  = tr("Undefined");
        types[Layer::TRANSISTOR] = tr("Transistor");
        types[Layer::LOGIC]      = tr("Logic");
        types[Layer::METAL]      = tr("Metal");

        for (auto& e : types)
            addItem(e.second);

        setCurrentText(types[type]);
    }

    LayerTypeSelectionBox::LayerTypeSelectionBox(LayerTypeSelectionBox& copy) : QComboBox(copy.parentWidget())
    {
        types[Layer::UNDEFINED]  = tr("Undefined");
        types[Layer::TRANSISTOR] = tr("Transistor");
        types[Layer::LOGIC]      = tr("Logic");
        types[Layer::METAL]      = tr("Metal");

        for (auto& e : types)
            addItem(e.second);

        setCurrentText(types[copy.get_layer_type()]);

        type = copy.get_layer_type();
    }

    Layer::LAYER_TYPE LayerTypeSelectionBox::get_layer_type()
    {
        for (auto& e : types)
        {
            if (e.second == currentText())
                type = e.first;
        }

        return type;
    }

    void LayerTypeSelectionBox::set_layer_type(Layer::LAYER_TYPE type)
    {
        this->type = type;

        setCurrentText(types[type]);
    }

    LayersEditWidget::LayersEditWidget(QWidget* parent, const Project_shptr& project)
            : QWidget(parent), project(project)
    {
        // List
        layers.setColumnCount(5);
        QStringList list;
        list.append(tr("ID"));
        list.append(tr("Enable"));
        list.append(tr("Description"));
        list.append(tr("Type"));
        list.append(tr("Background"));
        layers.setHorizontalHeaderLabels(list);
        layers.setSelectionBehavior(QTableView::SelectRows);
        layers.setSelectionMode(QTableView::SingleSelection);

        // Add/remove Buttons
        layers_add_button.setIcon(QIcon(GET_ICON_PATH("plus.png")));
        layers_remove_button.setIcon(QIcon(GET_ICON_PATH("minus.png")));
        layers_control_buttons_layout.addWidget(&layers_add_button, 0, 0);
        layers_control_buttons_layout.addWidget(&layers_remove_button, 1, 0);

        // Up/Down buttons
        layers_up_buttons.setIcon(QIcon(GET_ICON_PATH("layer_up.png")));
        layers_down_buttons.setIcon(QIcon(GET_ICON_PATH("layer_down.png")));
        layers_control_buttons_layout.addWidget(&layers_up_buttons, 2, 0);
        layers_control_buttons_layout.addWidget(&layers_down_buttons, 3, 0);

        layers_control_buttons_layout.setRowStretch(4, 1);

        // Layout
        layout.addWidget(&layers, 0, 0);
        layout.addLayout(&layers_control_buttons_layout, 0, 1);

        setLayout(&layout);

        // Initialize the layer list
        if (project != nullptr)
        {
            for (auto iter = project->get_logic_model()->layers_begin(); iter != project->get_logic_model()->layers_end(); ++iter)
            {
                Layer_shptr layer = *iter;

                if (layer == nullptr)
                    continue;

                layers.insertRow(layers.rowCount());

                // Id
                auto id_item = new QTableWidgetItem(QString::number(layer->get_layer_id()));
                id_item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
                layers.setItem(layers.rowCount() - 1, 0, id_item);

                // Enabled
                auto enabled = new QTableWidgetItem();
                enabled->setFlags(Qt::ItemIsEnabled | Qt::ItemIsUserCheckable);
                enabled->setCheckState(layer->is_enabled() ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
                layers.setItem(layers.rowCount() - 1, 1, enabled);

                // Description
                layers.setItem(layers.rowCount() - 1,
                               2,
                               new QTableWidgetItem(QString::fromStdString(layer->get_description())));

                // Type
                auto cb = new LayerTypeSelectionBox(layer->get_layer_type(), this);
                layers.setCellWidget(layers.rowCount() - 1, 3, cb);

                // Background
                auto bb = new LayerBackgroundSelectionButton(this, layer);
                layers.setCellWidget(layers.rowCount() - 1, 4, bb);
            }
        }

        auto header_view = static_cast<QHeaderView*>(layers.horizontalHeader());
        header_view->setSectionResizeMode(QHeaderView::Stretch);

        QObject::connect(&layers_add_button, SIGNAL(clicked()), this, SLOT(on_layer_add()));
        QObject::connect(&layers_remove_button, SIGNAL(clicked()), this, SLOT(on_layer_remove()));
        QObject::connect(&layers_up_buttons, SIGNAL(clicked()), this, SLOT(on_layer_up()));
        QObject::connect(&layers_down_buttons, SIGNAL(clicked()), this, SLOT(on_layer_down()));
    }

    void LayersEditWidget::set_project(const Project_shptr& project)
    {
        this->project = project;
    }

    unsigned int LayersEditWidget::get_layer_count()
    {
        return static_cast<unsigned int>(layers.rowCount());
    }

    QSize LayersEditWidget::get_max_size()
    {
        QSize max{0, 0};
        for (unsigned int i = 0; i < static_cast<unsigned int>(layers.rowCount()); i++)
        {
            LayerBackgroundSelectionButton* background = dynamic_cast<LayerBackgroundSelectionButton*>(layers.cellWidget(i, 4));

            if (!is_file(background->get_image_path()))
                continue;

            if (QImageReader::imageFormat(background->get_image_path().c_str()).isEmpty())
                continue;

            QImageReader reader(background->get_image_path().c_str());
            QSize size = reader.size();

            if (!size.isValid())
            {
                debug(TM, "can't read size of %s\n", background->get_image_path().c_str());
                continue;
            }

            if (size.width() > max.width())
                max.setWidth(size.width());

            if (size.height() > max.height())
                max.setHeight(size.height());
        }

        return max;
    }

    void LayersEditWidget::on_layer_add()
    {
        layers.insertRow(layers.rowCount());

        // Id
        auto id_item = new QTableWidgetItem("?");
        id_item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        layers.setItem(layers.rowCount() - 1, 0, id_item);

        // Enabled
        auto enabled = new QTableWidgetItem();
        enabled->setFlags(Qt::ItemIsEnabled | Qt::ItemIsUserCheckable);
        enabled->setCheckState(Qt::CheckState::Checked);
        layers.setItem(layers.rowCount() - 1, 1, enabled);

        // Description
        layers.setItem(layers.rowCount() - 1, 2, new QTableWidgetItem(""));

        // Type
        auto cb = new LayerTypeSelectionBox(Layer::UNDEFINED, this);
        layers.setCellWidget(layers.rowCount() - 1, 3, cb);

        // Background
        auto bb = new LayerBackgroundSelectionButton(this, nullptr);
        layers.setCellWidget(layers.rowCount() - 1, 4, bb);

        layers.selectRow(layers.rowCount() - 1);
    }

    void LayersEditWidget::on_layer_remove()
    {
        QItemSelectionModel* select = layers.selectionModel();

        if (!select->hasSelection())
            return;

        QModelIndexList index = select->selectedRows();

        for (auto & sel : index)
        {
            if (sel.isValid())
                layers.removeRow(sel.row());
        }
    }

    void LayersEditWidget::validate()
    {
        if (project == nullptr)
            return;

        LogicModel::layer_collection layer_collection;

        for (unsigned int i = 0; i < static_cast<unsigned int>(layers.rowCount()); i++)
        {
            Layer_shptr layer;

            // Id
            QString text_id = layers.item(i, 0)->text();
            if (text_id == '?')
            {
                // New layer
                layer = std::make_shared<Layer>(BoundingBox(project->get_logic_model()->get_width(), project->get_logic_model()->get_height()), project->get_project_type());
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
            layer->set_layer_type(dynamic_cast<LayerTypeSelectionBox*>(layers.cellWidget(i, 3))->get_layer_type());

            // Image
            LayerBackgroundSelectionButton* background = dynamic_cast<LayerBackgroundSelectionButton*>(layers.cellWidget(i, 4));


            if (project->get_project_type() == ProjectType::Normal)
            {
                // If Normal project type, then just load the new image background
                // This can take a very long time

                try
                {
                    if (background->has_new_image())
                    {
                        // Start progress dialog
                        ProgressDialog progress_dialog(this->parentWidget(),
                                                    tr("Importation and conversion of the new background image. "
                                                        "This operation can take a lot of time, but will be performed only once."),
                                                    nullptr);

                        // Set the job to start the background loading.
                        progress_dialog.set_job([&layer, &background, this]()
                                                {
                                                    load_new_background_image(layer, project->get_project_directory(), background->get_image_path());
                                                });

                        // Start the process
                        progress_dialog.exec();

                        if (progress_dialog.was_canceled())
                            debug(TM, "The background image importation and conversion operation has been canceled.");
                    }
                }
                catch (std::exception& e)
                {
                    QMessageBox::critical(this,
                                        tr("Error"),
                                        tr("Can't import the background image.") + "\n" + tr("Error:") + " " +
                                        QString::fromStdString(e.what()));

                    return;
                }
            }
            else
            {
                if (background->has_new_image())
                {
                    debug(TM, "Background image path: %s", background->get_image_path());

                    // If attached project type, then just create the background image without loading
                    BackgroundImage_shptr bg_image = std::make_shared<BackgroundImage>(
                            layer->get_width(),
                            layer->get_height(),
                            background->get_image_path(),
                            true,
                            1,
                            10,
                            TileLoadingType::Async,
                            WorkspaceNotificationList{{WorkspaceTarget::WorkspaceBackground, WorkspaceNotification::Update},
                                                    {WorkspaceTarget::Workspace, WorkspaceNotification::Draw}});

                    // Set the image to the layer
                    layer->set_image(bg_image);
                }
            }

            layer->set_layer_pos(i);
            layer_collection.push_back(layer);
        }

        project->get_logic_model()->set_layers(layer_collection);
    }

    void LayersEditWidget::on_layer_up()
    {
        QItemSelectionModel* select = layers.selectionModel();

        if (!select->hasSelection())
            return;

        QModelIndexList index = select->selectedRows();

        for (auto & sel : index)
        {
            move_row(sel.row(), UP);
        }
    }

    void LayersEditWidget::on_layer_down()
    {
        QItemSelectionModel* select = layers.selectionModel();

        if (!select->hasSelection())
            return;

        QModelIndexList index = select->selectedRows();

        for (auto & sel : index)
        {
            move_row(sel.row(), DOWN);
        }
    }

    void LayersEditWidget::move_row(int row_index, RowMoveDirection direction)
    {
        LayersEditRow source;
        LayersEditRow destination;

        if (direction == UP)
        {
            if (row_index == 0)
                return;

            // Get source
            source.id = layers.takeItem(row_index, 0);
            source.enabled = layers.takeItem(row_index, 1);
            source.description = layers.takeItem(row_index, 2);
            source.type = new LayerTypeSelectionBox(*dynamic_cast<LayerTypeSelectionBox*>(layers.cellWidget(row_index, 3)));
            source.background = new LayerBackgroundSelectionButton(*dynamic_cast<LayerBackgroundSelectionButton*>(layers.cellWidget(row_index, 4)));

            // Get destination
            destination.id = layers.takeItem(row_index - 1, 0);
            destination.enabled = layers.takeItem(row_index - 1, 1);
            destination.description = layers.takeItem(row_index - 1, 2);
            destination.type = new LayerTypeSelectionBox(*dynamic_cast<LayerTypeSelectionBox*>(layers.cellWidget(row_index - 1, 3)));
            destination.background = new LayerBackgroundSelectionButton(*dynamic_cast<LayerBackgroundSelectionButton*>(layers.cellWidget(row_index - 1, 4)));


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
            if (row_index == layers.rowCount() - 1)
                return;

            // Get source
            source.id = layers.takeItem(row_index, 0);
            source.enabled = layers.takeItem(row_index, 1);
            source.description = layers.takeItem(row_index, 2);
            source.type = new LayerTypeSelectionBox(*dynamic_cast<LayerTypeSelectionBox*>(layers.cellWidget(row_index, 3)));
            source.background = new LayerBackgroundSelectionButton(*dynamic_cast<LayerBackgroundSelectionButton*>(layers.cellWidget(row_index, 4)));

            // Get destination
            destination.id = layers.takeItem(row_index + 1, 0);
            destination.enabled = layers.takeItem(row_index + 1, 1);
            destination.description = layers.takeItem(row_index + 1, 2);
            destination.type = new LayerTypeSelectionBox(*dynamic_cast<LayerTypeSelectionBox*>(layers.cellWidget(row_index + 1, 3)));
            destination.background = new LayerBackgroundSelectionButton(*dynamic_cast<LayerBackgroundSelectionButton*>(layers.cellWidget(row_index + 1, 4)));


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
