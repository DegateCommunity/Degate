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

#include "ViaEditDialog.h"

namespace degate
{

    ViaEditDialog::ViaEditDialog(QWidget* parent, Via_shptr& via, Project_shptr& project)
        : QDialog(parent),
          project(project),
          via(via),
          fill_color_edit(parent)
    {
        // Name
        name_label.setText(tr("Name:"));
        name_edit.setText(QString::fromStdString(via->get_name()));

        // Fill color
        fill_color_label.setText(tr("Fill color:"));
        fill_color_edit.set_color(via->get_fill_color());

        // Directions
        directions[Via::DIRECTION_UP] = tr("Up");
        directions[Via::DIRECTION_DOWN] = tr("Down");
        directions[Via::DIRECTION_UNDEFINED] = tr("Undefined");

        direction_label.setText(tr("Via direction:"));

        for (auto& e : directions)
            direction_edit.addItem(e.second);

        direction_edit.setCurrentText(directions[via->get_direction()]);

        // Diameter
        diameter_label.setText(tr("Via diameter:"));
        diameter_edit.setMinimum(1);
        diameter_edit.setMaximum(100000);
        diameter_edit.setValue(static_cast<int>(via->get_diameter()));

        // Buttons
        validate_button.setText(tr("Ok"));
        cancel_button.setText(tr("Cancel"));
        buttons_layout.addStretch(1);

        // Buttons layout
        buttons_layout.addWidget(&validate_button);
        buttons_layout.addWidget(&cancel_button);

        // Layout
        layout.addWidget(&name_label, 0, 0);
        layout.addWidget(&name_edit, 0, 1);
        layout.addWidget(&fill_color_label, 1, 0);
        layout.addWidget(&fill_color_edit, 1, 1);
        layout.addWidget(&direction_label, 2, 0);
        layout.addWidget(&direction_edit, 2, 1);
        layout.addWidget(&diameter_label, 3, 0);
        layout.addWidget(&diameter_edit, 3, 1);
        layout.addLayout(&buttons_layout, 4, 0);

        QObject::connect(&validate_button, SIGNAL(clicked()), this, SLOT(validate()));
        QObject::connect(&cancel_button, SIGNAL(clicked()), this, SLOT(reject()));
        QObject::connect(&direction_edit,
                         SIGNAL(currentTextChanged(const QString&)),
                         this,
                         SLOT(direction_changed(void)));

        setLayout(&layout);
    }

    void ViaEditDialog::validate()
    {
        via->set_name(name_edit.text().toStdString());
        via->set_fill_color(fill_color_edit.get_color());
        via->set_diameter(static_cast<unsigned int>(diameter_edit.value()));

        for (auto& e : directions)
        {
            if (e.second == direction_edit.currentText())
                via->set_direction(e.first);
        }

        accept();
    }

    void ViaEditDialog::direction_changed()
    {
        if (direction_edit.currentText() == directions[Via::DIRECTION_UP])
            fill_color_edit.set_color(project->get_default_color(DEFAULT_COLOR_VIA_UP));
        else if (direction_edit.currentText() == directions[Via::DIRECTION_DOWN])
            fill_color_edit.set_color(project->get_default_color(DEFAULT_COLOR_VIA_DOWN));
    }
} // namespace degate