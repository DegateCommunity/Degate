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

		setWindowTitle("Degate");
		setWindowIcon(QIcon("res/degate_logo.png"));

		// Set the actual theme and icon theme from preferences
		THEME_MANAGER.init(PREFERENCES_HANDLER.get_theme(), PREFERENCES_HANDLER.get_icon_theme());


		// Workspace

		workspace = new WorkspaceRenderer(this);
		setCentralWidget(workspace);
		

		// Menu bar

		setMenuBar(&menu_bar);

		QMenu* project_menu = menu_bar.addMenu("Project");
		QAction* project_new_action = project_menu->addAction("New");
		project_new_action->setIcon(QIcon(GET_ICON_PATH("folder_new.png")));
		QObject::connect(project_new_action, SIGNAL(triggered()), this, SLOT(on_menu_project_new()));
		QAction* project_import_action = project_menu->addAction("Open");
		project_import_action->setIcon(QIcon(GET_ICON_PATH("folder.png")));
		QObject::connect(project_import_action, SIGNAL(triggered()), this, SLOT(on_menu_project_importer()));
		QAction* project_export_action = project_menu->addAction("Save");
		project_export_action->setIcon(QIcon(GET_ICON_PATH("save.png")));
        project_export_action->setShortcut(Qt::CTRL + Qt::Key_S);
		QObject::connect(project_export_action, SIGNAL(triggered()), this, SLOT(on_menu_project_exporter()));
		QAction* project_close_action = project_menu->addAction("Close");
		project_close_action->setIcon(QIcon(GET_ICON_PATH("close.png")));
		QObject::connect(project_close_action, SIGNAL(triggered()), this, SLOT(on_menu_project_close()));
		project_menu->addSeparator();
		QAction* project_create_subproject_action = project_menu->addAction("Create subproject from selection");
		QObject::connect(project_create_subproject_action, SIGNAL(triggered()), this, SLOT(on_menu_project_create_subproject()));
		project_menu->addSeparator();
		QAction* project_quit_action = project_menu->addAction("Quit");
		project_quit_action->setIcon(QIcon(GET_ICON_PATH("quit.png")));
		QObject::connect(project_quit_action, SIGNAL(triggered()), this, SLOT(on_menu_project_quit()));

		QMenu* edit_menu = menu_bar.addMenu("Edit");
		QAction* preferences_edit_action = edit_menu->addAction("Preferences");
		QObject::connect(preferences_edit_action, SIGNAL(triggered()), this, SLOT(on_menu_edit_preferences()));

		QMenu* view_menu = menu_bar.addMenu("View");
		QAction* show_gates_view_action = view_menu->addAction("Show gates");
		show_gates_view_action->setCheckable(true);
		show_gates_view_action->setChecked(true);
		QObject::connect(show_gates_view_action, SIGNAL(toggled(bool)), workspace, SLOT(show_gates(bool)));
		QAction* show_gates_name_view_action = view_menu->addAction("Show gates name");
		show_gates_name_view_action->setCheckable(true);
		show_gates_name_view_action->setChecked(true);
		QObject::connect(show_gates_name_view_action, SIGNAL(toggled(bool)), workspace, SLOT(show_gates_name(bool)));
		QAction* show_ports_view_action = view_menu->addAction("Show ports");
		show_ports_view_action->setCheckable(true);
		show_ports_view_action->setChecked(true);
		QObject::connect(show_ports_view_action, SIGNAL(toggled(bool)), workspace, SLOT(show_ports(bool)));
		QAction* show_ports_name_view_action = view_menu->addAction("Show ports name");
		show_ports_name_view_action->setCheckable(true);
		show_ports_name_view_action->setChecked(true);
		QObject::connect(show_ports_name_view_action, SIGNAL(toggled(bool)), workspace, SLOT(show_ports_name(bool)));
		QAction* show_annotations_view_action = view_menu->addAction("Show annotations");
		show_annotations_view_action->setCheckable(true);
		show_annotations_view_action->setChecked(true);
		QObject::connect(show_annotations_view_action, SIGNAL(toggled(bool)), workspace, SLOT(show_annotations(bool)));
		QAction* show_annotations_name_view_action = view_menu->addAction("Show annotations name");
		show_annotations_name_view_action->setCheckable(true);
		show_annotations_name_view_action->setChecked(true);
		QObject::connect(show_annotations_name_view_action, SIGNAL(toggled(bool)), workspace, SLOT(show_annotations_name(bool)));

		QMenu* layer_menu = menu_bar.addMenu("Layer");
		QAction* layers_edit_action = layer_menu->addAction(QIcon(GET_ICON_PATH("edit.png")), "Edit layers");
		QObject::connect(layers_edit_action, SIGNAL(triggered()), this, SLOT(on_menu_layer_edit()));
		QAction* background_import_action = layer_menu->addAction("Import background image");
		QObject::connect(background_import_action, SIGNAL(triggered()), this, SLOT(on_menu_layer_import_background()));

		QMenu* gate_menu = menu_bar.addMenu("Gate");
		QAction* edit_gate_action = gate_menu->addAction(QIcon(GET_ICON_PATH("edit.png")), "Edit selected");
		QObject::connect(edit_gate_action, SIGNAL(triggered()), this, SLOT(on_menu_gate_edit()));
		QAction* new_gate_template_action = gate_menu->addAction("Create gate template from selection");
		QObject::connect(new_gate_template_action, SIGNAL(triggered()), this, SLOT(on_menu_gate_new_gate_template()));
		QAction* new_gate_action = gate_menu->addAction("Create gate from selection");
		QObject::connect(new_gate_action, SIGNAL(triggered()), this, SLOT(on_menu_gate_new_gate()));
		gate_menu->addSeparator();
		QAction* gate_library_action = gate_menu->addAction(QIcon(GET_ICON_PATH("book.png")), "Gate library");
		QObject::connect(gate_library_action, SIGNAL(triggered()), this, SLOT(on_menu_gate_library()));

		QMenu* annotation_menu = menu_bar.addMenu("Annotation");
		QAction* edit_annotation_action = annotation_menu->addAction(QIcon(GET_ICON_PATH("edit.png")), "Edit selected");
		QObject::connect(edit_annotation_action, SIGNAL(triggered()), this, SLOT(on_menu_annotation_edit()));
		QAction* create_annotation_action = annotation_menu->addAction("Create from selection");
		QObject::connect(create_annotation_action, SIGNAL(triggered()), this, SLOT(on_menu_annotation_create()));

		QMenu* logic_menu = menu_bar.addMenu("Logic");
		QAction* remove_object_action = logic_menu->addAction(QIcon(GET_ICON_PATH("remove.png")), "Remove selected object");
		QObject::connect(remove_object_action, SIGNAL(triggered()), this, SLOT(on_menu_logic_remove_selected_object()));

		QMenu* help_menu = menu_bar.addMenu("Help");
		QAction* about_action = help_menu->addAction("About");
		about_action->setIcon(style()->standardIcon(QStyle::SP_MessageBoxQuestion));
		QObject::connect(about_action, SIGNAL(triggered()), this, SLOT(on_menu_help_about()));


		// Status bar

		status_bar.setStyleSheet("QStatusBar::item { border: none; } ""QStatusBar QLabel { border: 1px solid black; border-radius: 3px; }");
		setStatusBar(&status_bar);
		status_bar.showMessage("Initialization...", SECOND(DEFAULT_STATUS_MESSAGE_DURATION * 2));

		status_bar.addPermanentWidget(&status_bar_layer);
		status_bar_layer.setText("Layer : 0/0 (none)");
		
		status_bar.addPermanentWidget(&status_bar_coords);
		status_bar_coords.setText("Coordinates : 0,0");
		QObject::connect(workspace, SIGNAL(mouse_coords_changed(int, int)), this, SLOT(change_status_bar_coords(int, int)));


		// Tool bar

		tool_bar = addToolBar("");
		tool_bar->setIconSize(QSize(25, 25));
		tool_bar->setMovable(false);
		tool_bar->setFloatable(false);
		setContextMenuPolicy(Qt::NoContextMenu);

		QAction* tool_via_up_action = tool_bar->addAction(QIcon(GET_ICON_PATH("layer_up.png")), "Layer up");
		QObject::connect(tool_via_up_action, SIGNAL(triggered()), this, SLOT(on_tool_via_up()));

		QAction* tool_via_down_action = tool_bar->addAction(QIcon(GET_ICON_PATH("layer_down.png")), "Layer down");
		QObject::connect(tool_via_down_action, SIGNAL(triggered()), this, SLOT(on_tool_via_down()));

		tool_bar->addSeparator();

		QAction* tool_zoom_in_action = tool_bar->addAction(QIcon(GET_ICON_PATH("plus.png")), "Zoom in");
		QObject::connect(tool_zoom_in_action, SIGNAL(triggered()), workspace, SLOT(zoom_in()));

		QAction* tool_zoom_out_action = tool_bar->addAction(QIcon(GET_ICON_PATH("minus.png")), "Zoom out");
		QObject::connect(tool_zoom_out_action, SIGNAL(triggered()), workspace, SLOT(zoom_out()));

		tool_bar->addSeparator();

		QAction* tool_gate_library = tool_bar->addAction(QIcon(GET_ICON_PATH("book.png")), "Gate library");
		QObject::connect(tool_gate_library, SIGNAL(triggered()), this, SLOT(on_menu_gate_library()));


		// Other
		QObject::connect(workspace, SIGNAL(project_changed(std::string)), this, SLOT(open_project(std::string)));
        QObject::connect(workspace, SIGNAL(right_mouse_button_released()), this, SLOT(show_context_menu()));
	}

	MainWindow::~MainWindow()
	{
		if (workspace != NULL)
			delete workspace;
	}

	void MainWindow::on_menu_help_about()
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
        status_bar.showMessage("Opening project...", SECOND(DEFAULT_STATUS_MESSAGE_DURATION));

		QString dir = QFileDialog::getExistingDirectory(this, "Import project");

		if(dir.isNull())
        {
            status_bar.showMessage("Project opening operation cancelled.", SECOND(DEFAULT_STATUS_MESSAGE_DURATION));

            return;
        }

		setEnabled(false);

		if(project != NULL)
			project.reset();

		try
		{
			open_project(dir.toStdString());
		}
		catch (const std::exception& ex)
		{
			setEnabled(true);
			
			return;
		}
		
		QString project_name = QString::fromStdString(project->get_name());

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

		setWindowTitle("Degate");

		status_bar_layer.setText("Layer : 0/0 (none)");
		
		status_bar.showMessage("Project closed.", SECOND(DEFAULT_STATUS_MESSAGE_DURATION));
	}

	void MainWindow::on_menu_project_new()
	{
		status_bar.showMessage("Creating a new project...", SECOND(DEFAULT_STATUS_MESSAGE_DURATION));

		QString dir = QFileDialog::getExistingDirectory(this, "Select the directory where the project will be created");

        if(dir.isNull())
        {
            status_bar.showMessage("Project creation operation cancelled.", SECOND(DEFAULT_STATUS_MESSAGE_DURATION));

            return;
        }

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

	void MainWindow::on_menu_project_create_subproject()
	{
		if(project == NULL || !workspace->has_area_selection())
			return;

		boost::format f("subproject_%1%");
		f % project->get_logic_model()->get_new_object_id();

		SubProjectAnnotation_shptr new_annotation(new SubProjectAnnotation(workspace->get_area_selection(), f.str()));
		new_annotation->set_fill_color(project->get_default_color(DEFAULT_COLOR_ANNOTATION));
		new_annotation->set_frame_color(project->get_default_color(DEFAULT_COLOR_ANNOTATION_FRAME));

		{
			AnnotationEditDialog dialog(new_annotation, this);
			dialog.exec();
		}

		project->get_logic_model()->add_object(project->get_logic_model()->get_current_layer()->get_layer_pos(), new_annotation);

		workspace->reset_area_selection();
		workspace->update_screen();
	}

	void MainWindow::on_menu_edit_preferences()
	{
		PreferencesDialog dialog(this);
		dialog.exec();
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

	void MainWindow::on_menu_gate_new_gate_template()
	{
		if(project == NULL || !workspace->has_area_selection())
			return;

		if(project->get_logic_model()->get_current_layer()->get_layer_type() != Layer::LOGIC)
		{
			QMessageBox::warning(this, "Warning", "You can create a new gate only on a logic layer.");
			return;
		}

		GateTemplate_shptr new_gate_template(new GateTemplate(workspace->get_area_selection().get_width(), workspace->get_area_selection().get_height()));
		grab_template_images(project->get_logic_model(), new_gate_template, workspace->get_area_selection());
		new_gate_template->set_object_id(project->get_logic_model()->get_new_object_id());

		Gate_shptr new_gate(new Gate(workspace->get_area_selection()));
		new_gate->set_gate_template(new_gate_template);
		new_gate->set_fill_color(project->get_default_color(DEFAULT_COLOR_GATE));
		new_gate->set_frame_color(project->get_default_color(DEFAULT_COLOR_GATE_FRAME));
		new_gate->set_object_id(project->get_logic_model()->get_new_object_id());

		project->get_logic_model()->add_gate_template(new_gate_template);

		{
			GateInstanceEditDialog dialog(this, new_gate, project);
			dialog.exec();
		}

		project->get_logic_model()->add_object(project->get_logic_model()->get_current_layer()->get_layer_pos(), new_gate);
		project->get_logic_model()->update_ports(new_gate);

		workspace->reset_area_selection();
		workspace->update_screen();
	}

	void MainWindow::on_menu_gate_new_gate()
	{
		if(project == NULL || !workspace->has_area_selection())
			return;

		if(project->get_logic_model()->get_current_layer()->get_layer_type() != Layer::LOGIC)
		{
			QMessageBox::warning(this, "Warning", "You can create a new gate only on a logic layer.");
			return;
		}

		SelectGateTemplateDialog select_dialog(project, this);
		select_dialog.exec();

		GateTemplate_shptr gate_template = select_dialog.get_selected_gate();

		Gate_shptr new_gate(new Gate(workspace->get_area_selection()));
		new_gate->set_gate_template(gate_template);
		
		{
			GateInstanceEditDialog dialog(this, new_gate, project);
			dialog.exec();
		}

		project->get_logic_model()->add_object(project->get_logic_model()->get_current_layer()->get_layer_pos(), new_gate);
		project->get_logic_model()->update_ports(new_gate);
		
		workspace->reset_area_selection();
		workspace->update_screen();
	}

	void MainWindow::on_menu_gate_edit()
	{
		if(project == NULL || !workspace->has_selection())
			return;
		
		if(Gate_shptr o = std::dynamic_pointer_cast<Gate>(workspace->get_selected_object()))
		{
			GateInstanceEditDialog dialog(this, o, project);
			dialog.exec();

			project->get_logic_model()->update_ports(o);
		}

		workspace->update_screen();
	}

	void MainWindow::on_menu_gate_library()
	{
		if(project == NULL)
			return;
		
		GateLibraryDialog dialog(project, this);
		dialog.exec();

		workspace->update_screen();
	}

    void MainWindow::on_menu_gate_port_edit()
    {
        if(project == NULL || !workspace->has_selection())
            return;

        if(GatePort_shptr o = std::dynamic_pointer_cast<GatePort>(workspace->get_selected_object()))
        {
            {
                PortPlacementDialog dialog(this, project, o->get_gate()->get_gate_template(), o->get_template_port());
                dialog.exec();
            }

            project->get_logic_model()->update_ports(o->get_gate());
        }

        workspace->update_screen();
    }

	void MainWindow::on_menu_annotation_create()
	{
		if(project == NULL || !workspace->has_area_selection())
			return;

		Annotation_shptr new_annotation(new Annotation(workspace->get_area_selection()));
		new_annotation->set_fill_color(project->get_default_color(DEFAULT_COLOR_ANNOTATION));
		new_annotation->set_frame_color(project->get_default_color(DEFAULT_COLOR_ANNOTATION_FRAME));

		{
			AnnotationEditDialog dialog(new_annotation, this);
			dialog.exec();
		}

		project->get_logic_model()->add_object(project->get_logic_model()->get_current_layer()->get_layer_pos(), new_annotation);

		workspace->reset_area_selection();
		workspace->update_screen();
	}

	void MainWindow::on_menu_annotation_edit()
	{
		if(project == NULL || !workspace->has_selection())
			return;

		if(Annotation_shptr o = std::dynamic_pointer_cast<Annotation>(workspace->get_selected_object()))
		{
			AnnotationEditDialog dialog(o, this);
			dialog.exec();

			workspace->update_screen();
		}
	}

	void MainWindow::on_menu_logic_remove_selected_object()
	{
		if(project == NULL || !workspace->has_selection())
			return;

		project->get_logic_model()->remove_object(workspace->pop_selected_object());
		workspace->update_screen();
	}

	void MainWindow::on_menu_project_quit()
	{
		close();
	}

	void MainWindow::on_tool_via_up()
	{
		if(project == NULL)
			return;

		project->get_logic_model()->set_current_layer(get_next_enabled_layer(project->get_logic_model())->get_layer_pos());

		status_bar_layer.setText("Layer : " + QString::number(project->get_logic_model()->get_current_layer()->get_layer_pos() + 1) + "/" + QString::number(project->get_logic_model()->get_num_layers()) + " (" + QString::fromStdString(project->get_logic_model()->get_current_layer()->get_layer_type_as_string()) + ")");

		workspace->update_screen();
	}

	void MainWindow::on_tool_via_down()
	{
		if(project == NULL)
			return;

		project->get_logic_model()->set_current_layer(get_prev_enabled_layer(project->get_logic_model())->get_layer_pos());

		status_bar_layer.setText("Layer : " + QString::number(project->get_logic_model()->get_current_layer()->get_layer_pos() + 1) + "/" + QString::number(project->get_logic_model()->get_num_layers()) + " (" + QString::fromStdString(project->get_logic_model()->get_current_layer()->get_layer_type_as_string()) + ")");

		workspace->update_screen();
	}

	void MainWindow::open_project(std::string path)
	{
		status_bar.showMessage("Import project/subproject...", SECOND(DEFAULT_STATUS_MESSAGE_DURATION));

		ProjectImporter projectImporter;

		if(project != NULL)
			on_menu_project_exporter();
		
		try
		{
			project = projectImporter.import_all(path);
		}
		catch (const std::exception& ex)
		{
			QMessageBox::StandardButton reply;
			reply = QMessageBox::question(this, "Project/Subproject", "The project do not exist, do you want to create it ?", QMessageBox::Yes | QMessageBox::No);

			if(reply == QMessageBox::Yes)
			{
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
					if(!file_exists(path)) 
						create_directory(path);

					project = Project_shptr(new Project(width, height, path, layer_count));
					project->set_name(project_name);

					LayersEditDialog layers_edit_dialog(project, this);
					layers_edit_dialog.exec();

					on_menu_project_exporter();
				}
			}
			else
			{
				std::cout << "Exception caught: " << ex.what() << std::endl;
				status_bar.showMessage("Project/Subproject import failed.", SECOND(DEFAULT_STATUS_MESSAGE_DURATION));
				project = NULL;
				setWindowTitle("Degate");

				throw ex;
			}
		}

		workspace->set_project(project);

		setWindowTitle("Degate : " + QString::fromStdString(project->get_name()) + " project");

		status_bar_layer.setText("Layer : " + QString::number(project->get_logic_model()->get_current_layer()->get_layer_pos() + 1) + "/" + QString::number(project->get_logic_model()->get_num_layers()) + " (" + QString::fromStdString(project->get_logic_model()->get_current_layer()->get_layer_type_as_string()) + ")");
		
		status_bar.showMessage("Project/Subproject imported.", SECOND(DEFAULT_STATUS_MESSAGE_DURATION));
	}

	void MainWindow::change_status_bar_coords(int x, int y)
	{
		status_bar_coords.setText("Coordinates : " + QString::number(x) + "," + QString::number(y));
	}

    void MainWindow::show_context_menu()
    {
        QMenu contextMenu(("Context menu"), this);

        // New
        QAction annotation_create_action("Create new annotation", this);
        QAction gate_template_create_action("Create new gate template", this);
        QAction gate_create_action("Create new gate", this);

        // Edit
        QAction annotation_edit_action("Edit selected annotation", this);
        QAction gate_edit_action("Edit selected gate", this);
        QAction gate_port_edit_action("Move selected port", this);

        // Delete
        QAction delete_action("Remove selected object", this);

        // Reset area
        QAction reset_selection_area_action("Reset selection area", this);

        if(workspace->has_area_selection())
        {
            connect(&annotation_create_action, SIGNAL(triggered()), this, SLOT(on_menu_annotation_create()));
            contextMenu.addAction(&annotation_create_action);

            connect(&gate_template_create_action, SIGNAL(triggered()), this, SLOT(on_menu_gate_new_gate_template()));
            contextMenu.addAction(&gate_template_create_action);

            connect(&gate_create_action, SIGNAL(triggered()), this, SLOT(on_menu_gate_new_gate()));
            contextMenu.addAction(&gate_create_action);

            contextMenu.addSeparator();

            connect(&reset_selection_area_action, SIGNAL(triggered()), workspace, SLOT(reset_area_selection()));
            contextMenu.addAction(&reset_selection_area_action);
        }
        else if(workspace->has_selection())
        {
            PlacedLogicModelObject_shptr object = workspace->get_selected_object();

            if(Annotation_shptr o = std::dynamic_pointer_cast<Annotation>(object))
            {
                connect(&annotation_edit_action, SIGNAL(triggered()), this, SLOT(on_menu_annotation_edit()));
                contextMenu.addAction(&annotation_edit_action);
            }
            else if (Gate_shptr o = std::dynamic_pointer_cast<Gate>(object))
            {
                connect(&gate_edit_action, SIGNAL(triggered()), this, SLOT(on_menu_gate_edit()));
                contextMenu.addAction(&gate_edit_action);
            }
            else if (GatePort_shptr o = std::dynamic_pointer_cast<GatePort>(object))
            {
                connect(&gate_port_edit_action, SIGNAL(triggered()), this, SLOT(on_menu_gate_port_edit()));
                contextMenu.addAction(&gate_port_edit_action);
            }

            connect(&delete_action, SIGNAL(triggered()), this, SLOT(on_menu_logic_remove_selected_object()));
            contextMenu.addAction(&delete_action);
        }
        else
            return;

        contextMenu.exec(QCursor::pos());
    }
}
