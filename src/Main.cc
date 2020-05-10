#include <QApplication>
#include <QSplashScreen>

#include "GUI/MainWindow.h"

int main(int argc, char* argv[])
{
	QApplication a(argc, argv);

    QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, QApplication::applicationDirPath());

	QSplashScreen splash(QPixmap(":/degate_splash.png"));
    splash.show();
	QApplication::processEvents();
	
	degate::MainWindow window;
	window.show();

	splash.finish(&window);

	return QApplication::exec();
}
