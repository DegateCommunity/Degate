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

#include "EMarkerEditDialog.h"

namespace degate
{

    EMarkerEditDialog::EMarkerEditDialog(EMarker_shptr &emarker, QWidget *parent) : emarker(emarker), QDialog(parent), fill_color(parent)
    {
        name_label.setText(tr("Name:"));
        name.setText(QString::fromStdString(emarker->get_name()));

        fill_color_label.setText(tr("Fill color:"));
        fill_color.set_color(emarker->get_fill_color());

        validate_button.setText(tr("Ok"));
        cancel_button.setText(tr("Cancel"));

        layout.addWidget(&name_label, 0, 0);
        layout.addWidget(&name, 0, 1);
        layout.addWidget(&fill_color_label, 1, 0);
        layout.addWidget(&fill_color, 1, 1);
        layout.addWidget(&validate_button, 3, 0);
        layout.addWidget(&cancel_button, 3, 1);

        QObject::connect(&validate_button, SIGNAL(clicked()), this, SLOT(validate()));
        QObject::connect(&cancel_button, SIGNAL(clicked()), this, SLOT(reject()));

        setLayout(&layout);
    }

    EMarkerEditDialog::~EMarkerEditDialog()
    {

    }

    void EMarkerEditDialog::validate()
    {
        emarker->set_name(name.text().toStdString());
        emarker->set_fill_color(fill_color.get_color());

        accept();
    }
}

