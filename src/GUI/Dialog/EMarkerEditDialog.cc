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

#include "EMarkerEditDialog.h"

namespace degate
{

    EMarkerEditDialog::EMarkerEditDialog(QWidget* parent, EMarker_shptr& emarker)
            : QDialog(parent), emarker(emarker), fill_color(parent)
    {
        // Name
        name_label.setText(tr("Name:"));
        name.setText(QString::fromStdString(emarker->get_name()));

        // Description
        description_label.setText(tr("Description:"));
        description.setText(QString::fromStdString(emarker->get_description()));

        // Is module port
        is_module_port_label.setText(tr("Is a module port?:"));
        is_module_port.setCheckState(emarker->is_module_port() ? Qt::Checked : Qt::Unchecked);

        // Fill color
        fill_color_label.setText(tr("Fill color:"));
        fill_color.set_color(emarker->get_fill_color());

        // Diameter
        diameter_label.setText(tr("Diameter:"));
        diameter_edit.setMinimum(1);
        diameter_edit.setMaximum(100000);
        diameter_edit.setValue(static_cast<int>(emarker->get_diameter()));

        // Buttons
        validate_button.setText(tr("Ok"));
        cancel_button.setText(tr("Cancel"));


        //////////////////////////////////////
        //// Layout
        //////////////////////////////////////

        // Name
        layout.addWidget(&name_label, 0, 0);
        layout.addWidget(&name, 0, 1);

        // Description
        layout.addWidget(&description_label, 1, 0);
        layout.addWidget(&description, 1, 1);

        // Is module port
        layout.addWidget(&is_module_port_label, 2, 0);
        layout.addWidget(&is_module_port, 2, 1);

        // Fill color
        layout.addWidget(&fill_color_label, 3, 0);
        layout.addWidget(&fill_color, 3, 1);

        // Diameter
        layout.addWidget(&diameter_label, 4, 0);
        layout.addWidget(&diameter_edit, 4, 1);

        // Buttons
        layout.addWidget(&validate_button, 5, 0);
        layout.addWidget(&cancel_button, 5, 1);

        QObject::connect(&validate_button, SIGNAL(clicked()), this, SLOT(validate()));
        QObject::connect(&cancel_button, SIGNAL(clicked()), this, SLOT(reject()));

        setLayout(&layout);
    }

    void EMarkerEditDialog::validate()
    {
        emarker->set_name(name.text().toStdString());
        emarker->set_description(description.text().toStdString());
        emarker->set_module_port(is_module_port.checkState() == Qt::Checked);
        emarker->set_fill_color(fill_color.get_color());
        emarker->set_diameter(static_cast<unsigned int>(diameter_edit.value()));

        accept();
    }
}

