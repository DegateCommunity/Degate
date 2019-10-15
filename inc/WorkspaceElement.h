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

#ifndef __WORKSPACEELEMENT_H__
#define __WORKSPACEELEMENT_H__

#include "Project.h"

#include <QtOpenGL/QtOpenGL>

namespace degate
{

	/**
	 * @class WorkspaceElement
	 * @brief Virtual class to create a workspace element (with OpenGL routines functions)
	 *
	 * Every workspace element need to know the current active project and have 3 basic OpenGL functions (init, update and draw).
	 *
	 * @warning The OpenGL context of a workspace element is always the actual active context where the init function is called (@see WorkspaceElement::init).
	 *
	 * To set the actual active OpenGL context with Qt @see QOpenGLWidget::makeCurrent().
	 * An element initialized in a context can't be use in another one, so don't forgot to set the right context when switching (eg. @see ImageRenderer or @see WorkspaceRenderer).
	 *
	 * A workspace element has one vbo buffer by default (generally the shape buffer of the element).
	 */
	class WorkspaceElement
	{
	public:

		/**
		 * Create a workspace element.
		 * Every workspace element need to know at least his parent.
		 *
		 * @param parent : the widget parent pointer.
		 */
		WorkspaceElement(QWidget* parent);
		~WorkspaceElement();

		/**
		 * Set the actual project.
		 * 
		 * @param new_project : shared_ptr to the new project.
		 */
		void set_project(const Project_shptr& new_project);

		/**
		 * Init OpenGL routine (vbo and shaders).
		 *
		 * @warning Always call this parent init method in the child implementation.
		 */
		virtual void init();

		/**
	     * Update all vbo.
	     */
		virtual void update() = 0;

		/**
	     * Draw.
	     * 
	     * @param projection : the projection matrix to apply. 
	     */
		virtual void draw(const QMatrix4x4& projection) = 0;

	protected:
		QWidget* parent = NULL;
		Project_shptr project = NULL;
		QOpenGLShaderProgram* program = NULL;
		GLuint vbo = 0;
		QOpenGLFunctions* context = NULL;
	};
}

#endif