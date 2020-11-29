/**
 * This file is part of the IC reverse engineering tool Degate.
 *
 * Copyright 2008, 2009, 2010 by Martin Schobert
 * Copyright 2019-2020 Dorian Bachelot
 *
 * Degate is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * Degate is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with degate. If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef __WORKSPACEBACKGROUND_H__
#define __WORKSPACEBACKGROUND_H__

#include "WorkspaceElement.h"

#include <vector>

#include <QFuture>

namespace degate
{

    /**
     * @class WorkspaceBackground
     * @brief Draw the current layer image (as background).
     */
    class WorkspaceBackground : public WorkspaceElement
    {
    public:

        /**
         * Create a workspace background element.
         * This will only set the parent, real creation will start with init and update functions.
         *
         * @param parent : the parent widget pointer.
         */
        explicit WorkspaceBackground(QWidget* parent);
        ~WorkspaceBackground();

        /**
         * Init all background OpenGL routine (vbo).
         */
        void init() override;

        /**
         * Update the background (all textures are reloaded).
         */
        void update() override;

        /**
         * Draw the background (all tiles will be draw).
         *
         * @param projection : the projection matrix to apply.
         */
        void draw(const QMatrix4x4& projection) override;

        /**
         * Destroy all OpenGL textures.
         */
        void free_textures();

        /**
         * Update the viewport.
         *
         * @param min_x : viewport min x.
         * @param max_x : viewport max x.
         * @param min_y : viewport min y.
         * @param max_y : viewport max x.
         * @param width : render area width.
         * @param height : render area height.
         */
        void update_viewport(float min_x, float max_x, float min_y, float max_y, float width, float height);

    private:
        /**
         * Create a background tile (OpenGL texture and vertices).
         *
         * @param x
         * @param y
         * @param pre_scaling : scaling of the image.
         * @param index : index of the tile.
         *
         * @return Returns the OpenGL texture ID of the tile.
         */
        GLuint create_background_tile(unsigned int x, unsigned int y, float pre_scaling, unsigned index);

        std::vector<GLuint> background_textures;
        BackgroundImage_shptr background_image = nullptr;

        float scale = 1;
        float viewport_min_x = 0, viewport_min_y = 0, viewport_max_x = 0, viewport_max_y = 0;
        float virtual_width = 0, virtual_height = 0;

        unsigned int tile_count = 0;

        QFutureWatcher<void> future;
    };
}

#endif