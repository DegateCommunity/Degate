/* -*-c++-*-

  This file is part of the IC reverse engineering tool degate.

  Copyright 2008, 2009, 2010 by Martin Schobert

  Degate is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  any later version.

  Degate is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with degate. If not, see <http://www.gnu.org/licenses/>.

*/

#include "MainWindow.h"
#include <QDesktopWidget>

namespace degate
{
	
	MainWindow::MainWindow(int width, int height)
	{
		if(width == 0 || height == 0)
			resize(QDesktopWidget().availableGeometry(this).size() * 0.6);
		else
			resize(width, height);

		setWindowIcon(QIcon("res/degate_logo.png"));

		setMenuBar(&menu_bar);

		QMenu* file_menu = menu_bar.addMenu("File");
		QAction* file_import_project_action = file_menu->addAction("Import project");
		QObject::connect(file_import_project_action, SIGNAL(triggered()), this, SLOT(on_menu_project_importer()));

		QMenu* about_menu = menu_bar.addMenu("About");
		QAction* about_action = about_menu->addAction("Degate");
		QObject::connect(about_action, SIGNAL(triggered()), this, SLOT(on_menu_about_degate()));

		workspace = new WorkspaceRenderer(this);
		setCentralWidget(workspace);
	}

	MainWindow::~MainWindow()
	{
		if(workspace != NULL)
			delete workspace;
	}

	void MainWindow::on_menu_about_degate()
	{
		const QString about_message = "<html><center>"
									  "<img src='res/degate_logo.png' alt='' width='100' height='87'> <br>"
									  "<strong> This is a forked version of Degate </strong> <br>"
									  "Degate is originally developed by Martin Schobert (martin@weltregierung.de). <br>"
									  "This software is released under the GNU General Public License Version 3. <br>"
									  "<a href='https://github.com/DorianBDev/degate'>Github</a> <br>"
									  "<a href='http://www.degate.org/'>Original website</a>"
									  "</center></html>";

		QMessageBox about("About degate",
						  about_message,
						  QMessageBox::Icon::NoIcon,
						  QMessageBox::Button::Ok,
						  QMessageBox::Button::NoButton,
						  QMessageBox::Button::NoButton,
						  this
						  );

		about.exec();
	}

	void MainWindow::on_menu_project_importer()
	{
		QString dir = QFileDialog::getExistingDirectory(this, "Import project");

		ProjectImporter projectImporter;
		project = projectImporter.import(dir.toStdString());
		QString project_name = QString::fromStdString(project->get_name());

		setWindowTitle("Degate : " + project_name + " project");

		workspace->set_project(project);

		QString message = "The project <i>" + project_name + "</i> was successfully loaded.";
		QMessageBox::information(this, "Import project", message);
	}
}
