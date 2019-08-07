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

#ifndef __WORKSPACEBACKGROUND_H__
#define __WORKSPACEBACKGROUND_H__

#include "WorkspaceElement.h"

#include <vector>

namespace degate
{
	class WorkspaceBackground : public WorkspaceElement
	{
	public:
		WorkspaceBackground(QWidget* widget_parent);
		~WorkspaceBackground();

		/*
		 * Init all background OpenGL routine (vbo).
		 */
		void init() override;

		/*
	     * Update the background (all textures are reloaded).
	     */
		void update() override;

		/*
	     * Draw the background (all tiles will be draw).
	     * 
	     * @param projection : the projection matrix to apply. 
	     */
		void draw(const QMatrix4x4& projection) override;

		/*
		 * Destroy all OpenGL textures.
		 */
		void free_textures();

	private:
		/*
		 * Create a background tile (OpenGL texture and vertices).
		 * 
		 * @param x
		 * @param y
		 * @param pre_scaling : scaling of the image.
		 * @param index : index of the tile.
		 */
		GLuint create_background_tile(unsigned int x, unsigned int y, float pre_scaling, unsigned index);

		std::vector<GLuint> background_textures;
		BackgroundImage_shptr background_image = NULL;
	};
}

#endif