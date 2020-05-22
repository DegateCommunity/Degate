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

#include "WorkspaceEMarkers.h"

#define TEXT_PADDING 2

namespace degate
{
    struct EMarkersVertex2D
    {
        QVector2D pos;
        QVector3D color;
        float alpha;
    };

    WorkspaceEMarkers::WorkspaceEMarkers(QWidget *parent) : WorkspaceElement(parent), text(parent)
    {

    }

    WorkspaceEMarkers::~WorkspaceEMarkers()
    {

    }

    void WorkspaceEMarkers::init()
    {
        WorkspaceElement::init();

        text.init();

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
    }

    void WorkspaceEMarkers::update()
    {
        if(project == NULL)
            return;

        Layer_shptr layer = project->get_logic_model()->get_current_layer();

        if(layer == NULL)
            return;

        // Keep only emarkers of the active layer.
        std::vector<EMarker_shptr> emarkers;
        for(Layer::object_iterator iter = layer->objects_begin(); iter != layer->objects_end(); ++iter)
        {
            if(EMarker_shptr a = std::dynamic_pointer_cast<EMarker>(*iter))
            {
                emarkers.push_back(a);
            }
        }
        emarkers_count = emarkers.size();

        if(emarkers_count == 0)
            return;

        context->glBindBuffer(GL_ARRAY_BUFFER, vbo);

        context->glBufferData(GL_ARRAY_BUFFER, emarkers_count * 6 * sizeof(EMarkersVertex2D), 0, GL_STATIC_DRAW);

        context->glBindBuffer(GL_ARRAY_BUFFER, 0);

        unsigned text_size = 0;

        unsigned index = 0;
        for(auto& e : emarkers)
        {
            create_emarker(e, index);
            e->set_index(index);

            text_size += e->get_name().length();
            index++;
        }

        text.update(text_size);

        unsigned text_offset = 0;
        for(auto& e : emarkers)
        {
            unsigned x = e->get_x();
            unsigned y = e->get_y() + e->get_diameter() / 2.0 + TEXT_PADDING;
            text.add_sub_text(text_offset, x, y, e->get_name(), 5, QVector3D(255, 255, 255), 1, true, false);

            text_offset += e->get_name().length();
            index++;
        }
    }

    void WorkspaceEMarkers::update(EMarker_shptr &emarker)
    {
        if(emarker == NULL)
            return;

        create_emarker(emarker, emarker->get_index());
    }

    void WorkspaceEMarkers::draw(const QMatrix4x4 &projection)
    {
        if(project == NULL || emarkers_count == 0)
            return;

        program->bind();

        program->setUniformValue("mvp", projection);

        context->glBindBuffer(GL_ARRAY_BUFFER, vbo);

        program->enableAttributeArray("pos");
        program->setAttributeBuffer("pos", GL_FLOAT, 0, 2, sizeof(EMarkersVertex2D));

        program->enableAttributeArray("color");
        program->setAttributeBuffer("color", GL_FLOAT, 2 * sizeof(float), 3, sizeof(EMarkersVertex2D));

        program->enableAttributeArray("alpha");
        program->setAttributeBuffer("alpha", GL_FLOAT, 5 * sizeof(float), 1, sizeof(EMarkersVertex2D));

        context->glDrawArrays(GL_TRIANGLES, 0, emarkers_count * 6);

        context->glBindBuffer(GL_ARRAY_BUFFER, 0);

        program->release();
    }

    void WorkspaceEMarkers::draw_name(const QMatrix4x4 &projection)
    {
        if(project == NULL || emarkers_count == 0)
            return;

        text.draw(projection);
    }

    void WorkspaceEMarkers::create_emarker(EMarker_shptr &emarker, unsigned int index)
    {
        if(emarker == NULL)
            return;

        context->glBindBuffer(GL_ARRAY_BUFFER, vbo);

        // Vertices and colors

        color_t color = emarker->get_fill_color() == 0 ? project->get_default_color(DEFAULT_COLOR_EMARKER) : emarker->get_fill_color();

        color = highlight_color_by_state(color, emarker->get_highlighted());

        EMarkersVertex2D temp;
        temp.color = QVector3D(MASK_R(color) / 255.0, MASK_G(color) / 255.0, MASK_B(color) / 255.0);
        temp.alpha = MASK_A(color) / 255.0;

        temp.pos = QVector2D(emarker->get_x() - emarker->get_diameter() / 2.0, emarker->get_y() - emarker->get_diameter() / 2.0);
        context->glBufferSubData(GL_ARRAY_BUFFER, index * 6 * sizeof(EMarkersVertex2D) + 0 * sizeof(EMarkersVertex2D), sizeof(EMarkersVertex2D), &temp);

        temp.pos = QVector2D(emarker->get_x() + emarker->get_diameter() / 2.0, emarker->get_y() - emarker->get_diameter() / 2.0);
        context->glBufferSubData(GL_ARRAY_BUFFER, index * 6 * sizeof(EMarkersVertex2D) + 1 * sizeof(EMarkersVertex2D), sizeof(EMarkersVertex2D), &temp);

        temp.pos = QVector2D(emarker->get_x() + emarker->get_diameter() / 2.0, emarker->get_y() + emarker->get_diameter() / 2.0);
        context->glBufferSubData(GL_ARRAY_BUFFER, index * 6 * sizeof(EMarkersVertex2D) + 2 * sizeof(EMarkersVertex2D), sizeof(EMarkersVertex2D), &temp);

        temp.pos = QVector2D(emarker->get_x() - emarker->get_diameter() / 2.0, emarker->get_y() - emarker->get_diameter() / 2.0);
        context->glBufferSubData(GL_ARRAY_BUFFER, index * 6 * sizeof(EMarkersVertex2D) + 4 * sizeof(EMarkersVertex2D), sizeof(EMarkersVertex2D), &temp);

        temp.pos = QVector2D(emarker->get_x() - emarker->get_diameter() / 2.0, emarker->get_y() + emarker->get_diameter() / 2.0);
        context->glBufferSubData(GL_ARRAY_BUFFER, index * 6 * sizeof(EMarkersVertex2D) + 3 * sizeof(EMarkersVertex2D), sizeof(EMarkersVertex2D), &temp);

        temp.pos = QVector2D(emarker->get_x() + emarker->get_diameter() / 2.0, emarker->get_y() + emarker->get_diameter() / 2.0);
        context->glBufferSubData(GL_ARRAY_BUFFER, index * 6 * sizeof(EMarkersVertex2D) + 5 * sizeof(EMarkersVertex2D), sizeof(EMarkersVertex2D), &temp);

        context->glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
}