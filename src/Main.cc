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

#include <csignal>
#include <QApplication>
#include <QSplashScreen>
#include <QTranslator>

#include "GUI/Preferences/PreferencesHandler.h"
#include "GUI/MainWindow.h"
#include "Core/Version.h"
#include "Core/Utils/CrashReport.h"

int main(int argc, char* argv[])
{
    int ret = 0;

    // Create Qt application
    QApplication a(argc, argv);

    // Bind signals
    std::signal(SIGSEGV, [](int signal) {
        std::string error = "A SIGSEGV occurred (" + std::to_string(signal) + ").";
        degate::crash_report(error);
        QApplication::closeAllWindows();
    });

    std::signal(SIGFPE, [](int signal) {
        std::string error = "A SIGFPE occurred (" + std::to_string(signal) + ").";
        degate::crash_report(error);
        QApplication::closeAllWindows();
    });

    std::signal(SIGILL, [](int signal) {
        std::string error = "A SIGILL occurred (" + std::to_string(signal) + ").";
        degate::crash_report(error);
        QApplication::closeAllWindows();
    });

    try
    {
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
        degate::crash_report(e.what());

        return EXIT_FAILURE;
    }

    return ret;
}
