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

#define SECOND(a) a * 1000

namespace degate
{
	MainWindow::MainWindow(int width, int height) : status_bar(this)
	{
		if (width == 0 || height == 0)
			resize(QDesktopWidget().availableGeometry(this).size() * 0.7);
		else
			resize(width, height);

		setWindowIcon(QIcon("res/degate_logo.png"));


		// Menu bar

		setMenuBar(&menu_bar);

		QMenu* project_menu = menu_bar.addMenu("Project");
		QAction* project_new_action = project_menu->addAction("New");
		project_new_action->setIcon(style()->standardIcon(QStyle::SP_FileDialogNewFolder));
		QObject::connect(project_new_action, SIGNAL(triggered()), this, SLOT(on_menu_project_new()));
		QAction* project_import_action = project_menu->addAction("Open");
		project_import_action->setIcon(style()->standardIcon(QStyle::SP_DirOpenIcon));
		QObject::connect(project_import_action, SIGNAL(triggered()), this, SLOT(on_menu_project_importer()));
		QAction* project_export_action = project_menu->addAction("Save");
		project_export_action->setIcon(style()->standardIcon(QStyle::SP_DriveFDIcon));
		QObject::connect(project_export_action, SIGNAL(triggered()), this, SLOT(on_menu_project_exporter()));
		QAction* project_close_action = project_menu->addAction("Close");
		project_close_action->setIcon(style()->standardIcon(QStyle::SP_DialogCloseButton));
		QObject::connect(project_close_action, SIGNAL(triggered()), this, SLOT(on_menu_project_close()));
		project_menu->addSeparator();
		QAction* project_quit_action = project_menu->addAction("Quit");
		project_quit_action->setIcon(style()->standardIcon(QStyle::SP_DockWidgetCloseButton));
		QObject::connect(project_quit_action, SIGNAL(triggered()), this, SLOT(on_menu_quit()));

		QMenu* layer_menu = menu_bar.addMenu("Layer");
		QAction* layers_edit_action = layer_menu->addAction("Edit layers");
		QObject::connect(layers_edit_action, SIGNAL(triggered()), this, SLOT(on_menu_layer_edit()));
		QAction* background_import_action = layer_menu->addAction("Import background image");
		QObject::connect(background_import_action, SIGNAL(triggered()), this, SLOT(on_menu_layer_import_background()));

		QMenu* gate_menu = menu_bar.addMenu("Gate");
		QAction* edit_gate_action = gate_menu->addAction("Edit selected");
		QObject::connect(edit_gate_action, SIGNAL(triggered()), this, SLOT(on_menu_gate_edit()));
		QAction* new_gate_action = gate_menu->addAction("Create from selection");
		QObject::connect(new_gate_action, SIGNAL(triggered()), this, SLOT(on_menu_gate_new_gate()));

		QMenu* about_menu = menu_bar.addMenu("About");
		QAction* about_action = about_menu->addAction("Degate");
		about_action->setIcon(style()->standardIcon(QStyle::SP_MessageBoxQuestion));
		QObject::connect(about_action, SIGNAL(triggered()), this, SLOT(on_menu_about_degate()));


		// Workspace

		workspace = new WorkspaceRenderer(this);
		setCentralWidget(workspace);


		// Status bar

		setStatusBar(&status_bar);
		status_bar.showMessage("Initialization...", SECOND(DEFAULT_STATUS_MESSAGE_DURATION));


		// Tool bar

		tool_bar = addToolBar("");
		tool_bar->setIconSize(QSize(20, 20));
		tool_bar->setMovable(false);
		tool_bar->setFloatable(false);
		setContextMenuPolicy(Qt::NoContextMenu);

		QAction* tool_via_up_action = tool_bar->addAction(QIcon("res/icon_up.png"), "Layer up");
		QObject::connect(tool_via_up_action, SIGNAL(triggered()), this, SLOT(on_tool_via_up()));

		QAction* tool_via_down_action = tool_bar->addAction(QIcon("res/icon_down.png"), "Layer down");
		QObject::connect(tool_via_down_action, SIGNAL(triggered()), this, SLOT(on_tool_via_down()));

		tool_bar->addSeparator();

		QAction* tool_zoom_in_action = tool_bar->addAction(QIcon("res/icon_plus.png"), "Zoom in");
		QObject::connect(tool_zoom_in_action, SIGNAL(triggered()), workspace, SLOT(zoom_in()));

		QAction* tool_zoom_out_action = tool_bar->addAction(QIcon("res/icon_minus.png"), "Zoom out");
		QObject::connect(tool_zoom_out_action, SIGNAL(triggered()), workspace, SLOT(zoom_out()));


		// Other
		QObject::connect(workspace, SIGNAL(project_changed(std::string)), this, SLOT(open_project(std::string)));
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

		setEnabled(false);

		if(project != NULL)
			project.reset();

		open_project(dir.toStdString());
		
		QString project_name = QString::fromStdString(project->get_name());

		setWindowTitle("Degate : " + project_name + " project");

		setEnabled(true);

		QString message = "The project <i>" + project_name + "</i> was successfully loaded.";
		QMessageBox::information(this, "Import project", message);
	}

	void MainWindow::on_menu_project_exporter()
	{
		if(project == NULL)
			return;

		status_bar.showMessage("Save project...", SECOND(DEFAULT_STATUS_MESSAGE_DURATION));

		ProjectExporter exporter;
		exporter.export_all(project->get_project_directory(), project);

		status_bar.showMessage("Project saved.", SECOND(DEFAULT_STATUS_MESSAGE_DURATION));
	}

	void MainWindow::on_menu_project_close()
	{
		status_bar.showMessage("Closing project...", SECOND(DEFAULT_STATUS_MESSAGE_DURATION));

		if(project == NULL)
			return;

		if(project->is_changed())
		{
			QMessageBox msgBox;
			msgBox.setText("The project has been modified.");
			msgBox.setInformativeText("Do you want to save your changes ?");
			msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
			msgBox.setDefaultButton(QMessageBox::Save);
			int ret = msgBox.exec();
			
			switch (ret)
			{
			 case QMessageBox::Save:
			      on_menu_project_exporter();
			      break;
			 case QMessageBox::Discard:
			      break;
			 case QMessageBox::Cancel:
			      return;
			      break;
			 default:
			      return;
			      break;
			}
		}

		project.reset();
		project = NULL;
		workspace->set_project(NULL);
		workspace->update_screen();

		status_bar.showMessage("Project closed.", SECOND(DEFAULT_STATUS_MESSAGE_DURATION));
	}

	void MainWindow::on_menu_project_new()
	{
		status_bar.showMessage("Creating a new project...", SECOND(DEFAULT_STATUS_MESSAGE_DURATION));

		QString dir = QFileDialog::getExistingDirectory(this, "Select the directory where the project will be created");
		NewProjectDialog dialog(this);
		dialog.exec();

		std::string project_name = dialog.get_project_name();
		unsigned layer_count = dialog.get_layer_count();
		unsigned width = dialog.get_width();
		unsigned height = dialog.get_height();

		if(layer_count == 0 || width == 0 || height == 0 || project_name.length() < 1)
		{
			QMessageBox::warning(this, "Invalid values", "The values you entered are invalid. Operation cancelled");

			status_bar.showMessage("New project operation cancelled.", SECOND(DEFAULT_STATUS_MESSAGE_DURATION));

			return;
		}
		else
		{
			const std::string project_dir = dir.toStdString() + "/" + project_name;

			if(!file_exists(project_dir)) 
				create_directory(project_dir);

			project = Project_shptr(new Project(width, height, project_dir, layer_count));
			project->set_name(project_name);

			workspace->set_project(project);

			LayersEditDialog layers_edit_dialog(project, this);
			layers_edit_dialog.exec();

			on_menu_project_exporter();
		}

		status_bar.showMessage("Created a new project.", SECOND(DEFAULT_STATUS_MESSAGE_DURATION));
	}

	void MainWindow::on_menu_layer_edit()
	{
		if(project == NULL)
			return;
		
		LayersEditDialog layers_edit_dialog(project, this);
		layers_edit_dialog.exec();

		workspace->update_screen();
	}

	void MainWindow::on_menu_layer_import_background()
	{
		if(project == NULL)
		{
			status_bar.showMessage("Failed to import new background image : no project opened.", SECOND(DEFAULT_STATUS_MESSAGE_DURATION));
			return;
		}

		status_bar.showMessage("Importing a new background image for the layer...", SECOND(DEFAULT_STATUS_MESSAGE_DURATION));

		QString res = QFileDialog::getOpenFileName(this, "Select the new background image");
		const std::string file_name = res.toStdString();

		load_background_image(project->get_logic_model()->get_current_layer(), project->get_project_directory(), file_name);

		workspace->update_screen();

		status_bar.showMessage("Imported a new background image for the layer.", SECOND(DEFAULT_STATUS_MESSAGE_DURATION));
	}

	void MainWindow::on_menu_gate_new_gate()
	{
		if(!workspace->has_selection())
			return;

		//Todo : orientation window here
		//Todo : check if logic layer

		GateTemplate_shptr new_gate_template(new GateTemplate(workspace->get_selection().get_width(), workspace->get_selection().get_height()));
		grab_template_images(project->get_logic_model(), new_gate_template, workspace->get_selection());

		Gate_shptr new_gate(new Gate(workspace->get_selection()));
		new_gate->set_gate_template(new_gate_template);

		GateInstanceEditDialog dialog(this, new_gate, project);
		dialog.exec();

		project->get_logic_model()->update_ports(new_gate);
		project->get_logic_model()->add_gate_template(new_gate_template);
		project->get_logic_model()->add_object(project->get_logic_model()->get_current_layer()->get_layer_pos(), new_gate);

		workspace->update_screen();
	}

	void MainWindow::on_menu_gate_edit()
	{
		if(Gate_shptr o = std::dynamic_pointer_cast<Gate>(workspace->get_selected_object()))
		{
			GateInstanceEditDialog dialog(this, o, project);
			dialog.exec();

			project->get_logic_model()->update_ports(o);
		}

		workspace->update_screen();
	}

	void MainWindow::on_menu_quit()
	{
		close();
	}

	void MainWindow::on_tool_via_up()
	{
		if(project == NULL)
			return;

		project->get_logic_model()->set_current_layer(get_next_enabled_layer(project->get_logic_model())->get_layer_pos());

		workspace->update_screen();
	}

	void MainWindow::on_tool_via_down()
	{
		if(project == NULL)
			return;

		project->get_logic_model()->set_current_layer(get_prev_enabled_layer(project->get_logic_model())->get_layer_pos());

		workspace->update_screen();
	}

	void MainWindow::open_project(std::string path)
	{
		status_bar.showMessage("Import project/subproject...", SECOND(DEFAULT_STATUS_MESSAGE_DURATION));

		ProjectImporter projectImporter;
		project = projectImporter.import_all(path);

		workspace->set_project(project);

		status_bar.showMessage("Project/Subproject imported.", SECOND(DEFAULT_STATUS_MESSAGE_DURATION));
	}
}
