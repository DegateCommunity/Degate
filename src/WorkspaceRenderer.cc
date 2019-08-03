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

#include "WorkspaceRenderer.h"

namespace degate
{

	WorkspaceRenderer::WorkspaceRenderer(QWidget* parent) : QOpenGLWidget(parent), background(this), gates(this)
	{
		setFocusPolicy(Qt::StrongFocus);
		setCursor(Qt::CrossCursor);
	}

	WorkspaceRenderer::~WorkspaceRenderer()
	{
		doneCurrent();
	}

	void WorkspaceRenderer::update_screen()
	{
		if (project == NULL)
			return;

		background.update();
		gates.update();

		update();
	}

	void WorkspaceRenderer::set_project(const Project_shptr& new_project)
	{
		assert(new_project != NULL);

		project = new_project;

		background.set_project(new_project);
		gates.set_project(new_project);

		update_screen();
	}

	void WorkspaceRenderer::free_textures()
	{
		background.free_textures();
	}

	void WorkspaceRenderer::initializeGL()
	{
		initializeOpenGLFunctions();

		glClearColor(0.0, 0.0, 0.0, 0.0);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);

		background.init();
		gates.init();
	}

	void WorkspaceRenderer::paintGL()
	{
		makeCurrent();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		background.draw(projection);
		gates.draw(projection);
	}

	void WorkspaceRenderer::resizeGL(int w, int h)
	{
		glViewport(0, 0, w, h);

		set_projection(NO_ZOOM, center_x, center_y);
	}

	QPointF WorkspaceRenderer::get_widget_mouse_position() const
	{
		const QPointF qt_widget_relative = mapFromGlobal(QCursor::pos());
		return QPoint(qt_widget_relative.x(), height() - qt_widget_relative.y());
	}

	QPointF WorkspaceRenderer::get_opengl_mouse_position() const
	{
		const QPointF widget_mouse_position = get_widget_mouse_position();
		return QPoint(viewport_min_x + widget_mouse_position.x() * scale,
		              viewport_min_y + widget_mouse_position.y() * scale);
	}

	void WorkspaceRenderer::set_projection(float scale_factor, float new_center_x, float new_center_y)
	{
		scale *= scale_factor;

		center_x = new_center_x;
		center_y = new_center_y;

		viewport_min_x = center_x - (static_cast<float>(width()) * scale) / 2.0;
		viewport_min_y = center_y - (static_cast<float>(height()) * scale) / 2.0;
		viewport_max_x = center_x + (static_cast<float>(width()) * scale) / 2.0;
		viewport_max_y = center_y + (static_cast<float>(height()) * scale) / 2.0;

		projection.setToIdentity();
		projection.scale(1, -1, 1);
		projection.ortho(viewport_min_x, viewport_max_x, viewport_max_y, viewport_min_y, -1, 1);
	}

	void WorkspaceRenderer::mousePressEvent(QMouseEvent* event)
	{
		QOpenGLWidget::mousePressEvent(event);

		mouse_last_pos = get_opengl_mouse_position();

		if (event->button() == Qt::LeftButton)
			setCursor(Qt::ClosedHandCursor);
	}

	void WorkspaceRenderer::mouseReleaseEvent(QMouseEvent* event)
	{
		QOpenGLWidget::mouseReleaseEvent(event);

		if (event->button() == Qt::LeftButton)
			setCursor(Qt::CrossCursor);
	}

	void WorkspaceRenderer::mouseMoveEvent(QMouseEvent* event)
	{
		QOpenGLWidget::mouseMoveEvent(event);

		if (event->buttons() & Qt::LeftButton)
		{
			int dx = get_opengl_mouse_position().x() - mouse_last_pos.x();
			int dy = get_opengl_mouse_position().y() - mouse_last_pos.y();

			center_x -= dx;
			center_y -= dy;
			set_projection(NO_ZOOM, center_x, center_y);

			update();
		}
	}

	void WorkspaceRenderer::wheelEvent(QWheelEvent* event)
	{
		QOpenGLWidget::wheelEvent(event);

		if (project == NULL)
			return;

		event->delta() > 0 ? set_projection(ZOOM_OUT, center_x, center_y) : set_projection(ZOOM_IN, center_x, center_y);

		event->accept();
		//Todo: update_screen(); after fixed the scaling manager (in the background class).
		update();
	}

	void WorkspaceRenderer::keyPressEvent(QKeyEvent* event)
	{
		QOpenGLWidget::keyPressEvent(event);
	}

	void WorkspaceRenderer::keyReleaseEvent(QKeyEvent* event)
	{
		QOpenGLWidget::keyReleaseEvent(event);
	}

	void WorkspaceRenderer::mouseDoubleClickEvent(QMouseEvent* event)
	{
		QOpenGLWidget::mouseDoubleClickEvent(event);
	}
}
