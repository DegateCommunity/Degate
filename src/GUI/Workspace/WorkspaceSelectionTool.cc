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

#include "WorkspaceSelectionTool.h"
#include "GUI/Preferences/PreferencesHandler.h"

namespace degate
{
    struct SelectionToolVertex2D
    {
        QVector2D pos;
        QVector3D color;
        float alpha;
    };

    WorkspaceSelectionTool::WorkspaceSelectionTool(QWidget* parent)
    {
        this->parent = parent;
    }

    WorkspaceSelectionTool::~WorkspaceSelectionTool()
    {
        if (program != nullptr)
            delete program;

        if (QOpenGLContext::currentContext() == nullptr || context == nullptr)
            return;

        if (context->glIsBuffer(vbo) == GL_TRUE)
            context->glDeleteBuffers(1, &vbo);

        if (context->glIsBuffer(line_vbo) == GL_TRUE)
            context->glDeleteBuffers(1, &line_vbo);
    }

    void WorkspaceSelectionTool::init()
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
        context->glGenBuffers(1, &line_vbo);

        context->glBindBuffer(GL_ARRAY_BUFFER, vbo);
        context->glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(SelectionToolVertex2D), nullptr, GL_STATIC_DRAW);

        context->glBindBuffer(GL_ARRAY_BUFFER, line_vbo);
        context->glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(SelectionToolVertex2D), nullptr, GL_STATIC_DRAW);

        context->glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void WorkspaceSelectionTool::update(float x, float y)
    {
        if (selection == false || project == nullptr)
            return;

        if (PREFERENCES_HANDLER.get_preferences().snap_to_grid && !object_selection_mode)
        {
            x = static_cast<float>(project->get_regular_horizontal_grid()->snap_to_grid(static_cast<int>(x)));
            y = static_cast<float>(project->get_regular_vertical_grid()->snap_to_grid(static_cast<int>(y)));
        }

        if (x < origin.x())
        {
            selection_box.set_max_x(origin.x());
            selection_box.set_min_x(x);
        }
        else
        {
            selection_box.set_max_x(x);
            selection_box.set_min_x(origin.x());
        }

        if (y < origin.y())
        {
            selection_box.set_max_y(origin.y());
            selection_box.set_min_y(y);
        }
        else
        {
            selection_box.set_max_y(y);
            selection_box.set_min_y(origin.y());
        }


        context->glBindBuffer(GL_ARRAY_BUFFER, vbo);


        // Vertices and colors

        SelectionToolVertex2D temp;

        temp.color = QVector3D(33, 76, 117) / 255.0;

        if (object_selection_mode)
            temp.alpha = 0.f;
        else
            temp.alpha = 0.6f;

        temp.pos = QVector2D(selection_box.get_min_x(), selection_box.get_min_y());
        context->glBufferSubData(GL_ARRAY_BUFFER, 0 * sizeof(SelectionToolVertex2D), sizeof(SelectionToolVertex2D), &temp);

        temp.pos = QVector2D(selection_box.get_max_x(), selection_box.get_min_y());
        context->glBufferSubData(GL_ARRAY_BUFFER, 1 * sizeof(SelectionToolVertex2D), sizeof(SelectionToolVertex2D), &temp);

        temp.pos = QVector2D(selection_box.get_min_x(), selection_box.get_max_y());
        context->glBufferSubData(GL_ARRAY_BUFFER, 2 * sizeof(SelectionToolVertex2D), sizeof(SelectionToolVertex2D), &temp);

        temp.pos = QVector2D(selection_box.get_max_x(), selection_box.get_min_y());
        context->glBufferSubData(GL_ARRAY_BUFFER, 4 * sizeof(SelectionToolVertex2D), sizeof(SelectionToolVertex2D), &temp);

        temp.pos = QVector2D(selection_box.get_min_x(), selection_box.get_max_y());
        context->glBufferSubData(GL_ARRAY_BUFFER, 3 * sizeof(SelectionToolVertex2D), sizeof(SelectionToolVertex2D), &temp);

        temp.pos = QVector2D(selection_box.get_max_x(), selection_box.get_max_y());
        context->glBufferSubData(GL_ARRAY_BUFFER, 5 * sizeof(SelectionToolVertex2D), sizeof(SelectionToolVertex2D), &temp);


        // Lines

        context->glBindBuffer(GL_ARRAY_BUFFER, line_vbo);

        temp.color = QVector3D(40, 80, 150) / 255.0;
        temp.alpha = 1;

        temp.pos = QVector2D(selection_box.get_min_x(), selection_box.get_min_y());
        context->glBufferSubData(GL_ARRAY_BUFFER, 0 * sizeof(SelectionToolVertex2D), sizeof(SelectionToolVertex2D), &temp);

        temp.pos = QVector2D(selection_box.get_max_x(), selection_box.get_min_y());
        context->glBufferSubData(GL_ARRAY_BUFFER, 1 * sizeof(SelectionToolVertex2D), sizeof(SelectionToolVertex2D), &temp);

        temp.pos = QVector2D(selection_box.get_min_x(), selection_box.get_min_y());
        context->glBufferSubData(GL_ARRAY_BUFFER, 2 * sizeof(SelectionToolVertex2D), sizeof(SelectionToolVertex2D), &temp);

        temp.pos = QVector2D(selection_box.get_min_x(), selection_box.get_max_y());
        context->glBufferSubData(GL_ARRAY_BUFFER, 3 * sizeof(SelectionToolVertex2D), sizeof(SelectionToolVertex2D), &temp);

        temp.pos = QVector2D(selection_box.get_max_x(), selection_box.get_min_y());
        context->glBufferSubData(GL_ARRAY_BUFFER, 4 * sizeof(SelectionToolVertex2D), sizeof(SelectionToolVertex2D), &temp);

        temp.pos = QVector2D(selection_box.get_max_x(), selection_box.get_max_y());
        context->glBufferSubData(GL_ARRAY_BUFFER, 5 * sizeof(SelectionToolVertex2D), sizeof(SelectionToolVertex2D), &temp);

        temp.pos = QVector2D(selection_box.get_min_x(), selection_box.get_max_y());
        context->glBufferSubData(GL_ARRAY_BUFFER, 6 * sizeof(SelectionToolVertex2D), sizeof(SelectionToolVertex2D), &temp);

        temp.pos = QVector2D(selection_box.get_max_x(), selection_box.get_max_y());
        context->glBufferSubData(GL_ARRAY_BUFFER, 7 * sizeof(SelectionToolVertex2D), sizeof(SelectionToolVertex2D), &temp);

        context->glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void WorkspaceSelectionTool::draw(const QMatrix4x4& projection)
    {
        if (selection == false || project == nullptr)
            return;

        program->bind();

        program->setUniformValue("mvp", projection);

        context->glBindBuffer(GL_ARRAY_BUFFER, vbo);

        program->enableAttributeArray("pos");
        program->setAttributeBuffer("pos", GL_FLOAT, 0, 2, sizeof(SelectionToolVertex2D));

        program->enableAttributeArray("color");
        program->setAttributeBuffer("color", GL_FLOAT, 2 * sizeof(float), 3, sizeof(SelectionToolVertex2D));

        program->enableAttributeArray("alpha");
        program->setAttributeBuffer("alpha", GL_FLOAT, 5 * sizeof(float), 1, sizeof(SelectionToolVertex2D));

        context->glDrawArrays(GL_TRIANGLES, 0, 6);

        context->glBindBuffer(GL_ARRAY_BUFFER, line_vbo);

        program->enableAttributeArray("pos");
        program->setAttributeBuffer("pos", GL_FLOAT, 0, 2, sizeof(SelectionToolVertex2D));

        program->enableAttributeArray("color");
        program->setAttributeBuffer("color", GL_FLOAT, 2 * sizeof(float), 3, sizeof(SelectionToolVertex2D));

        program->enableAttributeArray("alpha");
        program->setAttributeBuffer("alpha", GL_FLOAT, 5 * sizeof(float), 1, sizeof(SelectionToolVertex2D));

        context->glDrawArrays(GL_LINES, 0, 8);

        context->glBindBuffer(GL_ARRAY_BUFFER, 0);

        program->release();
    }

    bool WorkspaceSelectionTool::has_selection()
    {
        return selection;
    }

    void WorkspaceSelectionTool::set_selection_state(bool value)
    {
        selection = value;
    }

    bool WorkspaceSelectionTool::is_object_selection_mode_active()
    {
        return object_selection_mode;
    }

    void WorkspaceSelectionTool::set_object_selection_mode_state(bool value)
    {
        object_selection_mode = value;
    }

    BoundingBox WorkspaceSelectionTool::get_selection_box()
    {
        return selection_box;
    }

    void WorkspaceSelectionTool::set_origin(float x, float y)
    {
        if (project == nullptr)
            return;

        if (PREFERENCES_HANDLER.get_preferences().snap_to_grid && !object_selection_mode)
        {
            x = static_cast<float>(project->get_regular_horizontal_grid()->snap_to_grid(static_cast<int>(x)));
            y = static_cast<float>(project->get_regular_vertical_grid()->snap_to_grid(static_cast<int>(y)));
        }

        origin.setX(x);
        origin.setY(y);
    }

    void WorkspaceSelectionTool::set_project(const Project_shptr& new_project)
    {
        project = new_project;
    }
}
