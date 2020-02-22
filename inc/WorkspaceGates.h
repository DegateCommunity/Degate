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

#ifndef __WORKSPACEGATES_H__
#define __WORKSPACEGATES_H__

#include "WorkspaceElement.h"
#include "Text.h"

namespace degate
{

	/**
	 * @class WorkspaceGates
	 * @brief Prepare and draw all gates on the workspace.
	 *
	 * This will prepare all OpenGL things (buffers, shaders...) to draw all gates on the workspace.
	 * One gate is composed of a square, an outline, a top-left aligned text, ports and ports name.
	 *
	 * The parent vbo buffer will store all squares, the line_vbo buffer will store all outlines and the port_vbo buffer will store all ports.
	 *
	 * @see WorkspaceElement
	 */
	class WorkspaceGates : public WorkspaceElement
	{
	public:

		/**
		 * Create a workspace gates element.
		 * This will only set the parent, real creation will start with init and update functions.
		 *
		 * @param parent : the parent widget pointer.
		 */
		WorkspaceGates(QWidget* parent);
		~WorkspaceGates();

		/**
		 * Init all background OpenGL routine (vbo).
		 */
		void init() override;

		/**
	     * Update the background (all textures are reloaded).
	     */
		void update() override;

		/**
		 * Update a specific gate.
		 * 
		 * @param gate : the gate object.
		 */
		void update(Gate_shptr& gate);

		/**
		 * Update a specific port.
		 * 
		 * @param port : the port object.
		 */
		void update(GatePort_shptr& port);

		/**
	     * Draw all gates.
	     * 
	     * @param projection : the projection matrix to apply.
	     */
		void draw(const QMatrix4x4& projection) override;

		/**
		 * Draw gates name.
		 *
		 * @param projection : the projection matrix to apply.
		 */
		void draw_gates_name(const QMatrix4x4& projection);

		/**
		 * Draw ports.
		 *
		 * @param projection : the projection matrix to apply.
		 */
		void draw_ports(const QMatrix4x4& projection);

		/**
		 * Draw ports name.
		 *
		 * @param projection : the projection matrix to apply.
		 */
		void draw_ports_name(const QMatrix4x4& projection);

	private:
		/**
		 * Create a gate in OpenGL buffers.
		 *
		 * @param gate : the gate object.
		 * @param index : the index of the gate for OpenGL buffers.
		 */
		void create_gate(Gate_shptr& gate, unsigned index);

		/**
		 * Create all ports of a specific gate in OpenGL buffers.
		 *
		 * @param gate : the gate object.
		 * @param index : the index of the gate for OpenGL buffers.
		 */
		void create_ports(Gate_shptr& gate, unsigned index);

		Text text;
		Text port_text;
		GLuint line_vbo = 0;
		GLuint port_vbo = 0;
		unsigned port_count = 0;

	};
}

#endif