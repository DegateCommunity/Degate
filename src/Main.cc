#include <QApplication>
#include <QSplashScreen>

#include "GUI/MainWindow.h"

int main(int argc, char* argv[])
{
	QApplication a(argc, argv);

	QSplashScreen splash(QPixmap("res/degate_splash.png"));
    splash.show();
	a.processEvents();
	
	degate::MainWindow window;
	window.show();

	splash.finish(&window);

	return a.exec();
}
