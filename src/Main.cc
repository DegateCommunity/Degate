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

#include <QApplication>
#include <QSplashScreen>
#include <QTranslator>
#include <QDateTime>

#include <fstream>

#include "GUI/Preferences/PreferencesHandler.h"
#include "GUI/MainWindow.h"
#include "Core/Version.h"

int main(int argc, char* argv[])
{
    int ret;

    try
    {
        QApplication a(argc, argv);

        QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, QApplication::applicationDirPath());

        degate::PreferencesHandler::get_instance().update_language();

        QSplashScreen splash(QPixmap(":/degate_splash.png"));
        splash.show();
        QApplication::processEvents();

        degate::MainWindow window;
        window.show();

        splash.finish(&window);

        // If it's a first launch or an updated Degate version, show help menu.
        if (degate::PreferencesHandler::get_instance().get_settings().value("first_launch", true).toBool() ||
            degate::PreferencesHandler::get_instance().get_settings().value("last_launch_version", "") != DEGATE_VERSION)
        {
            window.on_menu_help_about();

            degate::PreferencesHandler::get_instance().get_settings().setValue("first_launch", false);
            degate::PreferencesHandler::get_instance().get_settings().setValue("last_launch_version", DEGATE_VERSION);
        }

        ret = QApplication::exec();
    }
    catch (const std::exception& e)
    {
        QString message = "[%1]\n"
                          "Build ABI: '%2'\n"
                          "Kernel type: '%3'\n"
                          "kernel version: '%4'\n"
                          "Current CPU architecture: '%5'\n"
                          "Build CPU architecture: '%6'\n"
                          "Product type: '%7'\n"
                          "Pretty product name: '%8'\n"
                          "Product version: '%9'\n"
                          "Degate version: '%10'\n"
                          "An unexpected error occurred. Error message:\n\n\t%11\n\n"
                          "-----------------------------------------------------\n";

        message = message.arg(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"))
                         .arg(QSysInfo::buildAbi())
                         .arg(QSysInfo::kernelType())
                         .arg(QSysInfo::kernelVersion())
                         .arg(QSysInfo::currentCpuArchitecture())
                         .arg(QSysInfo::buildCpuArchitecture())
                         .arg(QSysInfo::productType())
                         .arg(QSysInfo::prettyProductName())
                         .arg(QSysInfo::productVersion())
                         .arg(DEGATE_VERSION)
                         .arg(QString::fromStdString(e.what()));

        std::ofstream log_file;
        log_file.open("error_log.txt", std::fstream::in | std::fstream::out | std::fstream::app);

        if (!log_file.is_open())
            return EXIT_FAILURE;

        log_file << message.toStdString();
        log_file.close();

        return EXIT_FAILURE;
    }

	return ret;
}
