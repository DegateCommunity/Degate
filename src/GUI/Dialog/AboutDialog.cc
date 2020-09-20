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

#include "AboutDialog.h"

#include "Core/Version.h"

#include <stdexcept>
#include <QFile>
#include <QTextStream>

namespace degate
{
    AboutDialog::AboutDialog(QWidget* parent) : QDialog(parent)
    {
        setWindowFlags(Qt::Window);

        // First tab
        const QString about_message =
                              "<html><center>"
                              "<img src=':/degate_logo.png' alt='' width='100' height='87'> <br><br>"
                              "<strong>" + tr("Welcome to Degate version %1") + "</strong><br><br>"
                              "<strong>" + tr("The current project maintainer is %2.", "... maintainer is NAME.") + "</strong><br><br>" +
                              tr("This project is a forked version of Degate and is still under development, if you find a bug please report it %3.", "... report it here.") + "<br>" +
                              tr("You can also help us by adding new languages, see the Localization section of the README.md file, available %4.", "... available here.")
                              + "<br><br>" +
                              tr("To obtain the latest update of Degate, please see %5 page.", "... please see this page.")
                              + "<br><br>" +
                              tr("This software is released under the GNU General Public License Version 3.") + "<br><br>"
                              "<a href='https://github.com/DegateCommunity/Degate/wiki'>Wiki</a> <br>"
                              "<a href='https://github.com/DegateCommunity/Degate'>Github</a> <br>"
                              "<a href='https://github.com/DegateCommunity/Degate/blob/develop/ROADMAP.md'>" + tr("Roadmap") + "</a> <br>"
                              "<a href='https://degate.readthedocs.io'>" + tr("Documentation") + "</a> <br>"
                              "<a href='http://www.degate.org/'>" + tr("Original website") + "</a>"
                              "</center></html>";

        about.setText(about_message.arg(DEGATE_VERSION)
                                        .arg("<a href='https://github.com/DorianBDev'>Dorian Bachelot</a>")
                                        .arg("<a href='https://github.com/DegateCommunity/Degate/issues'>" + tr("here") + "</a>")
                                        .arg("<a href='https://github.com/DegateCommunity/Degate/blob/develop/README.md#localization'>" + tr("here") + "</a>")
                                        .arg("<a href='https://github.com/DegateCommunity/Degate/releases'>" + tr("this", "... please see this page.") + "</a>"));
        about.setReadOnly(true);
        about.setOpenExternalLinks(true);

        // Second tab
        QFile file(":/CHANGELOG.md");
        if (!file.open(QFile::ReadOnly | QFile::Text))
        {
            throw std::runtime_error("Can't open the changelog file. Maybe a CMake error during the changelog copy process.");
        }

        QTextStream in(&file);
        changelog.setMarkdown(in.readAll());
        changelog.setReadOnly(true);
        changelog.setOpenExternalLinks(true);

        // Tabs
        tabs.addTab(&about, tr("About"));
        tabs.addTab(&changelog, tr("Changelog"));

        layout.addWidget(&tabs);
        setLayout(&layout);

        // Size
        if (parent != nullptr)
            resize(parent->size() * 0.55);
    }
}
