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

#ifndef __NEWPROJECTDIALOG_H__
#define __NEWPROJECTDIALOG_H__

#include "GUI/Widget/LayersEditWidget.h"

#include <QDialog>
#include <QSpinBox>
#include <QLabel>
#include <QLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QGroupBox>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QCheckBox>

namespace degate
{

    /**
     * @class NewProjectDialog
     * @brief Dialog for creating a new project.
     *
     * @see QDialog
     */
    class NewProjectDialog : public QDialog
    {
        Q_OBJECT

    public:

        /**
         * Create the new project dialog.
         *
         * @param parent : the parent of the dialog.
         * @param project_name : the project name. Can be empty, it will let the user choose.
         * @param project_path : the project path. If empty, it will open a file dialog to select it.
         */
        NewProjectDialog(QWidget* parent, const std::string& project_name = "", const std::string& project_path = "");
        ~NewProjectDialog() override = default;

        /**
         * Get the newly created project (if creation failed, return nullptr).
         */
        Project_shptr get_project();

    protected slots:
        /**
         * Validate and create the project.
         */
        void validate();

        /**
         * Called when the project directory path button is pressed.
         */
        void set_project_directory_path();

    private:
        QVBoxLayout layout;

        // Content

        QGroupBox project_group;
        QGridLayout project_group_layout;

        QLabel project_name_label;
        QLineEdit project_name_edit;

        QLabel project_path_label;
        QPushButton project_path_button;

        QLabel attached_mode_label;
        QCheckBox attached_mode_box;

        QLabel layers_edit_label;
        LayersEditWidget layers_edit_widget;

        // Validation
        QGridLayout control_layout;
        QPushButton validate_button;

        // Project
        Project_shptr project = nullptr;
        std::string project_directory;
        bool user_selected_directory = false;
    };
}

#endif