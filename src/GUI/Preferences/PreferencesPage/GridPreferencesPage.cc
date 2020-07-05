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

#include "GridPreferencesPage.h"

namespace degate
{

    GridPreferencesPage::GridPreferencesPage(QWidget* parent) : PreferencesPage(parent)
    {
        // Grid category
        auto grid_layout = PreferencesPage::add_category(tr("Grid"));

        grid_color_edit.set_color(PREFERENCES_HANDLER.get_preferences().grid_color);
        PreferencesPage::add_widget(grid_layout, tr("Grid color:"), &grid_color_edit);

        max_grid_lines_count_edit.setMinimum(1);
        max_grid_lines_count_edit.setMaximum(10000000);
        max_grid_lines_count_edit.setValue(PREFERENCES_HANDLER.get_preferences().max_grid_lines_count);
        PreferencesPage::add_widget(grid_layout, tr("Max grid lines count:"), &max_grid_lines_count_edit);
    }

    void GridPreferencesPage::apply(Preferences& preferences)
    {
        preferences.grid_color           = grid_color_edit.get_color();
        preferences.max_grid_lines_count = max_grid_lines_count_edit.value();
    }
}
