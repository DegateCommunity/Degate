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

    ViaEditDialog::ViaEditDialog(Via_shptr &via, QWidget *parent): via(via), QDialog(parent), fill_color(parent)
    {
        name_label.setText("Name :");
        name.setText(QString::fromStdString(via->get_name()));

        fill_color_label.setText("Fill color :");
        fill_color.set_color(via->get_fill_color());

        direction_label.setText("Via direction :");
        direction.addItem("Undefined");
        direction.addItem("Up");
        direction.addItem("Down");

        if(via->get_direction() == Via::DIRECTION_UP)
            direction.setCurrentText("Up");
        else if(via->get_direction() == Via::DIRECTION_DOWN)
            direction.setCurrentText("Down");
        else
            direction.setCurrentText("Undefined");

        validate_button.setText("Ok");
        cancel_button.setText("Cancel");

        layout.addWidget(&name_label, 0, 0);
        layout.addWidget(&name, 0, 1);
        layout.addWidget(&fill_color_label, 1, 0);
        layout.addWidget(&fill_color, 1, 1);
        layout.addWidget(&direction_label, 2, 0);
        layout.addWidget(&direction, 2, 1);
        layout.addWidget(&validate_button, 3, 0);
        layout.addWidget(&cancel_button, 3, 1);

        QObject::connect(&validate_button, SIGNAL(clicked()), this, SLOT(validate()));
        QObject::connect(&cancel_button, SIGNAL(clicked()), this, SLOT(close()));

        setLayout(&layout);
    }

    ViaEditDialog::~ViaEditDialog()
    {

    }

    void ViaEditDialog::validate()
    {
        via->set_name(name.text().toStdString());
        via->set_fill_color(fill_color.get_color());

        if(direction.currentText() == "Up")
            via->set_direction(Via::DIRECTION_UP);
        else if(direction.currentText() == "Down")
            via->set_direction(Via::DIRECTION_DOWN);
        else
            via->set_direction(Via::DIRECTION_UNDEFINED);

        close();
    }
}