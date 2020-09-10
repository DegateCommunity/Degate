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

#include "WorkspaceRegularGrid.h"

#include "GUI/Preferences/PreferencesHandler.h"

namespace degate
{
    struct RegularGridVertex2D
    {
        QVector2D pos;
        QVector3D color;
        float     alpha;
    };

    WorkspaceRegularGrid::WorkspaceRegularGrid(QWidget* parent) : WorkspaceElement(parent)
    {
        update_color();
    }

    WorkspaceRegularGrid::~WorkspaceRegularGrid()
    {

    }

    void WorkspaceRegularGrid::init()
    {
        WorkspaceElement::init();

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
        const char* fsrc  =
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

        // Init grid color.
        grid_color = PREFERENCES_HANDLER.get_preferences().grid_color;
        converted_grid_color = QVector3D(MASK_R(grid_color) / 255.0, MASK_G(grid_color) / 255.0, MASK_B(grid_color) / 255.0);
        converted_grid_alpha = MASK_A(grid_color) / 255.0;
    }

    void WorkspaceRegularGrid::update()
    {
        if (project == nullptr)
            return;

        //TODO: This can have performance drawbacks, another solution is to prepare different grid sizes
        // (multiple distances) and draw them for the whole project size, and regarding the viewport size it
        // should draw one or another grid (benefits: only one grid transfer to the GPU, just an if during draw (1 draw
        // call). The gpu will draw only visible lines (even if there are many lines passed to the gpu) so normally no
        // overhead.

        update_color();

        double distance_x = project->get_regular_horizontal_grid()->get_distance();
        double distance_y = project->get_regular_vertical_grid()->get_distance();

        if (distance_x < 1 || distance_y < 1)
            show_grid = false;
        else if (!show_grid)
            show_grid = true;

        double offset_x = project->get_regular_horizontal_grid()->get_min();
        double offset_y = project->get_regular_vertical_grid()->get_min();

        // Adapt distance to have a limited number of lines drawn
        unsigned int new_width_count  = static_cast<unsigned int>(std::ceil(viewport.get_width() / distance_x)) + 1;
        unsigned int new_height_count = static_cast<unsigned int>(std::ceil(viewport.get_height() / distance_y)) + 1;

        while ((new_width_count + new_height_count) > PREFERENCES_HANDLER.get_preferences().max_grid_lines_count)
        {
            distance_x *= 2;
            distance_y *= 2;

            new_width_count  = static_cast<unsigned int>(std::ceil(viewport.get_width() / distance_x)) + 1;
            new_height_count = static_cast<unsigned int>(std::ceil(viewport.get_height() / distance_y)) + 1;
        }

        context->glBindBuffer(GL_ARRAY_BUFFER, vbo);

        // If the number of lines is the same as before, skip buffer update
        if (new_height_count != height_count || new_width_count != width_count)
        {
            width_count = new_width_count;
            height_count = new_height_count;

            context->glBufferData(GL_ARRAY_BUFFER,
                                  2 * (width_count + height_count) * sizeof(RegularGridVertex2D),
                                  nullptr,
                                  GL_STATIC_DRAW);

        }

        RegularGridVertex2D temp;
        temp.color = converted_grid_color;
        temp.alpha = converted_grid_alpha;

        unsigned int index = 0;


        // Update vertical lines
        double shift_x = viewport.get_min_x() / distance_x;
        shift_x = viewport.get_min_x() + offset_x - std::floor(shift_x) * distance_x;
        shift_x = distance_x - shift_x;

        for (int x = 0; x < width_count; x += 1)
        {
            temp.pos = QVector2D(viewport.get_min_x() + shift_x + x * distance_x, viewport.get_min_y());
            context->glBufferSubData(GL_ARRAY_BUFFER,
                                     index * 2 * sizeof(RegularGridVertex2D) + 0 * sizeof(RegularGridVertex2D),
                                     sizeof(RegularGridVertex2D), &temp);

            temp.pos = QVector2D(viewport.get_min_x() + shift_x + x * distance_x, viewport.get_max_y());
            context->glBufferSubData(GL_ARRAY_BUFFER,
                                     index * 2 * sizeof(RegularGridVertex2D) + 1 * sizeof(RegularGridVertex2D),
                                     sizeof(RegularGridVertex2D), &temp);

            index++;
        }


        // Update horizontal lines
        double shift_y = viewport.get_min_y() / distance_y;
        shift_y = viewport.get_min_y() + offset_y - std::floor(shift_y) * distance_y;
        shift_y = distance_y - shift_y;

        for (int y = 0; y < height_count; y += 1)
        {
            temp.pos = QVector2D(viewport.get_min_x(), viewport.get_min_y() + shift_y + y * distance_y);
            context->glBufferSubData(GL_ARRAY_BUFFER,
                                     index * 2 * sizeof(RegularGridVertex2D) + 0 * sizeof(RegularGridVertex2D),
                                     sizeof(RegularGridVertex2D), &temp);

            temp.pos = QVector2D(viewport.get_max_x(), viewport.get_min_y() + shift_y + y * distance_y);
            context->glBufferSubData(GL_ARRAY_BUFFER,
                                     index * 2 * sizeof(RegularGridVertex2D) + 1 * sizeof(RegularGridVertex2D),
                                     sizeof(RegularGridVertex2D), &temp);

            index++;
        }

        context->glBindBuffer(GL_ARRAY_BUFFER, 0);

        assert(context->glGetError() == GL_NO_ERROR);
    }

    void WorkspaceRegularGrid::draw(const QMatrix4x4& projection)
    {
        if (project == nullptr || !show_grid)
            return;

        program->bind();

        program->setUniformValue("mvp", projection);

        context->glBindBuffer(GL_ARRAY_BUFFER, vbo);

        program->enableAttributeArray("pos");
        program->setAttributeBuffer("pos", GL_FLOAT, 0, 2, sizeof(RegularGridVertex2D));

        program->enableAttributeArray("color");
        program->setAttributeBuffer("color", GL_FLOAT, 2 * sizeof(float), 3, sizeof(RegularGridVertex2D));

        program->enableAttributeArray("alpha");
        program->setAttributeBuffer("alpha", GL_FLOAT, 5 * sizeof(float), 1, sizeof(RegularGridVertex2D));

        context->glDrawArrays(GL_LINES, 0, 2 * (width_count + height_count));

        context->glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void WorkspaceRegularGrid::viewport_update(const BoundingBox& viewport)
    {
        this->viewport = viewport;
    }

    void WorkspaceRegularGrid::update_color()
    {
        color_t preferences_color = PREFERENCES_HANDLER.get_preferences().grid_color;
        if (grid_color != preferences_color)
        {
            grid_color = preferences_color;

            converted_grid_color = QVector3D(MASK_R(grid_color) / 255.0, MASK_G(grid_color) / 255.0, MASK_B(grid_color) / 255.0);
            converted_grid_alpha = MASK_A(grid_color) / 255.0;
        }
    }
}