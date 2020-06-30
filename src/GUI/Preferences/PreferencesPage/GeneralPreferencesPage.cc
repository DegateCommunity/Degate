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

        //////////
        // Layout creation
        //////////

        // Theme category
        auto language_layout = PreferencesPage::add_category(tr("Language"));

        // Theme
        PreferencesPage::add_widget(language_layout, tr("Language:"), &language_edit);
    }

    void GeneralPreferencesPage::apply(Preferences& preferences)
    {
        // Language
        for(auto& e : languages)
        {
            if(e.second == language_edit.currentText())
                preferences.language = e.first;
        }
    }
}
