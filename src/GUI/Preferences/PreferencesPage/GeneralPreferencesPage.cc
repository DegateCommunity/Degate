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

#include "GeneralPreferencesPage.h"

namespace degate
{

    GeneralPreferencesPage::GeneralPreferencesPage(QWidget* parent) : PreferencesPage(parent)
    {
        //////////
        // Widgets creation
        //////////

        languages[""] = tr("System");
        languages["en"] = "English";
        languages["fr"] = "Français";

        QStringList language_list;
        for(auto& e : languages)
            language_list.append(e.second);

        language_edit.addItems(language_list);
        language_edit.setCurrentText(languages[PREFERENCES_HANDLER.get_preferences().language]);

        auto_save_status_edit.setChecked(PREFERENCES_HANDLER.get_preferences().auto_save_status);

        auto_save_interval_edit.setMinimum(1);
        auto_save_interval_edit.setMaximum(1000);
        auto_save_interval_edit.setValue(PREFERENCES_HANDLER.get_preferences().auto_save_interval);
        auto_save_interval_edit.setEnabled(auto_save_status_edit.isChecked());
        QObject::connect(&auto_save_status_edit, SIGNAL(toggled(bool)), &auto_save_interval_edit, SLOT(setEnabled(bool)));

        //////////
        // Layout creation
        //////////

        // Language category
        auto language_layout = PreferencesPage::add_category(tr("Language"));

        // Language
        PreferencesPage::add_widget(language_layout, tr("Language:"), &language_edit);

        // Auto save category
        auto auto_save_layout = PreferencesPage::add_category(tr("Auto save"));

        PreferencesPage::add_widget(auto_save_layout, tr("Auto save status:"), &auto_save_status_edit);
        PreferencesPage::add_widget(auto_save_layout, tr("Auto save interval (in minutes):"), &auto_save_interval_edit);
    }

    void GeneralPreferencesPage::apply(Preferences& preferences)
    {
        // Language
        for(auto& e : languages)
        {
            if(e.second == language_edit.currentText())
                preferences.language = e.first;
        }

        preferences.auto_save_status = auto_save_status_edit.isChecked();
        preferences.auto_save_interval = auto_save_interval_edit.value();
    }
}
