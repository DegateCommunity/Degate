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

#ifndef __PERFORMANCESPREFERENCESPAGE_H__
#define __PERFORMANCESPREFERENCESPAGE_H__

#include "GUI/Preferences/ThemeManager.h"
#include "GUI/Preferences/PreferencesPage/PreferencesPage.h"

#include <QSpinBox>

namespace degate
{
    /**
     * @class PerformancesPreferencesPage
     * @brief Widget to change performances preferences (like cache size).
     *
     * @see PreferencesDialog
     */
    class PerformancesPreferencesPage : public PreferencesPage
    {
        Q_OBJECT

    public:
        /**
         * Create the performances preferences page widget.
         *
         * @param parent : the parent of the widget.
         */
        explicit PerformancesPreferencesPage(QWidget* parent);
        ~PerformancesPreferencesPage() override = default;

        /**
         * Update preferences, the preferences editor will call this function to apply changes for every page.
         *
         * @param preferences : the updated preferences to apply changes to.
         */
        void apply(Preferences& preferences) override;

    private:
        QLabel introduction_label;
        QSpinBox cache_size_edit;
        QSpinBox image_importer_cache_size_edit;
        QSpinBox max_concurrent_thread_count_edit;

    };
}

#endif //__PERFORMANCESPREFERENCESPAGE_H__
