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

#ifndef __THEMEMANAGER_H__
#define __THEMEMANAGER_H__

#include "SingletonBase.h"

#include <QIcon.h>
#include <QStyleFactory.h>
#include <QApplication.h>
#include <QMainWindow>
#include <QCommonStyle>

namespace degate
{
	enum Theme
	{
		NATIVE_THEME = 0,
		LIGHT_THEME = 1,
		DARK_THEME   = 2
	};

	enum IconTheme
	{
		LIGHT_ICON_THEME = 0,
		DARK_ICON_THEME = 1
	};

	/**
	 * Handle the current theme and icon theme.
	 * Can't be changed after init, need restart.
	 */
	class ThemeManager : public SingletonBase<ThemeManager>
	{
	public:
		ThemeManager();
		~ThemeManager();

		/**
		 * Set the theme, this will the a default icon theme.
		 */
		void init(Theme theme, IconTheme icon_theme);

		/**
		 * Get the path of the icon following the theme used and his name.
		 */
		QString get_icon_path(std::string icon_name);

	private:
		Theme theme;
		IconTheme icon_theme;
		bool is_init = false;
		
	};

	/**
	 * Convert the theme to string.
	 * e.g. DARK_THEME => "dark".
	 */
	std::string theme_to_string(Theme theme);

	/**
	 * Convert a string to theme.
	 * e.g. "dark" => DARK_THEME.
	 */
	Theme string_to_theme(std::string theme);

	/**
	 * Convert the icon theme to string.
	 * e.g. DARK_ICON_THEME => "dark".
	 */
	std::string icon_theme_to_string(IconTheme theme);

	/**
	 * Convert a string to icon theme.
	 * e.g. "dark" => DARK_ICON_THEME.
	 */
	IconTheme string_to_icon_theme(std::string theme);
	
}

#define THEME_MANAGER ThemeManager::get_instance()
#define GET_ICON_PATH(icon_name) ThemeManager::get_instance().get_icon_path(icon_name)

#endif