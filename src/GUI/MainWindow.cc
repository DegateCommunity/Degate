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

#include <GUI/MainWindow.h>
#include <memory>

#define SECOND(a) a * 1000

namespace degate
{
	MainWindow::MainWindow(int width, int height) : status_bar(this), tools_group(this)
	{
		if (width == 0 || height == 0)
			resize(QDesktopWidget().availableGeometry(this).size() * 0.7);
		else
			resize(width, height);

		setWindowTitle("Degate");
		setWindowIcon(QIcon(":/degate_logo.png"));


		// Workspace

		workspace = new WorkspaceRenderer(this);
		setCentralWidget(workspace);


		// Menu bar

		setMenuBar(&menu_bar);

		// Project menu
		QMenu* project_menu = menu_bar.addMenu("Project");

		project_new_action = project_menu->addAction("New");
		QObject::connect(project_new_action, SIGNAL(triggered()), this, SLOT(on_menu_project_new()));

		project_import_action = project_menu->addAction("Open");
		QObject::connect(project_import_action, SIGNAL(triggered()), this, SLOT(on_menu_project_importer()));

		project_export_action = project_menu->addAction("Save");
        project_export_action->setShortcut(Qt::CTRL + Qt::Key_S);
		QObject::connect(project_export_action, SIGNAL(triggered()), this, SLOT(on_menu_project_exporter()));

		project_close_action = project_menu->addAction("Close");
		QObject::connect(project_close_action, SIGNAL(triggered()), this, SLOT(on_menu_project_close()));

		project_menu->addSeparator();
		project_create_subproject_action = project_menu->addAction("Create subproject from selection");
		QObject::connect(project_create_subproject_action, SIGNAL(triggered()), this, SLOT(on_menu_project_create_subproject()));

		project_menu->addSeparator();
        project_settings_action = project_menu->addAction("Project settings");
        QObject::connect(project_settings_action, SIGNAL(triggered()), this, SLOT(on_menu_project_settings()));

		project_menu->addSeparator();
		project_quit_action = project_menu->addAction("Quit");
		QObject::connect(project_quit_action, SIGNAL(triggered()), this, SLOT(on_menu_project_quit()));


		// Edit menu
		QMenu* edit_menu = menu_bar.addMenu("Edit");
		preferences_edit_action = edit_menu->addAction("Preferences");
		QObject::connect(preferences_edit_action, SIGNAL(triggered()), this, SLOT(on_menu_edit_preferences()));


		// View menu
		QMenu* view_menu = menu_bar.addMenu("View");

		show_gates_view_action = view_menu->addAction("Show gates");
		show_gates_view_action->setCheckable(true);
		show_gates_view_action->setChecked(true);
		QObject::connect(show_gates_view_action, SIGNAL(toggled(bool)), workspace, SLOT(show_gates(bool)));

		show_gates_name_view_action = view_menu->addAction("Show gates name");
		show_gates_name_view_action->setCheckable(true);
		show_gates_name_view_action->setChecked(true);
		QObject::connect(show_gates_name_view_action, SIGNAL(toggled(bool)), workspace, SLOT(show_gates_name(bool)));

		show_ports_view_action = view_menu->addAction("Show ports");
		show_ports_view_action->setCheckable(true);
		show_ports_view_action->setChecked(true);
		QObject::connect(show_ports_view_action, SIGNAL(toggled(bool)), workspace, SLOT(show_ports(bool)));

		show_ports_name_view_action = view_menu->addAction("Show ports name");
		show_ports_name_view_action->setCheckable(true);
		show_ports_name_view_action->setChecked(true);
		QObject::connect(show_ports_name_view_action, SIGNAL(toggled(bool)), workspace, SLOT(show_ports_name(bool)));

		show_annotations_view_action = view_menu->addAction("Show annotations");
		show_annotations_view_action->setCheckable(true);
		show_annotations_view_action->setChecked(true);
		QObject::connect(show_annotations_view_action, SIGNAL(toggled(bool)), workspace, SLOT(show_annotations(bool)));

		show_annotations_name_view_action = view_menu->addAction("Show annotations name");
		show_annotations_name_view_action->setCheckable(true);
		show_annotations_name_view_action->setChecked(true);
		QObject::connect(show_annotations_name_view_action, SIGNAL(toggled(bool)), workspace, SLOT(show_annotations_name(bool)));

        show_emarkers_view_action = view_menu->addAction("Show emarkers");
        show_emarkers_view_action->setCheckable(true);
        show_emarkers_view_action->setChecked(true);
        QObject::connect(show_emarkers_view_action, SIGNAL(toggled(bool)), workspace, SLOT(show_emarkers(bool)));

        show_emarkers_name_view_action = view_menu->addAction("Show emarkers name");
        show_emarkers_name_view_action->setCheckable(true);
        show_emarkers_name_view_action->setChecked(true);
        QObject::connect(show_emarkers_name_view_action, SIGNAL(toggled(bool)), workspace, SLOT(show_emarkers_name(bool)));

        show_vias_view_action = view_menu->addAction("Show vias");
        show_vias_view_action->setCheckable(true);
        show_vias_view_action->setChecked(true);
        QObject::connect(show_vias_view_action, SIGNAL(toggled(bool)), workspace, SLOT(show_vias(bool)));

        show_vias_name_view_action = view_menu->addAction("Show vias name");
        show_vias_name_view_action->setCheckable(true);
        show_vias_name_view_action->setChecked(true);
        QObject::connect(show_vias_name_view_action, SIGNAL(toggled(bool)), workspace, SLOT(show_vias_name(bool)));

        show_wires_view_action = view_menu->addAction("Show wires");
        show_wires_view_action->setCheckable(true);
        show_wires_view_action->setChecked(true);
        QObject::connect(show_wires_view_action, SIGNAL(toggled(bool)), workspace, SLOT(show_wires(bool)));


        // Layer menu
		QMenu* layer_menu = menu_bar.addMenu("Layer");

		layers_edit_action = layer_menu->addAction("Edit layers");
		QObject::connect(layers_edit_action, SIGNAL(triggered()), this, SLOT(on_menu_layer_edit()));

		background_import_action = layer_menu->addAction("Import background image");
		QObject::connect(background_import_action, SIGNAL(triggered()), this, SLOT(on_menu_layer_import_background()));


		// Gate menu
		QMenu* gate_menu = menu_bar.addMenu("Gate");

		edit_gate_action = gate_menu->addAction("Edit selected");
		QObject::connect(edit_gate_action, SIGNAL(triggered()), this, SLOT(on_menu_gate_edit()));

		new_gate_template_action = gate_menu->addAction("Create gate template from selection");
		QObject::connect(new_gate_template_action, SIGNAL(triggered()), this, SLOT(on_menu_gate_new_gate_template()));

		new_gate_action = gate_menu->addAction("Create gate from selection");
		QObject::connect(new_gate_action, SIGNAL(triggered()), this, SLOT(on_menu_gate_new_gate()));

		gate_menu->addSeparator();
		gate_library_action = gate_menu->addAction("Gate library");
		QObject::connect(gate_library_action, SIGNAL(triggered()), this, SLOT(on_menu_gate_library()));


		// Annotation menu
		QMenu* annotation_menu = menu_bar.addMenu("Annotation");

		edit_annotation_action = annotation_menu->addAction("Edit selected");
		QObject::connect(edit_annotation_action, SIGNAL(triggered()), this, SLOT(on_menu_annotation_edit()));

		create_annotation_action = annotation_menu->addAction("Create from selection");
		QObject::connect(create_annotation_action, SIGNAL(triggered()), this, SLOT(on_menu_annotation_create()));


		// EMarker menu
        QMenu* emarker_menu = menu_bar.addMenu("EMarker");

        edit_emarker_action = emarker_menu->addAction("Edit selected");
        QObject::connect(edit_emarker_action, SIGNAL(triggered()), this, SLOT(on_menu_emarker_edit()));


        // Via menu
        QMenu* via_menu = menu_bar.addMenu("Via");

        edit_via_action = via_menu->addAction("Edit selected");
        QObject::connect(edit_via_action, SIGNAL(triggered()), this, SLOT(on_menu_via_edit()));


        // Logic menu
		QMenu* logic_menu = menu_bar.addMenu("Logic");

		remove_object_action = logic_menu->addAction("Remove selected object");
        remove_object_action->setShortcut(Qt::Key_Delete);
		QObject::connect(remove_object_action, SIGNAL(triggered()), this, SLOT(on_menu_logic_remove_selected_object()));


		// Help menu
		QMenu* help_menu = menu_bar.addMenu("Help");

		about_action = help_menu->addAction("About");
		about_action->setIcon(style()->standardIcon(QStyle::SP_MessageBoxQuestion));
		QObject::connect(about_action, SIGNAL(triggered()), this, SLOT(on_menu_help_about()));


		// Status bar

		status_bar.setStyleSheet("QStatusBar::item { border: none; } ""QStatusBar QLabel { border: 1px solid black; border-radius: 3px; }");
		setStatusBar(&status_bar);
		status_bar.showMessage("Initialization...", SECOND(DEFAULT_STATUS_MESSAGE_DURATION));

		status_bar.addPermanentWidget(&status_bar_layer);
		status_bar_layer.setText("Layer : 0/0 (none)");

		status_bar.addPermanentWidget(&status_bar_coords);
		status_bar_coords.setText("Coordinates : 0,0");
		QObject::connect(workspace, SIGNAL(mouse_coords_changed(int, int)), this, SLOT(change_status_bar_coords(int, int)));


		// Tool bar

		tool_bar = addToolBar("");
		tool_bar->setIconSize(QSize(27, 27));
		tool_bar->setMovable(false);
		tool_bar->setFloatable(false);
		setContextMenuPolicy(Qt::NoContextMenu);

		tool_via_up_action = tool_bar->addAction("Layer up");
		QObject::connect(tool_via_up_action, SIGNAL(triggered()), this, SLOT(on_tool_via_up()));

		tool_via_down_action = tool_bar->addAction("Layer down");
		QObject::connect(tool_via_down_action, SIGNAL(triggered()), this, SLOT(on_tool_via_down()));

		tool_bar->addSeparator();

		tool_zoom_in_action = tool_bar->addAction("Zoom in");
		QObject::connect(tool_zoom_in_action, SIGNAL(triggered()), workspace, SLOT(zoom_in()));

		tool_zoom_out_action = tool_bar->addAction("Zoom out");
		QObject::connect(tool_zoom_out_action, SIGNAL(triggered()), workspace, SLOT(zoom_out()));

		tool_bar->addSeparator();

		tool_gate_library = tool_bar->addAction("Gate library");
		QObject::connect(tool_gate_library, SIGNAL(triggered()), this, SLOT(on_menu_gate_library()));

        tool_bar->addSeparator();

        area_selection_tool = tools_group.addAction("Area");
        QObject::connect(area_selection_tool, SIGNAL(triggered()), workspace, SLOT(use_area_selection_tool()));
        area_selection_tool->setCheckable(true);
        area_selection_tool->setChecked(true);
        workspace->use_area_selection_tool();

        wire_tool = tools_group.addAction("Wire");
        QObject::connect(wire_tool, SIGNAL(triggered()), workspace, SLOT(use_wire_tool()));
        wire_tool->setCheckable(true);

        tool_bar->addActions(tools_group.actions());

		// Other
		QObject::connect(workspace, SIGNAL(project_changed(std::string)), this, SLOT(open_project(std::string)));
        QObject::connect(workspace, SIGNAL(right_mouse_button_released()), this, SLOT(show_context_menu()));

        QObject::connect(&THEME_MANAGER, SIGNAL(icon_theme_changed()), this, SLOT(reload_icons()));
        reload_icons();
	}

	MainWindow::~MainWindow()
	{
		if(workspace != nullptr)
			delete workspace;

		Text::save_fonts_to_cache();
	}

    void MainWindow::reload_icons()
    {
	    /* Menus */

	    // Project menu
        project_new_action->setIcon(QIcon(GET_ICON_PATH("folder_new.png")));
        project_import_action->setIcon(QIcon(GET_ICON_PATH("folder.png")));
        project_export_action->setIcon(QIcon(GET_ICON_PATH("save.png")));
        project_close_action->setIcon(QIcon(GET_ICON_PATH("close.png")));
        project_quit_action->setIcon(QIcon(GET_ICON_PATH("quit.png")));

        // Layer menu
        layers_edit_action->setIcon(QIcon(GET_ICON_PATH("edit.png")));

        // Gate menu
        edit_gate_action->setIcon(QIcon(GET_ICON_PATH("edit.png")));
        gate_library_action->setIcon(QIcon(GET_ICON_PATH("book.png")));

        // Annotation menu
        edit_annotation_action->setIcon(QIcon(GET_ICON_PATH("edit.png")));

        // EMarker menu
        edit_emarker_action->setIcon(QIcon(GET_ICON_PATH("edit.png")));

        // Via menu
        edit_via_action->setIcon(QIcon(GET_ICON_PATH("edit.png")));

        // Logic menu
        remove_object_action->setIcon(QIcon(GET_ICON_PATH("remove.png")));


        /* Toolbar */
        tool_via_up_action->setIcon(QIcon(GET_ICON_PATH("layer_up.png")));
        tool_via_down_action->setIcon(QIcon(GET_ICON_PATH("layer_down.png")));
        tool_zoom_in_action->setIcon(QIcon(GET_ICON_PATH("plus.png")));
        tool_zoom_out_action->setIcon(QIcon(GET_ICON_PATH("minus.png")));
        tool_gate_library->setIcon(QIcon(GET_ICON_PATH("book.png")));
        area_selection_tool->setIcon(QIcon(GET_ICON_PATH("area_selection_tool.png")));
        wire_tool->setIcon(QIcon(GET_ICON_PATH("wire_tool.png")));
    }

	void MainWindow::on_menu_help_about()
	{
		const QString about_message = "<html><center>"
			"<img src=':/degate_logo.png' alt='' width='100' height='87'> <br>"
            "<strong>Version : " DEGATE_VERSION "</strong><br><br>"
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

		if(dir.isNull())
        {
            status_bar.showMessage("Project opening operation cancelled.", SECOND(DEFAULT_STATUS_MESSAGE_DURATION));

            return;
        }

		setEnabled(false);

		if(project != nullptr)
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
		if(project == nullptr)
			return;

		status_bar.showMessage("Saving project...");

		ProjectExporter exporter;
		exporter.export_all(project->get_project_directory(), project);

		status_bar.showMessage("Project saved.", SECOND(DEFAULT_STATUS_MESSAGE_DURATION));
	}

	void MainWindow::on_menu_project_close()
	{
		status_bar.showMessage("Closing project...");

		if(project == nullptr)
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
                    return;
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
		project = nullptr;
		workspace->set_project(nullptr);
		workspace->update_screen();

		setWindowTitle("Degate");

		status_bar_layer.setText("Layer : 0/0 (none)");

		status_bar.showMessage("Project closed.", SECOND(DEFAULT_STATUS_MESSAGE_DURATION));
	}

	void MainWindow::on_menu_project_new()
	{
		status_bar.showMessage("Creating a new project...");

		QString dir = QFileDialog::getExistingDirectory(this, "Select the directory where the project will be created");

        if(dir.isNull())
        {
            status_bar.showMessage("New project creation operation cancelled.", SECOND(DEFAULT_STATUS_MESSAGE_DURATION));

            return;
        }

		NewProjectDialog dialog(this);
        auto res = dialog.exec();

        if(res == QDialog::Accepted)
        {
            std::string project_name = dialog.get_project_name();
            unsigned layer_count = dialog.get_layer_count();
            unsigned width = dialog.get_width();
            unsigned height = dialog.get_height();

            if(layer_count == 0 || width == 0 || height == 0 || project_name.length() < 1)
            {
                QMessageBox::warning(this, "Invalid values", "The values you entered are invalid. Operation cancelled");

                status_bar.showMessage("New project creation operation cancelled.", SECOND(DEFAULT_STATUS_MESSAGE_DURATION));

                return;
            }
            else
            {
                const std::string project_dir = dir.toStdString() + "/" + project_name;

                if(!file_exists(project_dir))
                    create_directory(project_dir);

                project = std::make_shared<Project>(width, height, project_dir, layer_count);
                project->set_name(project_name);

                workspace->set_project(project);

                LayersEditDialog layers_edit_dialog(project, this);
                layers_edit_dialog.exec();

                on_menu_project_exporter();
            }

            status_bar.showMessage("Created a new project.", SECOND(DEFAULT_STATUS_MESSAGE_DURATION));
        }
        else
            status_bar.showMessage("New project creation operation cancelled.", SECOND(DEFAULT_STATUS_MESSAGE_DURATION));
	}

	void MainWindow::on_menu_project_create_subproject()
	{
		if(project == nullptr || !workspace->has_area_selection())
			return;

		boost::format f("subproject_%1%");
		f % project->get_logic_model()->get_new_object_id();

		SubProjectAnnotation_shptr new_annotation(new SubProjectAnnotation(workspace->get_area_selection(), f.str()));
		new_annotation->set_fill_color(project->get_default_color(DEFAULT_COLOR_ANNOTATION));
		new_annotation->set_frame_color(project->get_default_color(DEFAULT_COLOR_ANNOTATION_FRAME));

        AnnotationEditDialog dialog(new_annotation, this);
        auto res = dialog.exec();

        if(res == QDialog::Accepted)
        {
            project->get_logic_model()->add_object(project->get_logic_model()->get_current_layer()->get_layer_pos(), new_annotation);

            workspace->reset_area_selection();
            workspace->update_screen();
        }
        else
            new_annotation.reset();
	}

	void MainWindow::on_menu_edit_preferences()
	{
		PreferencesEditor dialog(this);
		dialog.exec();
	}

	void MainWindow::on_menu_layer_edit()
	{
		if(project == nullptr)
			return;

		LayersEditDialog layers_edit_dialog(project, this);
		layers_edit_dialog.exec();

		workspace->update_screen();
	}

	void MainWindow::on_menu_layer_import_background()
	{
		if(project == nullptr)
		{
			status_bar.showMessage("Failed to import new background image : no project opened.", SECOND(DEFAULT_STATUS_MESSAGE_DURATION));
			return;
		}

		status_bar.showMessage("Importing a new background image for the layer...");

		QString res = QFileDialog::getOpenFileName(this, "Select the new background image");
		const std::string file_name = res.toStdString();

		if(res.isNull())
        {
            status_bar.showMessage("New background image import cancelled.", SECOND(DEFAULT_STATUS_MESSAGE_DURATION));
            return;
        }

		load_background_image(project->get_logic_model()->get_current_layer(), project->get_project_directory(), file_name);

		workspace->update_screen();

		status_bar.showMessage("Imported a new background image for the layer.", SECOND(DEFAULT_STATUS_MESSAGE_DURATION));
	}

	void MainWindow::on_menu_gate_new_gate_template()
	{
		if(project == nullptr || !workspace->has_area_selection())
			return;

		if(project->get_logic_model()->get_current_layer()->get_layer_type() != Layer::LOGIC)
		{
			QMessageBox::warning(this, "Warning", "You can create a new gate only on a logic layer.");
			return;
		}

		auto new_gate_template = std::make_shared<GateTemplate>(workspace->get_area_selection().get_width(),workspace->get_area_selection().get_height());
		grab_template_images(project->get_logic_model(), new_gate_template, workspace->get_area_selection());
		new_gate_template->set_object_id(project->get_logic_model()->get_new_object_id());
        new_gate_template->set_fill_color(project->get_default_color(DEFAULT_COLOR_GATE));
        new_gate_template->set_frame_color(project->get_default_color(DEFAULT_COLOR_GATE_FRAME));

		auto new_gate = std::make_shared<Gate>(workspace->get_area_selection());
		new_gate->set_gate_template(new_gate_template);
		new_gate->set_fill_color(project->get_default_color(DEFAULT_COLOR_GATE));
		new_gate->set_frame_color(project->get_default_color(DEFAULT_COLOR_GATE_FRAME));
		new_gate->set_object_id(project->get_logic_model()->get_new_object_id());

		project->get_logic_model()->add_gate_template(new_gate_template);

        GateInstanceEditDialog dialog(this, new_gate, project);
        auto res = dialog.exec();

        if(res == QDialog::Accepted)
        {
            project->get_logic_model()->add_object(project->get_logic_model()->get_current_layer()->get_layer_pos(), new_gate);
            project->get_logic_model()->update_ports(new_gate);

            workspace->reset_area_selection();
            workspace->update_screen();
        }
        else
        {
            project->get_logic_model()->remove_gate_template(new_gate_template);
            new_gate_template.reset();
        }
	}

	void MainWindow::on_menu_gate_new_gate()
	{
		if(project == nullptr || !workspace->has_area_selection())
			return;

		if(project->get_logic_model()->get_current_layer()->get_layer_type() != Layer::LOGIC)
		{
			QMessageBox::warning(this, "Warning", "You can create a new gate only on a logic layer.");
			return;
		}

		SelectGateTemplateDialog select_dialog(project, this);
		auto res = select_dialog.exec();

        if(res == QDialog::Rejected)
            return;

		GateTemplate_shptr gate_template = select_dialog.get_selected_gate();

		if(gate_template == nullptr)
		    return;

		Gate_shptr new_gate(new Gate(workspace->get_area_selection()));
		new_gate->set_gate_template(gate_template);

        GateInstanceEditDialog dialog(this, new_gate, project);
        res = dialog.exec();

        if(res == QDialog::Accepted)
        {
            project->get_logic_model()->add_object(project->get_logic_model()->get_current_layer()->get_layer_pos(),
                                                   new_gate);
            project->get_logic_model()->update_ports(new_gate);

            workspace->reset_area_selection();
            workspace->update_screen();
        }
        else
        {
            new_gate.reset();
        }
	}

	void MainWindow::on_menu_gate_edit()
	{
		if(project == nullptr || !workspace->has_selection())
			return;

		if(Gate_shptr o = std::dynamic_pointer_cast<Gate>(workspace->get_selected_objects().back()))
		{
			GateInstanceEditDialog dialog(this, o, project);
			dialog.exec();

			project->get_logic_model()->update_ports(o);
		}

		workspace->update_screen();
	}

	void MainWindow::on_menu_gate_library()
	{
		if(project == nullptr)
			return;

		GateLibraryDialog dialog(project, this);
		dialog.exec();

		workspace->update_screen();
	}

    void MainWindow::on_menu_gate_port_edit()
    {
        if(project == nullptr || !workspace->has_selection())
            return;

        if(GatePort_shptr o = std::dynamic_pointer_cast<GatePort>(workspace->get_selected_objects().back()))
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
		if(project == nullptr || !workspace->has_area_selection())
			return;

		Annotation_shptr new_annotation(new Annotation(workspace->get_area_selection()));
		new_annotation->set_fill_color(project->get_default_color(DEFAULT_COLOR_ANNOTATION));
		new_annotation->set_frame_color(project->get_default_color(DEFAULT_COLOR_ANNOTATION_FRAME));

        AnnotationEditDialog dialog(new_annotation, this);
        auto res = dialog.exec();

        if(res == QDialog::Accepted)
        {
            project->get_logic_model()->add_object(project->get_logic_model()->get_current_layer()->get_layer_pos(), new_annotation);

            workspace->reset_area_selection();
            workspace->update_screen();
        }
        else
            new_annotation.reset();
	}

	void MainWindow::on_menu_annotation_edit()
	{
		if(project == nullptr || !workspace->has_selection())
			return;

		if(Annotation_shptr o = std::dynamic_pointer_cast<Annotation>(workspace->get_selected_objects().back()))
		{
			AnnotationEditDialog dialog(o, this);
			dialog.exec();

			workspace->update_screen();
		}
	}

    void MainWindow::on_menu_emarker_edit()
    {
        if(project == nullptr || !workspace->has_selection())
            return;

        if(EMarker_shptr o = std::dynamic_pointer_cast<EMarker>(workspace->get_selected_objects().back()))
        {
            EMarkerEditDialog dialog(o, this);
            dialog.exec();

            workspace->update_screen();
        }
    }

    void MainWindow::on_menu_via_edit()
    {
        if(project == nullptr || !workspace->has_selection())
            return;

        if(Via_shptr o = std::dynamic_pointer_cast<Via>(workspace->get_selected_objects().back()))
        {
            ViaEditDialog dialog(o, this, project);
            dialog.exec();

            workspace->update_screen();
        }
    }

	void MainWindow::on_menu_logic_remove_selected_object()
	{
        if(project == nullptr || !workspace->has_selection())
            return;

        auto objects = workspace->get_selected_objects();
        workspace->reset_selection();

        for (auto& object : objects)
            project->get_logic_model()->remove_object(object);

        workspace->update_screen();
	}

    void MainWindow::on_menu_project_settings()
    {
        if(project == nullptr)
            return;

        ProjectSettingsDialog dialog(this, project);
        dialog.exec();

        setWindowTitle("Degate : " + QString::fromStdString(project->get_name()) + " project");

        workspace->update_screen();
    }

	void MainWindow::on_menu_project_quit()
	{
		close();
	}

	void MainWindow::on_tool_via_up()
	{
		if(project == nullptr)
			return;

		project->get_logic_model()->set_current_layer(get_next_enabled_layer(project->get_logic_model())->get_layer_pos());

		status_bar_layer.setText("Layer : " + QString::number(project->get_logic_model()->get_current_layer()->get_layer_pos() + 1) + "/" + QString::number(project->get_logic_model()->get_num_layers()) + " (" + QString::fromStdString(project->get_logic_model()->get_current_layer()->get_layer_type_as_string()) + ")");

        workspace->reset_selection();

		workspace->update_screen();
	}

	void MainWindow::on_tool_via_down()
	{
		if(project == nullptr)
			return;

		project->get_logic_model()->set_current_layer(get_prev_enabled_layer(project->get_logic_model())->get_layer_pos());

		status_bar_layer.setText("Layer : " + QString::number(project->get_logic_model()->get_current_layer()->get_layer_pos() + 1) + "/" + QString::number(project->get_logic_model()->get_num_layers()) + " (" + QString::fromStdString(project->get_logic_model()->get_current_layer()->get_layer_type_as_string()) + ")");

        workspace->reset_selection();

		workspace->update_screen();
	}

	void MainWindow::open_project(const std::string& path)
	{
		status_bar.showMessage("Importing project/subproject...");

		ProjectImporter projectImporter;

		if(project != nullptr)
		    on_menu_project_exporter();

		try
		{
			std::shared_ptr<Project> imported_project = projectImporter.import_all(path);

			if(imported_project == nullptr)
            {
                QMessageBox::StandardButton reply;
                reply = QMessageBox::question(this, "Subproject", "The project/subproject do not exist, do you want to create it ?", QMessageBox::Yes | QMessageBox::No);

                if(reply == QMessageBox::Yes)
                {
                    NewProjectDialog dialog(this);
                    auto res = dialog.exec();

                    if(res == QDialog::Accepted)
                    {
                        std::string project_name = dialog.get_project_name();
                        unsigned layer_count = dialog.get_layer_count();
                        unsigned width = dialog.get_width();
                        unsigned height = dialog.get_height();

                        if(layer_count == 0 || width == 0 || height == 0 || project_name.length() < 1)
                        {
                            QMessageBox::warning(this, "Invalid values", "The values you entered are invalid. Operation cancelled");

                            status_bar.showMessage("New project/subproject operation cancelled.", SECOND(DEFAULT_STATUS_MESSAGE_DURATION));

                            return;
                        }
                        else
                        {
                            if(!file_exists(path))
                                create_directory(path);

                            project = std::make_shared<Project>(width, height, path, layer_count);
                            project->set_name(project_name);

                            LayersEditDialog layers_edit_dialog(project, this);
                            layers_edit_dialog.exec();

                            on_menu_project_exporter();
                        }
                    }
                    else
                    {
                        return;
                    }
                }
                else
                {
                    return;
                }
            }
			else
            {
                project = imported_project;
            }
		}
		catch (const std::exception& e)
		{
            std::cout << "Exception caught: " << e.what() << std::endl;
            QMessageBox::warning(this, "Project/Subproject import failed", "The project/subproject cannot be imported (maybe corrupted).");
            status_bar.showMessage("Project/Subproject import failed.", SECOND(DEFAULT_STATUS_MESSAGE_DURATION));

            return;
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
	    if(project == nullptr)
	        return;

        QMenu contextMenu(("Context menu"), this);

        // New
        QAction annotation_create_action("Create new annotation", this);
        QAction gate_template_create_action("Create new gate template", this);
        QAction gate_create_action("Create new gate", this);
        QAction emarker_create_action("Create new EMarker", this);
        QAction via_create_action("Create new via", this);

        // Edit
        QAction annotation_edit_action("Edit selected annotation", this);
        QAction gate_edit_action("Edit selected gate", this);
        QAction gate_port_edit_action("Move selected port", this);
        QAction emarker_edit_action("Edit selected EMarker", this);
        QAction via_edit_action("Edit selected via", this);

        // Via
        QAction via_follow_action("Follow via", this);

        // Delete
        QAction delete_action("Remove selected object", this);

        // Reset area
        QAction reset_selection_area_action("Reset selection area", this);

        // Reset wire tool
        QAction reset_wire_tool_action("Reset wire tool", this);

        // Get current opengl mouse position
        context_menu_mouse_position = workspace->get_opengl_mouse_position();

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
            PlacedLogicModelObject_shptr object = workspace->get_selected_objects().back();

            if(Annotation_shptr annotation = std::dynamic_pointer_cast<Annotation>(object))
            {
                connect(&annotation_edit_action, SIGNAL(triggered()), this, SLOT(on_menu_annotation_edit()));
                contextMenu.addAction(&annotation_edit_action);
            }
            else if (Gate_shptr gate = std::dynamic_pointer_cast<Gate>(object))
            {
                connect(&gate_edit_action, SIGNAL(triggered()), this, SLOT(on_menu_gate_edit()));
                contextMenu.addAction(&gate_edit_action);
            }
            else if (GatePort_shptr gate_port = std::dynamic_pointer_cast<GatePort>(object))
            {
                connect(&gate_port_edit_action, SIGNAL(triggered()), this, SLOT(on_menu_gate_port_edit()));
                contextMenu.addAction(&gate_port_edit_action);
            }
            else if (EMarker_shptr emarker = std::dynamic_pointer_cast<EMarker>(object))
            {
                connect(&emarker_edit_action, SIGNAL(triggered()), this, SLOT(on_menu_emarker_edit()));
                contextMenu.addAction(&emarker_edit_action);
            }
            else if (Via_shptr via = std::dynamic_pointer_cast<Via>(object))
            {
                if(via->get_direction() == Via::DIRECTION_UP)
                {
                    connect(&via_follow_action, SIGNAL(triggered()), this, SLOT(on_tool_via_up()));
                    contextMenu.addAction(&via_follow_action);
                }
                else
                {
                    connect(&via_follow_action, SIGNAL(triggered()), this, SLOT(on_tool_via_down()));
                    contextMenu.addAction(&via_follow_action);
                }

                connect(&via_edit_action, SIGNAL(triggered()), this, SLOT(on_menu_via_edit()));
                contextMenu.addAction(&via_edit_action);
            }

            connect(&delete_action, SIGNAL(triggered()), this, SLOT(on_menu_logic_remove_selected_object()));
            contextMenu.addAction(&delete_action);
        }
        else
        {
            connect(&emarker_create_action, SIGNAL(triggered()), this, SLOT(on_emarker_create()));
            contextMenu.addAction(&emarker_create_action);

            connect(&via_create_action, SIGNAL(triggered()), this, SLOT(on_via_create()));
            contextMenu.addAction(&via_create_action);

            if(workspace->get_current_tool() == WIRE)
            {
                contextMenu.addSeparator();

                connect(&reset_wire_tool_action, SIGNAL(triggered()), workspace, SLOT(reset_wire_tool()));
                contextMenu.addAction(&reset_wire_tool_action);
            }
        }

        contextMenu.exec(QCursor::pos());
    }

    void MainWindow::on_emarker_create()
    {
        if(project == nullptr)
            return;

        EMarker_shptr new_emarker = std::make_shared<EMarker>(context_menu_mouse_position.x(), context_menu_mouse_position.y());
        new_emarker->set_fill_color(project->get_default_color(DEFAULT_COLOR_EMARKER));

        EMarkerEditDialog dialog(new_emarker, this);
        auto res = dialog.exec();

        if(res == QDialog::Accepted)
        {
            project->get_logic_model()->add_object(project->get_logic_model()->get_current_layer()->get_layer_pos(), new_emarker);

            workspace->update_screen();
        }
        else
            new_emarker.reset();
    }

    void MainWindow::on_via_create()
    {
        if(project == nullptr)
            return;

        Via_shptr new_via = std::make_shared<Via>(context_menu_mouse_position.x(), context_menu_mouse_position.y(), Via::DIRECTION_UNDEFINED);
        new_via->set_diameter(project->get_default_via_diameter());

        ViaEditDialog dialog(new_via, this, project);
        auto res = dialog.exec();

        if(res == QDialog::Accepted)
        {
            project->get_logic_model()->add_object(project->get_logic_model()->get_current_layer()->get_layer_pos(),
                                                   new_via);

            workspace->update_screen();
        }
        else
            new_via.reset();
    }
}
