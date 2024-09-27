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

#include "Core/Utils/CrashReport.h"
#include "Core/Utils/DegateExceptions.h"
#include "GUI/Dialog/AnnotationEditDialog.h"
#include "GUI/Dialog/GateEditDialog.h"
#include "GUI/Preferences/PreferencesHandler.h"
#include "GUI/Workspace/WorkspaceNotifier.h"
#include "GUI/Workspace/WorkspaceRenderer.h"
#include "Globals.h"

#include <qt6/QtOpenGLWidgets/qopenglwidget.h>

namespace degate
{

    WorkspaceRenderer::WorkspaceRenderer(QWidget* parent)
        : QOpenGLWidget(parent),
          background(this),
          gates(this),
          annotations(this),
          emarkers(this),
          vias(this),
          wires(this),
          selection_tool(this),
          wire_tool(this),
          regular_grid(this)
    {
        setFocusPolicy(Qt::StrongFocus);
        setCursor(Qt::CrossCursor);
        setMouseTracking(true);

        selected_objects.set_object_update_function(
                std::bind(&WorkspaceRenderer::update_object, this, std::placeholders::_1));
    }

    WorkspaceRenderer::~WorkspaceRenderer()
    {
        // Prevent cleanup if OpenGL functions wheren't initialized
        if (!initialized)
            return;

        makeCurrent();

        this->cleanup();

        doneCurrent();

        auto updated_preferences = PREFERENCES_HANDLER.get_preferences();
        updated_preferences.show_grid = draw_grid;
        PREFERENCES_HANDLER.update(updated_preferences);
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

    void WorkspaceRenderer::update_objects()
    {
        makeCurrent();

        if (project == nullptr)
            return;

        gates.update();
        emarkers.update();
        vias.update();
        wires.update();

        update();
    }

    void WorkspaceRenderer::update_type(PlacedLogicModelObject_shptr& object)
    {
        makeCurrent();

        if (project == nullptr)
            return;

        if (std::dynamic_pointer_cast<Gate>(object) || std::dynamic_pointer_cast<GatePort>(object))
        {
            gates.update();
        }
        else if (std::dynamic_pointer_cast<Annotation>(object))
        {
            annotations.update();
        }
        else if (std::dynamic_pointer_cast<EMarker>(object))
        {
            emarkers.update();
        }
        else if (std::dynamic_pointer_cast<Via>(object))
        {
            vias.update();
        }
        else if (std::dynamic_pointer_cast<Wire>(object))
        {
            wires.update();
        }

        update();
    }

    void WorkspaceRenderer::update_background()
    {
        makeCurrent();

        if (project == nullptr)
            return;

        background.update();

        update();
    }

    void WorkspaceRenderer::update_gates()
    {
        makeCurrent();

        if (project == nullptr)
            return;

        gates.update();

        update();
    }

    void WorkspaceRenderer::update_annotations()
    {
        makeCurrent();

        if (project == nullptr)
            return;

        annotations.update();

        update();
    }

    void WorkspaceRenderer::update_emarkers()
    {
        makeCurrent();

        if (project == nullptr)
            return;

        emarkers.update();

        update();
    }

    void WorkspaceRenderer::update_vias()
    {
        makeCurrent();

        if (project == nullptr)
            return;

        vias.update();

        update();
    }

    void WorkspaceRenderer::update_wires()
    {
        makeCurrent();

        if (project == nullptr)
            return;

        wires.update();

        update();
    }

    void WorkspaceRenderer::update_regular_grid()
    {
        makeCurrent();

        if (project == nullptr)
            return;

        regular_grid.update();

        update();
    }

    void WorkspaceRenderer::set_project(const Project_shptr& new_project)
    {
        reset_area_selection();
        reset_selection();
        reset_wire_tool();

        project = new_project;

        background.set_project(new_project);
        gates.set_project(new_project);
        annotations.set_project(new_project);
        emarkers.set_project(new_project);
        vias.set_project(new_project);
        wires.set_project(new_project);
        selection_tool.set_project(new_project);
        wire_tool.set_project(new_project);
        regular_grid.set_project(new_project);

        makeCurrent();

        regular_grid.viewport_update(BoundingBox(viewport_min_x, viewport_max_x, viewport_min_y, viewport_max_y));
        regular_grid.update();

        // Reset scale
        scale = 1.0;

        // If project set, then center view and max zoom out
        if (project != nullptr)
        {
            if (width() < height())
            {
                set_projection(static_cast<float>(project->get_width()) / static_cast<float>(width()),
                               project->get_width() / 2.0,
                               project->get_height() / 2.0);
            }
            else
            {
                set_projection(static_cast<float>(project->get_height()) / static_cast<float>(height()),
                               project->get_width() / 2.0,
                               project->get_height() / 2.0);
            }
        }
        else
        {
            // Otherwise, just center the view
            center_view(QPointF{width() / 2.0, height() / 2.0});
        }

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

    BoundingBox WorkspaceRenderer::get_safe_area_selection()
    {
        return get_safe_bounding_box(get_area_selection());
    }

    ObjectSet& WorkspaceRenderer::get_selected_objects()
    {
        return selected_objects;
    }

    void WorkspaceRenderer::add_object_to_selection(PlacedLogicModelObject_shptr& object)
    {
        selected_objects.add(object, project->get_logic_model());
    }

    void WorkspaceRenderer::reset_area_selection()
    {
        selection_tool.set_selection_state(false);
        update();
    }

    void WorkspaceRenderer::reset_selection()
    {
        if (selected_objects.empty())
            return;

        selected_objects.clear();
    }

    void WorkspaceRenderer::reset_wire_tool()
    {
        wire_tool.reset_line_drawing();
        last_created_wire = nullptr;

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
        if (selected_objects.empty())
            return false;
        else
            return true;
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

    void WorkspaceRenderer::show_grid(bool value)
    {
        draw_grid = value;

        if (draw_grid == true)
        {
            makeCurrent();
            regular_grid.update();
            update();
        }
    }

    void WorkspaceRenderer::update_grid()
    {
        if (draw_grid == true)
        {
            makeCurrent();
            regular_grid.update();
            update();
        }
    }

    void WorkspaceRenderer::free_textures()
    {
        background.free_textures();
    }

    void WorkspaceRenderer::cleanup()
    {
        WorkspaceNotifier::get_instance().undefine(WorkspaceTarget::Workspace);

        makeCurrent();

        // Delete opengl objects here
        Text::delete_context();
    }

    void WorkspaceRenderer::initializeGL()
    {
        makeCurrent();

        initializeOpenGLFunctions();

        initialized = true;

        Text::init_context();

        //QColor color = QApplication::palette().color(QWidget::backgroundRole());
        //glClearColor(color.red() / 255.0, color.green() / 255.0, color.blue() / 255.0, 1.0);

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
        regular_grid.init();

        // Get and print OpenGL version
        QOpenGLContext* ctx = QOpenGLContext::currentContext();
        QSurfaceFormat sf = ctx->format();
        debug(TM, "OpenGL version: %d.%d.%d", sf.majorVersion(), sf.minorVersion(), sf.profile());

        // Get and print GLSL version
        QOpenGLFunctions* glFuncs = QOpenGLContext::currentContext()->functions();
        debug(TM, "GLSL version: %s", glFuncs->glGetString(GL_SHADING_LANGUAGE_VERSION));

        // Define the draw notification for the workspace (renderer), just a repaint
        WorkspaceNotifier::get_instance().define(WorkspaceTarget::Workspace, WorkspaceNotification::Draw, [=]() {
            this->repaint();
        });
    }

    void WorkspaceRenderer::paintGL()
    {
        makeCurrent();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        background.draw(projection);

        if (draw_wires)
            wires.draw(projection);

        if (draw_annotations)
            annotations.draw(projection);

        if (draw_annotations_name)
            annotations.draw_name(projection);

        if (draw_gates)
            gates.draw(projection);

        if (draw_gates_name)
            gates.draw_gates_name(projection);

        if (draw_ports)
            gates.draw_ports(projection);

        if (draw_ports_name)
            gates.draw_ports_name(projection);

        if (draw_emarkers)
            emarkers.draw(projection);

        if (draw_emarkers_name)
            emarkers.draw_name(projection);

        if (draw_vias)
            vias.draw(projection);

        if (draw_vias_name)
            vias.draw_name(projection);

        if (current_tool == WorkspaceTool::AREA_SELECTION)
            selection_tool.draw(projection);

        if (current_tool == WorkspaceTool::WIRE)
            wire_tool.draw(projection);

        if (draw_grid)
            regular_grid.draw(projection);
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

    QPointF WorkspaceRenderer::get_safe_opengl_mouse_position() const
    {
        return get_safe_position(get_opengl_mouse_position());
    }

    WorkspaceTool WorkspaceRenderer::get_current_tool() const
    {
        return current_tool;
    }

    void WorkspaceRenderer::update_object(PlacedLogicModelObject_shptr object)
    {
        if (object == nullptr)
            return;

        makeCurrent();

        if (Gate_shptr gate = std::dynamic_pointer_cast<Gate>(object))
        {
            gates.update(gate);
        }
        else if (GatePort_shptr gate_port = std::dynamic_pointer_cast<GatePort>(object))
        {
            gates.update(gate_port);
        }

        if (object->get_layer() == project->get_logic_model()->get_current_layer())
        {
            if (Annotation_shptr annotation = std::dynamic_pointer_cast<Annotation>(object))
            {
                annotations.update(annotation);
            }
            else if (EMarker_shptr emarker = std::dynamic_pointer_cast<EMarker>(object))
            {
                emarkers.update(emarker);
            }
            else if (Via_shptr via = std::dynamic_pointer_cast<Via>(object))
            {
                vias.update(via);
            }
            else if (Wire_shptr wire = std::dynamic_pointer_cast<Wire>(object))
            {
                wires.update(wire);
            }
        }

        update();
    }

    void WorkspaceRenderer::center_view(QPointF point)
    {
        set_projection(NO_ZOOM, point.x(), point.y());
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

        background.update_viewport(viewport_min_x,
                                   viewport_max_x,
                                   viewport_min_y,
                                   viewport_max_y,
                                   static_cast<float>(width()),
                                   static_cast<float>(height()));

        regular_grid.viewport_update(BoundingBox(viewport_min_x, viewport_max_x, viewport_min_y, viewport_max_y));

        if (draw_grid)
            regular_grid.update();

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

        // Area selection + CTRL
        if (event->button() == Qt::RightButton && current_tool == WorkspaceTool::AREA_SELECTION &&
            QApplication::keyboardModifiers().testFlag(Qt::ControlModifier))
        {
            reset_area_selection();
        }
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
            if (project == nullptr)
                return;

            QPointF pos = get_opengl_mouse_position();

            LogicModel_shptr lmodel = project->get_logic_model();
            Layer_shptr layer = lmodel->get_current_layer();
            PlacedLogicModelObject_shptr plo = layer->get_object_at_position(pos.x(),
                                                                             pos.y(),
                                                                             0,
                                                                             !draw_annotations,
                                                                             !draw_gates,
                                                                             !draw_ports,
                                                                             !draw_emarkers,
                                                                             !draw_vias,
                                                                             !draw_wires);

            // Check if there is a gate or gate port on the logic layer
            try
            {
                PlacedLogicModelObject_shptr logic_plo;
                layer = get_first_logic_layer(lmodel);
                logic_plo = layer->get_object_at_position(pos.x(),
                                                          pos.y(),
                                                          0,
                                                          true,
                                                          !draw_gates,
                                                          !draw_ports,
                                                          true,
                                                          true,
                                                          true);

                if (plo == nullptr)
                {
                    plo = logic_plo;
                }
                else if (std::dynamic_pointer_cast<GatePort>(logic_plo) != nullptr)
                {
                    plo = logic_plo;
                }
                else if (std::dynamic_pointer_cast<Via>(plo) == nullptr &&
                         std::dynamic_pointer_cast<EMarker>(plo) == nullptr &&
                         std::dynamic_pointer_cast<Gate>(logic_plo) != nullptr)
                {
                    plo = logic_plo;
                }
            }
            catch (CollectionLookupException const&)
            {
            }

            // If no CTRL reset selection (single selection)
            if (!selected_objects.empty() && !QApplication::keyboardModifiers().testFlag(Qt::ControlModifier))
                reset_selection();

            if (plo != nullptr)
                add_object_to_selection(plo);
        }

        // Selection imply no area selection
        if (!mouse_moved && !selected_objects.empty() && current_tool == WorkspaceTool::AREA_SELECTION)
        {
            reset_area_selection();
            update();
        }

        // Wire tool
        if (event->button() == Qt::RightButton && current_tool == WorkspaceTool::WIRE && project != nullptr)
        {
            wire_tool.end_line_drawing();

            // Create wire
            Wire_shptr new_wire(new Wire(wire_tool.get_line()));
            new_wire->set_fill_color(project->get_default_color(DEFAULT_COLOR_WIRE));
            new_wire->set_diameter(project->get_default_wire_diameter());

            // Registre wire
            project->get_logic_model()->add_object(project->get_logic_model()->get_current_layer()->get_layer_pos(),
                                                   new_wire);

            // Restart line drawing
            wire_tool.start_line_drawing(wire_tool.get_line().get_to_x(), wire_tool.get_line().get_to_y());

            // Connect to previous wire
            if (last_created_wire != nullptr)
            {
                ObjectSet set;
                set.add(last_created_wire);
                set.add(new_wire);

                connect_objects(project->get_logic_model(), set.begin(), set.end());
            }

            last_created_wire = new_wire;

            emit project_changed();

            update_wires();
        }

        // Area selection + CTRL
        if (event->button() == Qt::RightButton && current_tool == WorkspaceTool::AREA_SELECTION &&
            selection_tool.is_object_selection_mode_active())
        {
            BoundingBox bb = get_safe_area_selection();
            reset_area_selection();

            Layer_shptr layer = project->get_logic_model()->get_current_layer();

            // Current layer
            for (Layer::qt_region_iterator iter = layer->region_begin(bb); iter != layer->region_end(); ++iter)
            {
                PlacedLogicModelObject_shptr plo = *iter;
                assert(plo != nullptr);

                selected_objects.add(plo);
            }

            try
            {
                layer = get_first_logic_layer(project->get_logic_model());
            }
            catch (std::exception&)
            {
            }

            if (project->get_logic_model()->get_current_layer() == layer)
                return;

            // Logic layer (gates and gate ports)
            for (Layer::qt_region_iterator iter = layer->region_begin(bb); iter != layer->region_end(); ++iter)
            {
                PlacedLogicModelObject_shptr plo = *iter;
                assert(plo != nullptr);

                if (std::dynamic_pointer_cast<GatePort>(plo) != nullptr ||
                    std::dynamic_pointer_cast<Gate>(plo) != nullptr)
                {
                    selected_objects.add(plo);
                }
            }

            selection_tool.set_object_selection_mode_state(false);
        }

        // Emit signal (for mouse context menu)
        if (event->button() == Qt::RightButton && !mouse_moved)
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

        // Area selection
        if (event->buttons() & Qt::RightButton && current_tool == WorkspaceTool::AREA_SELECTION)
        {
            mouse_moved = true;

            // If there is no area selection, start new one and set new origin
            if (!selection_tool.has_selection())
            {
                selection_tool.set_selection_state(true);

                // Area selection + CTRL
                if (QApplication::keyboardModifiers().testFlag(Qt::ControlModifier))
                    selection_tool.set_object_selection_mode_state(true);
                else
                    selection_tool.set_object_selection_mode_state(false);

                selection_tool.set_origin(get_opengl_mouse_position().x(), get_opengl_mouse_position().y());
            }

            // Update other area extremity on mouse position
            selection_tool.update(get_opengl_mouse_position().x(), get_opengl_mouse_position().y());

            // If an object is selected, reset selection
            if (!selected_objects.empty())
                reset_selection();

            update();
        }

        if (event->buttons() & Qt::RightButton && current_tool == WorkspaceTool::WIRE)
        {
            mouse_moved = true;

            if (wire_tool.has_ended())
                wire_tool.reset_line_drawing();

            if (!wire_tool.has_started())
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

        QPoint wheel_delta = event->angleDelta();

        if (wheel_delta.y() < 0)
            zoom_out();
        else if (wheel_delta.y() > 0)
            zoom_in();
        else
            QOpenGLWidget::wheelEvent(event);

        event->accept();
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

        if (event->key() == Qt::Key_Escape)
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
            if (project == nullptr)
                return;

            QPointF pos = get_opengl_mouse_position();

            LogicModel_shptr lmodel = project->get_logic_model();
            Layer_shptr layer = lmodel->get_current_layer();
            PlacedLogicModelObject_shptr plo = layer->get_object_at_position(pos.x(),
                                                                             pos.y(),
                                                                             0,
                                                                             !draw_annotations,
                                                                             !draw_gates,
                                                                             !draw_ports,
                                                                             !draw_emarkers,
                                                                             !draw_vias,
                                                                             !draw_wires);

            // Check if there is a gate or gate port on the logic layer
            if (plo == nullptr)
            {
                try
                {
                    layer = get_first_logic_layer(lmodel);
                    plo = layer->get_object_at_position(pos.x(),
                                                        pos.y(),
                                                        0,
                                                        !draw_annotations,
                                                        !draw_gates,
                                                        !draw_ports,
                                                        !draw_emarkers,
                                                        !draw_vias,
                                                        !draw_wires);
                }
                catch (CollectionLookupException const&)
                {
                }
            }

            if (plo != nullptr)
            {
                if (SubProjectAnnotation_shptr sp = std::dynamic_pointer_cast<SubProjectAnnotation>(plo))
                {
                    std::string dir = join_pathes(project->get_project_directory(), sp->get_path());
                    debug(TM, "Will open or create project at %s", dir.c_str());

                    emit project_changed(dir);
                }
                else if (Gate_shptr gate = std::dynamic_pointer_cast<Gate>(plo))
                {
                    GateInstanceEditDialog dialog(this, gate, project);
                    dialog.exec();

                    project->get_logic_model()->update_ports(gate);

                    makeCurrent();
                    gates.update();
                    update();

                    emit project_changed();
                }
                else if (GatePort_shptr gate_port = std::dynamic_pointer_cast<GatePort>(plo))
                {
                    {
                        PortPlacementDialog dialog(this,
                                                   project,
                                                   gate_port->get_gate()->get_gate_template(),
                                                   gate_port->get_template_port());
                        dialog.exec();
                    }

                    project->get_logic_model()->update_ports(gate_port->get_gate());

                    makeCurrent();
                    gates.update();
                    update();

                    emit project_changed();
                }
                else if (Annotation_shptr annotation = std::dynamic_pointer_cast<Annotation>(plo))
                {
                    AnnotationEditDialog dialog(this, annotation);
                    dialog.exec();

                    makeCurrent();
                    annotations.update();
                    update();

                    emit project_changed();
                }
                else if (EMarker_shptr emarker = std::dynamic_pointer_cast<EMarker>(plo))
                {
                    EMarkerEditDialog dialog(this, emarker);
                    dialog.exec();

                    makeCurrent();
                    emarkers.update();
                    update();

                    emit project_changed();
                }
                else if (Via_shptr via = std::dynamic_pointer_cast<Via>(plo))
                {
                    ViaEditDialog dialog(this, via, project);
                    dialog.exec();

                    makeCurrent();
                    vias.update();
                    update();

                    emit project_changed();
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

    QPointF WorkspaceRenderer::get_safe_position(QPointF position) const
    {
        if (project == nullptr)
            return position;

        QPointF res(position);

        if (position.x() < 0)
            res.setX(0);

        if (position.y() < 0)
            res.setY(0);

        if (position.x() > project->get_bounding_box().get_max_x())
            res.setX(project->get_bounding_box().get_max_x());

        if (position.y() > project->get_bounding_box().get_max_y())
            res.setY(project->get_bounding_box().get_max_y());

        return res;
    }

    BoundingBox WorkspaceRenderer::get_safe_bounding_box(BoundingBox bounding_box) const
    {
        if (project == nullptr)
            return bounding_box;

        BoundingBox res(bounding_box);

        if (bounding_box.get_min_x() < 0)
            res.set_min_x(0);

        if (bounding_box.get_min_y() < 0)
            res.set_min_y(0);

        if (bounding_box.get_max_x() < 0)
            res.set_max_x(0);

        if (bounding_box.get_max_y() < 0)
            res.set_max_y(0);

        if (bounding_box.get_min_x() > project->get_bounding_box().get_max_x())
            res.set_min_x(project->get_bounding_box().get_max_x());

        if (bounding_box.get_min_y() > project->get_bounding_box().get_max_y())
            res.set_min_y(project->get_bounding_box().get_max_y());

        if (bounding_box.get_max_x() > project->get_bounding_box().get_max_x())
            res.set_max_x(project->get_bounding_box().get_max_x());

        if (bounding_box.get_max_y() > project->get_bounding_box().get_max_y())
            res.set_max_y(project->get_bounding_box().get_max_y());

        return res;
    }

    bool WorkspaceRenderer::event(QEvent* event)
    {
        if (!this->context_initialized && event->type() == QEvent::Show)
        {
            auto result = QOpenGLWidget::event(event);
            this->context_initialized = true;

            // Check OpenGL initialization
            if (this->context() == nullptr || !this->context()->isValid())
            {
                QMessageBox::critical(
                        this,
                        "Error",
                        "Degate couldn't create an OpenGL context. Please install appropriate drivers to run the "
                        "software.");
                throw DegateRuntimeException("No OpenGL driver installed on the machine.");
            }

            // Check that we are not using software OpenGL through llvmpipe
            auto functions = this->context()->functions();
            if (functions != nullptr)
            {
                const std::string vendor = reinterpret_cast<const char*>(functions->glGetString(GL_VENDOR));
                const std::string renderer = reinterpret_cast<const char*>(functions->glGetString(GL_RENDERER));
                const std::string version = reinterpret_cast<const char*>(functions->glGetString(GL_VERSION));
                debug(TM, "Detected OpenGL vendor: %s", vendor.c_str());
                debug(TM, "Detected OpenGL renderer: %s", renderer.c_str());
                debug(TM, "Detected OpenGL version: %s", version.c_str());

                if (renderer.find("llvmpipe") != std::string::npos)
                {
                    QMessageBox::warning(this,
                                         "OpenGL",
                                         "You are using a software implementation of OpenGL because no real driver has "
                                         "been found. Please note that this will have an impact on performances, and "
                                         "could even not work at all.");
                }
            }
            return result;
        }
        else
        {
            return QOpenGLWidget::event(event);
        }
    }
} // namespace degate
