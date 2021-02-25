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

#include "PerformancesPreferencesPage.h"

#include "Globals.h"

#include <QMessageBox>

namespace degate
{

    PerformancesPreferencesPage::PerformancesPreferencesPage(QWidget* parent)
            : PreferencesPage(parent)
    {
        //////////
        // Layout creation
        //////////

        // Cache category
        auto theme_layout = PreferencesPage::add_category(tr("Cache"));

        // Cache size spinbox
        PreferencesPage::add_widget(theme_layout, tr("Cache size (in Mb):"), &cache_size_edit);
        cache_size_edit.setMinimum(MINIMUM_CACHE_SIZE);
        cache_size_edit.setMaximum(std::numeric_limits<int>::max());
        cache_size_edit.setValue(PREFERENCES_HANDLER.get_preferences().cache_size);

        // Image importer cache size spinbox
        PreferencesPage::add_widget(theme_layout, tr("Image importer cache size (in Mb):"), &image_importer_cache_size_edit);
        image_importer_cache_size_edit.setMinimum(MINIMUM_CACHE_SIZE);
        image_importer_cache_size_edit.setMaximum(std::numeric_limits<int>::max());
        image_importer_cache_size_edit.setValue(PREFERENCES_HANDLER.get_preferences().image_importer_cache_size);
    }

    void PerformancesPreferencesPage::apply(Preferences& preferences)
    {
        if (static_cast<int>(preferences.cache_size) != cache_size_edit.value() ||
            static_cast<int>(preferences.image_importer_cache_size) != image_importer_cache_size_edit.value())
        {
            QMessageBox::information(this, tr("Preferences"), tr("You must restart Degate for some changes to take effect."));
        }

        preferences.cache_size = static_cast<unsigned int>(cache_size_edit.value());
        preferences.image_importer_cache_size = static_cast<unsigned int>(image_importer_cache_size_edit.value());
    }
}
