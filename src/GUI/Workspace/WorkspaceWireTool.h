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

#ifndef __WORKSPACEWIRETOOL_H__
#define __WORKSPACEWIRETOOL_H__

#include "Core/Project/Project.h"
#include "Core/Primitive/Line.h"

#include <QtOpenGL/QtOpenGL>
#include <QtWidgets/QWidget>

namespace degate
{
    /**
	 * @class WorkspaceWireTool
	 * @brief Prepare, draw and manage the wire tool.
	 *
	 * This will draw the wire (when the drawing started) and manage the line (until the drawing end).
	 * The wire tool is composed of a square.
	 *
	 * The vbo buffer will store the line.
	 */
    class WorkspaceWireTool
    {
    public:

        /**
         * Create a workspace wire tool.
         * This will only set the parent, real creation will start with init and update functions.
         *
         * @param parent : the parent widget pointer.
         */
        WorkspaceWireTool(QWidget* parent);
        ~WorkspaceWireTool();

        /**
         * Init OpenGL routine (vbo).
         */
        void init();

        /**
         * Update all vbo with the new line extremity values.
         * The line will be draw from line.from_x to x and from line.from_y to y.
         *
         * @param x : the new extremity x coordinate.
         * @param y : the new extremity y coordinate.
         */
        void update(float x, float y);

        /**
         * Draw the selection.
         *
         * @param projection : the projection matrix to apply.
         */
        void draw(const QMatrix4x4& projection);

        /**
         * Start the line drawing, this will set the origin point.
         *
         * @param x : the x coordinate of the new origin.
         * @param y : the y coordinate of the new origin.
         */
        void start_line_drawing(float x, float y);

        /**
         * End the line drawing, the end point will be defined by the last update call.
         */
        void end_line_drawing();

         /**
          * Stop drawing the line and reset all. To start new line drawing call the start_line_drawing function.
          *
          * @see start_line_drawing
          */
         void reset_line_drawing();

        /**
         * Know if the line drawing started.
         *
         * @return Returns true if the line drawing started.
         */
        bool has_started();

        /**
         * Know if the line drawing ended.
         *
         * @return Returns true if the line drawing ended.
         */
        bool has_ended();

        /**
         * Get the drew line.
         *
         * @return Returns the drew line.
         */
        Line get_line();

        /**
         * Set the current active project.
         *
         * @param project : the current active project.
         */
        void set_project(const Project_shptr& new_project);

    private:
        Project_shptr project = nullptr;
        QWidget* parent = nullptr;
        QOpenGLShaderProgram* program = nullptr;
        GLuint vbo = 0;
        QOpenGLFunctions* context = nullptr;
        bool started = false;
        bool ended = false;
        Line line;
    };
}

#endif //__WORKSPACEWIRETOOL_H__
