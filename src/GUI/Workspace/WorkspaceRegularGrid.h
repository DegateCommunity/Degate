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

#ifndef __WORKSPACEREGULARGRID_H__
#define __WORKSPACEREGULARGRID_H__

#include "GUI/Workspace/WorkspaceElement.h"
#include "Core/Grid/RegularGrid.h"

namespace degate
{
    /**
     * @class WorkspaceRegularGrid
     * @brief Represents the regular grid on the workspace.
     *
     * Grid lines are drawn regarding the viewport, not on the whole project. This means that every time the viewport
     * move/change, it needs to update each line position. To reduce overhead (same as if the grid was independent of
     * the viewport and drawn on the whole project) when there are too many lines on screen, it will reduce the distance
     * between each line (grid unzoom).
     *
     * @warning This may not use the most optimal method to draw the grid. It can have performance issues, if so
     *          it will need a rework (not hard or long).
     *
     * @see WorkspaceElement
     */
    class WorkspaceRegularGrid : public WorkspaceElement
    {
    public:

        /**
         * Create the workspace regular grid.
         *
         * @param parent : the parent widget.
         */
        explicit WorkspaceRegularGrid(QWidget* parent);
        ~WorkspaceRegularGrid();

        /**
		 * Init OpenGL routine (vbo and shaders).
		 *
		 * @warning Always call this parent init method in the child implementation.
		 */
        void init() override;

        /**
	     * Update the grid vbo.
         *
         * Every time the viewport change this function should be called. Grid lines are drawn only on the viewport,
         * not on the whole project. So it needs to update line positions every time the viewport change (move/zoom).
	     */
        void update() override;

        /**
	     * Draw the grid.
	     *
	     * @param projection : the projection matrix to apply.
	     */
        void draw(const QMatrix4x4& projection) override;

        /**
         * Update the viewport size.
         *
         * You generally need to call the update function after the viewport dimensions changed.
         *
         * @param viewport : the new viewport bounding box.
         */
        void viewport_update(const BoundingBox& viewport);

    protected:
        /**
         * Check if the color changed (from the grid color of the preferences) and if it changed, update the converted grid color.
         */
        void update_color();

    private:
        BoundingBox viewport;

        QVector3D converted_grid_color = QVector3D(1, 1, 1);
        float     converted_grid_alpha = 0.75;
        color_t   grid_color;

        unsigned int width_count  = 0;
        unsigned int height_count = 0;
        bool         show_grid    = true;
    };
}

#endif //__WORKSPACEREGULARGRID_H__
