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
		~MainWindow();

	public slots:

	    /**
	     * Load/Reload all icons of the main window.
	     */
	    void reload_icons();

		/* Project menu */
		
		/**
		 * Create and open the about window.
		 */
		void on_menu_help_about();

		/**
		 * Import a new project, it will force the user to find the project folder.
		 */
		void on_menu_project_importer();

		/**
		 * Export (save) the current project at the same place he where loaded.
		 */
		void on_menu_project_exporter();

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
		 * Quit the application without saving.
		 */
		void on_menu_project_quit();


		/* Edit menu */

		/**
		 * Open the preference window to update preferences.
		 */
		void on_menu_edit_preferences();


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
		 * Edit/Move the selected gate port.
		 */
		 void on_menu_gate_port_edit();

		
		/* Annotation menu */
		
		/**
		 * Create a new annotation from selection, this will create the annotation and then open the annotation edit dialog. 
		 */
		void on_menu_annotation_create();

		/**
		 * Open the annotation edit dialog for the selected annotation.
		 */
		void on_menu_annotation_edit();


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
		 * Remove the selected object from the logic model.
		 */
		void on_menu_logic_remove_selected_object();


		/* Other */
		
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
		void open_project(std::string path);

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

	private:
		QMenuBar menu_bar;
		QToolBar* tool_bar = NULL;
		QStatusBar status_bar;
		QLabel status_bar_coords;
		QLabel status_bar_layer;

		QActionGroup tools_group;

		Project_shptr project;
		WorkspaceRenderer* workspace = NULL;
		QPointF context_menu_mouse_position;

	private:

	    /* Menus */

	    // Project menu
        QAction* project_new_action;
        QAction* project_import_action;
        QAction* project_export_action;
        QAction* project_close_action;
        QAction* project_create_subproject_action;
        QAction* project_quit_action;

        // Edit menu
        QAction* preferences_edit_action;

        // View menu
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

        // Layer menu
        QAction* layers_edit_action;
        QAction* background_import_action;

        // Gate menu
        QAction* edit_gate_action;
        QAction* new_gate_template_action;
        QAction* new_gate_action;
        QAction* gate_library_action;

        // Annotation menu
        QAction* edit_annotation_action;
        QAction* create_annotation_action;

        // EMarker menu
        QAction* edit_emarker_action;

        // Via menu
        QAction* edit_via_action;

        // Logic menu
        QAction* remove_object_action;

        // Help menu
        QAction* about_action;


        /* Toolbar */
        QAction* tool_via_up_action;
        QAction* tool_via_down_action;
        QAction* tool_zoom_in_action;
        QAction* tool_zoom_out_action;
        QAction* tool_gate_library;
        QAction* area_selection_tool;
        QAction* wire_tool;
	};
}

#endif
