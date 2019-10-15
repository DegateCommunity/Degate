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

#include "WorkspaceElement.h"

namespace degate
{
	WorkspaceElement::WorkspaceElement(QWidget* parent)
	{
		this->parent = parent;
	}

	WorkspaceElement::~WorkspaceElement()
	{
		if (program != NULL)
			delete program;

		if(QOpenGLContext::currentContext() == NULL || context == NULL)
			return;

		context->glDeleteBuffers(1, &vbo);
	}

	void WorkspaceElement::set_project(const Project_shptr& new_project)
	{
		project = new_project;
	}

	void WorkspaceElement::init()
	{
		// The OpenGL context of a workspace element is always the current active context.
		context = QOpenGLContext::currentContext()->functions();

		context->glGenBuffers(1, &vbo);
	}
}
