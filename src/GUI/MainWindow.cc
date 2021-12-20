/**
 * This file is part of the IC reverse engineering tool Degate.
 *
 * Copyright 2008, 2009, 2010 by Martin Schobert
 * Copyright 2019-2020 Dorian Bachelot
 *
 * Degate is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * Degate is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with degate. If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "MainWindow.h"
#include "GUI/Dialog/ProgressDialog.h"
#include "GUI/Dialog/AboutDialog.h"
#include "Core/Version.h"
#include "Core/Utils/CrashReport.h"

#ifdef SYS_WINDOWS
#include <QtPlatformHeaders/QWindowsWindowFunctions>
#endif

#include <memory>

#define SECOND(a) a * 1000

namespace degate
{
    MainWindow::MainWindow(int width, int height) : status_bar(this), tools_group(this), updater(this)
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
        project_menu = menu_bar.addMenu("");

        project_new_action = project_menu->addAction("");
        QObject::connect(project_new_action, SIGNAL(triggered()), this, SLOT(on_menu_project_new()));

        project_import_action = project_menu->addAction("");
        QObject::connect(project_import_action, SIGNAL(triggered()), this, SLOT(on_menu_project_importer()));

        project_export_action = project_menu->addAction("");
        project_export_action->setShortcut(Qt::CTRL + Qt::Key_S);
        QObject::connect(project_export_action, SIGNAL(triggered()), this, SLOT(on_menu_project_save()));

        project_recent_projects_submenu = project_menu->addMenu("");

        project_close_action = project_menu->addAction("");
        QObject::connect(project_close_action, SIGNAL(triggered()), this, SLOT(on_menu_project_close()));

        project_menu->addSeparator();
        project_create_subproject_action = project_menu->addAction("");
        QObject::connect(project_create_subproject_action, SIGNAL(triggered()), this, SLOT(on_menu_project_create_subproject()));

        project_menu->addSeparator();
        project_settings_action = project_menu->addAction("");
        QObject::connect(project_settings_action, SIGNAL(triggered()), this, SLOT(on_menu_project_settings()));

        project_menu->addSeparator();
        project_quit_action = project_menu->addAction("");
        QObject::connect(project_quit_action, SIGNAL(triggered()), this, SLOT(on_menu_project_quit()));


        // Edit menu
        edit_menu = menu_bar.addMenu("");
        preferences_edit_action = edit_menu->addAction("");
        QObject::connect(preferences_edit_action, SIGNAL(triggered()), this, SLOT(on_menu_edit_preferences()));


        // View menu
        view_menu = menu_bar.addMenu("");

        show_gates_view_action = view_menu->addAction("");
        show_gates_view_action->setCheckable(true);
        show_gates_view_action->setChecked(true);
        QObject::connect(show_gates_view_action, SIGNAL(toggled(bool)), workspace, SLOT(show_gates(bool)));

        show_gates_name_view_action = view_menu->addAction("");
        show_gates_name_view_action->setCheckable(true);
        show_gates_name_view_action->setChecked(true);
        QObject::connect(show_gates_name_view_action, SIGNAL(toggled(bool)), workspace, SLOT(show_gates_name(bool)));

        show_ports_view_action = view_menu->addAction("");
        show_ports_view_action->setCheckable(true);
        show_ports_view_action->setChecked(true);
        QObject::connect(show_ports_view_action, SIGNAL(toggled(bool)), workspace, SLOT(show_ports(bool)));

        show_ports_name_view_action = view_menu->addAction("");
        show_ports_name_view_action->setCheckable(true);
        show_ports_name_view_action->setChecked(true);
        QObject::connect(show_ports_name_view_action, SIGNAL(toggled(bool)), workspace, SLOT(show_ports_name(bool)));

        show_annotations_view_action = view_menu->addAction("");
        show_annotations_view_action->setCheckable(true);
        show_annotations_view_action->setChecked(true);
        QObject::connect(show_annotations_view_action, SIGNAL(toggled(bool)), workspace, SLOT(show_annotations(bool)));

        show_annotations_name_view_action = view_menu->addAction("");
        show_annotations_name_view_action->setCheckable(true);
        show_annotations_name_view_action->setChecked(true);
        QObject::connect(show_annotations_name_view_action, SIGNAL(toggled(bool)), workspace, SLOT(show_annotations_name(bool)));

        show_emarkers_view_action = view_menu->addAction("");
        show_emarkers_view_action->setCheckable(true);
        show_emarkers_view_action->setChecked(true);
        QObject::connect(show_emarkers_view_action, SIGNAL(toggled(bool)), workspace, SLOT(show_emarkers(bool)));

        show_emarkers_name_view_action = view_menu->addAction("");
        show_emarkers_name_view_action->setCheckable(true);
        show_emarkers_name_view_action->setChecked(true);
        QObject::connect(show_emarkers_name_view_action, SIGNAL(toggled(bool)), workspace, SLOT(show_emarkers_name(bool)));

        show_vias_view_action = view_menu->addAction("");
        show_vias_view_action->setCheckable(true);
        show_vias_view_action->setChecked(true);
        QObject::connect(show_vias_view_action, SIGNAL(toggled(bool)), workspace, SLOT(show_vias(bool)));

        show_vias_name_view_action = view_menu->addAction("");
        show_vias_name_view_action->setCheckable(true);
        show_vias_name_view_action->setChecked(true);
        QObject::connect(show_vias_name_view_action, SIGNAL(toggled(bool)), workspace, SLOT(show_vias_name(bool)));

        show_wires_view_action = view_menu->addAction("");
        show_wires_view_action->setCheckable(true);
        show_wires_view_action->setChecked(true);
        QObject::connect(show_wires_view_action, SIGNAL(toggled(bool)), workspace, SLOT(show_wires(bool)));

        view_menu->addSeparator();

        grid_configuration_view_action = view_menu->addAction("");
        QObject::connect(grid_configuration_view_action, SIGNAL(triggered()), this, SLOT(on_grid_configuration()));

        show_grid_view_action = view_menu->addAction("");
        show_grid_view_action->setCheckable(true);
        bool show_grid = PREFERENCES_HANDLER.get_preferences().show_grid;
        show_grid_view_action->setChecked(show_grid);
        workspace->show_grid(show_grid);
        QObject::connect(show_grid_view_action, SIGNAL(toggled(bool)), workspace, SLOT(show_grid(bool)));

        snap_to_grid_view_action = view_menu->addAction("");
        snap_to_grid_view_action->setCheckable(true);
        bool snap_to_grid = PREFERENCES_HANDLER.get_preferences().snap_to_grid;
        snap_to_grid_view_action->setChecked(snap_to_grid);
        QObject::connect(snap_to_grid_view_action, SIGNAL(toggled(bool)), this, SLOT(on_menu_view_snap_to_grid(bool)));

        view_menu->addSeparator();

        fullscreen_view_action = view_menu->addAction("");
        fullscreen_view_action->setCheckable(true);
        fullscreen_view_action->setShortcut(Qt::Key_F11);
        QObject::connect(fullscreen_view_action, SIGNAL(toggled(bool)), this, SLOT(on_menu_view_fullscreen(bool)));


        // Layer menu
        layer_menu = menu_bar.addMenu("");

        layers_edit_action = layer_menu->addAction("");
        QObject::connect(layers_edit_action, SIGNAL(triggered()), this, SLOT(on_menu_layer_edit()));

        background_import_action = layer_menu->addAction("");
        QObject::connect(background_import_action, SIGNAL(triggered()), this, SLOT(on_menu_layer_import_background()));


        // Gate menu
        gate_menu = menu_bar.addMenu("");

        edit_gate_action = gate_menu->addAction("");
        QObject::connect(edit_gate_action, SIGNAL(triggered()), this, SLOT(on_menu_gate_edit()));

        new_gate_template_action = gate_menu->addAction("");
        QObject::connect(new_gate_template_action, SIGNAL(triggered()), this, SLOT(on_menu_gate_new_gate_template()));

        new_gate_action = gate_menu->addAction("");
        QObject::connect(new_gate_action, SIGNAL(triggered()), this, SLOT(on_menu_gate_new_gate()));

        gate_menu->addSeparator();

        gate_library_action = gate_menu->addAction("");
        QObject::connect(gate_library_action, SIGNAL(triggered()), this, SLOT(on_menu_gate_library()));

        gate_list_action = gate_menu->addAction("");
        QObject::connect(gate_list_action, SIGNAL(triggered()), this, SLOT(on_menu_gate_list()));

        gate_menu->addSeparator();

        auto_name_gates_rows_action = gate_menu->addAction("");
        QObject::connect(auto_name_gates_rows_action, &QAction::triggered, this, [this]()
        {
            on_menu_gate_automatic_naming(AutoNameGates::ORIENTATION::ALONG_ROWS);
        });

        auto_name_gates_columns_action = gate_menu->addAction("");
        QObject::connect(auto_name_gates_columns_action, &QAction::triggered, this, [this]()
        {
            on_menu_gate_automatic_naming(AutoNameGates::ORIENTATION::ALONG_COLS);
        });


        // Annotation menu
        annotation_menu = menu_bar.addMenu("");

        edit_annotation_action = annotation_menu->addAction("");
        QObject::connect(edit_annotation_action, SIGNAL(triggered()), this, SLOT(on_menu_annotation_edit()));

        create_annotation_action = annotation_menu->addAction("");
        QObject::connect(create_annotation_action, SIGNAL(triggered()), this, SLOT(on_menu_annotation_create()));

        annotation_list_action = annotation_menu->addAction("");
        QObject::connect(annotation_list_action, SIGNAL(triggered()), this, SLOT(on_menu_annotation_list()));


        // EMarker menu
        emarker_menu = menu_bar.addMenu("");

        edit_emarker_action = emarker_menu->addAction("");
        QObject::connect(edit_emarker_action, SIGNAL(triggered()), this, SLOT(on_menu_emarker_edit()));


        // Via menu
        via_menu = menu_bar.addMenu("");

        edit_via_action = via_menu->addAction("");
        QObject::connect(edit_via_action, SIGNAL(triggered()), this, SLOT(on_menu_via_edit()));


        // Logic menu
        logic_menu = menu_bar.addMenu("");

        remove_objects_action = logic_menu->addAction("");
        remove_objects_action->setShortcut(Qt::Key_Delete);
        QObject::connect(remove_objects_action, SIGNAL(triggered()), this, SLOT(on_menu_logic_remove_selected_objects()));

        interconnect_objects_action = logic_menu->addAction("");
        interconnect_objects_action->setShortcut(Qt::CTRL + Qt::Key_C);
        QObject::connect(interconnect_objects_action, SIGNAL(triggered()), this, SLOT(on_menu_logic_interconnect_selected_objects()));

        isolate_objects_action = logic_menu->addAction("");
        isolate_objects_action->setShortcut(Qt::CTRL + Qt::Key_X);
        QObject::connect(isolate_objects_action, SIGNAL(triggered()), this, SLOT(on_menu_logic_isolate_selected_objects()));

        move_selected_gates_into_module = logic_menu->addAction("");
        QObject::connect(move_selected_gates_into_module, SIGNAL(triggered()), this, SLOT(on_menu_logic_move_selected_gates_into_module()));

        inspect_selected_object_action = logic_menu->addAction("");
        QObject::connect(inspect_selected_object_action, SIGNAL(triggered()), this, SLOT(
                on_menu_logic_inspect_selected_object()));


        // Matching menu
        matching_menu = menu_bar.addMenu("");

        template_matching_action = matching_menu->addAction("");
        QObject::connect(template_matching_action, SIGNAL(triggered()), this, SLOT(on_menu_matching_template_matching()));

        via_matching_action = matching_menu->addAction("");
        QObject::connect(via_matching_action, SIGNAL(triggered()), this, SLOT(on_menu_matching_via_matching()));

        wire_matching_action = matching_menu->addAction("");
        QObject::connect(wire_matching_action, SIGNAL(triggered()), this, SLOT(on_menu_matching_wire_matching()));


        // Help menu
        help_menu = menu_bar.addMenu("");

        help_action = help_menu->addAction("");
        QObject::connect(help_action, SIGNAL(triggered()), this, SLOT(on_menu_help_open_help()));

        documentation_action = help_menu->addAction("");
        QObject::connect(documentation_action, SIGNAL(triggered()), this, SLOT(on_menu_help_documentation()));

        help_menu->addSeparator();

        check_updates_action = help_menu->addAction("");
        QObject::connect(check_updates_action, &QAction::triggered, this, [this]()
        {
            on_menu_help_check_updates(true, false);
        });

        help_menu->addSeparator();

        if (file_exists(DEGATE_IN_CONFIGURATION(ERROR_FILE_NAME)))
        {
            open_error_file_action = help_menu->addAction("");
            QObject::connect(open_error_file_action, SIGNAL(triggered()), this, SLOT(on_menu_help_open_error_file()));
        }
        else
            open_error_file_action = nullptr;

        bug_report_action = help_menu->addAction("");
        QObject::connect(bug_report_action, SIGNAL(triggered()), this, SLOT(on_menu_help_bug_report()));

        help_menu->addSeparator();

        degate_website_action = help_menu->addAction("");
        QObject::connect(degate_website_action, SIGNAL(triggered()), this, SLOT(on_menu_help_degate_website()));

        about_action = help_menu->addAction("");
        about_action->setIcon(style()->standardIcon(QStyle::SP_MessageBoxQuestion));
        QObject::connect(about_action, SIGNAL(triggered()), this, SLOT(on_menu_help_about()));


        // Status bar

        status_bar.setStyleSheet("QStatusBar::item { border: none; } QStatusBar QLabel { border: 3px solid black; border-radius: 0px; padding: 2px; }");
        setStatusBar(&status_bar);
        status_bar.showMessage(tr("Initialization..."), SECOND(DEFAULT_STATUS_MESSAGE_DURATION));

        status_bar.addPermanentWidget(&status_bar_layer);
        update_status_bar_layer_info();

        status_bar.addPermanentWidget(&status_bar_coords);
        change_status_bar_coords(0, 0);
        QObject::connect(workspace, SIGNAL(mouse_coords_changed(int, int)), this, SLOT(change_status_bar_coords(int, int)));

        update_status_bar_layer_info();


        // Tool bar

        tool_bar = addToolBar("");
        tool_bar->setIconSize(QSize(27, 27));
        tool_bar->setMovable(false);
        tool_bar->setFloatable(false);
        setContextMenuPolicy(Qt::NoContextMenu);

        tool_via_up_action = tool_bar->addAction("");
        QObject::connect(tool_via_up_action, SIGNAL(triggered()), this, SLOT(on_tool_via_up()));

        tool_via_down_action = tool_bar->addAction("");
        QObject::connect(tool_via_down_action, SIGNAL(triggered()), this, SLOT(on_tool_via_down()));

        tool_bar->addSeparator();

        tool_zoom_in_action = tool_bar->addAction("");
        QObject::connect(tool_zoom_in_action, SIGNAL(triggered()), workspace, SLOT(zoom_in()));

        tool_zoom_out_action = tool_bar->addAction("");
        QObject::connect(tool_zoom_out_action, SIGNAL(triggered()), workspace, SLOT(zoom_out()));

        tool_bar->addSeparator();

        area_selection_tool = tools_group.addAction("");
        QObject::connect(area_selection_tool, SIGNAL(triggered()), workspace, SLOT(use_area_selection_tool()));
        area_selection_tool->setCheckable(true);
        area_selection_tool->setChecked(true);
        workspace->use_area_selection_tool();

        wire_tool = tools_group.addAction("");
        QObject::connect(wire_tool, SIGNAL(triggered()), workspace, SLOT(use_wire_tool()));
        wire_tool->setCheckable(true);

        tool_bar->addActions(tools_group.actions());

        tool_bar->addSeparator();

        tool_gate_library = tool_bar->addAction("");
        QObject::connect(tool_gate_library, SIGNAL(triggered()), this, SLOT(on_menu_gate_library()));

        rule_violations_action = tool_bar->addAction("");
        QObject::connect(rule_violations_action, SIGNAL(triggered()), this, SLOT(on_rule_violations_dialog()));

        modules_action = tool_bar->addAction("");
        QObject::connect(modules_action, SIGNAL(triggered()), this, SLOT(on_modules_dialog()));

        // Other
        QObject::connect(workspace, SIGNAL(project_changed(std::string)), this, SLOT(open_project(std::string)));
        QObject::connect(workspace, SIGNAL(right_mouse_button_released()), this, SLOT(show_context_menu()));

        QObject::connect(&THEME_MANAGER, SIGNAL(icon_theme_changed()), this, SLOT(reload_icons()));
        reload_icons();

        QObject::connect(&PREFERENCES_HANDLER, SIGNAL(language_changed()), this, SLOT(reload_texts()));

        reload_texts();

        QObject::connect(workspace, SIGNAL(project_changed()), this, SLOT(project_changed()));

        auto_save_timer.setInterval(PREFERENCES_HANDLER.get_preferences().auto_save_interval * 60000);
        auto_save_timer.start();

        QObject::connect(&auto_save_timer, SIGNAL(timeout()), this, SLOT(auto_save()));

        // Workaround for a bug on Windows that occurs when using QOpenGLWidget + fullscreen mode.
        // See: https://doc.qt.io/qt-5/windows-issues.html#fullscreen-opengl-based-windows.
        #ifdef SYS_WINDOWS
            this->topLevelWidget()->winId();
            QWindowsWindowFunctions::setHasBorderInFullScreen(this->topLevelWidget()->windowHandle(), true);
        #endif

        // Check for updates.
        if (PREFERENCES_HANDLER.get_preferences().automatic_updates_check)
        {
            QTimer::singleShot(0, [this](){ on_menu_help_check_updates(false, true); });
        }

        reload_recent_projects_list();
    }

    MainWindow::~MainWindow()
    {
        on_menu_project_close();

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
        remove_objects_action->setIcon(QIcon(GET_ICON_PATH("remove.png")));


        /* Toolbar */
        tool_via_up_action->setIcon(QIcon(GET_ICON_PATH("layer_up.png")));
        tool_via_down_action->setIcon(QIcon(GET_ICON_PATH("layer_down.png")));
        tool_zoom_in_action->setIcon(QIcon(GET_ICON_PATH("plus.png")));
        tool_zoom_out_action->setIcon(QIcon(GET_ICON_PATH("minus.png")));
        tool_gate_library->setIcon(QIcon(GET_ICON_PATH("book.png")));
        area_selection_tool->setIcon(QIcon(GET_ICON_PATH("area_selection_tool.png")));
        wire_tool->setIcon(QIcon(GET_ICON_PATH("wire_tool.png")));
        rule_violations_action->setIcon(QIcon(GET_ICON_PATH("rule_violations.png")));
        modules_action->setIcon(QIcon(GET_ICON_PATH("modules.png")));
    }

    void MainWindow::reload_texts()
    {
        // Project menu
        project_menu->setTitle(tr("Project"));
        project_new_action->setText(tr("New"));
        project_import_action->setText(tr("Open"));
        project_export_action->setText(tr("Save"));
        project_recent_projects_submenu->setTitle(tr("Recent projects"));
        project_close_action->setText(tr("Close"));
        project_create_subproject_action->setText(tr("Create subproject from selection"));
        project_settings_action->setText(tr("Project settings"));
        project_quit_action->setText(tr("Quit"));

        // Edit menu
        edit_menu->setTitle(tr("Edit"));
        preferences_edit_action->setText(tr("Preferences"));

        // View menu
        view_menu->setTitle(tr("View"));
        show_gates_view_action->setText(tr("Show gates"));
        show_gates_name_view_action->setText(tr("Show gates name"));
        show_ports_view_action->setText(tr("Show ports"));
        show_ports_name_view_action->setText(tr("Show ports name"));
        show_annotations_view_action->setText(tr("Show annotations"));
        show_annotations_name_view_action->setText(tr("Show annotations name"));
        show_emarkers_view_action->setText(tr("Show emarkers"));
        show_emarkers_name_view_action->setText(tr("Show emarkers name"));
        show_vias_view_action->setText(tr("Show vias"));
        show_vias_name_view_action->setText(tr("Show vias name"));
        show_wires_view_action->setText(tr("Show wires"));
        grid_configuration_view_action->setText(tr("Grid configuration"));
        show_grid_view_action->setText(tr("Show grid"));
        snap_to_grid_view_action->setText(tr("Snap to grid"));
        fullscreen_view_action->setText(tr("Fullscreen"));

        // Layer menu
        layer_menu->setTitle(tr("Layer"));
        layers_edit_action->setText(tr("Edit layers"));
        background_import_action->setText(tr("Import background image"));

        // Gate menu
        gate_menu->setTitle(tr("Gate"));
        edit_gate_action->setText(tr("Edit selected"));
        new_gate_template_action->setText(tr("Create gate template from selection"));
        new_gate_action->setText(tr("Create gate from selection"));
        gate_library_action->setText(tr("Gate library"));
        gate_list_action->setText(tr("Gate list"));
        auto_name_gates_rows_action->setText(tr("Automatic naming along rows"));
        auto_name_gates_columns_action->setText(tr("Automatic naming along columns"));

        // Annotation menu
        annotation_menu->setTitle(tr("Annotation"));
        edit_annotation_action->setText(tr("Edit selected"));
        create_annotation_action->setText(tr("Create from selection"));
        annotation_list_action->setText(tr("Annotation list"));

        // EMarker menu
        emarker_menu->setTitle(tr("EMarker"));
        edit_emarker_action->setText(tr("Edit selected"));

        // Via menu
        via_menu->setTitle(tr("Via"));
        edit_via_action->setText(tr("Edit selected"));

        // Logic menu
        logic_menu->setTitle(tr("Logic"));
        remove_objects_action->setText(tr("Remove selected objects"));
        interconnect_objects_action->setText(tr("Interconnect selected objects"));
        isolate_objects_action->setText(tr("Isolate selected objects"));
        logic_menu->addSeparator();
        move_selected_gates_into_module->setText(tr("Move selected gates into module"));
        inspect_selected_object_action->setText(tr("Inspect selected object"));

        // Matching menu
        matching_menu->setTitle(tr("Matching"));
        template_matching_action->setText(tr("Template matching"));
        via_matching_action->setText(tr("Via matching"));
        wire_matching_action->setText(tr("Wire matching"));

        // Help menu
        help_menu->setTitle(tr("Help"));
        help_action->setText(tr("Help"));
        documentation_action->setText(tr("Documentation"));
        check_updates_action->setText(tr("Check for updates"));
        if (open_error_file_action != nullptr)
            open_error_file_action->setText(tr("Open error file location"));
        bug_report_action->setText(tr("Bug report"));
        degate_website_action->setText(tr("Degate's website"));
        about_action->setText(tr("About"));

        // Status bar
        update_status_bar_layer_info();

        // Window title
        update_window_title();

        // Tool bar
        tool_via_up_action->setText(tr("Layer up"));
        tool_via_down_action->setText(tr("Layer down"));
        tool_zoom_in_action->setText(tr("Zoom in"));
        tool_zoom_out_action->setText(tr("Zoom out"));
        tool_gate_library->setText(tr("Gate library"));
        area_selection_tool->setText(tr("Area selection tool"));
        wire_tool->setText(tr("Wire tool"));
        rule_violations_action->setText(tr("Rule violations"));
        modules_action->setText(tr("Modules"));

        close_sub_windows();
    }

    void MainWindow::on_menu_project_importer()
    {
        QString dir = QFileDialog::getExistingDirectory(this, tr("Import project"));

        if (dir.isNull())
        {
            status_bar.showMessage(tr("Project opening operation cancelled."), SECOND(DEFAULT_STATUS_MESSAGE_DURATION));

            return;
        }

        try
        {
            open_project(dir.toStdString());
        }
        catch (const std::exception&)
        {
            return;
        }

        if (project == nullptr)
            return;

        QString project_name = QString::fromStdString(project->get_name());

        QString message = tr("The project") + " <i>" + project_name + "</i> " + tr("was successfully loaded.");
        QMessageBox::information(this, tr("Import project"), message);
    }

    void MainWindow::on_menu_project_save()
    {
        if (project == nullptr)
            return;

        status_bar.showMessage(tr("Saving project..."));

        ProjectExporter exporter;
        exporter.export_all(project->get_project_directory(), project);

        status_bar.showMessage(tr("Project saved."), SECOND(DEFAULT_STATUS_MESSAGE_DURATION));

        project->set_changed(false);
        update_window_title();
    }

    void MainWindow::on_menu_project_close()
    {
        status_bar.showMessage(tr("Closing project..."));

        close_sub_windows();

        if (project == nullptr)
            return;

        if (project->is_changed())
        {
            QMessageBox msg_box(this);
            msg_box.setText(tr("The project has been modified."));
            msg_box.setInformativeText(tr("Do you want to save your changes ?"));
            msg_box.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
            msg_box.setDefaultButton(QMessageBox::Save);
            int ret = msg_box.exec();

            switch (ret)
            {
                case QMessageBox::Save:
                    on_menu_project_save();
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
        project = nullptr;

        workspace->set_project(nullptr);
        workspace->update_screen();

        update_window_title();

        update_status_bar_layer_info();


        reload_recent_projects_list();

        status_bar.showMessage(tr("Project closed."), SECOND(DEFAULT_STATUS_MESSAGE_DURATION));
    }

    void MainWindow::on_menu_project_new()
    {
        on_menu_project_close();

        status_bar.showMessage(tr("Creating a new project..."));

        NewProjectDialog dialog(this);

        int res;
        try
        {
            res = dialog.exec();
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
        }

        if (res == QDialog::Accepted)
        {
            project = dialog.get_project();

            project->get_logic_model()->set_current_layer(get_prev_enabled_layer(project->get_logic_model())->get_layer_pos());

            workspace->set_project(project);

            on_menu_project_save();

            status_bar.showMessage(tr("Created a new project."), SECOND(DEFAULT_STATUS_MESSAGE_DURATION));
        }
        else
        {
            status_bar.showMessage(tr("New project creation operation cancelled."), SECOND(DEFAULT_STATUS_MESSAGE_DURATION));
        }
    }

    void MainWindow::on_menu_project_create_subproject()
    {
        if (project == nullptr || !workspace->has_area_selection())
            return;

        boost::format f("subproject_%1%");
        f % project->get_logic_model()->get_new_object_id();

        SubProjectAnnotation_shptr new_annotation(new SubProjectAnnotation(workspace->get_safe_area_selection(), f.str()));
        new_annotation->set_fill_color(project->get_default_color(DEFAULT_COLOR_ANNOTATION));
        new_annotation->set_frame_color(project->get_default_color(DEFAULT_COLOR_ANNOTATION_FRAME));

        AnnotationEditDialog dialog(this, new_annotation);
        auto res = dialog.exec();

        if (res == QDialog::Accepted)
        {
            project->get_logic_model()->add_object(project->get_logic_model()->get_current_layer()->get_layer_pos(), new_annotation);

            workspace->reset_area_selection();
            workspace->update_screen();

            project_changed();
        }
        else
            new_annotation.reset();
    }

    void MainWindow::on_menu_edit_preferences()
    {
        PreferencesEditor dialog(this);
        dialog.exec();

        workspace->update_grid();
    }

    void MainWindow::on_menu_view_snap_to_grid(bool value)
    {
        Preferences new_preferences = PREFERENCES_HANDLER.get_preferences();
        new_preferences.snap_to_grid = value;

        PREFERENCES_HANDLER.update(new_preferences);
    }

    void MainWindow::on_menu_view_fullscreen(bool value)
    {
        if (value)
            showFullScreen();
        else
            showNormal();
    }

    void MainWindow::on_menu_layer_edit()
    {
        if (project == nullptr)
            return;

        LayersEditDialog layers_edit_dialog(this, project);
        layers_edit_dialog.exec();

        if (!layers_edit_dialog.project_need_reopen())
        {
            update_status_bar_layer_info();

            workspace->update_screen();

            project_changed();
        }
        else
        {
            reopen_project();
        }
    }

    void MainWindow::on_menu_layer_import_background()
    {
        if (project == nullptr)
        {
            status_bar.showMessage(tr("Failed to import new background image : no project opened."), SECOND(DEFAULT_STATUS_MESSAGE_DURATION));
            return;
        }

        status_bar.showMessage(tr("Importing a new background image for the layer..."));

        QString res = QFileDialog::getOpenFileName(this, tr("Select the new background image"));
        const std::string file_name = res.toStdString();

        if (res.isNull())
        {
            status_bar.showMessage(tr("New background image import cancelled."), SECOND(DEFAULT_STATUS_MESSAGE_DURATION));
            return;
        }

        // Start progress dialog
        ProgressDialog progress_dialog(this,
                                       tr("Importation and conversion of the new background image. "
                                          "This operation can take a lot of time, but will be performed only once."),
                                       nullptr);

        // Set the job to start the background loading.
        progress_dialog.set_job([file_name, this]()
                                {
                                    load_new_background_image(project->get_logic_model()->get_current_layer(), project->get_project_directory(), file_name);
                                });

        // Start the process
        progress_dialog.exec();

        if (progress_dialog.was_canceled())
            debug(TM, "The background image importation and conversion operation has been canceled.");

        // Create reader (to get image size).
        QImageReader reader(file_name.c_str());

        if (!project->update_size(reader.size().width(), reader.size().height()))
        {
            workspace->update_background();

            status_bar.showMessage(tr("Imported a new background image for the layer."), SECOND(DEFAULT_STATUS_MESSAGE_DURATION));

            project_changed();
        }
        else
        {
            reopen_project();
        }
    }

    void MainWindow::on_menu_gate_new_gate_template()
    {
        if (project == nullptr || !workspace->has_area_selection())
            return;

        if (project->get_logic_model()->get_current_layer()->get_layer_type() != Layer::LOGIC)
        {
            QMessageBox::warning(this, tr("Warning"), tr("You can create a new gate only on a logic layer."));
            return;
        }

        auto new_gate_template = std::make_shared<GateTemplate>(workspace->get_safe_area_selection()
                                                                         .get_width(),
                                                                workspace->get_safe_area_selection()
                                                                         .get_height());
        grab_template_images(project->get_logic_model(),
                             new_gate_template,
                             workspace->get_safe_area_selection());
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

        if (res == QDialog::Accepted)
        {
            project->get_logic_model()->add_object(project->get_logic_model()->get_current_layer()->get_layer_pos(), new_gate);
            project->get_logic_model()->update_ports(new_gate);

            workspace->reset_area_selection();
            workspace->update_gates();

            project_changed();
        }
        else
        {
            project->get_logic_model()->remove_gate_template(new_gate_template);
            new_gate_template.reset();
        }
    }

    void MainWindow::on_menu_gate_new_gate()
    {
        if (project == nullptr || !workspace->has_area_selection())
            return;

        if (project->get_logic_model()->get_current_layer()->get_layer_type() != Layer::LOGIC)
        {
            QMessageBox::warning(this, tr("Warning"), tr("You can create a new gate only on a logic layer."));
            return;
        }

        SelectGateTemplateDialog select_dialog(this, project, true);
        auto res = select_dialog.exec();

        if (res == QDialog::Rejected)
            return;

        GateTemplate_shptr gate_template = select_dialog.get_selected_gate();

        if (gate_template == nullptr)
            return;

        Gate_shptr new_gate(new Gate(workspace->get_safe_area_selection()));
        new_gate->set_gate_template(gate_template);

        GateInstanceEditDialog dialog(this, new_gate, project);
        res = dialog.exec();

        if (res == QDialog::Accepted)
        {
            project->get_logic_model()->add_object(project->get_logic_model()->get_current_layer()->get_layer_pos(),
                                                   new_gate);
            project->get_logic_model()->update_ports(new_gate);

            workspace->reset_area_selection();
            workspace->update_gates();

            project_changed();
        }
        else
        {
            new_gate.reset();
        }
    }

    void MainWindow::on_menu_gate_edit()
    {
        if (project == nullptr || !workspace->has_selection())
            return;

        if (Gate_shptr o = std::dynamic_pointer_cast<Gate>(workspace->get_selected_objects().back()))
        {
            GateInstanceEditDialog dialog(this, o, project);
            dialog.exec();

            project->get_logic_model()->update_ports(o);
        }

        workspace->update_gates();

        project_changed();
    }

    void MainWindow::on_menu_gate_library()
    {
        if (project == nullptr)
            return;

        GateLibraryDialog dialog(this, project);
        dialog.exec();

        workspace->update_gates();
    }

    void MainWindow::on_menu_gate_list()
    {
        if (project == nullptr)
            return;

        if (gate_list_dialog == nullptr)
        {
            gate_list_dialog = new GateListDialog(this, project);
            gate_list_dialog->setWindowFlags(Qt::Window);

            QObject::connect(gate_list_dialog,
                             SIGNAL(goto_object(PlacedLogicModelObject_shptr&)),
                             this,
                             SLOT(goto_object(PlacedLogicModelObject_shptr&)));
        }

        gate_list_dialog->show();
        gate_list_dialog->clearFocus();
    }

    void MainWindow::on_menu_gate_port_edit()
    {
        if (project == nullptr || !workspace->has_selection())
            return;

        if (GatePort_shptr o = std::dynamic_pointer_cast<GatePort>(workspace->get_selected_objects().back()))
        {
            {
                PortPlacementDialog dialog(this, project, o->get_gate()->get_gate_template(), o->get_template_port());
                dialog.exec();
            }

            project->get_logic_model()->update_ports(o->get_gate());
        }

        workspace->update_gates();

        project_changed();
    }

    void MainWindow::on_menu_gate_automatic_naming(AutoNameGates::ORIENTATION orientation)
    {
        if (project == nullptr)
            return;

        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this,
                                      tr("Automatic naming"),
                                      tr("The operation may destroy previously set names. "
                                         "Are you sure you want to name all gates?"),
                                      QMessageBox::Yes | QMessageBox::No);

        if (reply == QMessageBox::No)
            return;

        AutoNameGates auto_name(project->get_logic_model(), orientation);
        auto_name.run();

        workspace->update_gates();

        project_changed();
    }

    void MainWindow::on_menu_annotation_create()
    {
        if (project == nullptr || !workspace->has_area_selection())
            return;

        Annotation_shptr new_annotation(new Annotation(workspace->get_safe_area_selection()));
        new_annotation->set_fill_color(project->get_default_color(DEFAULT_COLOR_ANNOTATION));
        new_annotation->set_frame_color(project->get_default_color(DEFAULT_COLOR_ANNOTATION_FRAME));

        AnnotationEditDialog dialog(this, new_annotation);
        auto res = dialog.exec();

        if (res == QDialog::Accepted)
        {
            project->get_logic_model()->add_object(project->get_logic_model()->get_current_layer()->get_layer_pos(), new_annotation);

            workspace->reset_area_selection();
            workspace->update_annotations();

            project_changed();
        }
        else
            new_annotation.reset();
    }

    void MainWindow::on_menu_annotation_edit()
    {
        if (project == nullptr || !workspace->has_selection())
            return;

        if (Annotation_shptr o = std::dynamic_pointer_cast<Annotation>(workspace->get_selected_objects().back()))
        {
            AnnotationEditDialog dialog(this, o);
            dialog.exec();

            workspace->update_annotations();

            project_changed();
        }
    }

    void MainWindow::on_menu_annotation_list()
    {
        if (project == nullptr)
            return;

        if (annotation_list_dialog == nullptr)
        {
            annotation_list_dialog = new AnnotationListDialog(this, project);
            annotation_list_dialog->setWindowFlags(Qt::Window);

            QObject::connect(annotation_list_dialog,
                             SIGNAL(goto_object(PlacedLogicModelObject_shptr&)),
                             this,
                             SLOT(goto_object(PlacedLogicModelObject_shptr&)));
        }

        annotation_list_dialog->show();
        annotation_list_dialog->clearFocus();
    }

    void MainWindow::on_menu_emarker_edit()
    {
        if (project == nullptr || !workspace->has_selection())
            return;

        if (EMarker_shptr o = std::dynamic_pointer_cast<EMarker>(workspace->get_selected_objects().back()))
        {
            EMarkerEditDialog dialog(this, o);
            dialog.exec();

            workspace->update_emarkers();

            project_changed();
        }
    }

    void MainWindow::on_menu_via_edit()
    {
        if (project == nullptr || !workspace->has_selection())
            return;

        if (Via_shptr o = std::dynamic_pointer_cast<Via>(workspace->get_selected_objects().back()))
        {
            ViaEditDialog dialog(this, o, project);
            dialog.exec();

            workspace->update_vias();

            project_changed();
        }
    }

    void MainWindow::on_menu_logic_remove_selected_objects()
    {
        if (project == nullptr || !workspace->has_selection())
            return;

        auto objects = workspace->get_selected_objects();
        workspace->reset_selection();

        for (auto& object : objects)
        {
            if (object != nullptr && !std::dynamic_pointer_cast<GatePort>(object))
                project->get_logic_model()->remove_object(object);
        }

        workspace->update_objects();
        workspace->update_annotations();

        if (modules_dialog != nullptr)
            modules_dialog->reload();

        project_changed();
    }

    void MainWindow::on_menu_logic_interconnect_selected_objects()
    {
        if (project == nullptr || !workspace->has_selection() || workspace->get_selected_objects().size() < 2)
            return;

        auto objects = workspace->get_selected_objects();

        if (!objects.check_for_all(&is_interconnectable))
        {
            QMessageBox::warning(this, tr("Error during interconnect"), tr("One of the objects you selected cannot have connections at all."));
            return;
        }

        connect_objects(project->get_logic_model(), objects.begin(), objects.end());

        workspace->update_objects();

        project_changed();
    }

    void MainWindow::on_menu_logic_isolate_selected_objects()
    {
        if (project == nullptr || !workspace->has_selection())
            return;

        auto objects = workspace->get_selected_objects();

        if (!objects.check_for_all(&is_interconnectable))
        {
            QMessageBox::warning(this, tr("Error during interconnect"), tr("One of the objects you selected cannot have connections at all."));
            return;
        }

        isolate_objects<std::set<PlacedLogicModelObject_shptr>::iterator>(project->get_logic_model(),
                                                                          objects.begin(),
                                                                          objects.end());

        workspace->update_objects();

        project_changed();
    }

    void MainWindow::on_menu_logic_move_selected_gates_into_module()
    {
        if (project == nullptr || !workspace->has_selection())
            return;

        for (auto& e : workspace->get_selected_objects())
        {
            if (!is_of_object_type<Gate>(e))
                return;
        }

        ModuleSelectionDialog dialog(project, this);
        auto ret = dialog.exec();

        if (ret != QDialog::Accepted)
            return;

        auto selected_module = dialog.get_selected_module();

        if (selected_module == nullptr)
            return;

        for (auto& e : workspace->get_selected_objects())
        {
            if (Gate_shptr o = std::dynamic_pointer_cast<Gate>(e))
            {
                auto root_module = project->get_logic_model()->get_main_module();

                // It will search recursively, not only in the main module.
                root_module->remove_gate(o);
                selected_module->add_gate(o);
            }
        }

        project_changed();
    }

    void MainWindow::on_menu_logic_inspect_selected_object()
    {
        if (project == nullptr)
            return;

        if (connection_inspector_dialog == nullptr)
        {
            connection_inspector_dialog = new ConnectionInspector(this, project);
            connection_inspector_dialog->setWindowFlags(Qt::Window);

            QObject::connect(connection_inspector_dialog,
                             SIGNAL(goto_object(PlacedLogicModelObject_shptr&)),
                             this,
                             SLOT(goto_object(PlacedLogicModelObject_shptr&)));
        }

        if (!workspace->get_selected_objects().empty())
            connection_inspector_dialog->set_object(workspace->get_selected_objects().back());

        connection_inspector_dialog->show();
        connection_inspector_dialog->clearFocus();
    }

    void MainWindow::on_menu_matching_template_matching()
    {
        if (project == nullptr)
            return;

        SelectGateTemplateDialog select_dialog(this, project, false);
        auto res = select_dialog.exec();

        if (res == QDialog::Rejected)
            return;

        auto gate_template = select_dialog.get_selected_gates();

        if (gate_template.empty())
            return;

        BoundingBox bounding_box;

        if (workspace->has_area_selection())
            bounding_box = workspace->get_safe_area_selection();
        else
            bounding_box = project->get_bounding_box();

        TemplateMatchingDialog template_matching_dialog(this, bounding_box, project, gate_template);
        template_matching_dialog.exec();

        workspace->update_gates();

        project_changed();
    }

    void MainWindow::on_menu_matching_via_matching()
    {
        if (project == nullptr)
            return;

        BoundingBox bounding_box;

        if (workspace->has_area_selection())
            bounding_box = workspace->get_safe_area_selection();
        else
            bounding_box = project->get_bounding_box();

        ViaMatchingDialog via_matching_dialog(this, bounding_box, project);
        via_matching_dialog.exec();

        workspace->update_vias();

        project_changed();
    }

    void MainWindow::on_menu_matching_wire_matching()
    {
        if (project == nullptr)
            return;

        BoundingBox bounding_box;

        if (workspace->has_area_selection())
            bounding_box = workspace->get_safe_area_selection();
        else
            bounding_box = project->get_bounding_box();

        WireMatchingDialog wire_matching_dialog(this, bounding_box, project);
        wire_matching_dialog.exec();

        workspace->update_wires();

        project_changed();
    }

    void MainWindow::on_menu_help_open_help()
    {
        static auto add_shortcut = [](const QString& shortcut, const QString& text){
            return "<tr><td><strong>" + shortcut + "</strong></td><td>" + text + "</td></tr>";
        };

        const QString help_message =
                "<html><center><strong>" +
                tr("Shortcuts:") + "</strong><br><table cellpadding=\"5\">" +

                add_shortcut(tr("LEFT click:"), tr("Object selection")) +
                add_shortcut(tr("RIGHT click:"), tr("Context menu")) +
                add_shortcut(tr("Hold LEFT click:"), tr("Move")) +
                add_shortcut(tr("Hold RIGHT click:"), tr("Area selection OR wire tool")) +
                add_shortcut(tr("CTRL + LEFT click:"), tr("Multiple object selection")) +
                add_shortcut(tr("CTRL + hold RIGHT click:"), tr("Selection of all objects in the area")) +
                add_shortcut(tr("DEL:"), tr("Delete selected objects")) +

                "</table></center></html>";

        QMessageBox help(tr("Degate help"),
                         help_message,
                         QMessageBox::Icon::NoIcon,
                         QMessageBox::Button::Ok,
                         QMessageBox::Button::NoButton,
                         QMessageBox::Button::NoButton,
                         this
                        );

        auto* about_layout = help.findChild<QGridLayout*>();
        QMargins about_margins = about_layout->contentsMargins();
        about_margins.setRight(40);
        about_layout->setContentsMargins(about_margins);

        help.exec();
    }

    void MainWindow::on_menu_help_documentation()
    {
        if (std::string(DEGATE_VERSION_TYPE) == "release")
            QDesktopServices::openUrl(QUrl("https://degate.readthedocs.io"));
        else
            QDesktopServices::openUrl(QUrl("https://degate.readthedocs.io/en/nightly/"));
    }

    void MainWindow::on_menu_help_check_updates(bool notify_no_update, bool ask_disabling_automatic_check)
    {
        updater.run(notify_no_update, ask_disabling_automatic_check);
    }

    void MainWindow::on_menu_help_open_error_file()
    {
        QDesktopServices::openUrl(QUrl(QString::fromStdString(DEGATE_CONFIGURATION_PATH)));
    }

    void MainWindow::on_menu_help_bug_report()
    {
        QDesktopServices::openUrl(QUrl("https://github.com/DegateCommunity/Degate/issues/new?template=bug_report.md"));
    }

    void MainWindow::on_menu_help_degate_website()
    {
        QDesktopServices::openUrl(QUrl("https://degatecommunity.github.io"));
    }

    void MainWindow::on_menu_help_about()
    {
        AboutDialog about(this);
        about.exec();
    }

    void MainWindow::on_menu_project_settings()
    {
        if (project == nullptr)
            return;

        ProjectSettingsDialog dialog(this, project);
        dialog.exec();

        update_window_title();

        workspace->update_screen();

        project_changed();
    }

    void MainWindow::on_menu_project_quit()
    {
        close();
    }

    void MainWindow::update_window_title()
    {
        if (project != nullptr)
        {
            QString window_title = tr("Degate : %1 project") + " [*]";
            setWindowTitle(window_title.arg(QString::fromStdString(project->get_name())));
            setWindowModified(project->is_changed());
        }
        else
            setWindowTitle("Degate");
    }

    void MainWindow::update_status_bar_layer_info()
    {
        std::map<Layer::LAYER_TYPE, QString> types = {
                {Layer::UNDEFINED,  tr("Undefined")},
                {Layer::TRANSISTOR, tr("Transistor")},
                {Layer::LOGIC,      tr("Logic")},
                {Layer::METAL,      tr("Metal")}
        };

        if (project != nullptr)
        {
            QString status_bar_message = tr("Layer: %1/%2 (%3)");

            status_bar_layer.setText(status_bar_message
                    .arg(project->get_logic_model()->get_current_layer()->get_layer_pos() + 1)
                    .arg(project->get_logic_model()->get_num_layers())
                    .arg(types[project->get_logic_model()->get_current_layer()->get_layer_type()]));
        }
        else
        {
            status_bar_layer.setText(tr("Layer:") + " 0/0 (" + tr("none") + ")");
        }
    }

    void MainWindow::on_tool_via_up()
    {
        if (project == nullptr)
            return;

        project->get_logic_model()->set_current_layer(get_next_enabled_layer(project->get_logic_model())->get_layer_pos());

        update_status_bar_layer_info();

        workspace->update_screen();
    }

    void MainWindow::on_tool_via_down()
    {
        if (project == nullptr)
            return;

        project->get_logic_model()->set_current_layer(get_prev_enabled_layer(project->get_logic_model())->get_layer_pos());

        update_status_bar_layer_info();

        workspace->update_screen();
    }

    void MainWindow::open_project(const std::string& path)
    {
        on_menu_project_close();

        status_bar.showMessage(tr("Importing project/subproject..."));

        ProjectImporter project_importer;

        ProgressDialog progress_dialog(this, tr("Opening project"), nullptr);

        std::string error_message;
        bool error = false;

        try
        {
            std::shared_ptr<Project> imported_project = nullptr;

            // First try, in worker thread
            progress_dialog.set_job([&]
            {
                try
                {
                    imported_project = project_importer.import_all(path);
                }
                catch (const std::exception& e)
                {
                    error_message = e.what();
                    error = true;

                    return;
                }
            });
            progress_dialog.exec();

            // Second try, in the main thread
            if (error)
            {
                try
                {
                    imported_project = project_importer.import_all(path);
                }
                catch (const std::exception& e)
                {
                    // If fail, just throw and cancel project opening
                    throw std::runtime_error(error_message.c_str());
                }
            }

            if (imported_project == nullptr)
            {
                QMessageBox::StandardButton reply;
                reply = QMessageBox::question(this, tr("Subproject"),
                                              tr("The project/subproject do not exist, do you want to create it?"),
                                              QMessageBox::Yes | QMessageBox::No);

                if (reply == QMessageBox::Yes)
                {
                    if (project != nullptr)
                        on_menu_project_close();

                    NewProjectDialog dialog(this, "", path);
                    auto res = dialog.exec();

                    if (res == QDialog::Accepted)
                    {
                        project = dialog.get_project();

                        on_menu_project_save();
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
                if (project != nullptr)
                    on_menu_project_close();

                project = imported_project;
            }
        }
        catch (const std::exception&)
        {
            QMessageBox::warning(this,
                                 tr("Project/Subproject import failed"),
                                 tr("The project/subproject cannot be imported (maybe corrupted).") + "\n\n"
                                 + tr("Error") + ": " + QString::fromStdString(error_message));
            status_bar.showMessage(tr("Project/Subproject import failed."), SECOND(DEFAULT_STATUS_MESSAGE_DURATION));

            return;
        }

        // Load project.
        workspace->set_project(project);

        update_window_title();
        update_status_bar_layer_info();

        PREFERENCES_HANDLER.add_recent_project(project);
        reload_recent_projects_list();

        status_bar.showMessage(tr("Project/Subproject imported."), SECOND(DEFAULT_STATUS_MESSAGE_DURATION));
    }

    void MainWindow::change_status_bar_coords(int x, int y)
    {
        status_bar_coords.setText(tr("Coordinates") + " : " + QString::number(x) + "," + QString::number(y));
    }

    void MainWindow::show_context_menu()
    {
        if (project == nullptr)
            return;

        QMenu context_menu(tr("Context menu"), this);

        // New
        QAction annotation_create_context_action(tr("Create new annotation"), this);
        QAction gate_template_create_context_action(tr("Create new gate template"), this);
        QAction gate_create_context_action(tr("Create new gate"), this);
        QAction emarker_create_context_action(tr("Create new emarker"), this);
        QAction via_create_context_action(tr("Create new via"), this);

        // Edit
        QAction annotation_edit_context_action(tr("Edit selected annotation"), this);
        QAction gate_edit_context_action(tr("Edit selected gate"), this);
        QAction gate_port_edit_context_action(tr("Move selected gate port"), this);
        QAction emarker_edit_context_action(tr("Edit selected emarker"), this);
        QAction via_edit_context_action(tr("Edit selected via"), this);

        // Via
        QAction via_follow_context_action(tr("Follow via"), this);

        // Delete
        QAction delete_context_action(tr("Remove selected object(s)"), this);

        // Reset area
        QAction reset_selection_area_context_action(tr("Reset selection area"), this);

        // Reset wire tool
        QAction reset_wire_tool_context_action(tr("Reset wire tool"), this);

        // Inspect selected object
        QAction inspect_selected_object_context_action(tr("Inspect selected object"), this);

        // Interconnect selected objects
        QAction interconnect_selected_objects_context_action(tr("Interconnect selected objects"), this);

        // Isolate selected object(s)
        QAction isolate_selected_objects_context_action(tr("Isolate selected object(s)"), this);

        // Move selected gate(s) into a specific module.
        QAction move_selected_gates_into_module_context_action(tr("Move selected gate(s) into a module"), this);

        // Get current opengl mouse position
        context_menu_mouse_position = workspace->get_safe_opengl_mouse_position();

        if (workspace->has_area_selection())
        {
            connect(&annotation_create_context_action, SIGNAL(triggered()), this, SLOT(on_menu_annotation_create()));
            context_menu.addAction(&annotation_create_context_action);

            connect(&gate_template_create_context_action, SIGNAL(triggered()), this, SLOT(on_menu_gate_new_gate_template()));
            context_menu.addAction(&gate_template_create_context_action);

            connect(&gate_create_context_action, SIGNAL(triggered()), this, SLOT(on_menu_gate_new_gate()));
            context_menu.addAction(&gate_create_context_action);

            context_menu.addSeparator();

            connect(&reset_selection_area_context_action, SIGNAL(triggered()), workspace, SLOT(reset_area_selection()));
            context_menu.addAction(&reset_selection_area_context_action);
        }
        else if (workspace->has_selection())
        {
            PlacedLogicModelObject_shptr object = workspace->get_selected_objects().back();

            if (is_of_object_type<Annotation>(object))
            {
                connect(&annotation_edit_context_action, SIGNAL(triggered()), this, SLOT(on_menu_annotation_edit()));
                context_menu.addAction(&annotation_edit_context_action);
            }
            else if (is_of_object_type<Gate>(object))
            {
                connect(&gate_edit_context_action, SIGNAL(triggered()), this, SLOT(on_menu_gate_edit()));
                context_menu.addAction(&gate_edit_context_action);

                connect(&move_selected_gates_into_module_context_action, SIGNAL(triggered()), this, SLOT(on_menu_logic_move_selected_gates_into_module()));
                context_menu.addAction(&move_selected_gates_into_module_context_action);
            }
            else if (is_of_object_type<GatePort>(object))
            {
                connect(&gate_port_edit_context_action, SIGNAL(triggered()), this, SLOT(on_menu_gate_port_edit()));
                context_menu.addAction(&gate_port_edit_context_action);
            }
            else if (is_of_object_type<EMarker>(object))
            {
                connect(&emarker_edit_context_action, SIGNAL(triggered()), this, SLOT(on_menu_emarker_edit()));
                context_menu.addAction(&emarker_edit_context_action);
            }
            else if (Via_shptr via = std::dynamic_pointer_cast<Via>(object))
            {
                if (via->get_direction() == Via::DIRECTION_UP)
                {
                    connect(&via_follow_context_action, SIGNAL(triggered()), this, SLOT(on_tool_via_up()));
                    context_menu.addAction(&via_follow_context_action);
                }
                else
                {
                    connect(&via_follow_context_action, SIGNAL(triggered()), this, SLOT(on_tool_via_down()));
                    context_menu.addAction(&via_follow_context_action);
                }

                connect(&via_edit_context_action, SIGNAL(triggered()), this, SLOT(on_menu_via_edit()));
                context_menu.addAction(&via_edit_context_action);
            }

            connect(&delete_context_action, SIGNAL(triggered()), this, SLOT(on_menu_logic_remove_selected_objects()));
            context_menu.addAction(&delete_context_action);

            if (workspace->get_selected_objects().check_for_all(&is_interconnectable))
            {
                connect(&inspect_selected_object_context_action, SIGNAL(triggered()), this, SLOT(on_menu_logic_inspect_selected_object()));
                context_menu.addAction(&inspect_selected_object_context_action);

                context_menu.addSeparator();

                if (workspace->get_selected_objects().size() >= 2)
                {
                    connect(&interconnect_selected_objects_context_action, SIGNAL(triggered()), this, SLOT(on_menu_logic_interconnect_selected_objects()));
                    context_menu.addAction(&interconnect_selected_objects_context_action);
                }

                connect(&isolate_selected_objects_context_action, SIGNAL(triggered()), this, SLOT(on_menu_logic_isolate_selected_objects()));
                context_menu.addAction(&isolate_selected_objects_context_action);
            }
        }
        else
        {
            connect(&emarker_create_context_action, SIGNAL(triggered()), this, SLOT(on_emarker_create()));
            context_menu.addAction(&emarker_create_context_action);

            connect(&via_create_context_action, SIGNAL(triggered()), this, SLOT(on_via_create()));
            context_menu.addAction(&via_create_context_action);

            if (workspace->get_current_tool() == WIRE)
            {
                context_menu.addSeparator();

                connect(&reset_wire_tool_context_action, SIGNAL(triggered()), workspace, SLOT(reset_wire_tool()));
                context_menu.addAction(&reset_wire_tool_context_action);
            }
        }

        context_menu.exec(QCursor::pos());
    }

    void MainWindow::on_emarker_create()
    {
        if (project == nullptr)
            return;

        EMarker_shptr new_emarker = std::make_shared<EMarker>(context_menu_mouse_position.x(), context_menu_mouse_position.y());
        new_emarker->set_fill_color(project->get_default_color(DEFAULT_COLOR_EMARKER));

        EMarkerEditDialog dialog(this, new_emarker);
        auto res = dialog.exec();

        if (res == QDialog::Accepted)
        {
            project->get_logic_model()->add_object(project->get_logic_model()->get_current_layer()->get_layer_pos(), new_emarker);

            workspace->update_emarkers();

            project_changed();
        }
        else
            new_emarker.reset();
    }

    void MainWindow::on_via_create()
    {
        if (project == nullptr)
            return;

        Via_shptr new_via = std::make_shared<Via>(context_menu_mouse_position.x(), context_menu_mouse_position.y(), Via::DIRECTION_UNDEFINED);
        new_via->set_diameter(project->get_default_via_diameter());

        ViaEditDialog dialog(this, new_via, project);
        auto res = dialog.exec();

        if (res == QDialog::Accepted)
        {
            project->get_logic_model()->add_object(project->get_logic_model()->get_current_layer()->get_layer_pos(),
                                                   new_via);

            workspace->update_vias();

            project_changed();
        }
        else
            new_via.reset();
    }

    void MainWindow::on_grid_configuration()
    {
        if (project == nullptr)
            return;

        RegularGridConfigurationDialog dialog(this, project);
        dialog.exec();

        workspace->update_grid();
    }

    void MainWindow::project_changed()
    {
        if (project == nullptr)
            return;

        project->set_changed();
        update_window_title();
    }

    void MainWindow::auto_save()
    {
        if (project == nullptr)
            return;

        if (PREFERENCES_HANDLER.get_preferences().auto_save_status)
        {
            auto_save_timer.setInterval(PREFERENCES_HANDLER.get_preferences().auto_save_interval * 60000);

            on_menu_project_save();
        }
    }

    void MainWindow::goto_object(PlacedLogicModelObject_shptr& object)
    {
        if (object == nullptr || project == nullptr)
            return;

        const BoundingBox& bounding_box = object->get_bounding_box();
        Layer_shptr layer;

        // Do not switch layer, if user jumps to a gate or a gate port.
        if (std::dynamic_pointer_cast<GatePort>(object) || std::dynamic_pointer_cast<Gate>(object))
        {
            layer = project->get_logic_model()->get_current_layer();
        }
        else
        {
            layer = object->get_layer();
        }

        workspace->reset_selection();

        if (layer != nullptr && layer->is_enabled())
            project->get_logic_model()->set_current_layer(layer->get_layer_pos());

        workspace->center_view(QPointF(bounding_box.get_center_x(), bounding_box.get_center_y()));
        update_status_bar_layer_info();

        workspace->add_object_to_selection(object);
        workspace->update_type(object);
    }

    void MainWindow::on_rule_violations_dialog()
    {
        if (project == nullptr)
            return;

        if (rcv_dialog == nullptr)
        {
            rcv_dialog = new RuleViolationsDialog(this, project);
            rcv_dialog->setWindowFlags(Qt::Window);

            QObject::connect(rcv_dialog,
                             SIGNAL(goto_object(PlacedLogicModelObject_shptr&)),
                             this,
                             SLOT(goto_object(PlacedLogicModelObject_shptr&)));
        }

        rcv_dialog->run_checks();
        rcv_dialog->show();
        rcv_dialog->clearFocus();

        project_changed();
    }

    void MainWindow::on_modules_dialog()
    {
        if (project == nullptr)
            return;

        if (modules_dialog == nullptr)
        {
            modules_dialog = new ModulesDialog(this, project);
            modules_dialog->setWindowFlags(Qt::Window);

            QObject::connect(modules_dialog,
                             SIGNAL(goto_object(PlacedLogicModelObject_shptr&)),
                             this,
                             SLOT(goto_object(PlacedLogicModelObject_shptr&)));
        }

        modules_dialog->show();
        modules_dialog->clearFocus();

        project_changed();
    }

    void MainWindow::reopen_project()
    {
        if (project == nullptr)
            return;

        // Get project directory.
        auto project_directory = project->get_project_directory();

        // Save project.
        on_menu_project_save();

        // Close project.
        on_menu_project_close();

        // Reopen the project.
        open_project(project_directory);
    }

    void MainWindow::closeEvent(QCloseEvent* event)
    {
        // When the main window is closed, automatically close the rule violations dialog.
        // Since the dialog is modeless and not linked to this window, we have to force close.
        close_sub_windows();

        QMainWindow::closeEvent(event);
    }

    void MainWindow::close_sub_windows()
    {
        if (rcv_dialog != nullptr)
        {
            rcv_dialog->close();

            delete rcv_dialog;

            rcv_dialog = nullptr;
        }

        if (modules_dialog != nullptr)
        {
            modules_dialog->close();

            delete modules_dialog;

            modules_dialog = nullptr;
        }

        if (connection_inspector_dialog != nullptr)
        {
            connection_inspector_dialog->close();

            delete connection_inspector_dialog;

            connection_inspector_dialog = nullptr;
        }

        if (annotation_list_dialog != nullptr)
        {
            annotation_list_dialog->close();

            delete annotation_list_dialog;

            annotation_list_dialog = nullptr;
        }

        if (gate_list_dialog != nullptr)
        {
            gate_list_dialog->close();

            delete gate_list_dialog;

            gate_list_dialog = nullptr;
        }
    }

    void MainWindow::reload_recent_projects_list()
    {
        project_recent_projects_submenu->clear();

        const auto& list = PREFERENCES_HANDLER.get_recent_projects();

        for (auto it = list.rbegin(); it != list.rend(); it++)
        {
            auto element = *it;
            auto* action = project_recent_projects_submenu->addAction(QString::fromStdString(element.first + " (" + element.second + ")"));

            QObject::connect(action, &QAction::triggered, this, [=]()
            {
                 open_project(element.second);
            });
        }
    }
}
