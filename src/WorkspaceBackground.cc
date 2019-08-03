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

#include "WorkspaceBackground.h"

namespace degate
{
	struct Vertex2D
	{
		QVector2D pos;
		QVector2D texCoord;
	};

	WorkspaceBackground::WorkspaceBackground(QWidget* widget_parent) : WorkspaceElement(widget_parent)
	{

	}

	WorkspaceBackground::~WorkspaceBackground()
	{

	}

	void WorkspaceBackground::init()
	{
		WorkspaceElement::init();

		QOpenGLShader* vshader = new QOpenGLShader(QOpenGLShader::Vertex);
		const char* vsrc =
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

		QOpenGLShader* fshader = new QOpenGLShader(QOpenGLShader::Fragment);
		const char* fsrc =
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

		glGenBuffers(1, &vbo);
	}

	void WorkspaceBackground::update()
	{
		if (project == NULL)
			return;

		free_textures();

		ScalingManager_shptr smgr = project->get_logic_model()->get_current_layer()->get_scaling_manager();
		ScalingManager<BackgroundImage>::image_map_element elem = smgr->get_image(1/*scale*/); //Todo: fix scaling manager

		background_image = elem.second;
		if (background_image == NULL)
			return;

		glBindBuffer(GL_ARRAY_BUFFER, vbo);

		glBufferData(GL_ARRAY_BUFFER, background_image->get_tiles_number() * 6 * sizeof(Vertex2D), 0, GL_STATIC_DRAW);

		unsigned index = 0;
		for (unsigned int x = 0; x < background_image->get_width(); x += background_image->get_tile_size())
		{
			for (unsigned int y = 0; y < background_image->get_height(); y += background_image->get_tile_size())
			{
				background_textures.push_back(create_background_tile(x, y, 1/*elem.first*/, index)); //Todo: fix scaling manager

				index++;
			}
		}

		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void WorkspaceBackground::draw(const QMatrix4x4& projection)
	{
		program->bind();
		glEnable(GL_TEXTURE_2D);
		glEnable(GL_BLEND);

		program->setUniformValue("mvp", projection);

		glBindBuffer(GL_ARRAY_BUFFER, vbo);

		program->enableAttributeArray("pos");
		program->setAttributeBuffer("pos", GL_FLOAT, 0, 2, sizeof(Vertex2D));

		program->enableAttributeArray("texCoord");
		program->setAttributeBuffer("texCoord", GL_FLOAT, 2 * sizeof(float), 2, sizeof(Vertex2D));

		unsigned index = 0;
		for (auto& e : background_textures)
		{
			glBindTexture(GL_TEXTURE_2D, e);
			glDrawArrays(GL_TRIANGLES, index * 6, 6);

			index++;
		}

		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindTexture(GL_TEXTURE_2D, 0);

		glDisable(GL_TEXTURE_2D);
		program->release();
	}

	void WorkspaceBackground::free_textures()
	{
		if(background_textures.size() < 1)
			return;

		glDeleteTextures(background_textures.size(), &background_textures[0]);

		background_textures.clear();
	}

	GLuint WorkspaceBackground::create_background_tile(unsigned x, unsigned y, float pre_scaling, unsigned indice)
	{
		assert(project != NULL);
		assert(background_image != NULL);

		const unsigned int tile_width = background_image->get_tile_size();

		// Real pixel coordinates
		float min_x = (static_cast<float>(x)) * pre_scaling;
		float min_y = (static_cast<float>(y)) * pre_scaling;
		float max_x = min_x + static_cast<float>(tile_width) * pre_scaling;
		float max_y = min_y + static_cast<float>(tile_width) * pre_scaling;


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
}
