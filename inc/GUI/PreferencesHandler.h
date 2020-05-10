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

#include "Core/SingletonBase.h"
#include "GUI/ThemeManager.h"
#include "Core/XMLExporter.h"
#include "Core/XMLImporter.h"

#include <QObject>

namespace degate
{
    /**
     * @struct Preferences
     * @brief Stores all preference values.
     */
    struct Preferences
    {
        /* Theme */
        Theme theme;
        IconTheme icon_theme;
        bool automatic_icon_theme;
    };

	/**
	 * @class PreferencesHandler
	 * @brief Handle preferences and load/write them from/in a config file.
	 *
	 * @see SingletonBase
	 */
	class PreferencesHandler : public QObject, public SingletonBase<PreferencesHandler>
	{
        Q_OBJECT

	public:

		/**
		 * Create the preferences handler.
		 */
		PreferencesHandler();
		~PreferencesHandler();

		/**
		 * Save preferences in the configuration file.
		 */
		void save();

		/**
		 * Update preferences.
		 */
		void update(Preferences updated_preferences);

		/**
		 * Get all stored preferences.
		 *
		 * @return Return a const reference of all preferences.
		 */
        const Preferences& get_preferences();

	signals:
	    /**
	     * Emitted when the icon theme changed.
	     */
	    void icon_theme_changed();

	    /**
	     * Emitted when the theme changed.
	     */
	    void theme_changed();

	private:
        QSettings settings;
        Preferences preferences;
	};
}

/**
 * Get the preferences handler instance.
 */
#define PREFERENCES_HANDLER PreferencesHandler::get_instance()

#endif