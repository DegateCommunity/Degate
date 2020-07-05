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

#ifndef __GRIDPREFERENCESPAGE_H__
#define __GRIDPREFERENCESPAGE_H__

#include <GUI/Preferences/PreferencesPage/PreferencesPage.h>
#include <GUI/Dialog/ColorPickerDialog.h>

#include <QSpinBox>

namespace degate
{
    /**
	 * @class GridPreferencesPage
	 * @brief Widget to change grid preferences.
	 *
	 * @see PreferencesDialog
	 */
    class GridPreferencesPage : public PreferencesPage
    {
    Q_OBJECT

    public:
        /**
		 * Create the grid preferences page widget.
		 *
		 * @param parent : the parent of the widget.
		 */
        GridPreferencesPage(QWidget* parent);
        ~GridPreferencesPage() = default;

        /**
         * Update preferences, the preferences editor will call this function to apply changes for every page.
         *
         * @param preferences : the updated preferences to apply changes to.
         */
        void apply(Preferences& preferences) override;

    private:
        ColorSelectionButton grid_color_edit;
        QSpinBox             max_grid_lines_count_edit;

    };
}

#endif //__GRIDPREFERENCESPAGE_H__
