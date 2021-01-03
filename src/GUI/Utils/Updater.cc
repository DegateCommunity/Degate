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

#include "Updater.h"

#include "GUI/Preferences/PreferencesHandler.h"

#include <QCoreApplication>
#include <QDir>
#include <QMessageBox>

namespace degate
{
    Updater::Updater(QWidget* parent)
            : parent(parent)
    {
        connect(&process, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(process_finished(int, QProcess::ExitStatus)));
        connect(&process, SIGNAL(error(QProcess::ProcessError)), this, SLOT(process_error(QProcess::ProcessError)));

        #if defined(Q_OS_WIN)
            tool_name = "maintenancetool.exe";
        #elif defined(Q_OS_MAC)
            tool_name = "../../../maintenancetool.app/Contents/MacOS/maintenancetool";
        #else
            tool_name = "maintenancetool";
        #endif
    }

    void Updater::run(bool notify_no_update, bool ask_disabling_automatic_check)
    {
        if (running)
            return;

        this->notify_no_update = notify_no_update;
        this->ask_disabling_automatic_check = ask_disabling_automatic_check;

        running = true;

        QString path = QDir(QCoreApplication::applicationDirPath()).absoluteFilePath(tool_name);

        QStringList args;
        args.append("ch");

        process.start(path, args);
    }

    void Updater::process_finished(int exit_code, QProcess::ExitStatus exit_status)
    {
        running = false;

        QByteArray data = process.readAllStandardOutput();

        if(!data.contains("<updates>"))
        {
            if (notify_no_update)
            {
                QString message =
                        "<html><center>" +
                        tr("No update available.") +
                        "</center></html>";

                QMessageBox::information(parent, tr("Check for updates"), message);
            }

            return;
        }

        QMessageBox::StandardButton reply;

        if (ask_disabling_automatic_check)
        {
            QString message =
                    "<html><center>"
                    + tr("A new update is available, install it now?") + "<br>" +
                    tr("(No will disable automatic check for updates)", "'No' is a button name (Yes/No message box).") +
                    "</center></html>";

            reply = QMessageBox::question(parent,
                                          tr("Check for updates"),
                                          message,
                                          QMessageBox::Yes | QMessageBox::No | QMessageBox::Close);
        }
        else
        {
            QString message =
                    "<html><center>"
                    + tr("A new update is available, install it now?") +
                    "</center></html>";

            reply = QMessageBox::question(parent,
                                          tr("Check for updates"),
                                          message,
                                          QMessageBox::Yes | QMessageBox::No);
        }

        if (reply == QMessageBox::Yes)
        {
            QStringList args("--su");
            bool success = QProcess::startDetached(tool_name, args);

            if (success)
                QCoreApplication::quit();
        }
        else if (reply == QMessageBox::No && ask_disabling_automatic_check)
        {
            Preferences new_preferences = PREFERENCES_HANDLER.get_preferences();
            new_preferences.automatic_updates_check = false;
            PREFERENCES_HANDLER.update(new_preferences);

            return;
        }
        else
            return;
    }

    void Updater::process_error(QProcess::ProcessError error)
    {
        switch (error)
        {
            case QProcess::Crashed:
                break;
            case QProcess::Timedout:
                break;
            case QProcess::ReadError:
                break;
            case QProcess::WriteError:
                break;
            case QProcess::UnknownError:
                break;

            case QProcess::FailedToStart :

                // The maintenance tool is not usable.
                Preferences new_preferences = PREFERENCES_HANDLER.get_preferences();
                new_preferences.automatic_updates_check = false;
                PREFERENCES_HANDLER.update(new_preferences);

                if (notify_no_update)
                {
                    QString message =
                            "<html><center>"
                            + tr("We cannot check for updates, your installation is either broken or has not been installed with a proper installer.") +
                            "<br>" +
                            tr("Please visit %1 page.") +
                            "</center></html>";

                    QMessageBox::information(parent,
                                             tr("Check for updates"),
                                             message.arg(
                                                     "<html><a href='https://github.com/DegateCommunity/Degate/releases'>" +
                                                     tr("this", "... please see this page.") +
                                                     "</a></html>"));
                }

                break;
        }

        running = false;
    }
}
