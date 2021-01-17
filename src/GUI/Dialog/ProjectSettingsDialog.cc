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
#include "ProjectSettingsDialog.h"

#include <utility>

namespace degate
{

    ProjectSettingsGeneralTab::ProjectSettingsGeneralTab(QWidget* parent, const Project_shptr& project)
            : QWidget(parent), project(project)
    {
        // Name
        project_name_label.setText(tr("Project name:"));
        project_name_edit.setText(QString::fromStdString(project->get_name()));
        layout.addWidget(&project_name_label, 0, 0);
        layout.addWidget(&project_name_edit, 0, 1);

        // Description
        project_description_label.setText(tr("Project description:"));
        project_description_edit.setText(QString::fromStdString(project->get_description()));
        layout.addWidget(&project_description_label, 1, 0);
        layout.addWidget(&project_description_edit, 1, 1);

        // Wire diameter
        wire_diameter_label.setText(tr("Default wire diameter:"));
        wire_diameter_edit.setValue(project->get_default_wire_diameter());
        wire_diameter_edit.setMinimum(1);
        layout.addWidget(&wire_diameter_label, 2, 0);
        layout.addWidget(&wire_diameter_edit, 2, 1);

        // Via diameter
        via_diameter_label.setText(tr("Default via diameter:"));
        via_diameter_edit.setValue(project->get_default_via_diameter());
        via_diameter_edit.setMinimum(1);
        layout.addWidget(&via_diameter_label, 3, 0);
        layout.addWidget(&via_diameter_edit, 3, 1);

        // Port diameter
        port_diameter_label.setText(tr("Default port diameter:"));
        port_diameter_edit.setValue(project->get_default_port_diameter());
        port_diameter_edit.setMinimum(1);
        layout.addWidget(&port_diameter_label, 4, 0);
        layout.addWidget(&port_diameter_edit, 4, 1);

        // Lambda (in pixel(s))
        lambda_label.setText(tr("Lambda (in pixel(s)):"));
        lambda_label.setToolTip(tr("It indicates how many pixels two electrically conductive objects may be apart "
                                   "from each other so that they are still considered to be connected."));
        lambda_edit.setMinimum(1);
        lambda_edit.setMaximum(std::numeric_limits<int>::max());
        lambda_edit.setValue(static_cast<int>(project->get_lambda()));
        layout.addWidget(&lambda_label, 5, 0);
        layout.addWidget(&lambda_edit, 5, 1);

        // Pixels per micrometer.
        pixels_per_micrometer_label.setText(tr("Pixel(s) per μm:"));
        pixels_per_micrometer_edit.setMinimum(0.0);
        pixels_per_micrometer_edit.setMaximum(std::numeric_limits<double>::max());
        pixels_per_micrometer_edit.setValue(project->get_pixel_per_um());
        layout.addWidget(&pixels_per_micrometer_label, 6, 0);
        layout.addWidget(&pixels_per_micrometer_edit, 6, 1);

        setLayout(&layout);
    }

    void ProjectSettingsGeneralTab::validate()
    {
        project->set_name(project_name_edit.text().toStdString());
        project->set_description(project_description_edit.text().toStdString());
        project->set_default_wire_diameter(wire_diameter_edit.value());
        project->set_default_via_diameter(via_diameter_edit.value());
        project->set_default_port_diameter(port_diameter_edit.value());
        project->set_lambda(static_cast<length_t>(lambda_edit.value()));
        project->set_pixel_per_um(pixels_per_micrometer_edit.value());

        project->get_logic_model()->set_default_gate_port_diameter(port_diameter_edit.value());
    }

    ProjectSettingsColorsTab::ProjectSettingsColorsTab(QWidget* parent, const Project_shptr& project)
            : QWidget(parent),
              project(project),
              wire_color_edit(parent),
              via_up_color_edit(parent),
              via_down_color_edit(parent),
              annotation_color_edit(parent),
              annotation_frame_color_edit(parent),
              gate_color_edit(parent),
              gate_frame_color_edit(parent),
              gate_port_color_edit(parent),
              emarker_color_edit(parent)
    {
        // Wire color
        wire_color_label.setText(tr("Wire default color:"));
        wire_color_edit.set_color(project->get_default_color(DEFAULT_COLOR_WIRE));
        layout.addWidget(&wire_color_label, 0, 0);
        layout.addWidget(&wire_color_edit, 0, 1);

        // Via up color
        via_up_color_label.setText(tr("Via up default color:"));
        via_up_color_edit.set_color(project->get_default_color(DEFAULT_COLOR_VIA_UP));
        layout.addWidget(&via_up_color_label, 1, 0);
        layout.addWidget(&via_up_color_edit, 1, 1);

        // Via down color
        via_down_color_label.setText(tr("Via down default color:"));
        via_down_color_edit.set_color(project->get_default_color(DEFAULT_COLOR_VIA_DOWN));
        layout.addWidget(&via_down_color_label, 2, 0);
        layout.addWidget(&via_down_color_edit, 2, 1);

        // Annotation color
        annotation_color_label.setText(tr("Annotation default color:"));
        annotation_color_edit.set_color(project->get_default_color(DEFAULT_COLOR_ANNOTATION));
        layout.addWidget(&annotation_color_label, 3, 0);
        layout.addWidget(&annotation_color_edit, 3, 1);

        // Annotation frame color
        annotation_frame_color_label.setText(tr("Annotation frame default color:"));
        annotation_frame_color_edit.set_color(project->get_default_color(DEFAULT_COLOR_ANNOTATION_FRAME));
        layout.addWidget(&annotation_frame_color_label, 4, 0);
        layout.addWidget(&annotation_frame_color_edit, 4, 1);

        // Gate color
        gate_color_label.setText(tr("Gate default color:"));
        gate_color_edit.set_color(project->get_default_color(DEFAULT_COLOR_GATE));
        layout.addWidget(&gate_color_label, 5, 0);
        layout.addWidget(&gate_color_edit, 5, 1);

        // Gate frame color
        gate_frame_color_label.setText(tr("Gate frame default color:"));
        gate_frame_color_edit.set_color(project->get_default_color(DEFAULT_COLOR_GATE_FRAME));
        layout.addWidget(&gate_frame_color_label, 6, 0);
        layout.addWidget(&gate_frame_color_edit, 6, 1);

        // Gate port color
        gate_port_color_label.setText(tr("Gate port default color:"));
        gate_port_color_edit.set_color(project->get_default_color(DEFAULT_COLOR_GATE_PORT));
        layout.addWidget(&gate_port_color_label, 7, 0);
        layout.addWidget(&gate_port_color_edit, 7, 1);

        // EMarker color
        emarker_color_label.setText(tr("EMarker default color:"));
        emarker_color_edit.set_color(project->get_default_color(DEFAULT_COLOR_EMARKER));
        layout.addWidget(&emarker_color_label, 8, 0);
        layout.addWidget(&emarker_color_edit, 8, 1);

        setLayout(&layout);
    }

    void ProjectSettingsColorsTab::validate()
    {
        project->set_default_color(DEFAULT_COLOR_WIRE, wire_color_edit.get_color());
        project->set_default_color(DEFAULT_COLOR_VIA_UP, via_up_color_edit.get_color());
        project->set_default_color(DEFAULT_COLOR_VIA_DOWN, via_down_color_edit.get_color());
        project->set_default_color(DEFAULT_COLOR_ANNOTATION, annotation_color_edit.get_color());
        project->set_default_color(DEFAULT_COLOR_ANNOTATION_FRAME, annotation_frame_color_edit.get_color());
        project->set_default_color(DEFAULT_COLOR_GATE, gate_color_edit.get_color());
        project->set_default_color(DEFAULT_COLOR_GATE_FRAME, gate_frame_color_edit.get_color());
        project->set_default_color(DEFAULT_COLOR_GATE_PORT, gate_port_color_edit.get_color());
        project->set_default_color(DEFAULT_COLOR_EMARKER, emarker_color_edit.get_color());
    }

    ProjectSettingsDialog::ProjectSettingsDialog(QWidget *parent, const Project_shptr& project)
        : QDialog(parent), general_tab(this, project), colors_tab(this, project)
    {
        introduction_label.setText(tr("Change here all project settings like project name or colors. "
                                      "Those settings are independent (per project)."));

        tab.addTab(&general_tab, tr("General"));
        tab.addTab(&colors_tab, tr("Colors"));

        apply_button.setText(tr("Apply"));
        close_button.setText(tr("Close"));
        buttons_layout.addStretch(1);
        buttons_layout.addWidget(&apply_button);
        buttons_layout.addWidget(&close_button);

        layout.addWidget(&introduction_label);
        layout.addSpacing(5);
        layout.addWidget(&tab);
        layout.addLayout(&buttons_layout);

        QObject::connect(&apply_button, SIGNAL(clicked()), this, SLOT(validate()));
        QObject::connect(&close_button, SIGNAL(clicked()), this, SLOT(accept()));

        setLayout(&layout);
    }

    void ProjectSettingsDialog::validate()
    {
        general_tab.validate();
        colors_tab.validate();
    }
}
