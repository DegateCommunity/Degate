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

#ifndef __WORKSPACEANNOTATIONS_H__
#define __WORKSPACEANNOTATIONS_H__

#include "WorkspaceElement.h"
#include "WorkspaceText.h"

namespace degate
{
	class WorkspaceAnnotations : public WorkspaceElement
	{
	public:
		WorkspaceAnnotations(QWidget* new_parent);
		~WorkspaceAnnotations();

		/*
		 * Init all background OpenGL routine (vbo).
		 */
		void init() override;

		/*
	     * Update all annotations.
	     */
		void update() override;

		/*
	     * Draw all annotations.
	     * 
	     * @param projection : the projection matrix to apply.
	     */
		void draw(const QMatrix4x4& projection) override;

	private:
		/*
		 * Create an annotation.
		 */
		void create_annotation(Annotation_shptr& annotation, unsigned indice);

		GLuint line_vbo;
		WorkspaceText text;

	};
}

#endif