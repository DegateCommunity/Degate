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

#ifndef __THEMEPICKERWIDGET_H__
#define __THEMEPICKERWIDGET_H__

#include "ThemeManager.h"

#include <QWidget>
#include <QComboBox>
#include <QVBoxLayout>

namespace degate
{
	class ThemePickerWidget : public QWidget
	{
		Q_OBJECT
		
	public:
		/**
		 * Set the actual state with the actual theme and icon theme.
		 */
		ThemePickerWidget(QWidget* parent, Theme actual_theme, IconTheme actual_icon_theme);
		~ThemePickerWidget();

		/**
		 * Get the theme.
		 */
		Theme get_theme();

		/**
		 * Get the icon theme.
		 */
		IconTheme get_icon_theme();
		
	private:
		QVBoxLayout layout;

		QComboBox theme_box;
		QComboBox icon_theme_box;
		
	};
}

#endif