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

#ifndef __PREFERENCESHANDLER_H__
#define __PREFERENCESHANDLER_H__

#include "Core/Primitive/SingletonBase.h"
#include "GUI/Preferences/ThemeManager.h"
#include "Core/XML/XMLExporter.h"
#include "Core/XML/XMLImporter.h"
#include "Core/Project/Project.h"

#include <QObject>

namespace degate
{

    /**
     * @struct Preferences
     * @brief Stores all preference values.
     */
    struct Preferences
    {
        ///////////
        // Appearance
        ///////////

        Theme     theme;
        IconTheme icon_theme;
        bool      automatic_icon_theme;


        ///////////
        // General
        ///////////

        QString      language;
        bool         auto_save_status;
        unsigned int auto_save_interval;
        bool         automatic_updates_check;


        ///////////
        // Grid
        ///////////

        color_t      grid_color;
        unsigned int max_grid_lines_count;
        bool         show_grid;
        bool         snap_to_grid;


        ///////////
        // Performances
        ///////////

        unsigned int cache_size;
        unsigned int image_importer_cache_size;

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
        ~PreferencesHandler() override;

        /**
         * Save preferences in the configuration file.
         */
        void save();

        /**
         * Update preferences.
         */
        void update(const Preferences& updated_preferences);

        /**
         * Update language regarding the language in the preferences.
         */
        void update_language();

        /**
         * Get all stored preferences.
         *
         * @return Returns a const reference of all preferences.
         */
        const Preferences& get_preferences();

        /**
         * Get the associated QSettings. You should avoid using this.
         *
         * @return Returns the QSettings of the application.
         */
        QSettings& get_settings();

        /**
         * Get the list of recent projects.
         *
         * The first element is the project name.
         * The second element is the project path.
         *
         * If a path is not reachable during loading, it will remove it from the list.
         *
         * The list is inverted, the most recently used project is at the back of the vector.
         *
         * @return Returns the list of recent projects.
         */
        std::vector<std::pair<std::string, std::string>> get_recent_projects();

        /**
         * Add a new project to the recent project list.
         *
         * @param project : The new project to add.
         */
        void add_recent_project(const Project_shptr& project);

    protected:
        /**
         * Remove invalid recent projects (that are not reachable).
         */
        void remove_invalid_recent_projects();

        /**
         * Load recent projects.
         */
        void load_recent_projects();

        /**
         * Save recent projects.
         */
        void save_recent_projects();

        /**
         * Insert a project in the recent projects list.
         *
         * @param project_name : The name of the project to insert.
         * @param project_path : The path of the project to insert.
         */
        void insert_recent_project(const std::string& project_name, const std::string& project_path);

    signals:
        /**
         * Emitted when the icon theme changed.
         */
        void icon_theme_changed();

        /**
         * Emitted when the theme changed.
         */
        void theme_changed();

        /**
         * Emitted when the language changed.
         */
        void language_changed();

    private:
        QSettings settings;
        Preferences preferences;
        std::shared_ptr<QTranslator> translator = nullptr;
        std::shared_ptr<QTranslator> qt_translator = nullptr;
        std::shared_ptr<QTranslator> base_translator = nullptr;
        std::vector<std::pair<std::string /* Project name */, std::string /* Project path */>> recent_projects;
    };
}

/**
 * Get the preferences handler instance.
 */
#define PREFERENCES_HANDLER PreferencesHandler::get_instance()

#endif