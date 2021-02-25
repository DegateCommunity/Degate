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

#ifndef __CONFIGURATIONPREFERENCESPAGE_H__
#define __CONFIGURATIONPREFERENCESPAGE_H__

#include "GUI/Preferences/PreferencesPage/PreferencesPage.h"

#include <QPushButton>

namespace degate
{
    /**
     * @class ConfigurationPreferencesPage
     * @brief Widget to clear cache and configuration.
     *
     * @see PreferencesDialog
     */
    class ConfigurationPreferencesPage : public PreferencesPage
    {
    Q_OBJECT

    public:
        /**
         * Create the configuration preferences page widget.
         *
         * @param parent : the parent of the widget.
         */
        explicit ConfigurationPreferencesPage(QWidget* parent);
        ~ConfigurationPreferencesPage() override = default;

        /**
         * Update preferences, the preferences editor will call this function to apply changes for every page.
         *
         * @param preferences : the updated preferences to apply changes to.
         */
        void apply(Preferences& preferences) override;

    private:
        QPushButton clear_cache_edit;
        QPushButton clear_configuration_edit;
        QPushButton clear_configuration_and_cache_edit;
    };
} // namespace degate

#endif //__CONFIGURATIONPREFERENCESPAGE_H__
