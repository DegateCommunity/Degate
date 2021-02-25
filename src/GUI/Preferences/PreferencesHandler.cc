/**
 * This file is part of the IC reverse engineering tool Degate.
 *
 * Copyright 2008, 2009, 2010 by Martin Schobert
 * Copyright 2019-2020 Dorian Bachelot
 *
 * Degate is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * Degate is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with degate. If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "PreferencesHandler.h"

#include <fstream>

#define RECENT_PROJECTS_LIST_FILE "recent.config"

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
        preferences.automatic_updates_check = settings.value("automatic_updates_check", true).toBool();


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


        ///////////
        // Performances
        ///////////

        // Cache size
        preferences.cache_size = settings.value("cache_size", 256).toUInt();

        // Image importer cache size
        preferences.image_importer_cache_size = settings.value("image_importer_cache_size", 256).toUInt();


        load_recent_projects();
    }

    PreferencesHandler::~PreferencesHandler()
    {
        save();
        save_recent_projects();
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
        settings.setValue("automatic_updates_check", preferences.automatic_updates_check);


        ///////////
        // Grid
        ///////////

        settings.setValue("grid_color", preferences.grid_color);
        settings.setValue("max_grid_lines_count", preferences.max_grid_lines_count);
        settings.setValue("show_grid", preferences.show_grid);
        settings.setValue("snap_to_grid", preferences.snap_to_grid);


        ///////////
        // Performances
        ///////////

        settings.setValue("cache_size", preferences.cache_size);
        settings.setValue("image_importer_cache_size", preferences.image_importer_cache_size);
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

        if (qt_translator != nullptr)
            QApplication::removeTranslator(qt_translator.get());

        if (base_translator != nullptr)
            QApplication::removeTranslator(base_translator.get());

        QString locale = preferences.language;
        if (locale == "")
            locale = QLocale::system().name().section('_', 0, 0);

        translator = std::make_shared<QTranslator>();
        translator->load(QString(":/languages/degate_") + locale);
        QApplication::installTranslator(translator.get());

        qt_translator = std::make_shared<QTranslator>();
        qt_translator->load("qt_" + locale, QLibraryInfo::location(QLibraryInfo::TranslationsPath));
        QApplication::installTranslator(qt_translator.get());

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

    std::vector<std::pair<std::string, std::string>> PreferencesHandler::get_recent_projects()
    {
        remove_invalid_recent_projects();

        return recent_projects;
    }

    void PreferencesHandler::add_recent_project(const Project_shptr& project)
    {
        insert_recent_project(project->get_name(), project->get_project_directory());
    }

    void PreferencesHandler::remove_invalid_recent_projects()
    {
        std::vector<std::pair<std::string, std::string>> elements;

        for (auto& e : recent_projects)
        {
            if (is_directory(e.second))
                elements.push_back(std::move(e));
        }

        recent_projects = std::move(elements);
    }

    void PreferencesHandler::load_recent_projects()
    {
        recent_projects.clear();

        if (!file_exists(DEGATE_IN_CONFIGURATION(RECENT_PROJECTS_LIST_FILE)))
            return;

        std::string project_name;
        std::string project_path;

        std::fstream file;
        file.open(DEGATE_IN_CONFIGURATION(RECENT_PROJECTS_LIST_FILE), std::fstream::in);

        if (!file.is_open())
        {
            debug(TM, "Can't open recent projects list file.");
            return;
        }

        std::string buffer;
        if (!std::getline(file, buffer))
        {
            debug(TM, "Can't read recent projects list file.");
            return;
        }

        int projects_count = std::stoi(buffer);

        if (projects_count <= 0)
        {
            debug(TM, "No project in the recent projects list.");
            return;
        }

        for (int i = 0; i < projects_count; i++)
        {
            if (!std::getline(file, project_name))
            {
                debug(TM, "Can't read project name for recent projects list file.");
                return;
            }

            if (!std::getline(file, project_path))
            {
                debug(TM, "Can't read project path for recent projects list file.");
                return;
            }

            insert_recent_project(project_name, project_path);
        }

        remove_invalid_recent_projects();
    }

    void PreferencesHandler::save_recent_projects()
    {
        std::fstream file;
        file.open(DEGATE_IN_CONFIGURATION(RECENT_PROJECTS_LIST_FILE), std::fstream::out | std::fstream::trunc);

        if (!file.is_open())
        {
            debug(TM, "Can't open recent projects file list.");
            return;
        }

        file << recent_projects.size();
        file << std::endl;

        for (const auto& e : recent_projects)
        {
            file << e.first;
            file << std::endl;

            file << e.second;
            file << std::endl;
        }
    }

    void PreferencesHandler::insert_recent_project(const std::string& project_name, const std::string& project_path)
    {
        for (auto it = recent_projects.begin(); it != recent_projects.end(); it++)
        {
            if ((*it).second == project_path)
            {
                recent_projects.erase(it);

                break;
            }
        }

        recent_projects.emplace_back(project_name, project_path);
    }
}
