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

#ifndef __WORKSPACEWIRES_H__
#define __WORKSPACEWIRES_H__

#include "GUI/Workspace/WorkspaceElement.h"
#include "Core/LogicModel/Wire/Wire.h"
#include "GUI/Text/Text.h"

namespace degate
{

    /**
	 * @class WorkspaceEMarkers
	 * @brief Prepare and draw all wires of the active layer on the workspace.
	 *
	 * The parent vbo buffer will store all squares.
	 *
	 * @see WorkspaceElement
	 */
    class WorkspaceWires : public WorkspaceElement
    {
    public:

        /**
		 * Create a WorkspaceWires element.
		 * This will only set the parent, real creation will start with init and update functions.
		 *
		 * @param parent : the parent widget pointer.
		 */
        WorkspaceWires(QWidget* parent);
        ~WorkspaceWires();

        /**
		 * Init all OpenGL routine (buffers, shaders...).
		 */
        void init() override;

        /**
         * Update all wires (fill buffers).
         */
        void update() override;

        /**
         * Update a specific wire (update buffers).
         *
         * @warning Call the update() function before.
         *
         * @param wire : the wire to update.
         */
        void update(Wire_shptr& wire);

        /**
         * Draw all wires (draw the square and outline buffers).
         *
         * @param projection : the projection matrix to apply.
         */
        void draw(const QMatrix4x4& projection) override;


    private:
        /**
		 * Create an wire in OpenGL buffers.
		 *
		 * @param wire : the wire object.
		 * @param index : the index of the annotation for OpenGL buffers.
		 */
        void create_wire(Wire_shptr& wire, unsigned index);

        unsigned wires_count = 0;

    };
}

#endif //__WORKSPACEWIRES_H__
