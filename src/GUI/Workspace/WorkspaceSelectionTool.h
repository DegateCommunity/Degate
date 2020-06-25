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

#include "Core/Project/Project.h"

#include <QtOpenGL/QtOpenGL>

namespace degate
{

	/**
	 * @class WorkspaceSelectionTool
	 * @brief Prepare, draw and manage the selection tool.
	 *
	 * This will draw the selection tool (when there is a selection) and manage the bounding box of the selection.
	 * The selection tool is composed of a square and a outline.
	 *
	 * The vbo buffer will store the square and the line_vbo buffer will store the outline.
	 *
	 * The origin point will store the origin of the selection tool.
	 * The selection_box will store the bounding box of the selection from the origin to the last (x,y) coordinates of the update function.
	 */
	class WorkspaceSelectionTool
	{
	public:

		/**
		 * Create a workspace selection tool.
		 * This will only set the parent, real creation will start with init and update functions.
		 *
		 * @param parent : the parent widget pointer.
		 */
		WorkspaceSelectionTool(QWidget* parent);
		~WorkspaceSelectionTool();

		/**
		 * Init OpenGL routine (vbo).
		 */
		void init();

		/**
	     * Update all vbo with new selection values.
	     * The square will be draw from origin.x to x and from origin.y to y.
	     *
	     * @param x : the new x coordinate.
	     * @param y : the new y coordinate.
	     */
		void update(float x, float y);

		/**
	     * Draw the selection.
	     * 
	     * @param projection : the projection matrix to apply. 
	     */
		void draw(const QMatrix4x4& projection);

		/**
		 * Get the selection state, if true there is a selection otherwise not.
		 *
		 * @return Returns true if there is a selection, otherwise not.
		 */
		bool has_selection();

		/**
		 * Set the selection state, if true there is a selection otherwise not.
		 *
		 * @param value : the new selection state.
		 */
		void set_selection_state(bool value);

        /**
         * Know if the object selection mode is active.
         * It is just a graphic effect that will hide the frame.
         *
         * @return Returns true if the object selection mode is active, otherwise not.
         */
        bool is_object_selection_mode_active();

        /**
         * Set the state of the object selection mode, if true this will activate the object selection mode otherwise not.
         * It is just a graphic effect that will hide the frame.
         *
         * @param value : the new object selection mode state.
         */
        void set_object_selection_mode_state(bool value);

		/**
		 * Get selection bounding box.
		 *
		 * @return Returns the selection bounding box.
		 */
		BoundingBox get_selection_box();

		/**
		 * Set the origin point of the selection.
		 *
		 * @param x : the x coordinate of the new origin.
		 * @param y : the y coordinate of the new origin.
		 */
		void set_origin(float x, float y);

        /**
         * Set the current active project.
         *
         * @param project : the current active project.
         */
        void set_project(const Project_shptr& new_project);

	private:
		QWidget* parent;

		QOpenGLShaderProgram* program = nullptr;
        QOpenGLFunctions* context = nullptr;
        Project_shptr project = nullptr;

		GLuint vbo = 0;
		GLuint line_vbo = 0;

		BoundingBox selection_box;
		QPointF origin;

		// Is there an area selection active
        bool selection = false;

        // Is the object selection mode active (CTRL + drag)
        bool object_selection_mode = false;
	};
}

#endif