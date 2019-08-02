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
		if (width == 0 || height == 0)
			resize(QDesktopWidget().availableGeometry(this).size() * 0.7);
		else
			resize(width, height);

		setWindowIcon(QIcon("res/degate_logo.png"));


		// Menu bar

		setMenuBar(&menu_bar);

		QMenu* file_menu = menu_bar.addMenu("File");
		QAction* file_import_project_action = file_menu->addAction("Import project");
		QObject::connect(file_import_project_action, SIGNAL(triggered()), this, SLOT(on_menu_project_importer()));

		QMenu* about_menu = menu_bar.addMenu("About");
		QAction* about_action = about_menu->addAction("Degate");
		QObject::connect(about_action, SIGNAL(triggered()), this, SLOT(on_menu_about_degate()));


		// Tool bar

		tool_bar = addToolBar("");
		tool_bar->setIconSize(QSize(30, 30));
		tool_bar->setMovable(false);
		tool_bar->setFloatable(false);

		QAction* tool_via_up_action = tool_bar->addAction(style()->standardIcon(QStyle::SP_ArrowUp), "Via up");
		QObject::connect(tool_via_up_action, SIGNAL(triggered()), this, SLOT(on_tool_via_up()));

		QAction* tool_via_down_action = tool_bar->addAction(style()->standardIcon(QStyle::SP_ArrowDown), "Via down");
		QObject::connect(tool_via_down_action, SIGNAL(triggered()), this, SLOT(on_tool_via_down()));


		// Workspace

		workspace = new WorkspaceRenderer(this);
		setCentralWidget(workspace);
	}

	MainWindow::~MainWindow()
	{
		if (workspace != NULL)
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

	void MainWindow::on_tool_via_up()
	{
		if(project == NULL)
			return;

		if(project->get_logic_model()->get_current_layer()->get_layer_pos() <= 0)
			return;

		project->get_logic_model()->set_current_layer(project->get_logic_model()->get_current_layer()->get_layer_pos() - 1);

		workspace->update_screen();
	}

	void MainWindow::on_tool_via_down()
	{
		if(project == NULL)
			return;

		if(project->get_logic_model()->get_current_layer()->get_layer_pos() >= project->get_logic_model()->get_num_layers() - 1)
			return;

		project->get_logic_model()->set_current_layer(project->get_logic_model()->get_current_layer()->get_layer_pos() + 1);

		workspace->update_screen();
	}
}
