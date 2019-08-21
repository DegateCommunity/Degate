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

#ifndef __IMAGERENDERER_H__
#define __IMAGERENDERER_H__

#include "Image.h"
#include "Project.h"

#include <QtOpenGL>

#define ZOOM_OUT 10.0 / 9.0
#define ZOOM_IN 9.0 / 10.0
#define NO_ZOOM 1

namespace degate
{
	class ImageRenderer : public QOpenGLWidget, protected QOpenGLFunctions
	{
		Q_OBJECT

	public:
		ImageRenderer(MemoryImage_shptr image, QWidget* parent, bool update_on_gl_initialize = true);
		~ImageRenderer();

		/**
	     * Update the screen.
	     */
		virtual void update_screen();

		/**
		 * Change the image with a new one (auto update).
		 */
		void change_image(MemoryImage_shptr image);

		/*
		 * Get the real mouse position (a public name for the opengl mouse position).
		 */
		QPointF get_real_mouse_position();

	protected:
		/*
		 * Destroy the OpenGL texture.
		 */
		void free_texture();
		virtual void initializeGL();
		virtual void paintGL();
		virtual void resizeGL(int w, int h);

		virtual void mousePressEvent(QMouseEvent* event);
		virtual void mouseReleaseEvent(QMouseEvent* event);
		virtual void mouseMoveEvent(QMouseEvent* event);
		virtual void wheelEvent(QWheelEvent* event);

		/*
		 * Set the new projection matrix with ortho operation.
		 * 
		 * @param scale_factor : NO_ZOOM, ZOOM_IN or ZOOM_OUT.
		 * @param new_center_x : x value of the new center.
		 * @param new_center_y : y value of the new center.
		 */
		void set_projection(float scale_factor, float new_center_x, float new_center_y);

		/*
		 * Get the mouse position relative to the widget with the y flipped (Qt 0,0 is on the upper left corner, we want it on the lower left corner, like OpenGL).
		 */
		QPointF get_widget_mouse_position() const;

		/*
		 * Get the mouse position relative to the OpenGL world (with 0,0 on the lower left corner).
		 */
		QPointF get_opengl_mouse_position() const;

		QMatrix4x4 projection;

	private:
		float scale = 1;
		float center_x = 0, center_y = 0;
		float viewport_min_x = 0, viewport_min_y = 0, viewport_max_x = 0, viewport_max_y = 0;
		QPointF mouse_last_pos;
		GLuint vbo = 0;
		GLuint texture = 0;
		QOpenGLShaderProgram* program = NULL;
		bool update_on_gl_initialize;

		MemoryImage_shptr image;
	};
}

#endif