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

#include <Core/Image/ImageRenderer.h>
#include <Core/LogicModel/Gate/GateTemplate.h>
#include <Core/LogicModel/Gate/GateTemplatePort.h>
#include <Core/Project/Project.h>
#include <GUI/Text/Text.h>

/**
 * Define the default port size for port placement.
 */
#define DEFAULT_PORT_SIZE 8

namespace degate
{

	/**
	 * @class PortPlacementWidget
	 * @brief Widget to place a port template of a gate template.
	 *
	 * @warning Each port placement widget get his own OpenGL context (@see ImageRenderer).
	 *
	 * @see GateEditDialog
	 */
	class PortPlacementWidget : public ImageRenderer
	{
		Q_OBJECT

	public:

		/**
		 * Create a port placement widget.
		 *
		 * @param project : the current active project.
		 * @param gate : the gate template of the port template to place.
		 * @param port : the gate template port to place.
		 * @param parent : the parent of the widget.
		 */
		PortPlacementWidget(Project_shptr project, GateTemplate_shptr gate, GateTemplatePort_shptr port, QWidget* parent);
		~PortPlacementWidget();

		/**
	     * Update the screen.
	     */
		void update_screen();

		/**
		 * Get the new port template position.
		 *
		 * @return Returns the new port template position.
		 */
		Point get_new_port_position();

	public slots:
		/**
		 * Go to the next layer on the image renderer.
		 */
		void next_layer();

		/**
		 * Go to the previous layer on the image renderer.
		 */
		void previous_layer();

	protected:
		/* Qt OpenGL functions */
		void initializeGL();
		void paintGL();
		void resizeGL(int w, int h);

		/* Qt mouse and keyboard events functions */
		void mousePressEvent(QMouseEvent* event);
		void mouseReleaseEvent(QMouseEvent* event);
		void mouseMoveEvent(QMouseEvent* event);
		void wheelEvent(QWheelEvent* event);

	private:
		/**
		 * Create a port of a specific gate.
		 *
		 * @param port : the gate template port to create.
		 */
		void create_port(GateTemplatePort_shptr port);

		/**
		 * Create a out port.
		 *
		 * @param x : the x coordinate of the port.
		 * @param y : the y coordinate of the port.
		 * @param size : the size of the port.
		 * @param color : the color of the port.
		 * @param alpha : the opacity of the port (between 0 and 1).
		 */
		void create_port_out(float x, float y, unsigned size, QVector3D color, float alpha);

		/**
		 * Create a in port.
		 *
		 * @param x : the x coordinate of the port.
		 * @param y : the y coordinate of the port.
		 * @param size : the size of the port.
		 * @param color : the color of the port.
		 * @param alpha : the opacity of the port (between 0 and 1).
		 */
		void create_port_in(float x, float y, unsigned size, QVector3D color, float alpha);

		/**
		 * Create a in/out port.
		 *
		 * @param x : the x coordinate of the port.
		 * @param y : the y coordinate of the port.
		 * @param size : the size of the port.
		 * @param color : the color of the port.
		 * @param alpha : the opacity of the port (between 0 and 1).
		 */
		void create_port_in_out(float x, float y, unsigned size, QVector3D color, float alpha);

		GateTemplate_shptr gate;
		Project_shptr project;
		GateTemplatePort_shptr port;
		unsigned int layer;
		GLuint vbo;
		QOpenGLShaderProgram* program = NULL;
		Text port_name_text;

		Point pos;

	};
}

#endif