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

#include "ImageRenderer.h"

namespace degate
{
    struct ImageVertex2D
    {
        QVector2D pos;
        QVector2D texCoord;
    };

    ImageRenderer::ImageRenderer(QWidget* parent, MemoryImage_shptr image, bool update_on_gl_initialize)
            : QOpenGLWidget(parent), update_on_gl_initialize(update_on_gl_initialize), image(image)
    {
        assert(image != nullptr);

        center_x = image->get_width() / 2.0;
        center_y = image->get_height() / 2.0;
    }

    ImageRenderer::~ImageRenderer()
    {
        makeCurrent();

        this->cleanup();

        if (vao.isCreated())
            vao.destroy();

        doneCurrent();
    }

    void ImageRenderer::update_screen()
    {
        makeCurrent();

        free_texture();

        vao.bind();
        glBindBuffer(GL_ARRAY_BUFFER, vbo);

        glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(ImageVertex2D), nullptr, GL_STATIC_DRAW);

        // Texture

        auto data = new GLuint[static_cast<std::size_t>(image->get_width()) *
                               static_cast<std::size_t>(image->get_height()) *
                               sizeof(GLuint)];
        image->raw_copy(data);

        glGenTextures(1, &texture);
        assert(glGetError() == GL_NO_ERROR);

        glBindTexture(GL_TEXTURE_2D, texture);
        assert(glGetError() == GL_NO_ERROR);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        assert(glGetError() == GL_NO_ERROR);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        assert(glGetError() == GL_NO_ERROR);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        assert(glGetError() == GL_NO_ERROR);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        assert(glGetError() == GL_NO_ERROR);

        //glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_FALSE);
        //assert(glGetError() == GL_NO_ERROR);

        glTexImage2D(GL_TEXTURE_2D,
                     0, // level
                     GL_RGBA, // BGRA,
                     image->get_width(), image->get_height(),
                     0, // border
                     GL_RGBA,
                     GL_UNSIGNED_BYTE,
                     data);
        assert(glGetError() == GL_NO_ERROR);

        delete[] data;

        glBindTexture(GL_TEXTURE_2D, 0);


        // Vertices

        ImageVertex2D temp;

        temp.pos = QVector2D(0, 0);
        temp.texCoord = QVector2D(0, 0);
        glBufferSubData(GL_ARRAY_BUFFER, 0 * sizeof(ImageVertex2D), sizeof(ImageVertex2D), &temp);

        temp.pos = QVector2D(image->get_width(), 0);
        temp.texCoord = QVector2D(1, 0);
        glBufferSubData(GL_ARRAY_BUFFER, 1 * sizeof(ImageVertex2D), sizeof(ImageVertex2D), &temp);

        temp.pos = QVector2D(0, image->get_height());
        temp.texCoord = QVector2D(0, 1);
        glBufferSubData(GL_ARRAY_BUFFER, 2 * sizeof(ImageVertex2D), sizeof(ImageVertex2D), &temp);

        temp.pos = QVector2D(image->get_width(), 0);
        temp.texCoord = QVector2D(1, 0);
        glBufferSubData(GL_ARRAY_BUFFER, 3 * sizeof(ImageVertex2D), sizeof(ImageVertex2D), &temp);

        temp.pos = QVector2D(0, image->get_height());
        temp.texCoord = QVector2D(0, 1);
        glBufferSubData(GL_ARRAY_BUFFER, 4 * sizeof(ImageVertex2D), sizeof(ImageVertex2D), &temp);

        temp.pos = QVector2D(image->get_width(), image->get_height());
        temp.texCoord = QVector2D(1, 1);
        glBufferSubData(GL_ARRAY_BUFFER, 5 * sizeof(ImageVertex2D), sizeof(ImageVertex2D), &temp);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        vao.release();

        update();
    }

    void ImageRenderer::change_image(MemoryImage_shptr image)
    {
        this->image = image;

        update_screen();
    }

    void ImageRenderer::free_texture()
    {
        if (glIsTexture(texture) == GL_TRUE)
            glDeleteTextures(1, &texture);
    }

    void ImageRenderer::cleanup()
    {
        makeCurrent();

        // Delete opengl objects here

        free_texture();

        if (program != nullptr)
            delete program;

        if (glIsBuffer(vbo) == GL_TRUE)
            glDeleteBuffers(1, &vbo);
    }

    void ImageRenderer::initializeGL()
    {
        makeCurrent();

        initializeOpenGLFunctions();

        glClearColor(0.0, 0.0, 0.0, 1.0);
        glEnable(GL_BLEND);
        glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ZERO, GL_ONE);
        glDisable(GL_LINE_SMOOTH);

        QOpenGLShader* vshader = new QOpenGLShader(QOpenGLShader::Vertex);
        const char* vsrc =
            "#version 330 core\n"
            "in vec2 pos;\n"
            "in vec2 texCoord;\n"
            "uniform mat4 mvp;\n"
            "out vec2 texCoord0;\n"
            "void main(void)\n"
            "{\n"
            "    gl_Position = mvp * vec4(pos, 0.0, 1.0);\n"
            "    texCoord0 = texCoord;\n"
            "}\n";
        vshader->compileSourceCode(vsrc);

        QOpenGLShader* fshader = new QOpenGLShader(QOpenGLShader::Fragment);
        const char* fsrc =
            "#version 330 core\n"
            "uniform sampler2D u_texture;\n"
            "in vec2 texCoord0;\n"
            "out vec4 color;\n"
            "void main(void)\n"
            "{\n"
            "    color = texture(u_texture, texCoord0);\n"
            "}\n";
        fshader->compileSourceCode(fsrc);

        program = new QOpenGLShaderProgram;
        program->addShader(vshader);
        program->addShader(fshader);

        delete vshader;
        delete fshader;

        program->link();

        glGenBuffers(1, &vbo);
        vao.create();

        if (update_on_gl_initialize)
            update_screen();
    }

    void ImageRenderer::paintGL()
    {
        makeCurrent();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        program->bind();

        program->setUniformValue("mvp", projection);

        vao.bind();
        glBindBuffer(GL_ARRAY_BUFFER, vbo);

        program->enableAttributeArray("pos");
        program->setAttributeBuffer("pos", GL_FLOAT, 0, 2, sizeof(ImageVertex2D));

        program->enableAttributeArray("texCoord");
        program->setAttributeBuffer("texCoord", GL_FLOAT, 2 * sizeof(float), 2, sizeof(ImageVertex2D));

        glBindTexture(GL_TEXTURE_2D, texture);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        vao.release();

        glBindTexture(GL_TEXTURE_2D, 0);

        program->release();
    }

    void ImageRenderer::resizeGL(int w, int h)
    {
        makeCurrent();

        glViewport(0, 0, w, h);

        set_projection(NO_ZOOM, center_x, center_y);
    }

    void ImageRenderer::mousePressEvent(QMouseEvent* event)
    {
        QOpenGLWidget::mousePressEvent(event);

        mouse_last_pos = get_opengl_mouse_position();

        if (event->button() == Qt::LeftButton)
            setCursor(Qt::ClosedHandCursor);
    }

    void ImageRenderer::mouseReleaseEvent(QMouseEvent* event)
    {
        QOpenGLWidget::mouseReleaseEvent(event);

        if (event->button() == Qt::LeftButton)
            setCursor(Qt::CrossCursor);
    }

    void ImageRenderer::mouseMoveEvent(QMouseEvent* event)
    {
        // Movement
        if (event->buttons() & Qt::LeftButton)
        {
            float dx = get_opengl_mouse_position().x() - mouse_last_pos.x();
            float dy = get_opengl_mouse_position().y() - mouse_last_pos.y();

            center_x -= dx;
            center_y -= dy;
            set_projection(NO_ZOOM, center_x, center_y);

            update();
        }
    }

    void ImageRenderer::wheelEvent(QWheelEvent* event)
    {
        QOpenGLWidget::wheelEvent(event);

        event->angleDelta().y() < 0 ? set_projection(ZOOM_OUT, center_x, center_y) : set_projection(ZOOM_IN, center_x, center_y);

        event->accept();

        update();
    }

    void ImageRenderer::set_projection(float scale_factor, float new_center_x, float new_center_y)
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

    QPointF ImageRenderer::get_widget_mouse_position() const
    {
        const QPointF qt_widget_relative = mapFromGlobal(QCursor::pos());
        return QPointF(qt_widget_relative.x(), qt_widget_relative.y());
    }

    QPointF ImageRenderer::get_opengl_mouse_position() const
    {
        const QPointF widget_mouse_position = get_widget_mouse_position();
        return QPointF(viewport_min_x + widget_mouse_position.x() * scale,
                      viewport_min_y + widget_mouse_position.y() * scale);
    }
}
