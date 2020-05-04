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

#ifndef __WORKSPACEVIA_H__
#define __WORKSPACEVIA_H__

#include "WorkspaceElement.h"
#include "Core/Via.h"
#include "Text.h"

namespace degate
{
    /**
	 * @class WorkspaceVias
	 * @brief Prepare and draw all vias of the active layer on the workspace.
	 *
	 * The parent vbo buffer will store all squares.
	 *
	 * @see WorkspaceElement
	 */
    class WorkspaceVias : public WorkspaceElement
    {
    public:

        /**
		 * Create a workspace vias element.
		 * This will only set the parent, real creation will start with init and update functions.
		 *
		 * @param parent : the parent widget pointer.
		 */
        WorkspaceVias(QWidget* parent);
        ~WorkspaceVias();

        /**
		 * Init all OpenGL routine (buffers, shaders...).
		 */
        void init() override;

        /**
         * Update all vias (fill buffers).
         */
        void update() override;

        /**
         * Update a specific via (update buffers).
         *
         * @warning Call the update() function before.
         *
         * @param via : the via to update.
         */
        void update(Via_shptr& via);

        /**
         * Draw all vias (draw the square and outline buffers).
         *
         * @param projection : the projection matrix to apply.
         */
        void draw(const QMatrix4x4& projection) override;

        /**
         * Draw vias name (draw the text).
         *
         * @param projection : the projection matrix to apply.
         */
        void draw_name(const QMatrix4x4& projection);

    private:
        /**
		 * Create an via in OpenGL buffers.
		 *
		 * @param via : the via object.
		 * @param index : the index of the annotation for OpenGL buffers.
		 */
        void create_via(Via_shptr& via, unsigned index);

        Text text;
        unsigned vias_count = 0;

    };
}

#endif //__WORKSPACEVIA_H__
