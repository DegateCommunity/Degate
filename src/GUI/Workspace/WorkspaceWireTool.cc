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

#include "WorkspaceWireTool.h"
#include <GUI/Preferences/PreferencesHandler.h>

namespace degate
{
    struct WireToolVertex2D
    {
        QVector2D pos;
        QVector3D color;
        float alpha;
    };

    WorkspaceWireTool::WorkspaceWireTool(QWidget *parent) : parent(parent)
    {

    }

    WorkspaceWireTool::~WorkspaceWireTool()
    {
        if (program != nullptr)
            delete program;

        if (QOpenGLContext::currentContext() == nullptr || context == nullptr)
            return;

        if (context->glIsBuffer(vbo) == GL_TRUE)
            context->glDeleteBuffers(1, &vbo);
    }

    void WorkspaceWireTool::init()
    {
        context = QOpenGLContext::currentContext()->functions();

        QOpenGLShader* vshader = new QOpenGLShader(QOpenGLShader::Vertex);
        const char* vsrc =
                "attribute vec2 pos;\n"
                "attribute vec3 color;\n"
                "attribute float alpha;\n"
                "uniform mat4 mvp;\n"
                "varying vec4 out_color;\n"
                "void main(void)\n"
                "{\n"
                "    gl_Position = mvp * vec4(pos, 0.0, 1.0);\n"
                "    out_color = vec4(color, alpha);\n"
                "}\n";
        vshader->compileSourceCode(vsrc);

        QOpenGLShader* fshader = new QOpenGLShader(QOpenGLShader::Fragment);
        const char* fsrc =
                "varying vec4 out_color;\n"
                "void main(void)\n"
                "{\n"
                "    gl_FragColor = out_color;\n"
                "}\n";
        fshader->compileSourceCode(fsrc);

        program = new QOpenGLShaderProgram;
        program->addShader(vshader);
        program->addShader(fshader);

        program->link();

        delete vshader;
        delete fshader;

        context->glEnable(GL_LINE_SMOOTH);

        context->glGenBuffers(1, &vbo);

        context->glBindBuffer(GL_ARRAY_BUFFER, vbo);
        context->glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(WireToolVertex2D), nullptr, GL_STATIC_DRAW);

        context->glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void WorkspaceWireTool::update(float x, float y)
    {
        if (started == false || ended == true || project == nullptr)
            return;

        if (PREFERENCES_HANDLER.get_preferences().snap_to_grid)
        {
            x = static_cast<float>(project->get_regular_horizontal_grid()->snap_to_grid(static_cast<int>(x)));
            y = static_cast<float>(project->get_regular_vertical_grid()->snap_to_grid(static_cast<int>(y)));
        }

        line.set_to_x(x);
        line.set_to_y(y);
        line.set_diameter(project->get_default_wire_diameter());


        context->glBindBuffer(GL_ARRAY_BUFFER, vbo);


        // Vertices and colors

        WireToolVertex2D temp;

        color_t color = project->get_default_color(DEFAULT_COLOR_WIRE);

        temp.color = QVector3D(MASK_R(color) / 255.0, MASK_G(color) / 255.0, MASK_B(color) / 255.0);
        temp.alpha = MASK_A(color) / 255.0;

        float radius = static_cast<float>(line.get_diameter()) / 2.0f;

        QVector2D difference_vector(line.get_to_x() - line.get_from_x(), line.get_to_y() - line.get_from_y());

        QVector2D parallel_vector = difference_vector;
        parallel_vector.normalize();
        float to_x = line.get_to_x() + radius * parallel_vector.x(), from_x = line.get_from_x() - radius * parallel_vector.x();
        float to_y = line.get_to_y() + radius * parallel_vector.y(), from_y = line.get_from_y() - radius * parallel_vector.y();
        QVector2D perpendicular_vector(difference_vector.y(), -difference_vector.x());
        perpendicular_vector.normalize();

        temp.pos = QVector2D(from_x + perpendicular_vector.x() * radius, from_y + perpendicular_vector.y() * radius);
        context->glBufferSubData(GL_ARRAY_BUFFER, 0 * sizeof(WireToolVertex2D), sizeof(WireToolVertex2D), &temp);

        temp.pos = QVector2D(from_x - perpendicular_vector.x() * radius, from_y - perpendicular_vector.y() * radius);
        context->glBufferSubData(GL_ARRAY_BUFFER, 1 * sizeof(WireToolVertex2D), sizeof(WireToolVertex2D), &temp);

        temp.pos = QVector2D(to_x + perpendicular_vector.x() * radius, to_y + perpendicular_vector.y() * radius);
        context->glBufferSubData(GL_ARRAY_BUFFER, 2 * sizeof(WireToolVertex2D), sizeof(WireToolVertex2D), &temp);

        temp.pos = QVector2D(to_x + perpendicular_vector.x() * radius, to_y + perpendicular_vector.y() * radius);
        context->glBufferSubData(GL_ARRAY_BUFFER, 4 * sizeof(WireToolVertex2D), sizeof(WireToolVertex2D), &temp);

        temp.pos = QVector2D(to_x - perpendicular_vector.x() * radius, to_y - perpendicular_vector.y() * radius);
        context->glBufferSubData(GL_ARRAY_BUFFER, 3 * sizeof(WireToolVertex2D), sizeof(WireToolVertex2D), &temp);

        temp.pos = QVector2D(from_x - perpendicular_vector.x() * radius, from_y - perpendicular_vector.y() * radius);
        context->glBufferSubData(GL_ARRAY_BUFFER, 5 * sizeof(WireToolVertex2D), sizeof(WireToolVertex2D), &temp);

        context->glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void WorkspaceWireTool::draw(const QMatrix4x4 &projection)
    {
        if (started == false || project == nullptr || (line.get_from_x() == line.get_to_x() && line.get_from_y() == line.get_to_y()))
            return;

        program->bind();

        program->setUniformValue("mvp", projection);

        context->glBindBuffer(GL_ARRAY_BUFFER, vbo);

        program->enableAttributeArray("pos");
        program->setAttributeBuffer("pos", GL_FLOAT, 0, 2, sizeof(WireToolVertex2D));

        program->enableAttributeArray("color");
        program->setAttributeBuffer("color", GL_FLOAT, 2 * sizeof(float), 3, sizeof(WireToolVertex2D));

        program->enableAttributeArray("alpha");
        program->setAttributeBuffer("alpha", GL_FLOAT, 5 * sizeof(float), 1, sizeof(WireToolVertex2D));

        context->glDrawArrays(GL_TRIANGLES, 0, 6);

        context->glBindBuffer(GL_ARRAY_BUFFER, 0);

        program->release();
    }

    void WorkspaceWireTool::start_line_drawing(float x, float y)
    {
        if (project == nullptr)
            return;

        if (PREFERENCES_HANDLER.get_preferences().snap_to_grid)
        {
            x = static_cast<float>(project->get_regular_horizontal_grid()->snap_to_grid(static_cast<int>(x)));
            y = static_cast<float>(project->get_regular_vertical_grid()->snap_to_grid(static_cast<int>(y)));
        }

        line.set_from_x(x);
        line.set_from_y(y);

        started = true;
        ended = false;

        update(x, y);
    }

    void WorkspaceWireTool::end_line_drawing()
    {
        if (project == nullptr)
            return;

        ended = true;
    }

    void WorkspaceWireTool::reset_line_drawing()
    {
        if (project == nullptr)
            return;

        started = false;
        ended = false;
    }

    bool WorkspaceWireTool::has_started()
    {
        return started;
    }

    bool WorkspaceWireTool::has_ended()
    {
        return ended;
    }

    Line WorkspaceWireTool::get_line()
    {
        return line;
    }

    void WorkspaceWireTool::set_project(const Project_shptr& new_project)
    {
        project = new_project;
    }
}