#include <QApplication>
#include <QSplashScreen>
#include <QTranslator>
#include <QDateTime>

#include <fstream>

#include "GUI/Preferences/PreferencesHandler.h"
#include "GUI/MainWindow.h"

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
