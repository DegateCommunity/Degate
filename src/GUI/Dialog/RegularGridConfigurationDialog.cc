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

#include "RegularGridConfigurationDialog.h"

namespace degate
{
    RegularGridConfigurationDialog::RegularGridConfigurationDialog(QWidget* parent, Project_shptr project)
            : QDialog(parent), project(project)
    {
        setWindowTitle(tr("Grid configuration"));

        introduction_label.setText(tr("All settings are project relative."));
        content_layout.addWidget(&introduction_label, 0, 0);

        x_label.setText("X");
        x_label.setAlignment(Qt::AlignCenter);
        content_layout.addWidget(&x_label, 0, 1);

        y_label.setText("Y");
        y_label.setAlignment(Qt::AlignCenter);
        content_layout.addWidget(&y_label, 0, 2);

        auto horizontal_grid = project->get_regular_horizontal_grid();
        auto vertical_grid = project->get_regular_vertical_grid();

        // Distance
        distance_label.setText(tr("Distance (space between each grid line in px, 0 to disable):"));
        x_distance_edit.setMinimum(0);
        x_distance_edit.setValue(horizontal_grid->get_distance());
        y_distance_edit.setMinimum(0);
        y_distance_edit.setValue(vertical_grid->get_distance());

        content_layout.addWidget(&distance_label, 1, 0);
        content_layout.addWidget(&x_distance_edit, 1, 1);
        content_layout.addWidget(&y_distance_edit, 1, 2);

        // Offset
        offset_label.setText(tr("Offset:"));
        x_offset_edit.setMinimum(0);
        x_offset_edit.setValue(horizontal_grid->get_min());
        y_offset_edit.setMinimum(0);
        y_offset_edit.setValue(vertical_grid->get_min());

        content_layout.addWidget(&offset_label, 2, 0);
        content_layout.addWidget(&x_offset_edit, 2, 1);
        content_layout.addWidget(&y_offset_edit, 2, 2);

        validate_button.setText(tr("Ok"));
        button_layout.addStretch(1);
        button_layout.addWidget(&validate_button);

        layout.addLayout(&content_layout);
        layout.addSpacing(10);
        layout.addLayout(&button_layout);

        setLayout(&layout);

        QObject::connect(&validate_button, SIGNAL(clicked()), this, SLOT(validate()));
    }

    RegularGridConfigurationDialog::~RegularGridConfigurationDialog() = default;

    void RegularGridConfigurationDialog::validate()
    {
        auto horizontal_grid = project->get_regular_horizontal_grid();
        auto vertical_grid = project->get_regular_vertical_grid();

        horizontal_grid->set_distance(x_distance_edit.value());
        vertical_grid->set_distance(y_distance_edit.value());

        horizontal_grid->set_range(x_offset_edit.value(), project->get_width());
        vertical_grid->set_range(y_offset_edit.value(), project->get_height());

        accept();
    }
}
