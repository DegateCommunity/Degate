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

#ifndef __PREFERENCESHANDLER_H__
#define __PREFERENCESHANDLER_H__

#include "SingletonBase.h"
#include "ThemeManager.h"
#include "XMLExporter.h"
#include "XMLImporter.h"

#define DEGATE_CONFIG_FILE_PATH "degate.config"

namespace degate
{

	/**
	 * Handle all preferences and load/write them from/in a config file.
	 */
	class PreferencesHandler : public SingletonBase<PreferencesHandler>, public XMLImporter, public XMLExporter
	{
	public:
		PreferencesHandler();
		~PreferencesHandler();

		/**
		 * Save all preferences in the config file.
		 */
		void save();

		/**
		 * Get the theme from preferences (config file).
		 */
		Theme get_theme();

		/**
		 * Set the theme, it will be saved in the config file (preferences).
		 * The change will be applied after restart when the preferences will be loaded.
		 */
		void set_theme(Theme theme);

		/**
		 * Get the icon theme from preferences (config file).
		 */
		IconTheme get_icon_theme();

		/**
		 * Set the icon theme, it will be saved in the config file (preferences).
		 * The change will be applied after restart when the preferences will be loaded.
		 */
		void set_icon_theme(IconTheme theme);

	private:
		Theme theme = NATIVE_THEME;
		IconTheme icon_theme = DARK_ICON_THEME;
		
	};
}

#define PREFERENCES_HANDLER PreferencesHandler::get_instance()

#endif