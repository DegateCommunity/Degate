/**
 * This file is part of the IC reverse engineering tool Degate.
 *
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

#include "ConfigurationPreferencesPage.h"

#include "GUI/Text/Text.h"

namespace degate
{
    ConfigurationPreferencesPage::ConfigurationPreferencesPage(QWidget* parent)
            : PreferencesPage(parent)
    {
        //////////
        // Layout creation
        //////////

        // Configuration category
        auto theme_layout = PreferencesPage::add_category(tr("Configuration"));

        // Clear configuration button
        PreferencesPage::add_widget(theme_layout, tr("Clear configuration:"), &clear_configuration_edit);
        clear_configuration_edit.setText(tr("Clear"));

        QObject::connect(&clear_configuration_edit, &QPushButton::clicked, this, [this]()
        {
            clear_directory(DEGATE_CONFIGURATION_PATH, { DEGATE_CACHE_PATH, DEGATE_IN_CONFIGURATION(FONTS_CONFIG_FILE_NAME) });
        });

        // Clear cache button
        PreferencesPage::add_widget(theme_layout, tr("Clear static cache:"), &clear_cache_edit);
        clear_cache_edit.setText(tr("Clear"));

        QObject::connect(&clear_cache_edit, &QPushButton::clicked, this, [this]()
        {
          remove_directory(DEGATE_CACHE_PATH);
          remove_file(DEGATE_IN_CONFIGURATION(FONTS_CONFIG_FILE_NAME));
        });

        // Clean configuration and cache button
        PreferencesPage::add_widget(theme_layout, tr("Clear files (remove the '.degate' configuration/cache directory):"), &clear_configuration_and_cache_edit);
        clear_configuration_and_cache_edit.setText(tr("Clear"));

        QObject::connect(&clear_configuration_and_cache_edit, &QPushButton::clicked, this, [this]()
        {
            remove_directory(DEGATE_CONFIGURATION_PATH);
        });
    }

    void ConfigurationPreferencesPage::apply(Preferences& preferences)
    {

    }
}
