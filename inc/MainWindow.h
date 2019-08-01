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

#include <degate.h>
#include <QMainWindow>
#include <QMenuBar>
#include <QMessageBox>
#include <QFileDialog>
#include <QLabel>

namespace degate
{
	class MainWindow : public QMainWindow
	{
		Q_OBJECT
		
	public:
		MainWindow(int width = 0, int height = 0);
		~MainWindow();

	public slots:
		void on_menu_about_degate();
		void on_menu_project_importer();

	private:
		QMenuBar menu_bar;

		Project_shptr project;
		WorkspaceRenderer* workspace;
	};
}

#endif