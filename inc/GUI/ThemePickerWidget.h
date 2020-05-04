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
#include <QGridLayout>
#include <QLabel>

namespace degate
{

	/**
	 * @class ThemePickerWidget
	 * @brief Widget to select the icon theme and global theme.
	 *
	 * @see PreferencesDialog
	 */
	class ThemePickerWidget : public QWidget
	{
		Q_OBJECT
		
	public:
		/**
		 * Create the theme picker widget.
		 *
		 * @param parent : the parent of the widget.
		 * @param actual_theme : the current active theme.
		 * @param actual_icon_theme : the current active icon theme.
		 */
		ThemePickerWidget(QWidget* parent, Theme actual_theme, IconTheme actual_icon_theme);
		~ThemePickerWidget();

		/**
		 * Get the theme.
		 *
		 * @return Return the selected theme.
		 */
		Theme get_theme();

		/**
		 * Get the icon theme.
		 *
		 * @return Return the selected icon theme.
		 */
		IconTheme get_icon_theme();
		
	private:
		QGridLayout layout;

		QLabel theme_label;
		QComboBox theme_box;
		QLabel icon_theme_label;
		QComboBox icon_theme_box;
		
	};
}

#endif