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

	class Vertex2D
	{
	public:
	    Vertex2D(){}
	    Vertex2D(const QPointF &p, const QPointF &c) : pos(p), texCoord(c)
	    {
	    }
	    QVector2D pos;
	    QVector2D texCoord;
	};

	WorkspaceRenderer::WorkspaceRenderer(QWidget * parent) : QOpenGLWidget(parent), background_vbo(QOpenGLBuffer::VertexBuffer)
	{
		setFocusPolicy(Qt::StrongFocus);
		setCursor(Qt::CrossCursor);

		view.setToIdentity();
	}

	WorkspaceRenderer::~WorkspaceRenderer()
	{
		doneCurrent();
		if(program != NULL)
			delete program;
	}

	void WorkspaceRenderer::update_screen()
	{
		if(project == NULL)
			return;

		update_background();

		update();
	}

	void WorkspaceRenderer::update_background()
	{
		if(project == NULL)
			return;

		ScalingManager_shptr smgr = project->get_logic_model()->get_current_layer()->get_scaling_manager();
		ScalingManager<BackgroundImage>::image_map_element elem = smgr->get_image(scale);

		background_image = elem.second;
		if(background_image == NULL)
			return;

		free_textures();

		glBindBuffer(GL_ARRAY_BUFFER, background_vbo);

		glBufferData(GL_ARRAY_BUFFER, background_image->get_tiles_number() * 6 * sizeof(Vertex2D), 0, GL_STATIC_DRAW);

		unsigned index = 0;
		for(unsigned int x = 0; x < background_image->get_width(); x+=background_image->get_tile_size())
		{
			for(unsigned int y = 0; y < background_image->get_height(); y+=background_image->get_tile_size())
			{
				background_textures.push_back(std::make_tuple(x, y, create_background_tile(x, y, elem.first, index)));

				index++;
			}
		}

		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void WorkspaceRenderer::set_project(const Project_shptr & new_project)
	{
		assert(new_project != NULL);

		project = new_project;

		background_image = project->get_logic_model()->get_current_layer()->get_image();

		update_screen();
	}

	void WorkspaceRenderer::free_textures()
	{
		for(auto const& e : background_textures)
		{
			glDeleteTextures(1, &std::get<2>(e));
		}

		background_textures.clear();
	}

	void WorkspaceRenderer::initializeGL()
	{
		initializeOpenGLFunctions();

		glClearColor(0.0, 0.0, 0.0, 0.0);
	    glEnable(GL_DEPTH_TEST);
	    glEnable(GL_CULL_FACE);

	    QOpenGLShader *vshader = new QOpenGLShader(QOpenGLShader::Vertex, this);
	    const char *vsrc =
	        "attribute vec2 pos;\n"
	        "attribute vec2 texCoord;\n"
	        "uniform mat4 mvp;\n"
	        "varying vec2 texCoord0;\n"
	        "void main(void)\n"
	        "{\n"
	        "    gl_Position = mvp * vec4(pos, 0.0, 1.0);\n"
	        "    texCoord0 = texCoord;\n"
	        "}\n";
	    vshader->compileSourceCode(vsrc);

	    QOpenGLShader *fshader = new QOpenGLShader(QOpenGLShader::Fragment, this);
	    const char *fsrc =
	        "uniform sampler2D texture;\n"
	        "varying vec2 texCoord0;\n"
	        "void main(void)\n"
	        "{\n"
	        "    gl_FragColor = texture2D(texture, texCoord0);\n"
	        "}\n";
	    fshader->compileSourceCode(fsrc);

	    program = new QOpenGLShaderProgram;
	    program->addShader(vshader);
	    program->addShader(fshader);

	    program->link();

		glGenBuffers(1, &background_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, background_vbo);

		program->bind();
		program->enableAttributeArray("pos");
		program->setAttributeBuffer("pos", GL_FLOAT, 0, 2, sizeof(Vertex2D));

		program->enableAttributeArray("texCoord");
		program->setAttributeBuffer("texCoord", GL_FLOAT, 2 * sizeof(float), 2, sizeof(Vertex2D));
		program->release();

		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void WorkspaceRenderer::paintGL()
	{
		makeCurrent();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		program->bind();
		glEnable(GL_TEXTURE_2D);
		glEnable(GL_BLEND);

		program->setUniformValue("mvp", projection * view);

		glBindBuffer(GL_ARRAY_BUFFER, background_vbo);

		unsigned index = 0;
		for(auto& e : background_textures)
		{
			glBindTexture(GL_TEXTURE_2D, std::get<2>(e));
			glDrawArrays(GL_TRIANGLES, index * 6, 6);

			index++;
		}

		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindTexture(GL_TEXTURE_2D, 0);

	    glDisable(GL_TEXTURE_2D);
		program->release();
	}

	void WorkspaceRenderer::resizeGL(int w, int h)
	{
		glViewport(0, 0, w, h);

		set_projection(NO_ZOOM, center_x, center_y);
	}

	GLuint WorkspaceRenderer::create_background_tile(unsigned x, unsigned y, float pre_scaling, unsigned indice)
	{
		assert(project != NULL);
		assert(background_image != NULL);

		const float tile_width = static_cast<float>(background_image->get_tile_size());


		// Real pixel coordinates
		float min_x = (static_cast<float>(x) - width() / 2.0) * pre_scaling;
		float min_y = (static_cast<float>(y) - height() / 2.0) * pre_scaling;
		float max_x = min_x + tile_width * pre_scaling;
		float max_y = min_y + tile_width * pre_scaling;


		// Texture

		auto data = new GLuint[tile_width * tile_width];
		assert(data != NULL);

		memset(data, 0, tile_width * tile_width * sizeof(GLuint));
		background_image->raw_copy(data, x, y);

		GLuint texture = 0;

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

		glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_FALSE);
		assert(glGetError() == GL_NO_ERROR);
	
		glTexImage2D(GL_TEXTURE_2D,
			       0, // level
			       GL_RGBA, // BGRA,
			       tile_width, tile_width,
			       0, // border
			       GL_RGBA,
			       GL_UNSIGNED_BYTE,
			       data);
		assert(glGetError() == GL_NO_ERROR);

		delete[] data;

		glBindTexture(GL_TEXTURE_2D, 0);


		// Vertices

		Vertex2D temp;

		temp.pos = QVector2D(min_x, min_y);
		temp.texCoord = QVector2D(0, 0);
		glBufferSubData(GL_ARRAY_BUFFER, indice * 6 * sizeof(Vertex2D) + 0 * sizeof(Vertex2D), sizeof(Vertex2D), &temp);

		temp.pos = QVector2D(max_x, min_y);
		temp.texCoord = QVector2D(1, 0);
		glBufferSubData(GL_ARRAY_BUFFER, indice * 6 * sizeof(Vertex2D) + 1 * sizeof(Vertex2D), sizeof(Vertex2D), &temp);

		temp.pos = QVector2D(min_x, max_y);
		temp.texCoord = QVector2D(0, 1);
		glBufferSubData(GL_ARRAY_BUFFER, indice * 6 * sizeof(Vertex2D) + 2 * sizeof(Vertex2D), sizeof(Vertex2D), &temp);

		temp.pos = QVector2D(max_x, min_y);
		temp.texCoord = QVector2D(1, 0);
		glBufferSubData(GL_ARRAY_BUFFER, indice * 6 * sizeof(Vertex2D) + 4 * sizeof(Vertex2D), sizeof(Vertex2D), &temp);

		temp.pos = QVector2D(min_x, max_y);
		temp.texCoord = QVector2D(0, 1);
		glBufferSubData(GL_ARRAY_BUFFER, indice * 6 * sizeof(Vertex2D) + 3 * sizeof(Vertex2D), sizeof(Vertex2D), &temp);

		temp.pos = QVector2D(max_x, max_y);
		temp.texCoord = QVector2D(1, 1);
		glBufferSubData(GL_ARRAY_BUFFER, indice * 6 * sizeof(Vertex2D) + 5 * sizeof(Vertex2D), sizeof(Vertex2D), &temp);


		return texture;
	}

	QPointF WorkspaceRenderer::get_widget_mouse_position() const
	{
		const QPointF qt_widget_relative = mapFromGlobal(QCursor::pos());
		return QPoint(qt_widget_relative.x(), height() - qt_widget_relative.y());
	}

	QPointF WorkspaceRenderer::get_opengl_mouse_position() const
	{
		const QPointF widget_mouse_position = get_widget_mouse_position();
		return QPoint(viewport_min_x + widget_mouse_position.x() * scale, viewport_min_y + widget_mouse_position.y() * scale);
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
		projection.scale(1, -1, 1);
		projection.ortho(viewport_min_x, viewport_max_x, viewport_max_y, viewport_min_y, -1, 1);
	}

	void WorkspaceRenderer::mousePressEvent(QMouseEvent *event)
	{
		QOpenGLWidget::mousePressEvent(event);

		mouse_last_pos = get_opengl_mouse_position();

		if(event->button() == Qt::LeftButton)
			setCursor(Qt::ClosedHandCursor);
	}

	void WorkspaceRenderer::mouseReleaseEvent(QMouseEvent *event)
	{
		QOpenGLWidget::mouseReleaseEvent(event);
		
		if(event->button() == Qt::LeftButton)
			setCursor(Qt::CrossCursor);
	}

	void WorkspaceRenderer::mouseMoveEvent(QMouseEvent *event)
	{
		QOpenGLWidget::mouseMoveEvent(event);

		if (event->buttons() & Qt::LeftButton)
		{
			int dx = get_opengl_mouse_position().x() - mouse_last_pos.x();
			int dy = get_opengl_mouse_position().y() - mouse_last_pos.y();

			center_x -= dx;
			center_y -= dy;
			set_projection(NO_ZOOM, center_x, center_y);

			update();
		}
	}

	void WorkspaceRenderer::wheelEvent(QWheelEvent *event)
	{
		QOpenGLWidget::wheelEvent(event);

		if(project == NULL)
			return;

		event->delta() > 0 ? set_projection(ZOOM_OUT, center_x, center_y) : set_projection(ZOOM_IN, center_x, center_y);

		event->accept();
		update();
	}

	void WorkspaceRenderer::keyPressEvent(QKeyEvent *event)
	{
		QOpenGLWidget::keyPressEvent(event);

	}

	void WorkspaceRenderer::keyReleaseEvent(QKeyEvent *event)
	{
		QOpenGLWidget::keyReleaseEvent(event);


	}

	void WorkspaceRenderer::mouseDoubleClickEvent(QMouseEvent *event)
	{
		QOpenGLWidget::mouseDoubleClickEvent(event);


	}

}
