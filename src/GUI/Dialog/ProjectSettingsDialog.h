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

#ifndef __PROJECTSETTINGSDIALOG_H__
#define __PROJECTSETTINGSDIALOG_H__

#include "Core/Project/Project.h"
#include "GUI/Dialog/ColorPickerDialog.h"

#include <QDialog>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QWidget>
#include <QTabWidget>
#include <QGridLayout>
#include <QSpinBox>

namespace degate
{
    /**
     * @class ProjectSettingsGeneralTab
     * @brief Tab to display and update general project settings.
     *
     * @see ProjectSettingsDialog.
     */
    class ProjectSettingsGeneralTab : public QWidget
    {
        Q_OBJECT

    public:
        ProjectSettingsGeneralTab(QWidget* parent, const Project_shptr& project);
        ~ProjectSettingsGeneralTab() override = default;

        /**
         * Validate all changes on project settings.
         */
        void validate();

    private:
        Project_shptr project = nullptr;
        QGridLayout layout;

        // Project name.
        QLabel project_name_label;
        QLineEdit project_name_edit;

        // Project description.
        QLabel project_description_label;
        QLineEdit project_description_edit;

        // Wire diameter.
        QLabel wire_diameter_label;
        QSpinBox wire_diameter_edit;

        // Via diameter.
        QLabel via_diameter_label;
        QSpinBox via_diameter_edit;

        // Port diameter.
        QLabel port_diameter_label;
        QSpinBox port_diameter_edit;

    };

    /**
     * @class ProjectSettingsColorsTab
     * @brief Tab to display and update default colors for all objects.
     *
     * @see ProjectSettingsDialog.
     */
    class ProjectSettingsColorsTab : public QWidget
    {
    Q_OBJECT

    public:
        ProjectSettingsColorsTab(QWidget* parent, const Project_shptr& project);
        ~ProjectSettingsColorsTab() override = default;

        /**
         * Validate all color changes.
         */
        void validate();

    private:
        Project_shptr project = nullptr;
        QGridLayout layout;

        // Wire color.
        QLabel wire_color_label;
        ColorSelectionButton wire_color_edit;

        // Via up color.
        QLabel via_up_color_label;
        ColorSelectionButton via_up_color_edit;

        // Via down color.
        QLabel via_down_color_label;
        ColorSelectionButton via_down_color_edit;

        // Annotation color.
        QLabel annotation_color_label;
        ColorSelectionButton annotation_color_edit;

        // Annotation frame color.
        QLabel annotation_frame_color_label;
        ColorSelectionButton annotation_frame_color_edit;

        // Gate color.
        QLabel gate_color_label;
        ColorSelectionButton gate_color_edit;

        // Gate frame color.
        QLabel gate_frame_color_label;
        ColorSelectionButton gate_frame_color_edit;

        // Gate port color.
        QLabel gate_port_color_label;
        ColorSelectionButton gate_port_color_edit;

        // EMarker color.
        QLabel emarker_color_label;
        ColorSelectionButton emarker_color_edit;

    };

    /**
     * @class ProjectSettingsDialog
     * @brief Display and update project settings.
     *
     * @see QDialog.
     */
    class ProjectSettingsDialog : public QDialog
    {
        Q_OBJECT

    public:
        ProjectSettingsDialog(QWidget* parent, const Project_shptr& project);
        ~ProjectSettingsDialog() override = default;

    public slots:

        /**
         * Validate all changes on project settings.
         */
        void validate();

    private:
        Project_shptr project = nullptr;
        QVBoxLayout layout;
        QTabWidget tab;
        QLabel introduction_label;

        // Buttons.
        QPushButton apply_button;
        QPushButton close_button;
        QHBoxLayout buttons_layout;

        // Tabs.
        ProjectSettingsGeneralTab general_tab;
        ProjectSettingsColorsTab colors_tab;

    };
}

#endif //__PROJECTSETTINGSDIALOG_H__
