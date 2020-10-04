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

#ifndef __WORKSPACEANNOTATIONS_H__
#define __WORKSPACEANNOTATIONS_H__

#include "WorkspaceElement.h"
#include "GUI/Text/Text.h"

namespace degate
{

    /**
     * @class WorkspaceAnnotations
     * @brief Prepare and draw all annotations of the active layer on the workspace.
     *
     * This will prepare all OpenGL things (buffers, shaders...) to draw all annotations of the active layer.
     * One annotation is composed of a square, an outline and a centered text.
     *
     * The parent vbo buffer will store all squares and the line_vbo buffer will store all outlines.
     *
     * @see WorkspaceElement
     */
    class WorkspaceAnnotations : public WorkspaceElement
    {
    public:

        /**
         * Create a WorkspaceAnnotations element.
         * This will only set the parent, real creation will start with init and update functions.
         *
         * @param parent : the parent widget pointer.
         */
        explicit WorkspaceAnnotations(QWidget* parent);
        ~WorkspaceAnnotations();

        /**
         * Init all OpenGL routine (buffers, shaders...).
         */
        void init() override;

        /**
         * Update all annotations (fill buffers).
         */
        void update() override;

        /**
         * Update a specific annotation (update buffers).
         *
         * @warning Call the update() function before.
         *
         * @param annotation : the annotation to update.
         */
        void update(Annotation_shptr& annotation);

        /**
         * Draw all annotations (draw the square and outline buffers).
         *
         * @param projection : the projection matrix to apply.
         */
        void draw(const QMatrix4x4& projection) override;

        /**
         * Draw annotations name (draw the workspace text).
         *
         * @param projection : the projection matrix to apply.
         */
        void draw_name(const QMatrix4x4& projection);

    private:
        /**
         * Create an annotation in OpenGL buffers.
         *
         * @param annotation : the annotation object.
         * @param index : the index of the annotation for OpenGL buffers.
         */
        void create_annotation(Annotation_shptr& annotation, unsigned index);

        /* Border buffer */
        GLuint line_vbo = 0;
        Text text;
        unsigned annotations_count = 0;

    };
}

#endif