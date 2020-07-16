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

#include <Core/Project/Project.h>
#include <Core/LogicModel/HlObjectSet.h>
#include <GUI/Workspace/WorkspaceBackground.h>
#include <GUI/Workspace/WorkspaceGates.h>
#include <GUI/Text/Text.h>
#include <GUI/Workspace/WorkspaceAnnotations.h>
#include <Core/LogicModel/Annotation/SubProjectAnnotation.h>
#include <Core/LogicModel/LogicModelHelper.h>
#include <GUI/Workspace/WorkspaceSelectionTool.h>
#include <GUI/Workspace/WorkspaceEMarkers.h>
#include <GUI/Dialog/EMarkerEditDialog.h>
#include <GUI/Workspace/WorkspaceVias.h>
#include <GUI/Dialog/ViaEditDialog.h>
#include <GUI/Workspace/WorkspaceWires.h>
#include <GUI/Workspace/WorkspaceWireTool.h>
#include <GUI/Workspace/WorkspaceRegularGrid.h>

#include <QtOpenGL/QtOpenGL>
#include <list>
#include <tuple>

/**
 * This define the zoom out factor (zoom *= zoom_out).
 *
 * @see WorkspaceRenderer
 */
#define ZOOM_OUT 10.0 / 9.0

/**
 * This define the zoom in factor (zoom *= zoom_in).
 *
 * @see WorkspaceRenderer
 */
#define ZOOM_IN 9.0 / 10.0

/**
 * This define the no zoom factor (zoom = zoom).
 *
 * @see WorkspaceRenderer
 */
#define NO_ZOOM 1

namespace degate
{
    /**
     * List of usable workspace tools.
     */
    enum WorkspaceTool
    {
        AREA_SELECTION,
        WIRE
    };

	/**
	 * @class WorkspaceRenderer
	 * @brief Hold all other workspace elements and draw the workspace.
	 *
	 * The workspace renderer use a lot of Qt functions (@see QOpenGLWidget and @see QOpenGLFunctions) and is directly connected to the main window.
	 * Somehow the workspace renderer is the workspace widget.
	 * This hold all other workspace elements (annotations, gates, background...), draw them and manage all related events.
	 *
	 * @warning The workspace renderer get his own OpenGL context (@see WorkspaceElement).
	 */
	class WorkspaceRenderer : public QOpenGLWidget, protected QOpenGLFunctions
	{
	Q_OBJECT

	public:

		/**
		 * Create a workspace renderer.
		 *
		 * @param parent : the parent widget pointer (usually the main window).
		 */
		WorkspaceRenderer(QWidget* parent = nullptr);
		~WorkspaceRenderer() override;

		/**
	     * Update the screen (call all update_XXX functions).
	     */
		void update_screen();

		/**
	     * Set the project for the workspace (the screen will be updated).
	     * 
	     * @param new_project : the project to set.
	     */
		void set_project(const Project_shptr& new_project);

		/**
		 * Get the area selection state, if true there is a selection area otherwise not.
		 *
		 * @return Returns true if there is a area selection.
		 */
		bool has_area_selection();

		/**
		 * Get the area selection bounding box.
		 *
		 * @return Returns the bounding box of the area selection.
		 */
		BoundingBox get_area_selection();

        /**
         * Get a safe area selection regarding project size (0 <= position <= max project size).
         * If values are out of bound it will set them to 0 or max project size.
         *
         * @return Returns the safe bounding box of the area selection.
         */
        BoundingBox get_safe_area_selection();
		
		/**
		 * Get the vector of selected objects.
		 *
		 * @return Returns all selected objects.
		 */
        ObjectSet& get_selected_objects();

		/**
		 * Add an object to the selection list.
		 *
		 * @param object : the object to add.
		 */
		void add_object_to_selection(PlacedLogicModelObject_shptr& object);

		/**
		 * Get the selection state, if true there is a selection otherwise not.
		 */
		bool has_selection();

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

        /**
         * Get a safe mouse position regarding project size (0 <= position <= max project size).
         * If values are out of bound it will set them to 0 or max project size.
         *
         * @return Returns the OpenGL relative safe mouse position.
         */
        QPointF get_safe_opengl_mouse_position() const;

        /**
         * Get the currently used tool.
         *
         * @return Returns the current tool.
         */
        WorkspaceTool get_current_tool() const;

        /**
		 * Update an object of the workspace.
		 */
        void update_object(PlacedLogicModelObject_shptr object);

	protected:
		/**
		 * Destroy all OpenGL textures.
		 */
		void free_textures();

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
		void keyPressEvent(QKeyEvent* event) override;
		void keyReleaseEvent(QKeyEvent* event) override;
		void mouseDoubleClickEvent(QMouseEvent* event) override;

		/**
		 * Set the new projection matrix with ortho operation.
		 * 
		 * @param scale_factor : NO_ZOOM, ZOOM_IN or ZOOM_OUT.
		 * @param new_center_x : x value of the new center.
		 * @param new_center_y : y value of the new center.
		 */
		void set_projection(float scale_factor, float new_center_x, float new_center_y);

	public slots:
		/**
		 * Zoom in.
		 */
		void zoom_in();

		/**
		 * Zoom out.
		 */
		void zoom_out();

		/**
		 * Draw gates or not.
		 *
		 * @param value : if true then gates will be drawn, not otherwise.
		 */
		void show_gates(bool value);
		
		/**
		 * Draw gates name or not.
		 *
		 * @param value : if true then gates name will be drawn, not otherwise.
		 */
		void show_gates_name(bool value);
		
		/**
		 * Draw ports or not.
		 *
		 * @param value : if true then ports will be drawn, not otherwise.
		 */
		void show_ports(bool value);

		/**
		 * Draw ports name or not.
		 *
		 * @param value : if true then ports name will be drawn, not otherwise.
		 */
		void show_ports_name(bool value);

		/**
		 * Draw annotations or not.
		 *
		 * @param value : if true then annotations will be drawn, not otherwise.
		 */
		void show_annotations(bool value);

		/**
		 * Draw annotations name or not.
		 *
		 * @param value : if true then annotations name will be drawn, not otherwise.
		 */
		void show_annotations_name(bool value);

		/**
		 * Draw emarkers or not.
		 *
		 * @param value : if true then emarker will be drawn, not otherwise.
		 */
		void show_emarkers(bool value);

		/**
		 * Draw emarkers name or not.
		 *
		 * @param value : if true then emarker name will be drawn, not otherwise.
		 */
		void show_emarkers_name(bool value);

        /**
         * Draw vias or not.
         *
         * @param value : if true then via will be drawn, not otherwise.
         */
        void show_vias(bool value);

        /**
         * Draw vias name or not.
         *
         * @param value : if true then via name will be drawn, not otherwise.
         */
        void show_vias_name(bool value);

        /**
         * Draw wires or not.
         *
         * @param value : if true then via will be drawn, not otherwise.
         */
        void show_wires(bool value);

        /**
         * Show the grid or not.
         *
         * @param value : if true then the grid will be drawn, not otherwise.
         */
        void show_grid(bool value);

        /**
         * Force grid update.
         */
        void update_grid();

        /**
         * No more area selection.
         */
        void reset_area_selection();

        /**
		 * No more selection (clear the selected objects vector).
		 */
        void reset_selection();

        /**
         * Restart line drawing of the wire tool.
         */
        void reset_wire_tool();

        /**
         * Switch the current used tool to the area selection tool.
         */
        void use_area_selection_tool();

        /**
         * Switch the current used tool to the wire tool.
         */
        void use_wire_tool();

	signals:
		/**
		 * Signal for when the project need to be changed.
		 *
		 * @param path : the path of the new project.
		 */
		void project_changed(std::string path);

		/**
		 * Signal for when the mouse coordinates changed.
		 *
		 * @param x : new x coordinate of the mouse.
		 * @param y : new y coordinate of the mouse.
		 */
		void mouse_coords_changed(int x, int y);

        /**
         * Signal emitted when the right mouse button is pressed.
         */
        void right_mouse_button_released();

        /**
         * Signal emitted when the project changed.
         */
        void project_changed();

	protected:
        /**
         * Get a safe position regarding project size (0 <= position <= max project size).
         * If values are out of bound it will set them to 0 or max project size.
         *
         * @param position : the position to be secured.
         *
         * @return Returns a safe position.
         */
        QPointF get_safe_position(QPointF position) const;

        /**
         * Get a safe bounding box regarding project size (0 <= position <= max project size).
	     * If values are out of bound it will set them to 0 or max project size.
         *
         * @param bounding_box : the bounding box to be secured.
         *
         * @return Returns a safe bounding box.
         */
        BoundingBox get_safe_bounding_box(BoundingBox bounding_box) const;

	private:
		// General
		Project_shptr project = nullptr;

		// View
		QMatrix4x4 projection;
		float scale = 1;
		float center_x = 0, center_y = 0;
		float viewport_min_x = 0, viewport_min_y = 0, viewport_max_x = 0, viewport_max_y = 0;

		// Mouse
		QPointF mouse_last_pos;
		bool mouse_moved = false;

		// Selection
        HlObjectSet selected_objects;

		// Background
		WorkspaceBackground background;

		// Gates
		WorkspaceGates gates;

		// Annotations
		WorkspaceAnnotations annotations;

		// EMarkers
		WorkspaceEMarkers emarkers;

		// Vias
		WorkspaceVias vias;

		// Wires
		WorkspaceWires wires;

		// Selection tool
		WorkspaceSelectionTool selection_tool;

		// Wire tool
        WorkspaceWireTool wire_tool;

        // Regular grid
        WorkspaceRegularGrid regular_grid;
        bool draw_grid = false;

		// View parameters
		bool draw_gates            = true;
		bool draw_gates_name       = true;
		bool draw_ports            = true;
		bool draw_ports_name       = true;
		bool draw_annotations      = true;
		bool draw_annotations_name = true;
        bool draw_emarkers         = true;
        bool draw_emarkers_name    = true;
        bool draw_vias             = true;
        bool draw_vias_name        = true;
        bool draw_wires            = true;

        // Used tool
        WorkspaceTool current_tool;

	};
}

#endif
