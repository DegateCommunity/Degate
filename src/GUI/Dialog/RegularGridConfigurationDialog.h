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

#ifndef __REGULARGRIDCONFIGURATIONDIALOG_H__
#define __REGULARGRIDCONFIGURATIONDIALOG_H__

#include "Core/Project/Project.h"
#include "Core/Grid/RegularGrid.h"

#include <QDialog>
#include <QGridLayout>
#include <QLabel>
#include <QSpinBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>

namespace degate
{
    /**
     * @class RegularGridConfigurationDialog
     * @brief Dialog to edit and configure the regular grid of the current project.
     *
     * @see QDialog
     */
    class RegularGridConfigurationDialog : public QDialog
    {
    Q_OBJECT

    public:
        /**
         * Create the dialog.
         *
         * @param parent : the parent of the widget.
         * @param project : the current valid project.
         */
        RegularGridConfigurationDialog(QWidget* parent, Project_shptr project);
        ~RegularGridConfigurationDialog() override;

    protected slots:
        /**
         * Apply all changes.
         */
        void validate();

    private:
        Project_shptr project = nullptr;

        // Layouts
        QVBoxLayout layout;
        QHBoxLayout button_layout;
        QGridLayout content_layout;

        // Introduction labels
        QLabel introduction_label;
        QLabel x_label;
        QLabel y_label;

        // Distance
        QLabel   distance_label;
        QSpinBox x_distance_edit;
        QSpinBox y_distance_edit;

        // Offset
        QLabel   offset_label;
        QSpinBox x_offset_edit;
        QSpinBox y_offset_edit;

        // Buttons
        QPushButton validate_button;

    };
}

#endif //__REGULARGRIDCONFIGURATIONDIALOG_H__
