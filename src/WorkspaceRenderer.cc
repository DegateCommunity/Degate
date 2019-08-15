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

	WorkspaceRenderer::WorkspaceRenderer(QWidget* parent) : QOpenGLWidget(parent), background(this), gates(this), annotations(this), selection_tool(this)
	{
		setFocusPolicy(Qt::StrongFocus);
		setCursor(Qt::CrossCursor);
	}

	WorkspaceRenderer::~WorkspaceRenderer()
	{
		doneCurrent();

		WorkspaceText::delete_font();
	}

	void WorkspaceRenderer::update_screen()
	{
		makeCurrent();

		if (project == NULL)
			return;

		background.update();
		gates.update();
		annotations.update();

		update();
	}

	void WorkspaceRenderer::set_project(const Project_shptr& new_project)
	{
		project = new_project;

		background.set_project(new_project);
		gates.set_project(new_project);
		annotations.set_project(new_project);

		set_projection(1, width() / 2.0, height() / 2.0);

		update_screen();
	}

	bool WorkspaceRenderer::has_selection()
	{
		return selection_tool.is_selection();
	}

	BoundingBox WorkspaceRenderer::get_selection()
	{
		return selection_tool.get_selection_box();
	}

	PlacedLogicModelObject_shptr WorkspaceRenderer::get_selected_object()
	{
		return selected_object;
	}

	void WorkspaceRenderer::free_textures()
	{
		background.free_textures();
	}

	void WorkspaceRenderer::initializeGL()
	{
		makeCurrent();

		initializeOpenGLFunctions();

		WorkspaceText::init_font();

		glClearColor(0.0, 0.0, 0.0, 0.0);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDisable(GL_DEPTH);

		background.init();
		gates.init();
		annotations.init();
		selection_tool.init();
	}

	void WorkspaceRenderer::paintGL()
	{
		makeCurrent();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		background.draw(projection);
		gates.draw(projection);
		annotations.draw(projection);
		selection_tool.draw(projection);
	}

	void WorkspaceRenderer::resizeGL(int w, int h)
	{
		glViewport(0, 0, w, h);

		set_projection(NO_ZOOM, center_x, center_y);
	}

	QPointF WorkspaceRenderer::get_widget_mouse_position() const
	{
		const QPointF qt_widget_relative = mapFromGlobal(QCursor::pos());
		return QPoint(qt_widget_relative.x(), qt_widget_relative.y());
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
		projection.ortho(viewport_min_x, viewport_max_x, viewport_max_y, viewport_min_y, -1, 1);
	}

	void WorkspaceRenderer::mousePressEvent(QMouseEvent* event)
	{
		QOpenGLWidget::mousePressEvent(event);

		mouse_last_pos = get_opengl_mouse_position();

		if (event->button() == Qt::LeftButton)
			setCursor(Qt::ClosedHandCursor);

		// Selection
		if(event->button() == Qt::RightButton)
		{
			selection_tool.set_selection(true);
			selection_tool.set_origin(get_opengl_mouse_position().x(), get_opengl_mouse_position().y());
		}
	}

	void WorkspaceRenderer::mouseReleaseEvent(QMouseEvent* event)
	{
		QOpenGLWidget::mouseReleaseEvent(event);

		if (event->button() == Qt::LeftButton)
			setCursor(Qt::CrossCursor);

		// Selection
		if (event->button() == Qt::LeftButton && !is_movement)
		{
			if(project == NULL)
				return;

			QPointF pos = get_opengl_mouse_position();

			LogicModel_shptr lmodel = project->get_logic_model();
			Layer_shptr layer = lmodel->get_current_layer();
			PlacedLogicModelObject_shptr plo = layer->get_object_at_position(pos.x(), pos.y(), 0);

			// check if there is a gate or gate port on the logic layer
			if(plo == NULL) 
			{
				try 
				{
					layer = get_first_logic_layer(lmodel);
					plo = layer->get_object_at_position(pos.x(), pos.y(), 0);
			    }
				catch(CollectionLookupException const& ex)
				{
				}
			}

			bool was_selected = false;

			if(selected_object != NULL)
			{
				selected_object->set_highlighted(PlacedLogicModelObject::HLIGHTSTATE_NOT);

				if(Annotation_shptr o = std::dynamic_pointer_cast<Annotation>(selected_object))
				{
					annotations.update(o);
				}
				else if (Gate_shptr o = std::dynamic_pointer_cast<Gate>(selected_object))
				{
					gates.update(o);
				}
				else if (GatePort_shptr o = std::dynamic_pointer_cast<GatePort>(selected_object))
				{
					gates.update(o);
				}

				selected_object = NULL;
				was_selected = true;
			}
			
			if(plo != NULL)
			{
				selected_object = plo;

				plo->set_highlighted(PlacedLogicModelObject::HLIGHTSTATE_ADJACENT);
			}

			if(plo != NULL || was_selected)
			{
				if(Annotation_shptr o = std::dynamic_pointer_cast<Annotation>(selected_object))
				{
					annotations.update(o);
				}
				else if (Gate_shptr o = std::dynamic_pointer_cast<Gate>(selected_object))
				{
					gates.update(o);
				}
				else if (GatePort_shptr o = std::dynamic_pointer_cast<GatePort>(selected_object))
				{
					gates.update(o);
				}

				update();
			}
		}

		is_movement = false;
	}

	void WorkspaceRenderer::mouseMoveEvent(QMouseEvent* event)
	{
		QOpenGLWidget::mouseMoveEvent(event);

		// Movement
		if (event->buttons() & Qt::LeftButton)
		{
			is_movement = true;

			float dx = get_opengl_mouse_position().x() - mouse_last_pos.x();
			float dy = get_opengl_mouse_position().y() - mouse_last_pos.y();

			center_x -= dx;
			center_y -= dy;
			set_projection(NO_ZOOM, center_x, center_y);

			update();
		}

		// Selection
		if(event->buttons() & Qt::RightButton)
		{
			selection_tool.update(get_opengl_mouse_position().x(), get_opengl_mouse_position().y());

			update();
		}
	}

	void WorkspaceRenderer::wheelEvent(QWheelEvent* event)
	{
		QOpenGLWidget::wheelEvent(event);

		event->delta() < 0 ? set_projection(ZOOM_OUT, center_x, center_y) : set_projection(ZOOM_IN, center_x, center_y);

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

		if (event->button() == Qt::LeftButton)
		{
			if(project == NULL)
				return;

			QPointF pos = get_opengl_mouse_position();

			LogicModel_shptr lmodel = project->get_logic_model();
			Layer_shptr layer = lmodel->get_current_layer();
			PlacedLogicModelObject_shptr plo = layer->get_object_at_position(pos.x(), pos.y(), 0);

			if(plo != NULL)
			{
				if(SubProjectAnnotation_shptr sp = std::dynamic_pointer_cast<SubProjectAnnotation>(plo))
				{
					std::string dir = join_pathes(project->get_project_directory(), sp->get_path());
					debug(TM, "Will open or create project at %s", dir.c_str());

					emit project_changed(dir);
				}
			}
		}

		// Selection
		if (event->button() == Qt::RightButton)
		{
			selection_tool.set_selection(false);
			update();
		}
	}

	void WorkspaceRenderer::zoom_in()
	{
		set_projection(ZOOM_IN, center_x, center_y);

		update();
	}

	void WorkspaceRenderer::zoom_out()
	{
		set_projection(ZOOM_OUT, center_x, center_y);

		update();
	}
}
