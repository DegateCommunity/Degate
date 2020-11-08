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

#include "NewProjectDialog.h"

#include <QMessageBox>
#include <QFileDialog>
#include <QSpacerItem>

namespace degate
{
    NewProjectDialog::NewProjectDialog(QWidget* parent, const std::string& project_name, const std::string& project_path)
            : QDialog(parent), layers_edit_widget(this, nullptr)
    {
        setWindowFlags(Qt::Window);
        setWindowTitle(tr("New project creation"));

        project_directory = project_path;

        // Project name
        project_name_label.setText(tr("Project name:"));
        if (project_name != "")
        {
            project_name_edit.setText(project_name.c_str());
            project_name_edit.setDisabled(true);
        }

        // Project path
        project_path_label.setText(tr("Project directory path:"));
        project_path_button.setText(tr("Set project directory path"));

        // Size group
        size_group.setLayout(&size_group_layout);
        size_group.setTitle(tr("You will not be able to change the dimensions of the project after its creation."));

        // Automatic size
        automatic_size_label.setText(tr("Automatic size (will use layers image sizes):"));
        automatic_size_edit.setCheckState(Qt::Checked);
        QObject::connect(&automatic_size_edit, SIGNAL(toggled(bool)), &width, SLOT(setDisabled(bool)));
        QObject::connect(&automatic_size_edit, SIGNAL(toggled(bool)), &height, SLOT(setDisabled(bool)));

        // Width
        width_label.setText(tr("Project width:"));
        width.setMinimum(0);
        width.setMaximum(std::numeric_limits<int>::max());
        width.setValue(0);
        width.setDisabled(true);

        // Height
        height_label.setText(tr("Project height:"));
        height.setMinimum(0);
        height.setMaximum(std::numeric_limits<int>::max());
        height.setValue(0);
        height.setDisabled(true);

        // Size group layout
        size_group_layout.addWidget(&automatic_size_label, 0, 0);
        size_group_layout.addWidget(&automatic_size_edit, 0, 1);
        size_group_layout.addWidget(&width_label, 1, 0);
        size_group_layout.addWidget(&width, 1, 1);
        size_group_layout.addWidget(&height_label, 2, 0);
        size_group_layout.addWidget(&height, 2, 1);

        // Validate button
        validate_button.setText(tr("Ok"));

        // Content layout
        content_layout.addWidget(&project_name_label, 0, 0);
        content_layout.addWidget(&project_name_edit, 0, 1);

        // Check if the project directory is empty, if true will enable path selection (with a dialog)
        if (project_directory == "")
        {
            content_layout.addWidget(&project_path_label, 1, 0);
            content_layout.addWidget(&project_path_button, 1, 1);
            QObject::connect(&project_path_button, SIGNAL(pressed()), this, SLOT(set_project_directory_path()));
            user_selected_directory = true;
        }

        // Control layout
        control_layout.addWidget(&validate_button, 0, 1);
        control_layout.setColumnStretch(0, 1);

        // Layers edit
        layers_edit_label.setText(tr("Create project layers:"));

        // General layout
        layout.addLayout(&content_layout);
        layout.addItem(new QSpacerItem(0, 20));
        layout.addWidget(&size_group);
        layout.addItem(new QSpacerItem(0, 20));
        layout.addWidget(&layers_edit_label);
        layout.addWidget(&layers_edit_widget);
        layout.addItem(new QSpacerItem(0, 20));
        layout.addLayout(&control_layout);

        QObject::connect(&validate_button, SIGNAL(pressed()), this, SLOT(validate()));

        setLayout(&layout);

        // Size
        if (parent != nullptr)
            resize(500, parent->size().height() * 0.70);
    }

    Project_shptr NewProjectDialog::get_project()
    {
        return project;
    }

    void NewProjectDialog::validate()
    {
        // If automatic size
        if (automatic_size_edit.isChecked())
        {
            QSize size = layers_edit_widget.get_max_size();

            width.setValue(size.width());
            height.setValue(size.height());
        }

        // Check values
        if (layers_edit_widget.get_layer_count() == 0 || width.value() == 0 || height.value() == 0 || project_name_edit.text().length() < 1)
        {
            QMessageBox::warning(this, tr("Invalid values"), tr("The values you entered are invalid."));
            return;
        }

        // Check if user selected directory, if true add project name to the path
        if (user_selected_directory)
            project_directory += project_name_edit.text().toStdString();

        // Create the project directory
        if (!file_exists(project_directory))
            create_directory(project_directory);

        // Create the project
        project = std::make_shared<Project>(width.value(), height.value(), project_directory, layers_edit_widget.get_layer_count());
        project->set_name(project_name_edit.text().toStdString());

        // Create each layer
        layers_edit_widget.set_project(project);
        layers_edit_widget.validate();

        accept();
    }

    void NewProjectDialog::set_project_directory_path()
    {
        QString dir = QFileDialog::getExistingDirectory(this, tr("Select the directory where the project will be created"));

        if (dir.isNull())
            reject();

        project_directory = dir.toStdString() + "/";

        project_path_button.setText(dir + "/" + project_name_edit.text());
    }
}
