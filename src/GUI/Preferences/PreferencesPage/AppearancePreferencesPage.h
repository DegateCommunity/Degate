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

#ifndef __APPEARANCEPREFERENCESPAGE_H__
#define __APPEARANCEPREFERENCESPAGE_H__

#include "GUI/Preferences/ThemeManager.h"
#include "GUI/Preferences/PreferencesPage/PreferencesPage.h"

#include <QComboBox>
#include <QCheckBox>

namespace degate
{

	/**
	 * @class AppearancePreferencesPage
	 * @brief Widget to change appearance preferences (like theme and icon theme).
	 *
	 * @see PreferencesDialog
	 */
	class AppearancePreferencesPage : public PreferencesPage
	{
		Q_OBJECT
		
	public:
		/**
		 * Create the appearance preferences page widget.
		 *
		 * @param parent : the parent of the widget.
		 */
		AppearancePreferencesPage(QWidget* parent);
		~AppearancePreferencesPage();

        /**
         * Update preferences, the preferences editor will call this function to apply changes for every page.
         *
         * @param preferences : the updated preferences to apply changes to.
         */
        void apply(Preferences& preferences) override ;
		
	private:

		QComboBox theme_box;
		QComboBox icon_theme_box;
        QCheckBox automatic_check_box;
	};
}

#endif