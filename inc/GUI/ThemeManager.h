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

#include "Core/SingletonBase.h"

#include <QIcon>
#include <QObject>
#include <QStyleFactory>
#include <QApplication>
#include <QMainWindow>
#include <QCommonStyle>

namespace degate
{

	/**
	 * @enum Theme
	 * @brief Enum of supported themes.
	 *
	 * @see ThemeManager
	 */
	enum Theme
	{
		NATIVE_THEME = 0, /*!< Native (OS) theme. */
		LIGHT_THEME = 1, /*!< Light theme. */
		DARK_THEME   = 2 /*!< Dark theme. */
	};

	/**
	 * @enum IconTheme
	 * @brief Enum of supported icon themes.
	 *
	 * @see ThemeManager
	 */
	enum IconTheme
	{
		LIGHT_ICON_THEME = 0, /*!< Light icon theme (use with dark theme) */
		DARK_ICON_THEME = 1 /*!< Dark icon theme (use with light theme) */
	};

	/**
	 * @class ThemeManager
	 * @brief Handle the current active theme and icon theme.
	 *
	 * @see SingletonBase
	 */
	class ThemeManager : public QObject, public SingletonBase<ThemeManager>
	{
        Q_OBJECT

	public:

		/**
		 * Create the theme manager.
		 */
		ThemeManager();
		~ThemeManager();

		/**
		 * Get the path of the icon following the theme used and his name.
		 *
		 * @param icon_name : the icon name.
		 */
		QString get_icon_path(const QString& icon_name);

	public slots:
	    /**
	     * Update the icon theme.
	     * Will emit icon_theme_changed().
	     */
        void update_icon_theme();

		/**
         * Apply the current theme.
         */
		void update_theme();

    signals:
        /**
         * Emitted when the icon theme changed.
         */
        void icon_theme_changed();
		
	};

	/**
	 * Convert the theme to string.
	 * e.g. DARK_THEME => "dark".
	 *
	 * @param theme : the theme to convert into string.
	 *
	 * @return Return the theme converted into string.
	 */
	std::string theme_to_string(Theme theme);

	/**
	 * Convert a string to theme.
	 * e.g. "dark" => DARK_THEME.
	 *
	 * @param theme : the string that handle the theme.
	 *
	 * @return Return the theme from the string.
	 */
	Theme string_to_theme(const std::string& theme);

	/**
	 * Convert the icon theme to string.
	 * e.g. DARK_ICON_THEME => "dark".
	 *
	 * @param theme : the icon theme to convert into string.
	 *
	 * @return Return the icon theme converted into string.
	 */
	std::string icon_theme_to_string(IconTheme theme);

	/**
	 * Convert a string to icon theme.
	 * e.g. "dark" => DARK_ICON_THEME.
	 *
	 * @param theme : the string that handle the icon theme.
	 *
	 * @return Return the icon theme from the string.
	 */
	IconTheme string_to_icon_theme(const std::string& theme);
	
}

/**
 * Get the theme manager instance.
 */
#define THEME_MANAGER ThemeManager::get_instance()

/**
 * Get an icon path from the theme manger with his name.
 *
 * @param icon_name : the icon name.
 */
#define GET_ICON_PATH(icon_name) ThemeManager::get_instance().get_icon_path(icon_name)

#endif