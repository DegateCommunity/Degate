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

#include "WorkspaceText.h"

namespace degate
{
	const static unsigned font_char_width[256] =
	{
		13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
		5, 5, 6, 9, 9, 15, 11, 3, 6, 6, 7, 10, 5, 6, 5, 5, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 5, 5, 10, 10, 10, 9, 
		17, 11, 11, 12, 12, 11, 10, 12, 11, 5, 9, 11, 9, 13, 11, 12, 11, 12, 11, 11, 9, 11, 11, 17, 11, 11, 9, 5, 5, 5, 7, 9,
		6, 9, 9, 9, 9, 9, 5, 9, 9, 4, 3, 8, 3, 13, 9, 9, 9, 9, 6, 8, 4, 9, 7, 11, 7, 9, 8, 6, 5, 6, 10, 13,
		9, 13, 4, 9, 6, 17, 9, 9, 6, 17, 11, 6, 17, 13, 9, 13, 13, 4, 4, 6, 6, 6, 9, 17, 4, 17, 8, 6, 16, 13, 8, 11,
		5, 5, 9, 9, 9, 9, 5, 9, 6, 13, 5, 9, 10, 6, 13, 9, 7, 9, 6, 6, 6, 9, 9, 6, 6, 6, 5, 9, 14, 14, 14, 10,
		11, 11, 11, 11, 11, 11, 17, 12, 11, 11, 11, 11, 5, 5, 5, 5, 12, 11, 12, 12, 12, 12, 12, 10, 13, 11, 11, 11, 11, 11, 11, 10,
		9, 9, 9, 9, 9, 9, 15, 9, 9, 9, 9, 9, 5, 5, 5, 5, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9
	};


	GLuint WorkspaceText::font_texture;
	QOpenGLShaderProgram WorkspaceText::program;
	QOpenGLFunctions* WorkspaceText::context;
	GLuint WorkspaceText::temp_vbo;

	struct Vertex2D
	{
		QVector2D pos;
		QVector2D tex_uv;
		QVector3D color;
		float alpha;
	};

	void WorkspaceText::init_font()
	{
		context = QOpenGLContext::currentContext()->functions();

		QOpenGLShader* vshader = new QOpenGLShader(QOpenGLShader::Vertex);
		const char* vsrc =
			"attribute vec2 pos;\n"
			"attribute vec2 uv;\n"
			"attribute vec3 color;\n"
			"attribute float alpha;\n"
			"uniform mat4 mvp;\n"
			"varying vec2 uv0;\n"
			"varying vec4 out_color;\n"
			"void main(void)\n"
			"{\n"
			"    gl_Position = mvp * vec4(pos, 0.0, 1.0);\n"
			"    uv0 = uv;\n"
			"	 out_color = vec4(color, alpha);\n"
			"}\n";
		vshader->compileSourceCode(vsrc);

		QOpenGLShader* fshader = new QOpenGLShader(QOpenGLShader::Fragment);
		const char* fsrc =
			"uniform sampler2D texture;\n"
			"varying vec2 uv0;\n"
			"varying vec4 out_color;\n"
			"void main(void)\n"
			"{\n"
			"    gl_FragColor = out_color * texture2D(texture, uv0);\n"
			"}\n";
		fshader->compileSourceCode(fsrc);

		program.addShader(vshader);
		program.addShader(fshader);

		program.link();

		QImage font_atlas("res/FontAtlas.png");
		assert(!font_atlas.isNull());

		auto data = new GLuint[512 * 512];
		assert(data != NULL);

		memcpy(data, font_atlas.bits(), 512 * 512 * sizeof(GLuint));

		context->glGenTextures(1, &font_texture);
		assert(context->glGetError() == GL_NO_ERROR);

		context->glBindTexture(GL_TEXTURE_2D, font_texture);
		assert(context->glGetError() == GL_NO_ERROR);

		context->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		assert(context->glGetError() == GL_NO_ERROR);

		context->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		assert(context->glGetError() == GL_NO_ERROR);

		context->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		assert(context->glGetError() == GL_NO_ERROR);

		context->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		assert(context->glGetError() == GL_NO_ERROR);

		context->glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_FALSE);
		assert(context->glGetError() == GL_NO_ERROR);

		context->glTexImage2D(GL_TEXTURE_2D,
		             0, // level
		             GL_RGBA, // BGRA,
		             512, 512,
		             0, // border
		             GL_RGBA,
		             GL_UNSIGNED_BYTE,
		             data);
		assert(context->glGetError() == GL_NO_ERROR);

		delete[] data;

		context->glBindTexture(GL_TEXTURE_2D, 0);

		context->glGenBuffers(1, &temp_vbo);
	}

	void WorkspaceText::delete_font()
	{
		if(QOpenGLContext::currentContext() == NULL)
			return;

		context = QOpenGLContext::currentContext()->functions();

		if(context == NULL)
			return;

		context->glDeleteTextures(1, &font_texture);

		context->glDeleteBuffers(1, &temp_vbo);
	}

	void WorkspaceText::draw_single(unsigned x, unsigned y, const char* text, const QMatrix4x4& projection, const unsigned size, const QVector3D& color, const float alpha)
	{
		float s = size / static_cast<float>(FONT_DEFAULT_SIZE);
		unsigned int len = strlen(text);
		const unsigned char* str = reinterpret_cast<const unsigned char*>(text);
		QVector3D final_color = color / 255.0;

		program.bind();
		program.setUniformValue("mvp", projection);

		context->glBindBuffer(GL_ARRAY_BUFFER, temp_vbo);
		context->glBufferData(GL_ARRAY_BUFFER, len * 6 * sizeof(Vertex2D), 0, GL_DYNAMIC_DRAW);

		program.enableAttributeArray("pos");
		program.setAttributeBuffer("pos", GL_FLOAT, 0, 2, sizeof(Vertex2D));

		program.enableAttributeArray("uv");
		program.setAttributeBuffer("uv", GL_FLOAT, 2 * sizeof(float), 2, sizeof(Vertex2D));

		program.enableAttributeArray("color");
		program.setAttributeBuffer("color", GL_FLOAT, 4 * sizeof(float), 3, sizeof(Vertex2D));

		program.enableAttributeArray("alpha");
		program.setAttributeBuffer("alpha", GL_FLOAT, 7 * sizeof(float), 1, sizeof(Vertex2D));

		Vertex2D temp;
		temp.alpha = alpha;
		temp.color = final_color;

		unsigned pixel_size = 0;
		for(unsigned i = 0; i < len; i++)
		{
			QVector2D uv((str[i] % FONT_GLYPH_PER_LINE) / static_cast<float>(FONT_GLYPH_PER_LINE), (str[i] / FONT_GLYPH_PER_LINE) / static_cast<float>(FONT_GLYPH_PER_LINE));

			temp.pos = QVector2D(x + pixel_size, y);
			temp.tex_uv = QVector2D(uv.x(), uv.y());
			context->glBufferSubData(GL_ARRAY_BUFFER, i * 6 * sizeof(Vertex2D) + 0 * sizeof(Vertex2D), sizeof(Vertex2D), &temp);

			temp.pos = QVector2D(x + font_char_width[str[i]] * s + pixel_size, y);
			temp.tex_uv = QVector2D(uv.x() + font_char_width[str[i]] / FONT_ATLAS_SIZE, uv.y());
			context->glBufferSubData(GL_ARRAY_BUFFER, i * 6 * sizeof(Vertex2D) + 1 * sizeof(Vertex2D), sizeof(Vertex2D), &temp);

			temp.pos = QVector2D(x + pixel_size, y + FONT_GLYPH_SIZE * s);
			temp.tex_uv = QVector2D(uv.x(), (uv.y() + FONT_GLYPH_SIZE / FONT_ATLAS_SIZE));
			context->glBufferSubData(GL_ARRAY_BUFFER, i * 6 * sizeof(Vertex2D) + 2 * sizeof(Vertex2D), sizeof(Vertex2D), &temp);

			temp.pos = QVector2D(x + pixel_size, y + FONT_GLYPH_SIZE * s);
			temp.tex_uv = QVector2D(uv.x(), (uv.y() + FONT_GLYPH_SIZE / FONT_ATLAS_SIZE));
			context->glBufferSubData(GL_ARRAY_BUFFER, i * 6 * sizeof(Vertex2D) + 3 * sizeof(Vertex2D), sizeof(Vertex2D), &temp);

			temp.pos = QVector2D(x + font_char_width[str[i]] * s + pixel_size, y);
			temp.tex_uv = QVector2D(uv.x() + font_char_width[str[i]] / FONT_ATLAS_SIZE, uv.y());
			context->glBufferSubData(GL_ARRAY_BUFFER, i * 6 * sizeof(Vertex2D) + 4 * sizeof(Vertex2D), sizeof(Vertex2D), &temp);

			temp.pos = QVector2D(x + font_char_width[str[i]] * s + pixel_size, y + FONT_GLYPH_SIZE * s);
			temp.tex_uv = QVector2D(uv.x() + font_char_width[str[i]] / FONT_ATLAS_SIZE, (uv.y() + FONT_GLYPH_SIZE / FONT_ATLAS_SIZE));
			context->glBufferSubData(GL_ARRAY_BUFFER, i * 6 * sizeof(Vertex2D) + 5 * sizeof(Vertex2D), sizeof(Vertex2D), &temp);

			pixel_size += font_char_width[str[i]] * s + TEXT_SPACE;
		}

		context->glEnable(GL_TEXTURE_2D);
		context->glBindTexture(GL_TEXTURE_2D, font_texture);

		context->glDrawArrays(GL_TRIANGLES, 0, len * 6);

		context->glBindTexture(GL_TEXTURE_2D, 0);
		context->glDisable(GL_TEXTURE_2D);

		context->glBindBuffer(GL_ARRAY_BUFFER, 0);
		program.release();
	}

	WorkspaceText::WorkspaceText(QWidget* new_parent) : parent(new_parent)
	{

	}

	WorkspaceText::~WorkspaceText()
	{
		context->glDeleteBuffers(1, &vbo);
	}

	void WorkspaceText::init()
	{
		context = QOpenGLContext::currentContext()->functions();

		context->glGenBuffers(1, &vbo);
	}

	void WorkspaceText::update(unsigned size)
	{
		context->glBindBuffer(GL_ARRAY_BUFFER, vbo);
		context->glBufferData(GL_ARRAY_BUFFER, size * 6 * sizeof(Vertex2D), 0, GL_DYNAMIC_DRAW);
		context->glBindBuffer(GL_ARRAY_BUFFER, 0);

		total_size = size;
	}

	void WorkspaceText::add_sub_text(unsigned offset, unsigned x, unsigned y, const char* text, const unsigned size, const QVector3D& color, const float alpha)
	{
		float s = size / static_cast<float>(FONT_DEFAULT_SIZE);
		unsigned int len = strlen(text);
		const unsigned char* str = reinterpret_cast<const unsigned char*>(text);
		QVector3D final_color = color / 255.0;

		context->glBindBuffer(GL_ARRAY_BUFFER, vbo);

		Vertex2D temp;
		temp.color = final_color;
		temp.alpha = alpha;

		unsigned pixel_size = 0;
		for(unsigned i = 0; i < len; i++)
		{
			QVector2D uv((str[i] % FONT_GLYPH_PER_LINE) / static_cast<float>(FONT_GLYPH_PER_LINE), (str[i] / FONT_GLYPH_PER_LINE) / static_cast<float>(FONT_GLYPH_PER_LINE));

			temp.pos = QVector2D(x + pixel_size, y);
			temp.tex_uv = QVector2D(uv.x(), uv.y());
			context->glBufferSubData(GL_ARRAY_BUFFER, offset * 6 * sizeof(Vertex2D) + i * 6 * sizeof(Vertex2D) + 0 * sizeof(Vertex2D), sizeof(Vertex2D), &temp);

			temp.pos = QVector2D(x + font_char_width[str[i]] * s + pixel_size, y);
			temp.tex_uv = QVector2D(uv.x() + font_char_width[str[i]] / FONT_ATLAS_SIZE, uv.y());
			context->glBufferSubData(GL_ARRAY_BUFFER, offset * 6 * sizeof(Vertex2D) + i * 6 * sizeof(Vertex2D) + 1 * sizeof(Vertex2D), sizeof(Vertex2D), &temp);

			temp.pos = QVector2D(x + pixel_size, y + FONT_GLYPH_SIZE * s);
			temp.tex_uv = QVector2D(uv.x(), (uv.y() + FONT_GLYPH_SIZE / FONT_ATLAS_SIZE));
			context->glBufferSubData(GL_ARRAY_BUFFER, offset * 6 * sizeof(Vertex2D) + i * 6 * sizeof(Vertex2D) + 2 * sizeof(Vertex2D), sizeof(Vertex2D), &temp);

			temp.pos = QVector2D(x + pixel_size, y + FONT_GLYPH_SIZE * s);
			temp.tex_uv = QVector2D(uv.x(), (uv.y() + FONT_GLYPH_SIZE / FONT_ATLAS_SIZE));
			context->glBufferSubData(GL_ARRAY_BUFFER, offset * 6 * sizeof(Vertex2D) + i * 6 * sizeof(Vertex2D) + 3 * sizeof(Vertex2D), sizeof(Vertex2D), &temp);

			temp.pos = QVector2D(x + font_char_width[str[i]] * s + pixel_size, y);
			temp.tex_uv = QVector2D(uv.x() + font_char_width[str[i]] / FONT_ATLAS_SIZE, uv.y());
			context->glBufferSubData(GL_ARRAY_BUFFER, offset * 6 * sizeof(Vertex2D) + i * 6 * sizeof(Vertex2D) + 4 * sizeof(Vertex2D), sizeof(Vertex2D), &temp);

			temp.pos = QVector2D(x + font_char_width[str[i]] * s + pixel_size, y + FONT_GLYPH_SIZE * s);
			temp.tex_uv = QVector2D(uv.x() + font_char_width[str[i]] / FONT_ATLAS_SIZE, (uv.y() + FONT_GLYPH_SIZE / FONT_ATLAS_SIZE));
			context->glBufferSubData(GL_ARRAY_BUFFER, offset * 6 * sizeof(Vertex2D) + i * 6 * sizeof(Vertex2D) + 5 * sizeof(Vertex2D), sizeof(Vertex2D), &temp);

			pixel_size += font_char_width[str[i]] * s + TEXT_SPACE;
		}

		context->glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void WorkspaceText::draw(const QMatrix4x4& projection)
	{
		program.bind();
		program.setUniformValue("mvp", projection);

		context->glBindBuffer(GL_ARRAY_BUFFER, vbo);

		program.enableAttributeArray("pos");
		program.setAttributeBuffer("pos", GL_FLOAT, 0, 2, sizeof(Vertex2D));

		program.enableAttributeArray("uv");
		program.setAttributeBuffer("uv", GL_FLOAT, 2 * sizeof(float), 2, sizeof(Vertex2D));

		program.enableAttributeArray("color");
		program.setAttributeBuffer("color", GL_FLOAT, 4 * sizeof(float), 3, sizeof(Vertex2D));

		program.enableAttributeArray("alpha");
		program.setAttributeBuffer("alpha", GL_FLOAT, 7 * sizeof(float), 1, sizeof(Vertex2D));

		context->glEnable(GL_TEXTURE_2D);
		context->glBindTexture(GL_TEXTURE_2D, font_texture);

		context->glDrawArrays(GL_TRIANGLES, 0, total_size * 6);

		context->glBindTexture(GL_TEXTURE_2D, 0);
		context->glDisable(GL_TEXTURE_2D);

		context->glBindBuffer(GL_ARRAY_BUFFER, 0);
		program.release();
	}
}
