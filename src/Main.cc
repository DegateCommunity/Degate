#include <QApplication>
#include <QSplashScreen>
#include <QTranslator>

#include "GUI/Preferences/PreferencesHandler.h"
#include "GUI/MainWindow.h"

int main(int argc, char* argv[])
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

	return QApplication::exec();
}
