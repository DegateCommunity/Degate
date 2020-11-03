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

#ifndef __GENERALPREFERENCESPAGE_H__
#define __GENERALPREFERENCESPAGE_H__

#include "GUI/Preferences/PreferencesPage/PreferencesPage.h"

#include <map>
#include <QComboBox>
#include <QCheckBox>
#include <QSpinBox>

namespace degate
{
    /**
     * @class GeneralPreferencesPage
     * @brief Widget to change general preferences (like language).
     *
     * @see PreferencesDialog
     */
    class GeneralPreferencesPage : public PreferencesPage
    {
    Q_OBJECT

    public:
        /**
         * Create the general preferences page widget.
         *
         * @param parent : the parent of the widget.
         */
        explicit GeneralPreferencesPage(QWidget* parent);
        ~GeneralPreferencesPage() override = default;

        /**
         * Update preferences, the preferences editor will call this function to apply changes for every page.
         *
         * @param preferences : the updated preferences to apply changes to.
         */
        void apply(Preferences& preferences) override;

    private:

        QComboBox language_edit;

        QCheckBox auto_save_status_edit;
        QSpinBox auto_save_interval_edit;
        QCheckBox automatic_updates_check_edit;

        std::map<QString, QString> languages;

    };
}

#endif //__GENERALPREFERENCESPAGE_H__
