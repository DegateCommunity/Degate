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

#include <GUI/Preferences/PreferencesPage/AppearancePreferencesPage.h>

namespace degate
{
	AppearancePreferencesPage::AppearancePreferencesPage(QWidget* parent) : PreferencesPage(parent)
	{
	    //////////
	    // Widgets creation
	    //////////

        auto introduction_label = new QLabel("You can change appearance preferences here, like theme and icon theme.");

        // Theme
		QStringList theme_list;
		theme_list.append("native");
		theme_list.append("light");
		theme_list.append("dark");
		theme_box.addItems(theme_list);
		theme_box.setCurrentText(QString::fromStdString(theme_to_string(PREFERENCES_HANDLER.get_preferences().theme)));

		// Icon theme
		QStringList icon_theme_list;
		icon_theme_list.append("light");
		icon_theme_list.append("dark");
		icon_theme_box.addItems(icon_theme_list);
		icon_theme_box.setCurrentText(QString::fromStdString(icon_theme_to_string(PREFERENCES_HANDLER.get_preferences().icon_theme)));
        icon_theme_box.setDisabled(PREFERENCES_HANDLER.get_preferences().automatic_icon_theme);

		// Automatic icon theme
        automatic_check_box.setChecked(PREFERENCES_HANDLER.get_preferences().automatic_icon_theme);
		QObject::connect(&automatic_check_box, SIGNAL(toggled(bool)), &icon_theme_box, SLOT(setDisabled(bool)));


        //////////
        // Layout creation
        //////////

        // Introduction label
        layout.addWidget(introduction_label);

        // Theme category
        auto theme_layout = PreferencesPage::add_category("Theme");

        // Theme
        PreferencesPage::add_widget(theme_layout, "Theme :", &theme_box);

        // Automatic icon theme checkbox
        PreferencesPage::add_widget(theme_layout, "Automatic icon theme :", &automatic_check_box);

        // Icon theme
        PreferencesPage::add_widget(theme_layout, "Icon theme :", &icon_theme_box);
	}

	AppearancePreferencesPage::~AppearancePreferencesPage()
	{
		
	}

    void AppearancePreferencesPage::apply(Preferences& preferences)
    {
	    // Theme
        preferences.theme = string_to_theme(theme_box.currentText().toStdString());

        // Icon theme
        if(automatic_check_box.isChecked())
        {
            if(theme_box.currentText().toStdString() == "native")
            {
                icon_theme_box.setCurrentText(QString::fromStdString(icon_theme_to_string(DARK_ICON_THEME)));
                preferences.icon_theme = DARK_ICON_THEME;
            }
            else if (theme_box.currentText().toStdString() == "light")
            {
                icon_theme_box.setCurrentText(QString::fromStdString(icon_theme_to_string(DARK_ICON_THEME)));
                preferences.icon_theme =  DARK_ICON_THEME;
            }
            else
            {
                icon_theme_box.setCurrentText(QString::fromStdString(icon_theme_to_string(LIGHT_ICON_THEME)));
                preferences.icon_theme =  LIGHT_ICON_THEME;
            }
        }
        else
            preferences.icon_theme =  string_to_icon_theme(icon_theme_box.currentText().toStdString());

        // Automatic icon theme
        preferences.automatic_icon_theme = automatic_check_box.isChecked();
    }
}
