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

#include "ViaEditDialog.h"

namespace degate
{

    ViaEditDialog::ViaEditDialog(Via_shptr& via, QWidget *parent, Project_shptr& project): via(via), QDialog(parent), fill_color_edit(parent), project(project)
    {
        name_label.setText("Name :");
        name_edit.setText(QString::fromStdString(via->get_name()));

        fill_color_label.setText("Fill color :");
        fill_color_edit.set_color(via->get_fill_color());

        direction_label.setText("Via direction :");
        direction_edit.addItem("Undefined");
        direction_edit.addItem("Up");
        direction_edit.addItem("Down");

        if(via->get_direction() == Via::DIRECTION_UP)
            direction_edit.setCurrentText("Up");
        else if(via->get_direction() == Via::DIRECTION_DOWN)
            direction_edit.setCurrentText("Down");
        else
            direction_edit.setCurrentText("Undefined");

        validate_button.setText("Ok");
        cancel_button.setText("Cancel");
        buttons_layout.addStretch(1);

        buttons_layout.addWidget(&validate_button);
        buttons_layout.addWidget(&cancel_button);

        layout.addWidget(&name_label, 0, 0);
        layout.addWidget(&name_edit, 0, 1);
        layout.addWidget(&fill_color_label, 1, 0);
        layout.addWidget(&fill_color_edit, 1, 1);
        layout.addWidget(&direction_label, 2, 0);
        layout.addWidget(&direction_edit, 2, 1);
        layout.addLayout(&buttons_layout, 3, 0);

        QObject::connect(&validate_button, SIGNAL(clicked()), this, SLOT(validate()));
        QObject::connect(&cancel_button, SIGNAL(clicked()), this, SLOT(reject()));
        QObject::connect(&direction_edit, SIGNAL(currentTextChanged(const QString&)), this, SLOT(direction_changed(void)));

        setLayout(&layout);
    }

    ViaEditDialog::~ViaEditDialog()
    {

    }

    void ViaEditDialog::validate()
    {
        via->set_name(name_edit.text().toStdString());
        via->set_fill_color(fill_color_edit.get_color());

        if(direction_edit.currentText() == "Up")
            via->set_direction(Via::DIRECTION_UP);
        else if(direction_edit.currentText() == "Down")
            via->set_direction(Via::DIRECTION_DOWN);
        else
            via->set_direction(Via::DIRECTION_UNDEFINED);

        accept();
    }

    void ViaEditDialog::direction_changed()
    {
        if(direction_edit.currentText() == "Up")
            fill_color_edit.set_color(project->get_default_color(DEFAULT_COLOR_VIA_UP));
        else if(direction_edit.currentText() == "Down")
            fill_color_edit.set_color(project->get_default_color(DEFAULT_COLOR_VIA_DOWN));
    }
}