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

#include "AppearancePreferencesPage.h"

namespace degate
{
	AppearancePreferencesPage::AppearancePreferencesPage(QWidget* parent) : PreferencesPage(parent)
	{
	    //////////
	    // Widgets creation
	    //////////

        introduction_label.setText(tr("You can change appearance preferences here, like theme and icon theme."));

        // Theme
        themes[NATIVE_THEME] = tr("Native");
        themes[LIGHT_THEME] = tr("Light");
        themes[DARK_THEME] = tr("Dark");

        QStringList theme_list;
        for (auto& e : themes)
            theme_list.append(e.second);

		theme_box.addItems(theme_list);
		theme_box.setCurrentText(themes[PREFERENCES_HANDLER.get_preferences().theme]);

		// Icon theme
        icon_themes[LIGHT_ICON_THEME] = tr("Light");
        icon_themes[DARK_ICON_THEME] = tr("Dark");

		QStringList icon_theme_list;
        for (auto& e : icon_themes)
            icon_theme_list.append(e.second);

		icon_theme_box.addItems(icon_theme_list);
		icon_theme_box.setCurrentText(icon_themes[PREFERENCES_HANDLER.get_preferences().icon_theme]);
        icon_theme_box.setDisabled(PREFERENCES_HANDLER.get_preferences().automatic_icon_theme);

		// Automatic icon theme
        automatic_check_box.setChecked(PREFERENCES_HANDLER.get_preferences().automatic_icon_theme);
		QObject::connect(&automatic_check_box, SIGNAL(toggled(bool)), &icon_theme_box, SLOT(setDisabled(bool)));


        //////////
        // Layout creation
        //////////

        // Introduction label
        layout.addWidget(&introduction_label);

        // Theme category
        auto theme_layout = PreferencesPage::add_category(tr("Theme"));

        // Theme
        PreferencesPage::add_widget(theme_layout, tr("Theme:"), &theme_box);

        // Automatic icon theme checkbox
        PreferencesPage::add_widget(theme_layout, tr("Automatic icon theme:"), &automatic_check_box);

        // Icon theme
        PreferencesPage::add_widget(theme_layout, tr("Icon theme:"), &icon_theme_box);
	}

	AppearancePreferencesPage::~AppearancePreferencesPage()
	{
		
	}

    void AppearancePreferencesPage::apply(Preferences& preferences)
    {
	    // Theme
	    for (auto& e : themes)
        {
	        if (e.second == theme_box.currentText())
                preferences.theme = e.first;
        }

        // Icon theme
        if (automatic_check_box.isChecked())
        {
            if (preferences.theme == NATIVE_THEME)
            {
                icon_theme_box.setCurrentText(icon_themes[DARK_ICON_THEME]);
                preferences.icon_theme = DARK_ICON_THEME;
            }
            else if (preferences.theme == LIGHT_THEME)
            {
                icon_theme_box.setCurrentText(icon_themes[DARK_ICON_THEME]);
                preferences.icon_theme = DARK_ICON_THEME;
            }
            else
            {
                icon_theme_box.setCurrentText(icon_themes[LIGHT_ICON_THEME]);
                preferences.icon_theme = LIGHT_ICON_THEME;
            }
        }
        else
        {
            for (auto& e : icon_themes)
            {
                if (e.second == icon_theme_box.currentText())
                    preferences.icon_theme = e.first;
            }
        }

        // Automatic icon theme
        preferences.automatic_icon_theme = automatic_check_box.isChecked();
    }
}
