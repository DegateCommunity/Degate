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

#include "PreferencesHandler.h"

namespace degate
{
	PreferencesHandler::PreferencesHandler() : settings(QString::fromStdString(DEGATE_IN_CONFIGURATION(DEGATE_CONFIGURATION_FILE_NAME)), QSettings::IniFormat)
	{
	    ///////////
	    // Appearance
	    ///////////

	    // Theme
		QString theme = settings.value("theme", "native").toString();
        preferences.theme = string_to_theme(theme.toStdString());

		// Icon Theme
		QString icon_theme = settings.value("icon_theme", "dark").toString();
        preferences.icon_theme = string_to_icon_theme(icon_theme.toStdString());

        // Automatic icon theme
        bool automatic_icon_theme = settings.value("automatic_icon_theme", true).toBool();
        preferences.automatic_icon_theme = automatic_icon_theme;


        ///////////
        // General
        ///////////

        // Language
        preferences.language = settings.value("language", "").toString();

        // Auto save
        preferences.auto_save_status = settings.value("auto_save_status", false).toBool();
        preferences.auto_save_interval = settings.value("auto_save_interval", 5).toUInt();


        ///////////
        // Grid
        ///////////

        // Grid color
        preferences.grid_color = settings.value("grid_color", 0x55FFFFFF).toUInt();

        // Max grid lines count (optimisation)
        preferences.max_grid_lines_count = settings.value("max_grid_lines_count", 200).toUInt();

        // Show grid
        preferences.show_grid = settings.value("show_grid", false).toBool();

        // Snap to grid
        preferences.snap_to_grid = settings.value("snap_to_grid", false).toBool();
	}

	PreferencesHandler::~PreferencesHandler()
	{
        save();
	}

	void PreferencesHandler::save()
	{
        ///////////
        // Appearance
        ///////////

        settings.setValue("theme", QString::fromStdString(theme_to_string(preferences.theme)));
        settings.setValue("icon_theme", QString::fromStdString(icon_theme_to_string(preferences.icon_theme)));
        settings.setValue("automatic_icon_theme", preferences.automatic_icon_theme);


        ///////////
        // General
        ///////////

        settings.setValue("language", preferences.language);
        settings.setValue("auto_save_status", preferences.auto_save_status);
        settings.setValue("auto_save_interval", preferences.auto_save_interval);


        ///////////
        // Grid
        ///////////

        settings.setValue("grid_color", preferences.grid_color);
        settings.setValue("max_grid_lines_count", preferences.max_grid_lines_count);
        settings.setValue("show_grid", preferences.show_grid);
        settings.setValue("snap_to_grid", preferences.snap_to_grid);
	}

    void PreferencesHandler::update(const Preferences& updated_preferences)
    {
	    if (preferences.theme != updated_preferences.theme)
        {
            preferences.theme = updated_preferences.theme;
            emit theme_changed();
        }

        if (preferences.icon_theme != updated_preferences.icon_theme)
        {
            preferences.icon_theme = updated_preferences.icon_theme;
            emit icon_theme_changed();
        }

        preferences.automatic_icon_theme = updated_preferences.automatic_icon_theme;

        if (preferences.language != updated_preferences.language)
        {
            preferences.language = updated_preferences.language;

            update_language();

            emit language_changed();
        }

        preferences = updated_preferences;
    }

    void PreferencesHandler::update_language()
    {
	    if (translator != nullptr)
            QApplication::removeTranslator(translator.get());

        if (base_translator != nullptr)
            QApplication::removeTranslator(base_translator.get());

        QString locale = preferences.language;
        if (locale == "")
            locale = QLocale::system().name().section('_', 0, 0);

        translator = std::make_shared<QTranslator>();
        translator->load(QString(":/languages/degate_") + locale);
        QApplication::installTranslator(translator.get());

        base_translator = std::make_shared<QTranslator>();
        base_translator->load("qtbase_" + locale, QLibraryInfo::location(QLibraryInfo::TranslationsPath));
        QApplication::installTranslator(base_translator.get());
    }

    const Preferences& PreferencesHandler::get_preferences()
    {
        return preferences;
    }

    QSettings& PreferencesHandler::get_settings()
    {
	    return settings;
    }
}
