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

#ifndef __WORKSPACERENDERER_H__
#define __WORKSPACERENDERER_H__

#include "Project.h"

#include <QtOpenGL/QtOpenGL>
#include <list>
#include <tuple>

#define ZOOM_IN 10.0 / 9.0
#define ZOOM_OUT 9.0 / 10.0
#define NO_ZOOM 1

namespace degate
{
	class WorkspaceRenderer : public QOpenGLWidget, protected QOpenGLFunctions
	{
		Q_OBJECT

	public:
		WorkspaceRenderer(QWidget * parent = 0);
		~WorkspaceRenderer();

		/**
	     * Update the screen (call all update_XXX functions).
	     */
		void update_screen();

		/**
	     * Update the background (all textures are reloaded).
	     */
		void update_background();

		/**
	     * Set the project for the workspace (the screen will be updated).
	     * 
	     * @param new_project : the project to set.
	     */
		void set_project(const Project_shptr & new_project);

	protected:
		/*
		 * Destroy all OpenGL textures.
		 */
		void free_textures();
		void initializeGL() override;
		void paintGL() override;
		void resizeGL(int w, int h) override;

		void mousePressEvent(QMouseEvent *event) override;
	    void mouseReleaseEvent(QMouseEvent *event) override;
	    void mouseMoveEvent(QMouseEvent *event) override;
	    void wheelEvent(QWheelEvent *event) override;
		void keyPressEvent(QKeyEvent *event) override;
		void keyReleaseEvent(QKeyEvent *event) override;
		void mouseDoubleClickEvent(QMouseEvent *event) override;

		/**
	     * Calculate the lower offset to top or left for a tile.
	     */
		unsigned int to_lower_tile_offset(unsigned int pos, unsigned int tile_width) const
		{
		  return (pos & ~(tile_width - 1));
		}

		/**
		 * Calculate the upper offset to top or left for a tile.
		 */
		unsigned int to_upper_tile_offset(unsigned int pos, unsigned int tile_width) const
		{
		  return (pos & ~(tile_width - 1)) + (tile_width - 1);
		}


	private:
		GLuint create_background_tile(unsigned int x, unsigned int y, float pre_scaling, unsigned indice);

		/*
		 * Get the mouse position relative to the widget with the y flipped (Qt 0,0 is on the upper left corner, we want it on the lower left corner, like OpenGL).
		 */
		QPointF get_widget_mouse_position() const;

		/*
		 * Get the mouse position relative to the OpenGL world (with 0,0 on the lower left corner).
		 */
		QPointF get_opengl_mouse_position() const;

		/*
		 * Set the new projection matrix with ortho operation.
		 * 
		 * @param scale_factor : NO_ZOOM, ZOOM_IN or ZOOM_OUT.
		 * @param new_center_x : x value of the new center.
		 * @param new_center_y : y value of the new center.
		 */
		void set_projection(float scale_factor, float new_center_x, float new_center_y);

		// General
		Project_shptr project = NULL;
		QOpenGLShaderProgram* program = NULL;
		QMatrix4x4 projection;
		QMatrix4x4 view;
		float scale = 1;
		float center_x = 0, center_y = 0;
		float viewport_min_x = 0, viewport_min_y = 0, viewport_max_x = 0, viewport_max_y = 0;
		QPointF mouse_last_pos;

		// Background
		std::list<std::tuple<unsigned int, unsigned int, GLuint>> background_textures;
		BackgroundImage_shptr background_image = NULL;
		GLuint background_vbo;

	};
}

#endif