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

#ifndef __ABOUTDIALOG_H__
#define __ABOUTDIALOG_H__

#include <QDialog>
#include <QTabWidget>
#include <QLabel>
#include <QTextBrowser>
#include <QTextDocument>
#include <QVBoxLayout>

namespace degate
{
    /**
     * @class AboutDialog
     * @brief Display information about Degate and the changelog in a dialog.
     */
    class AboutDialog : public QDialog
    {
        Q_OBJECT

    public:
        /**
         * Create the "about" dialog.
         * It will be composed of 2 tabs.
         *
         * @param parent : the parent widget.
         */
        explicit AboutDialog(QWidget* parent);
        ~AboutDialog() override = default;

    private:
        QVBoxLayout layout;
        QTabWidget tabs;

        // First tab
        QTextBrowser about;

        // Second tab
        QTextBrowser tools;

        // Third tab
        QTextBrowser changelog;

        // Fourth tab
        QTextBrowser license;

    };
}

#endif //__ABOUTDIALOG_H__
