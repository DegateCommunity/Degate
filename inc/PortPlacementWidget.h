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

#ifndef __PORTPLACEMENTWIDGET_H__
#define __PORTPLACEMENTWIDGET_H__

#include "ImageRenderer.h"
#include "GateTemplate.h"
#include "GateTemplatePort.h"
#include "Project.h"

#define DEFAULT_PORT_SIZE 8

namespace degate
{
	class PortPlacementWidget : public ImageRenderer
	{
		Q_OBJECT

	public:
		PortPlacementWidget(Project_shptr project, GateTemplate_shptr gate, GateTemplatePort_shptr port, QWidget* parent);
		~PortPlacementWidget();

		/**
	     * Update the screen.
	     */
		void update_screen();

	public slots:
		void next_layer();
		void previous_layer();

	protected:
		void initializeGL();
		void paintGL();
		void resizeGL(int w, int h);

		void mousePressEvent(QMouseEvent* event);
		void mouseReleaseEvent(QMouseEvent* event);
		void mouseMoveEvent(QMouseEvent* event);
		void wheelEvent(QWheelEvent* event);

	private:
		/*
		 * Create a port of a specific gate.
		 */
		void create_port(GateTemplatePort_shptr port);
		void draw_port_out(int x, int y, int size, QVector3D color, float alpha);
		void draw_port_in(int x, int y, int size, QVector3D color, float alpha);
		void draw_port_in_out(int x, int y, int size, QVector3D color, float alpha);

		GateTemplate_shptr gate;
		Project_shptr project;
		GateTemplatePort_shptr port;
		unsigned int layer;
		GLuint vbo;
		QOpenGLShaderProgram* program = NULL;

	};
}

#endif