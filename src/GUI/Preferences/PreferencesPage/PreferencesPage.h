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

#ifndef __PREFERENCESPAGE_H__
#define __PREFERENCESPAGE_H__

#include "GUI/Preferences/PreferencesHandler.h"

#include <QWidget>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>

namespace degate
{
    /**
     * @class PreferencesPage
     * @brief Virtual class to create a new preferences page.
     */
    class PreferencesPage : public QWidget
    {
        Q_OBJECT

    public:

        explicit PreferencesPage(QWidget* parent);
        ~PreferencesPage() = default;

        /**
         * Update preferences, the preferences editor will call this function to apply changes for every page.
         *
         * @param preferences : the updated preferences to apply changes to.
         */
        virtual void apply(Preferences& preferences) = 0;

        /**
         * Add a new category that will store multiple widgets.
         *
         * @param name : the name of the category.
         *
         * @return Returns the new category layout (to use with add_widget function for example).
         */
        QGridLayout* add_category(const QString& name);

        /**
         * Add a widget and a label to a category layout.
         *
         * @param category_layout : the category layout (@see add_category function).
         * @param label : the label describing the widget.
         * @param widget : the widget (can be null) to add to the category.
         */
        static void add_widget(QGridLayout* category_layout, const QString& label, QWidget* widget = nullptr);

    protected:
        QVBoxLayout layout;
    };
}

#endif
