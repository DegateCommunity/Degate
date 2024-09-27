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

#include "Core/Utils/CrashReport.h"
#include "Core/Version.h"
#include "GUI/MainWindow.h"
#include "GUI/Preferences/PreferencesHandler.h"

#include <QApplication>
#include <QSplashScreen>
#include <QTranslator>
#include <csignal>

/**
 * @class Degate
 * @brief Implementation of QApplication.
 * 
 * Useful to catch exceptions.
 */
class Degate final : public QApplication
{
public:
    /**
     * Constructor, @see QApplication.
     * 
     * Only one QApplication can exist, accessible through QApplication::.
     */
    Degate(int& argc, char** argv) : QApplication(argc, argv)
    {
    }

    /**
     * Reimplement notify() to catch exceptions.
     */
    virtual bool notify(QObject* receiver, QEvent* event) override
    {
        try
        {
            return QApplication::notify(receiver, event);
        }
        catch (const std::exception& e)
        {
            degate::crash_report(e.what());
            exit(EXIT_FAILURE);
        }

        return false;
    }
};

int main(int argc, char* argv[])
{
    int ret = 0;

    // Set OpenGL 3.3 Core Profile as default
    QSurfaceFormat format;
    format.setDepthBufferSize(24);
    format.setStencilBufferSize(8);
    format.setVersion(3, 3);
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setRenderableType(QSurfaceFormat::OpenGL);
    QSurfaceFormat::setDefaultFormat(format);

    // Create Qt application
    Degate app(argc, argv);

    // Bind signals
    std::signal(SIGSEGV, [](int signal) {
        std::string error = "A SIGSEGV occurred (" + std::to_string(signal) + ").";
        degate::crash_report(error);
    });

    std::signal(SIGFPE, [](int signal) {
        std::string error = "A SIGFPE occurred (" + std::to_string(signal) + ").";
        degate::crash_report(error);
    });

    std::signal(SIGILL, [](int signal) {
        std::string error = "A SIGILL occurred (" + std::to_string(signal) + ").";
        degate::crash_report(error);
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
            degate::PreferencesHandler::get_instance().get_settings().value("last_launch_version", "") !=
                    DEGATE_VERSION)
        {
            window.on_menu_help_about();

            degate::PreferencesHandler::get_instance().get_settings().setValue("first_launch", false);
            degate::PreferencesHandler::get_instance().get_settings().setValue("last_launch_version", DEGATE_VERSION);
        }

        ret = QApplication::exec();
    }
    catch (const std::exception& e)
    {
        debug(TM, "Exception occured: %s", e.what());
        degate::crash_report(e.what());
    }

    return ret;
}
