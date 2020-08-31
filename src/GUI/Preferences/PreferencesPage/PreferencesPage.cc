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

#include "GUI/Preferences/PreferencesPage/PreferencesPage.h"

#include <QGroupBox>

namespace degate
{
    PreferencesPage::PreferencesPage(QWidget* parent) : QWidget(parent)
    {
        setLayout(&layout);
    }

    QGridLayout* PreferencesPage::add_category(const QString& name)
    {
        layout.addSpacing(5);

        // Create the category group box
        auto category_group_layout = new QGroupBox(name);

        layout.addWidget(category_group_layout);

        // Create the new category layout
        auto category_layout = new QGridLayout();
        //category_layout->setColumnStretch(1, 1);
        category_group_layout->setLayout(category_layout);

        return category_layout;
    }

    void PreferencesPage::add_widget(QGridLayout* category_layout, const QString& label, QWidget* widget)
    {
        if (category_layout == nullptr)
            return;

        category_layout->addWidget(new QLabel(label), category_layout->rowCount(), 0);
        if (widget != nullptr)
            category_layout->addWidget(widget, category_layout->rowCount() - 1, 1);
    }
}