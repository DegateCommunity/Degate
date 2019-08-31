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

#include "ThemePickerWidget.h"

namespace degate
{
	ThemePickerWidget::ThemePickerWidget(QWidget* parent, Theme actual_theme, IconTheme actual_icon_theme) : QWidget(parent)
	{
		QStringList theme_list;
		theme_list.append("native");
		theme_list.append("light");
		theme_list.append("dark");
		theme_box.addItems(theme_list);
		theme_box.setCurrentText(QString::fromStdString(theme_to_string(actual_theme)));

		QStringList icon_theme_list;
		icon_theme_list.append("light");
		icon_theme_list.append("dark");
		icon_theme_box.addItems(icon_theme_list);
		icon_theme_box.setCurrentText(QString::fromStdString(icon_theme_to_string(actual_icon_theme)));

		layout.addWidget(&theme_box);
		layout.addWidget(&icon_theme_box);
		
		setLayout(&layout);
	}

	ThemePickerWidget::~ThemePickerWidget()
	{
		
	}

	Theme ThemePickerWidget::get_theme()
	{
		return string_to_theme(theme_box.currentText().toStdString());
	}

	IconTheme ThemePickerWidget::get_icon_theme()
	{
		return string_to_icon_theme(icon_theme_box.currentText().toStdString());;
	}
}
