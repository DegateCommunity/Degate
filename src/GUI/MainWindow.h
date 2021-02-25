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

#ifndef __MAINWINDOW_H__
#define __MAINWINDOW_H__

#include "Core/Project/ProjectImporter.h"
#include "GUI/Workspace/WorkspaceRenderer.h"
#include "Core/Project/ProjectExporter.h"
#include "GUI/Dialog/NewProjectDialog.h"
#include "GUI/Dialog/GateEditDialog.h"
#include "GUI/Dialog/LayersEditDialog.h"
#include "GUI/Dialog/SelectGateTemplateDialog.h"
#include "GUI/Dialog/GateLibraryDialog.h"
#include "GUI/Preferences/ThemeManager.h"
#include "GUI/Preferences/PreferencesHandler.h"
#include "GUI/Preferences/PreferencesEditor.h"
#include "GUI/Dialog/AnnotationEditDialog.h"
#include "GUI/Dialog/EMarkerEditDialog.h"
#include "GUI/Dialog/ProjectSettingsDialog.h"
#include "GUI/Dialog/TemplateMatchingDialog.h"
#include "GUI/Dialog/ViaMatchingDialog.h"
#include "GUI/Dialog/WireMatchingDialog.h"
#include "GUI/Dialog/RegularGridConfigurationDialog.h"
#include "GUI/Dialog/RuleViolationsDialog.h"
#include "GUI/Dialog/ModulesDialog.h"
#include "GUI/Dialog/ConnectionInspector.h"
#include "Core/LogicModel/Gate/AutoNameGates.h"
#include "GUI/Dialog/AnnotationListDialog.h"
#include "GUI/Dialog/GateListDialog.h"
#include "GUI/Utils/Updater.h"

#include <QMainWindow>
#include <QMenuBar>
#include <QMessageBox>
#include <QFileDialog>
#include <QToolBar>

/**
 * This define the default status message duration for the status bar.
 *
 * @see MainWindow
 */
#define DEFAULT_STATUS_MESSAGE_DURATION 30

namespace degate
{

    /**
     * @class MainWindow
     * @brief The Qt main window of Degate.
     *
     * The class hold the menu, workspace, status bar and more.
     *
     * @see QMainWindow
     */
    class MainWindow : public QMainWindow
    {
    Q_OBJECT

    public:

        /**
         * Create the main window, if width and height are equal to 0 then the size will be 70% of the available screen space.
         *
         * @param width : the desired window with.
         * @param height : the desired window height.
         */
        MainWindow(int width = 0, int height = 0);
        ~MainWindow() override;

    public slots:

        /**
         * Load/Reload all icons of the main window.
         */
        void reload_icons();

        /**
         * Load/Reload all texts of the main window (useful when language changed).
         */
        void reload_texts();

        /* Project menu */

        /**
         * Import a new project, it will force the user to find the project folder.
         */
        void on_menu_project_importer();

        /**
         * Save (export) the current project at the same place he where loaded.
         */
        void on_menu_project_save();

        /**
         * Close the current project (if one is opened).
         */
        void on_menu_project_close();

        /**
         * Create a new project, it will force the user to select a project folder.
         */
        void on_menu_project_new();

        /**
         * Create a subproject from selection.
         */
        void on_menu_project_create_subproject();

        /**
         * Open the project settings dialog.
         */
        void on_menu_project_settings();

        /**
         * Quit the application without saving.
         */
        void on_menu_project_quit();


        /* Edit menu */

        /**
         * Open the preference window to update preferences.
         */
        void on_menu_edit_preferences();


        /* Show menu */

        /**
         * Enable/disable snap to grid.
         */
        void on_menu_view_snap_to_grid(bool value);

        /**
         * Enable/disable fullscreen mode.
         */
        void on_menu_view_fullscreen(bool value);


        /* Layer menu */

        /**
         * Open a new window to edit layers properties of the current project.
         */
        void on_menu_layer_edit();

        /**
         * Import a new background image for the current layer (this open a new window to select the background image).
         */
        void on_menu_layer_import_background();


        /* Gate menu */

        /**
         * Create a new gate template from selection, this will create the gate template, create the gate instance and then open the gate instance edit dialog.
         */
        void on_menu_gate_new_gate_template();

        /**
         * Create a new gate from selection, this will open a window to select a gate template, create the gate and then open the gate instance edit dialog.
         */
        void on_menu_gate_new_gate();

        /**
         * Open the gate edit instance dialog for the selected gate.
         */
        void on_menu_gate_edit();

        /**
         * Open the gate library window, this will list all gates template of the project.
         */
        void on_menu_gate_library();

        /**
          * Open/show a dialog that lists all gates.
          */
        void on_menu_gate_list();

        /**
         * Edit/Move the selected gate port.
         */
         void on_menu_gate_port_edit();

         /**
          * Auto name gates along rows or columns.
          *
          * @param orientation : auto name in rows or columns.
          */
         void on_menu_gate_automatic_naming(AutoNameGates::ORIENTATION orientation);


        /* Annotation menu */

        /**
         * Create a new annotation from selection, this will create the annotation and then open the annotation edit dialog.
         */
        void on_menu_annotation_create();

        /**
         * Open the annotation edit dialog for the selected annotation.
         */
        void on_menu_annotation_edit();

        /**
         * Open/show a dialog that lists all annotations.
         */
        void on_menu_annotation_list();


        /* EMarker menu */

        /**
         * Open the emarker edit dialog for the selected emarker.
         */
        void on_menu_emarker_edit();


        /* Via menu */

        /**
         * Open the via edit dialog for the selected via.
         */
        void on_menu_via_edit();


        /* Logic menu */

        /**
         * Remove all selected objects from the logic model.
         */
        void on_menu_logic_remove_selected_objects();

        /**
         * Connect all selected objects together.
         */
        void on_menu_logic_interconnect_selected_objects();

        /**
         * Break connections of all selected objects (isolation).
         */
        void on_menu_logic_isolate_selected_objects();

        /**
         * Move selected gate(s) into a specific module.
         * It will open a module selector dialog.
         */
        void on_menu_logic_move_selected_gates_into_module();

        /**
         * Open connection inspector dialog.
         */
        void on_menu_logic_inspect_selected_object();


        /* Matching menu */

        /**
         * Start a template matching process.
         */
        void on_menu_matching_template_matching();

        /**
         * Start a via matching process.
         */
        void on_menu_matching_via_matching();

        /**
         * Start a wire matching process.
         */
        void on_menu_matching_wire_matching();


        /* Help menu */

        /**
         * Create and open the help window.
         */
        void on_menu_help_open_help();

        /**
         * Redirect to the Degate documentation URL.
         */
        void on_menu_help_documentation();

        /**
         * Check for updates (it will only work if Degate was installed with an installer (need maintenance tool of Qt Ifw).
         *
         * @param notify_no_update : if true, will notify the user if no update is available.
         * @param ask_disabling_automatic_check : if true, will ask if the user want to disable automatic check for updates.
         */
        void on_menu_help_check_updates(bool notify_no_update, bool ask_disabling_automatic_check);

        /**
         * Create and open the about window.
         */
        void on_menu_help_about();


        /* Other */

        /**
         * Update the window title.
         */
        void update_window_title();

        /**
         * Update the status bar layer info.
         */
        void update_status_bar_layer_info();

        /**
         * Change the current layer to the next (up) layer.
         */
        void on_tool_via_up();

        /**
         * Change the current layer to the previous (down) layer.
         */
        void on_tool_via_down();

        /**
         * Open a project with a path already specified.
         *
         * @param path : the path of the project to open.
         *
         * @see WorkspaceRenderer::project_changed
         */
        void open_project(const std::string& path);

        /**
         * Update the mouse coordinates on the status bar.
         *
         * @param x : the new x coordinate.
         * @param y : the new y coordinate.
         *
         * @see WorkspaceRenderer::mouse_coords_changed
         */
        void change_status_bar_coords(int x, int y);

        /**
         * Show a context menu on mouse.
         */
        void show_context_menu();

        /**
         * Create a new emarker at mouse position, this will create the emarker and then open the emarker edit dialog.
         */
        void on_emarker_create();

        /**
         * Create a new via at mouse position, this will create the via and then open the via edit dialog.
         */
        void on_via_create();

        /**
         * Show the grid configuration dialog.
         */
        void on_grid_configuration();

        /**
         * Call this when the project change (saved version != current version).
         */
        void project_changed();

        /**
         * Called when it's time to auto save (linked to the auto_save_timer).
         */
        void auto_save();

        /**
         * Center view on a specific object.
         *
         * @param object : the object to center view on.
         */
        void goto_object(PlacedLogicModelObject_shptr& object);

        /**
         * Open rule violations dialog.
         */
        void on_rule_violations_dialog();

        /**
         * Open modules dialog.
         */
        void on_modules_dialog();

        /**
         * Reopen the project.
         */
        void reopen_project();

    protected:
        void closeEvent(QCloseEvent* event) override;

        /**
         * Close and delete all remaining sub windows.
         * For example it will close and delete rcv_dialog and modules_dialog.
         */
        void close_sub_windows();

        /**
         * Reload the recent projects list of the 'Project' menu.
         */
        void reload_recent_projects_list();

    private:
        QMenuBar menu_bar;
        QToolBar* tool_bar = nullptr;
        QStatusBar status_bar;
        QLabel status_bar_coords;
        QLabel status_bar_layer;

        QActionGroup tools_group;

        Project_shptr project;
        WorkspaceRenderer* workspace = nullptr;
        QPointF context_menu_mouse_position;

    private:

        /* Menus */

        // Project menu
        QMenu* project_menu;
        QAction* project_new_action;
        QAction* project_import_action;
        QAction* project_export_action;
        QMenu* project_recent_projects_submenu;
        QAction* project_close_action;
        QAction* project_create_subproject_action;
        QAction* project_settings_action;
        QAction* project_quit_action;

        // Edit menu
        QMenu* edit_menu;
        QAction* preferences_edit_action;

        // View menu
        QMenu* view_menu;
        QAction* show_gates_view_action;
        QAction* show_gates_name_view_action;
        QAction* show_ports_view_action;
        QAction* show_ports_name_view_action;
        QAction* show_annotations_view_action;
        QAction* show_annotations_name_view_action;
        QAction* show_emarkers_view_action;
        QAction* show_emarkers_name_view_action;
        QAction* show_vias_view_action;
        QAction* show_vias_name_view_action;
        QAction* show_wires_view_action;
        QAction* grid_configuration_view_action;
        QAction* show_grid_view_action;
        QAction* snap_to_grid_view_action;
        QAction* fullscreen_view_action;

        // Layer menu
        QMenu* layer_menu;
        QAction* layers_edit_action;
        QAction* background_import_action;

        // Gate menu
        QMenu* gate_menu;
        QAction* edit_gate_action;
        QAction* new_gate_template_action;
        QAction* new_gate_action;
        QAction* gate_library_action;
        QAction* gate_list_action;
        QAction* auto_name_gates_rows_action;
        QAction* auto_name_gates_columns_action;

        // Annotation menu
        QMenu* annotation_menu;
        QAction* edit_annotation_action;
        QAction* create_annotation_action;
        QAction* annotation_list_action;

        // EMarker menu
        QMenu* emarker_menu;
        QAction* edit_emarker_action;

        // Via menu
        QMenu* via_menu;
        QAction* edit_via_action;

        // Logic menu
        QMenu* logic_menu;
        QAction* remove_objects_action;
        QAction* interconnect_objects_action;
        QAction* isolate_objects_action;
        QAction* move_selected_gates_into_module;
        QAction* inspect_selected_object_action;

        // Template matching menu
        QMenu* matching_menu;
        QAction* template_matching_action;
        QAction* via_matching_action;
        QAction* wire_matching_action;

        // Help menu
        QMenu* help_menu;
        QAction* help_action;
        QAction* documentation_action;
        QAction* check_updates_action;
        QAction* about_action;


        /* Toolbar */
        QAction* tool_via_up_action;
        QAction* tool_via_down_action;
        QAction* tool_zoom_in_action;
        QAction* tool_zoom_out_action;
        QAction* tool_gate_library;
        QAction* area_selection_tool;
        QAction* wire_tool;
        QAction* rule_violations_action;
        QAction* modules_action;

        // QTimer for auto save
        QTimer auto_save_timer;

        /* Dialogs */
        RuleViolationsDialog* rcv_dialog = nullptr;
        ModulesDialog* modules_dialog = nullptr;
        ConnectionInspector* connection_inspector_dialog = nullptr;
        AnnotationListDialog* annotation_list_dialog = nullptr;
        GateListDialog* gate_list_dialog = nullptr;

        Updater updater;
    };
}

#endif
