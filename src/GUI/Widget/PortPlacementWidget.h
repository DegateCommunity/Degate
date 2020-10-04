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

#ifndef __PORTPLACEMENTWIDGET_H__
#define __PORTPLACEMENTWIDGET_H__

#include "GUI/Utils/ImageRenderer.h"
#include "Core/LogicModel/Gate/GateTemplate.h"
#include "Core/LogicModel/Gate/GateTemplatePort.h"
#include "Core/Project/Project.h"
#include "GUI/Text/Text.h"

/**
 * Define the default port size for port placement.
 */
#define DEFAULT_PORT_SIZE 8

namespace degate
{

    /**
     * @class PortPlacementWidget
     * @brief Widget to place a port template of a gate template.
     *
     * @warning Each port placement widget get his own OpenGL context (@see ImageRenderer).
     *
     * @see GateEditDialog
     */
    class PortPlacementWidget : public ImageRenderer
    {
        Q_OBJECT

    public:

        /**
         * Create a port placement widget.
         *
         * @param parent : the parent of the widget.
         * @param project : the current active project.
         * @param gate : the gate template of the port template to place.
         * @param port : the gate template port to place.
         */
        PortPlacementWidget(QWidget* parent, Project_shptr project, const GateTemplate_shptr& gate, const GateTemplatePort_shptr& port);
        ~PortPlacementWidget() override;

        /**
         * Update the screen.
         */
        void update_screen() override;

        /**
         * Get the new port template position.
         *
         * @return Returns the new port template position.
         */
        Point get_new_port_position();

    public slots:
        /**
         * Go to the next layer on the image renderer.
         */
        void next_layer();

        /**
         * Go to the previous layer on the image renderer.
         */
        void previous_layer();

    protected:
        /* Qt OpenGL functions */
        void initializeGL() override;
        void paintGL() override;
        void resizeGL(int w, int h) override;

        /* Qt mouse and keyboard events functions */
        void mousePressEvent(QMouseEvent* event) override;
        void mouseReleaseEvent(QMouseEvent* event) override;
        void mouseMoveEvent(QMouseEvent* event) override;
        void wheelEvent(QWheelEvent* event) override;

    private:
        /**
         * Create a port of a specific gate.
         *
         * @param port : the gate template port to create.
         */
        void create_port(GateTemplatePort_shptr port);

        /**
         * Create a out port.
         *
         * @param x : the x coordinate of the port.
         * @param y : the y coordinate of the port.
         * @param size : the size of the port.
         * @param color : the color of the port.
         * @param alpha : the opacity of the port (between 0 and 1).
         */
        void create_port_out(float x, float y, unsigned size, QVector3D color, float alpha);

        /**
         * Create a in port.
         *
         * @param x : the x coordinate of the port.
         * @param y : the y coordinate of the port.
         * @param size : the size of the port.
         * @param color : the color of the port.
         * @param alpha : the opacity of the port (between 0 and 1).
         */
        void create_port_in(float x, float y, unsigned size, QVector3D color, float alpha);

        /**
         * Create a in/out port.
         *
         * @param x : the x coordinate of the port.
         * @param y : the y coordinate of the port.
         * @param size : the size of the port.
         * @param color : the color of the port.
         * @param alpha : the opacity of the port (between 0 and 1).
         */
        void create_port_in_out(float x, float y, unsigned size, QVector3D color, float alpha);

        GateTemplate_shptr gate;
        Project_shptr project;
        GateTemplatePort_shptr port;
        unsigned int layer;
        GLuint vbo;
        QOpenGLShaderProgram* program = nullptr;
        Text port_name_text;

        Point pos;

    };
}

#endif