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

#ifndef __IMAGERENDERER_H__
#define __IMAGERENDERER_H__

#include "Core/Image/Image.h"
#include "Core/Project/Project.h"

#include <QtOpenGL>
#include <QtOpenGLWidgets/QOpenGLWidget>

/**
 * This define the zoom out factor (zoom *= zoom_out).
 *
 * @see ImageRenderer
 */
#define ZOOM_OUT 10.0f / 9.0f

/**
 * This define the zoom in factor (zoom *= zoom_in).
 *
 * @see ImageRenderer
 */
#define ZOOM_IN 9.0f / 10.0f

/**
 * This define the no zoom factor (zoom = zoom).
 *
 * @see ImageRenderer
 */
#define NO_ZOOM 1.f

namespace degate
{

    /**
     * @class ImageRenderer
     * @brief Hold and draw an memory image on the screen with OpenGL.
     *
     * This is a virtual class (eg. @see PortPlacementWidget).
     *
     * The image renderer use a lot of Qt functions (@see QOpenGLWidget and @see QOpenGLFunctions).
     * This draw the image and manage all related events.
     *
     * @warning Each image renderer get his own OpenGL context (@see WorkspaceElement).
     */
    class ImageRenderer : public QOpenGLWidget, protected QOpenGLFunctions
    {
        Q_OBJECT

    public:

        /**
         * Create the image renderer.
         *
         * @param parent : the parent of the renderer.
         * @param image : the memory image to draw.
         * @param update_on_gl_initialize : if true call the update_screen function after initializeGL function.
         */
        ImageRenderer(QWidget* parent, MemoryImage_shptr image, bool update_on_gl_initialize = true);
        ~ImageRenderer() override;

        /**
         * Update the screen.
         */
        virtual void update_screen();

        /**
         * Change the image with a new one (auto update).
         *
         * @param image : the new image.
         */
        void change_image(MemoryImage_shptr image);

    protected:
        /**
         * Destroy the OpenGL texture.
         */
        void free_texture();

        /**
         * Delete all opengl objects (called when QOpenGLContext::aboutToBeDestroyed signal is emitted).
         */
        void cleanup();

        /* Qt OpenGL functions */
        void initializeGL() override;
        void paintGL() override;
        void resizeGL(int w, int h) override;

        /* Qt mouse and keyboard events functions */
        void mousePressEvent(QMouseEvent* event) override;
        void mouseReleaseEvent(QMouseEvent* event) override;
        void mouseMoveEvent(QMouseEvent* event) override;
        void wheelEvent(QWheelEvent* event) override;

        /**
         * Set the new projection matrix with ortho operation.
         *
         * @param scale_factor : NO_ZOOM, ZOOM_IN or ZOOM_OUT.
         * @param new_center_x : x value of the new center.
         * @param new_center_y : y value of the new center.
         */
        void set_projection(float scale_factor, float new_center_x, float new_center_y);

        /**
         * Get the mouse position relative to the widget with the y flipped (Qt 0,0 is on the upper left corner, we want it on the lower left corner, like OpenGL).
         *
         * @return Returns the widget relative mouse position.
         */
        QPointF get_widget_mouse_position() const;

        /**
         * Get the mouse position relative to the OpenGL world (with 0,0 on the lower left corner).
         *
         * @return Returns the OpenGL relative mouse position.
         */
        QPointF get_opengl_mouse_position() const;

        // Protected members
        QMatrix4x4 projection;
        QOpenGLVertexArrayObject vao;

    private:
        float scale = 1;
        float center_x = 0, center_y = 0;
        float viewport_min_x = 0, viewport_min_y = 0, viewport_max_x = 0, viewport_max_y = 0;
        QPointF mouse_last_pos;
        GLuint vbo = 0;
        GLuint texture = 0;
        QOpenGLShaderProgram* program = nullptr;
        bool update_on_gl_initialize;

        MemoryImage_shptr image;
    };
}

#endif