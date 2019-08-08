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

#ifndef __WORKSPACESELECTIONTOOL_H__
#define __WORKSPACESELECTIONTOOL_H__

#include "Project.h"

#include <QtOpenGL/QtOpenGL>

namespace degate
{
	class WorkspaceSelectionTool
	{
	public:
		WorkspaceSelectionTool(QWidget* widget_parent);
		~WorkspaceSelectionTool();

		/*
		 * Init OpenGL routine (vbo).
		 */
		void init();

		/*
	     * Update all vbo with new selection values.
	     */
		void update(int actual_x, int actual_y);

		/*
	     * Draw the selection.
	     * 
	     * @param projection : the projection matrix to apply. 
	     */
		void draw(const QMatrix4x4& projection);

		/*
		 * Get the selection state, if true there is a selection otherwise not.
		 */
		bool is_selection();

		/*
		 * Set the selection state, if true there is a selection otherwise not.
		 */
		void set_selection(bool value);

		/*
		 * Get selection bounding box.
		 */
		BoundingBox get_selection_box();

		/*
		 * Set the origin point of the selection.
		 */
		void set_origin(int origin_x, int origin_y);

	private:
		QWidget* parent;
		QOpenGLShaderProgram* program = NULL;
		GLuint vbo;
		GLuint line_vbo;
		QOpenGLFunctions* context;
		bool selection = false;
		BoundingBox selection_box;
		QPoint origin;
	};
}

#endif