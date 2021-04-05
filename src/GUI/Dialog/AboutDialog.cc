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

        QString version_type;
        if (QString(DEGATE_VERSION_TYPE) == QString("nightly"))
            version_type = "<span style='color:darkred;'>" + tr("Nightly version.") + "</span><br><br>";

        QString release_date;
        if (QString(DEGATE_RELEASE_DATE) == QString("unreleased"))
            release_date = "<span style='color:darkred;'>" + tr("Unreleased version.") + "</span>";
        else
            release_date = tr("Version released on %1.", "... released on dd/mm/yy").arg(DEGATE_RELEASE_DATE);

        // First tab
        const QString about_message =
                              "<html><center><br>"
                              "<img src=':/degate_logo.png' alt='' width='100' height='87'> <br><br>"
                              "<strong>" + tr("Welcome to Degate version %1.") + "</strong><br>"
                              + version_type +
                              "<strong>" + tr("The current project maintainer is %2.", "... maintainer is NAME.") + "</strong><br><br>" +
                              tr("This project is a forked version of Degate and is still under development, if you find a bug please report it %3.", "... report it here.") + "<br>" +
                              tr("You can also help us by adding new languages, see the Localization section of the README.md file, available %4.", "... available here.")
                              + "<br><br>" +
                              tr("This software is released under the GNU General Public License Version 3.") + "<br>"
                              + release_date + "<br><br>"
                              "<a href='https://github.com/DegateCommunity/Degate'>Github</a> <br>"
                              "<a href='https://github.com/DegateCommunity/Degate/blob/develop/ROADMAP.md'>" + tr("Roadmap") + "</a> <br>"
                              "<a href='https://degate.readthedocs.io'>" + tr("Documentation") + "</a>"
                              "</center></html>";

        about.setText(about_message.arg(DEGATE_VERSION)
                                   .arg("<a href='https://github.com/DorianBDev'>Dorian Bachelot</a>")
                                   .arg("<a href='https://github.com/DegateCommunity/Degate/issues'>" + tr("here") + "</a>")
                                   .arg("<a href='https://github.com/DegateCommunity/Degate/blob/develop/README.md#localization'>" + tr("here") + "</a>"));
        about.setReadOnly(true);
        about.setOpenExternalLinks(true);

        // Second tab
        const QString tools_message =
                "<html><center><br>" +
                tr("Tools used:") + "<br>"
                                    "<a href='https://www.qt.io/'>Qt</a><br>"
                                    "<a href='https://www.boost.org/'>Boost</a><br><br>" +
                tr("Themes used:") + "<br>"
                                     "<a href='https://github.com/ColinDuquesnoy/QDarkStyleSheet'>" + tr("Blue dark") + "</a><br>"
                                     "</center></html>";

        tools.setText(tools_message);
        tools.setReadOnly(true);
        tools.setOpenExternalLinks(true);

        // Third tab
        QFile file_changelog(":/CHANGELOG.md");
        if (!file_changelog.open(QFile::ReadOnly | QFile::Text))
        {
            throw std::runtime_error("Can't open the changelog file.");
        }

        QTextStream in_changelog(&file_changelog);
        changelog.setMarkdown(in_changelog.readAll());
        changelog.setReadOnly(true);
        changelog.setOpenExternalLinks(true);

        file_changelog.close();

        // Fourth tab
        QFile file_license(":/LICENSE.TXT");
        if (!file_license.open(QFile::ReadOnly | QFile::Text))
        {
            throw std::runtime_error("Can't open the license file.");
        }

        QTextStream in_license(&file_license);
        license.setText(in_license.readAll());
        license.setReadOnly(true);
        license.setOpenExternalLinks(true);

        file_license.close();

        // Tabs
        tabs.addTab(&about, tr("About"));
        tabs.addTab(&tools, tr("Tools"));
        tabs.addTab(&changelog, tr("Changelog"));
        tabs.addTab(&license, tr("License"));

        layout.addWidget(&tabs);
        setLayout(&layout);

        // Size
        resize(700, 375);
    }
}
