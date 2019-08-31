#include <QApplication>

#include "MainWindow.h"

int main(int argc, char* argv[])
{
	QApplication a(argc, argv);
	
	degate::MainWindow window;
	window.show();

	return a.exec();
}
