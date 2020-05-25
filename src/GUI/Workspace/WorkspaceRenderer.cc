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
#include "GUI/Dialog/GateEditDialog.h"
#include "GUI/Dialog/AnnotationEditDialog.h"

namespace degate
{

	WorkspaceRenderer::WorkspaceRenderer(QWidget* parent) : QOpenGLWidget(parent), background(this), gates(this), annotations(this), emarkers(this), vias(this), wires(this), selection_tool(this), wire_tool(this)
	{
		setFocusPolicy(Qt::StrongFocus);
		setCursor(Qt::CrossCursor);
		setMouseTracking(true);
	}

	WorkspaceRenderer::~WorkspaceRenderer()
	{
		makeCurrent();

        // Use cleanup function for opengl objects destruction

		doneCurrent();
	}

	void WorkspaceRenderer::update_screen()
	{
		makeCurrent();

		if (project == nullptr)
			return;

		background.update();
		gates.update();
		annotations.update();
        emarkers.update();
        vias.update();
        wires.update();

		update();
	}

	void WorkspaceRenderer::set_project(const Project_shptr& new_project)
	{
	    reset_area_selection();
	    reset_selection();

		project = new_project;

		background.set_project(new_project);
		gates.set_project(new_project);
		annotations.set_project(new_project);
        emarkers.set_project(new_project);
        vias.set_project(new_project);
        wires.set_project(new_project);
        selection_tool.set_project(new_project);
        wire_tool.set_project(new_project);

		set_projection(1, width() / 2.0, height() / 2.0);

		update_screen();
	}

	bool WorkspaceRenderer::has_area_selection()
	{
		return selection_tool.has_selection();
	}

	BoundingBox WorkspaceRenderer::get_area_selection()
	{
		return selection_tool.get_selection_box();
	}

	PlacedLogicModelObject_shptr WorkspaceRenderer::get_selected_object()
	{
		return selected_object;
	}

	void WorkspaceRenderer::reset_area_selection()
	{
		selection_tool.set_selection(false);
		update();
	}

	void WorkspaceRenderer::reset_selection()
	{
		if(selected_object == nullptr)
			return;
		
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
		else if (EMarker_shptr o = std::dynamic_pointer_cast<EMarker>(selected_object))
        {
            emarkers.update(o);
        }
        else if (Via_shptr o = std::dynamic_pointer_cast<Via>(selected_object))
        {
            vias.update(o);
        }
        else if (Wire_shptr o = std::dynamic_pointer_cast<Wire>(selected_object))
        {
            wires.update(o);
        }

		selected_object = nullptr;
	}

    void WorkspaceRenderer::reset_wire_tool()
    {
        wire_tool.reset_line_drawing();

        update();
    }

    void WorkspaceRenderer::use_area_selection_tool()
    {
        reset_selection();
        wire_tool.reset_line_drawing();

        current_tool = WorkspaceTool::AREA_SELECTION;

        update();
    }

    void WorkspaceRenderer::use_wire_tool()
    {
	    reset_area_selection();
	    reset_selection();

        current_tool = WorkspaceTool::WIRE;

        update();
    }

	bool WorkspaceRenderer::has_selection()
	{
		if(selected_object == nullptr)
			return false;
		else
			return true;
	}

	PlacedLogicModelObject_shptr WorkspaceRenderer::pop_selected_object()
	{
		if(selected_object == nullptr)
			return nullptr;
		
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
        else if (EMarker_shptr o = std::dynamic_pointer_cast<EMarker>(selected_object))
        {
            emarkers.update(o);
        }
        else if (Via_shptr o = std::dynamic_pointer_cast<Via>(selected_object))
        {
            vias.update(o);
        }
        else if (Wire_shptr o = std::dynamic_pointer_cast<Wire>(selected_object))
        {
            wires.update(o);
        }

		PlacedLogicModelObject_shptr temp = selected_object;
		selected_object = nullptr;

		return temp;
	}

	void WorkspaceRenderer::show_gates(bool value)
	{
		draw_gates = value;

		update();
	}

	void WorkspaceRenderer::show_gates_name(bool value)
	{
		draw_gates_name = value;

		update();
	}

	void WorkspaceRenderer::show_ports(bool value)
	{
		draw_ports = value;

		update();
	}

	void WorkspaceRenderer::show_ports_name(bool value)
	{
		draw_ports_name = value;

		update();
	}

	void WorkspaceRenderer::show_annotations(bool value)
	{
		draw_annotations = value;

		update();
	}

	void WorkspaceRenderer::show_annotations_name(bool value)
	{
		draw_annotations_name = value;

		update();
	}

    void WorkspaceRenderer::show_emarkers(bool value)
    {
        draw_emarkers = value;

        update();
    }

    void WorkspaceRenderer::show_emarkers_name(bool value)
    {
        draw_emarkers_name = value;

        update();
    }

    void WorkspaceRenderer::show_vias(bool value)
    {
        draw_vias = value;

        update();
    }

    void WorkspaceRenderer::show_vias_name(bool value)
    {
        draw_vias_name = value;

        update();
    }

    void WorkspaceRenderer::show_wires(bool value)
    {
        draw_wires = value;

        update();
    }

	void WorkspaceRenderer::free_textures()
	{
		background.free_textures();
	}

    void WorkspaceRenderer::cleanup()
    {
        makeCurrent();

        // Delete opengl objects here
        Text::delete_context();
    }

	void WorkspaceRenderer::initializeGL()
	{
		makeCurrent();

		initializeOpenGLFunctions();

		Text::init_context();

		glClearColor(0.0, 0.0, 0.0, 1.0);
		glEnable(GL_BLEND);
        glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ZERO, GL_ONE);
        glDisable(GL_LINE_SMOOTH);

		background.init();
		gates.init();
		annotations.init();
        emarkers.init();
        vias.init();
		selection_tool.init();
		wires.init();
        wire_tool.init();

        connect(context(), &QOpenGLContext::aboutToBeDestroyed, this, &WorkspaceRenderer::cleanup);
	}

	void WorkspaceRenderer::paintGL()
	{
		makeCurrent();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		background.draw(projection);

		if(draw_wires)
		    wires.draw(projection);

		if(draw_annotations)
			annotations.draw(projection);

		if(draw_annotations_name)
			annotations.draw_name(projection);
		
		if(draw_gates)
			gates.draw(projection);
		
		if(draw_gates_name)
			gates.draw_gates_name(projection);

		if(draw_ports)
			gates.draw_ports(projection);

		if(draw_ports_name)
			gates.draw_ports_name(projection);

        if(draw_emarkers)
            emarkers.draw(projection);

        if(draw_emarkers_name)
            emarkers.draw_name(projection);

        if(draw_vias)
            vias.draw(projection);

        if(draw_vias_name)
            vias.draw_name(projection);

        if(current_tool == WorkspaceTool::AREA_SELECTION)
		    selection_tool.draw(projection);

        if(current_tool == WorkspaceTool::WIRE)
            wire_tool.draw(projection);
	}

	void WorkspaceRenderer::resizeGL(int w, int h)
	{
		makeCurrent();
		
		glViewport(0, 0, w, h);

		set_projection(NO_ZOOM, center_x, center_y);
	}

	QPointF WorkspaceRenderer::get_widget_mouse_position() const
	{
		const QPointF qt_widget_relative = mapFromGlobal(QCursor::pos());
		return QPointF(qt_widget_relative.x(), qt_widget_relative.y());
	}

	QPointF WorkspaceRenderer::get_opengl_mouse_position() const
	{
		const QPointF widget_mouse_position = get_widget_mouse_position();
		return QPointF(viewport_min_x + widget_mouse_position.x() * scale,
		              viewport_min_y + widget_mouse_position.y() * scale);
	}

    WorkspaceTool WorkspaceRenderer::get_current_tool() const
    {
	    return current_tool;
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
        makeCurrent();

		QOpenGLWidget::mousePressEvent(event);

		mouse_last_pos = get_opengl_mouse_position();

		if (event->button() == Qt::LeftButton)
			setCursor(Qt::ClosedHandCursor);
	}

	void WorkspaceRenderer::mouseReleaseEvent(QMouseEvent* event)
	{
        makeCurrent();

		QOpenGLWidget::mouseReleaseEvent(event);

		if (event->button() == Qt::LeftButton)
			setCursor(Qt::CrossCursor);

		// Selection
		if (event->button() == Qt::LeftButton && !mouse_moved)
		{
			if(project == nullptr)
				return;

			QPointF pos = get_opengl_mouse_position();

			LogicModel_shptr lmodel = project->get_logic_model();
			Layer_shptr layer = lmodel->get_current_layer();
			PlacedLogicModelObject_shptr plo = layer->get_object_at_position(pos.x(), pos.y(), 0, !draw_annotations, !draw_gates, !draw_ports, !draw_emarkers, !draw_vias, !draw_wires);

			// Check if there is a gate or gate port on the logic layer
            try
            {
                PlacedLogicModelObject_shptr logic_plo;
                layer = get_first_logic_layer(lmodel);
                logic_plo = layer->get_object_at_position(pos.x(), pos.y(), 0, true, !draw_gates, !draw_ports, true, true, true);

                if(plo == nullptr)
                {
                    plo = logic_plo;
                }
                else if (std::dynamic_pointer_cast<GatePort>(logic_plo) != nullptr)
                {
                    plo = logic_plo;
                }
                else if (std::dynamic_pointer_cast<Via>(plo) == nullptr && std::dynamic_pointer_cast<EMarker>(plo) == nullptr && std::dynamic_pointer_cast<Gate>(logic_plo) != nullptr)
                {
                    plo = logic_plo;
                }
            }
            catch(CollectionLookupException const& ex)
            {
            }

			bool was_selected = false;

			if(selected_object != nullptr)
			{
				reset_selection();
				was_selected = true;
			}
			
			if(plo != nullptr)
			{
				selected_object = plo;

				plo->set_highlighted(PlacedLogicModelObject::HLIGHTSTATE_ADJACENT);
			}

			if(plo != nullptr || was_selected)
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
                else if (EMarker_shptr o = std::dynamic_pointer_cast<EMarker>(selected_object))
                {
                    emarkers.update(o);
                }
                else if (Via_shptr o = std::dynamic_pointer_cast<Via>(selected_object))
                {
                    vias.update(o);
                }
                else if (Wire_shptr o = std::dynamic_pointer_cast<Wire>(selected_object))
                {
                    wires.update(o);
                }

				update();
			}
		}

        // Selection imply no area selection
        if (selected_object != nullptr && current_tool == WorkspaceTool::AREA_SELECTION)
        {
            reset_area_selection();
            update();
        }

        if(event->button() == Qt::RightButton && current_tool == WorkspaceTool::WIRE && project != nullptr)
        {
            wire_tool.end_line_drawing();

            Wire_shptr new_wire(new Wire(wire_tool.get_line()));
            new_wire->set_fill_color(project->get_default_color(DEFAULT_COLOR_WIRE));
            new_wire->set_diameter(project->get_default_wire_diameter());

            project->get_logic_model()->add_object(project->get_logic_model()->get_current_layer()->get_layer_pos(), new_wire);

            wire_tool.start_line_drawing(wire_tool.get_line().get_to_x(), wire_tool.get_line().get_to_y());

            update_screen();
        }

        // Emit signal (for mouse context menu)
		if(event->button() == Qt::RightButton && !mouse_moved)
		    emit right_mouse_button_released();

		mouse_moved = false;
	}

	void WorkspaceRenderer::mouseMoveEvent(QMouseEvent* event)
	{
        makeCurrent();

		QOpenGLWidget::mouseMoveEvent(event);

		// Movement
		if (event->buttons() & Qt::LeftButton)
		{
            mouse_moved = true;

			float dx = get_opengl_mouse_position().x() - mouse_last_pos.x();
			float dy = get_opengl_mouse_position().y() - mouse_last_pos.y();

			center_x -= dx;
			center_y -= dy;
			set_projection(NO_ZOOM, center_x, center_y);

			update();
		}

		// Selection
		if(event->buttons() & Qt::RightButton && current_tool == WorkspaceTool::AREA_SELECTION)
		{
            mouse_moved = true;

            // If there is no area selection, start new one and set new origin
            if(!selection_tool.has_selection())
            {
                selection_tool.set_selection(true);
                selection_tool.set_origin(get_opengl_mouse_position().x(), get_opengl_mouse_position().y());
            }

            // Update other area extremity on mouse position
			selection_tool.update(get_opengl_mouse_position().x(), get_opengl_mouse_position().y());

            // If an object is selected, reset selection
			if(selected_object != nullptr)
			    reset_selection();

			update();
		}

		if(event->buttons() & Qt::RightButton && current_tool == WorkspaceTool::WIRE)
        {
            mouse_moved = true;

            if(wire_tool.has_ended())
                wire_tool.reset_line_drawing();

            if(!wire_tool.has_started())
                wire_tool.start_line_drawing(get_opengl_mouse_position().x(), get_opengl_mouse_position().y());

            wire_tool.update(get_opengl_mouse_position().x(), get_opengl_mouse_position().y());

            update();
        }

		// Mouse coords signal
		emit mouse_coords_changed(get_opengl_mouse_position().x(), get_opengl_mouse_position().y());
	}

	void WorkspaceRenderer::wheelEvent(QWheelEvent* event)
	{
        makeCurrent();

		QOpenGLWidget::wheelEvent(event);

		event->delta() < 0 ? set_projection(ZOOM_OUT, center_x, center_y) : set_projection(ZOOM_IN, center_x, center_y);

		event->accept();
		//Todo: update_screen(); after fixed the scaling manager (in the background class).
		update();
	}

	void WorkspaceRenderer::keyPressEvent(QKeyEvent* event)
	{
        makeCurrent();

		QOpenGLWidget::keyPressEvent(event);
	}

	void WorkspaceRenderer::keyReleaseEvent(QKeyEvent* event)
	{
        makeCurrent();

		QOpenGLWidget::keyReleaseEvent(event);

		if(event->key() == Qt::Key_Escape)
        {
            wire_tool.reset_line_drawing();
            update();
        }
	}

	void WorkspaceRenderer::mouseDoubleClickEvent(QMouseEvent* event)
	{
        makeCurrent();

		QOpenGLWidget::mouseDoubleClickEvent(event);

		if (event->button() == Qt::LeftButton)
		{
			if(project == nullptr)
				return;

			QPointF pos = get_opengl_mouse_position();

			LogicModel_shptr lmodel = project->get_logic_model();
			Layer_shptr layer = lmodel->get_current_layer();
			PlacedLogicModelObject_shptr plo = layer->get_object_at_position(pos.x(), pos.y(), 0, !draw_annotations, !draw_gates, !draw_ports, !draw_emarkers, !draw_vias, !draw_wires);

			// Check if there is a gate or gate port on the logic layer
			if(plo == nullptr) 
			{
				try 
				{
					layer = get_first_logic_layer(lmodel);
					plo = layer->get_object_at_position(pos.x(), pos.y(), 0, !draw_annotations, !draw_gates, !draw_ports, !draw_emarkers, !draw_vias, !draw_wires);
			    }
				catch(CollectionLookupException const& ex)
				{
				}
			}

			if(plo != nullptr)
			{
				if(SubProjectAnnotation_shptr sp = std::dynamic_pointer_cast<SubProjectAnnotation>(plo))
				{
					std::string dir = join_pathes(project->get_project_directory(), sp->get_path());
					debug(TM, "Will open or create project at %s", dir.c_str());

					emit project_changed(dir);
				}
				else if(Gate_shptr sp = std::dynamic_pointer_cast<Gate>(plo))
				{
					GateInstanceEditDialog dialog(this, sp, project);
					dialog.exec();

                    project->get_logic_model()->update_ports(sp);

					makeCurrent();
					gates.update();
					update();
				}
				else if(GatePort_shptr sp = std::dynamic_pointer_cast<GatePort>(plo))
				{
					{
						PortPlacementDialog dialog(this, project, sp->get_gate()->get_gate_template(), sp->get_template_port());
						dialog.exec();
					}

					project->get_logic_model()->update_ports(sp->get_gate());

					makeCurrent();
					gates.update();
					update();
				}
				else if(Annotation_shptr o = std::dynamic_pointer_cast<Annotation>(get_selected_object()))
				{
					AnnotationEditDialog dialog(o, this);
					dialog.exec();

					annotations.update();
					update();
				}
                else if (EMarker_shptr o = std::dynamic_pointer_cast<EMarker>(get_selected_object()))
                {
                    EMarkerEditDialog dialog(o, this);
                    dialog.exec();

                    emarkers.update();
                    update();
                }
                else if (Via_shptr o = std::dynamic_pointer_cast<Via>(get_selected_object()))
                {
                    ViaEditDialog dialog(o, this, project);
                    dialog.exec();

                    vias.update();
                    update();
                }
			}
		}

		setCursor(Qt::CrossCursor);
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
          