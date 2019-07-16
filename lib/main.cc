#include <QApplication>
#include <QPushButton>
#include <stdio.h>


int main(int argc, char* argv[])
{
	printf("Test");

	QApplication a(argc, argv);

	QPushButton bn("Test");
	bn.show();

	return a.exec();
}
