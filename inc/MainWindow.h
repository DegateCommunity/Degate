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

#include "ProjectImporter.h"
#include "WorkspaceRenderer.h"
#include "ProjectExporter.h"
#include "NewProjectDialog.h"
#include "GateEditDialog.h"
#include "LayersEditDialog.h"
#include "SelectGateTemplateDialog.h"
#include "GateLibraryDialog.h"
#include "ThemeManager.h"
#include "PreferencesHandler.h"
#include "PreferencesDialog.h"
#include "AnnotationEditDialog.h"

#include <degate.h>
#include <QMainWindow>
#include <QMenuBar>
#include <QMessageBox>
#include <QFileDialog>
#include <QToolBar>

#define DEFAULT_STATUS_MESSAGE_DURATION 10

namespace degate
{
	class MainWindow : public QMainWindow
	{
	Q_OBJECT

	public:
		MainWindow(int width = 0, int height = 0);
		~MainWindow();

	public slots:
		void on_menu_help_about();
		void on_menu_project_importer();
		void on_menu_project_exporter();
		void on_menu_project_close();
		void on_menu_project_new();
		void on_menu_project_create_subproject();
		void on_menu_edit_preferences();
		void on_menu_layer_edit();
		void on_menu_layer_import_background();
		void on_menu_gate_new_gate_template();
		void on_menu_gate_new_gate();
		void on_menu_gate_edit();
		void on_menu_gate_library();
		void on_menu_annotation_create();
		void on_menu_annotation_edit();
		void on_menu_logic_remove_selected_object();
		void on_menu_quit();
		void on_tool_via_up();
		void on_tool_via_down();
		void open_project(std::string path);
		void change_status_bar_coords(int x, int y);

	private:
		QMenuBar menu_bar;
		QToolBar* tool_bar;
		QStatusBar status_bar;
		QLabel status_bar_coords;

		Project_shptr project;
		WorkspaceRenderer* workspace;
	};
}

#endif
