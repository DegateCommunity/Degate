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

#ifndef __WORKSPACEEMARKER_H__
#define __WORKSPACEEMARKER_H__

#include "WorkspaceElement.h"
#include "EMarker.h"
#include "Text.h"

namespace degate
{

    /**
	 * @class WorkspaceEMarkers
	 * @brief Prepare and draw all emarkers of the active layer on the workspace.
	 *
	 * The parent vbo buffer will store all squares.
	 *
	 * @see WorkspaceElement
	 */
    class WorkspaceEMarkers : public WorkspaceElement
    {
    public:

        /**
		 * Create a WorkspaceEMarkers element.
		 * This will only set the parent, real creation will start with init and update functions.
		 *
		 * @param parent : the parent widget pointer.
		 */
        WorkspaceEMarkers(QWidget* parent);
        ~WorkspaceEMarkers();

        /**
		 * Init all OpenGL routine (buffers, shaders...).
		 */
        void init() override;

        /**
         * Update all emarkers (fill buffers).
         */
        void update() override;

        /**
         * Update a specific emarker (update buffers).
         *
         * @warning Call the update() function before.
         *
         * @param emarker : the emarker to update.
         */
        void update(EMarker_shptr& emarker);

        /**
         * Draw all emarkers (draw the square and outline buffers).
         *
         * @param projection : the projection matrix to apply.
         */
        void draw(const QMatrix4x4& projection) override;

        /**
         * Draw emarkers name (draw the text).
         *
         * @param projection : the projection matrix to apply.
         */
        void draw_name(const QMatrix4x4& projection);

    private:
        /**
		 * Create an emarker in OpenGL buffers.
		 *
		 * @param emarker : the emarker object.
		 * @param index : the index of the annotation for OpenGL buffers.
		 */
        void create_emarker(EMarker_shptr& emarker, unsigned index);

        Text text;
        unsigned emarkers_count = 0;

    };
}

#endif //__WORKSPACEEMARKER_H__
