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

#include "WorkspaceGates.h"

namespace degate
{
	struct Vertex2D
	{
		QVector2D pos;
		QVector3D color;
		float alpha;
	};

	WorkspaceGates::WorkspaceGates(QWidget* new_parent) : WorkspaceElement(new_parent), text(new_parent)
	{
	}

	WorkspaceGates::~WorkspaceGates()
	{
		context->glDeleteBuffers(1, &line_vbo);
	}

	void WorkspaceGates::init()
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

		context->glGenBuffers(1, &vbo);
		context->glGenBuffers(1, &line_vbo);

		context->glEnable(GL_LINE_SMOOTH);
	}

	void WorkspaceGates::update()
	{
		if(project == NULL || project->get_logic_model()->get_gates_count() == 0)
			return;

		context->glBindBuffer(GL_ARRAY_BUFFER, vbo);

		context->glBufferData(GL_ARRAY_BUFFER, project->get_logic_model()->get_gates_count() * 6 * sizeof(Vertex2D), 0, GL_STATIC_DRAW);

		context->glBindBuffer(GL_ARRAY_BUFFER, line_vbo);

		context->glBufferData(GL_ARRAY_BUFFER, project->get_logic_model()->get_gates_count() * 8 * sizeof(Vertex2D), 0, GL_STATIC_DRAW);

		context->glBindBuffer(GL_ARRAY_BUFFER, 0);

		unsigned text_size = 0;

		unsigned indice = 0;
		for(LogicModel::gate_collection::iterator iter = project->get_logic_model()->gates_begin(); iter != project->get_logic_model()->gates_end(); ++iter)
		{
			create_gate(iter->second, indice);

			text_size += iter->second->get_gate_template()->get_name().length();
			indice++;
		}

		text.update(text_size);

		unsigned text_offset = 0;
		for(LogicModel::gate_collection::iterator iter = project->get_logic_model()->gates_begin(); iter != project->get_logic_model()->gates_end(); ++iter)
		{
			text.add_sub_text(text_offset, iter->second->get_min_x(), iter->second->get_min_y(), iter->second->get_gate_template()->get_name().c_str(), 10, QVector3D(50, 50, 50), 1);

			text_offset += iter->second->get_gate_template()->get_name().length();
			indice++;
		}
	}

	void WorkspaceGates::draw(const QMatrix4x4& projection)
	{
		if(project == NULL || project->get_logic_model()->get_gates_count() == 0)
			return;

		program->bind();

		context->glEnable(GL_BLEND);

		program->setUniformValue("mvp", projection);

		context->glBindBuffer(GL_ARRAY_BUFFER, vbo);

		program->enableAttributeArray("pos");
		program->setAttributeBuffer("pos", GL_FLOAT, 0, 2, sizeof(Vertex2D));

		program->enableAttributeArray("color");
		program->setAttributeBuffer("color", GL_FLOAT, 2 * sizeof(float), 3, sizeof(Vertex2D));

		program->enableAttributeArray("alpha");
		program->setAttributeBuffer("alpha", GL_FLOAT, 5 * sizeof(float), 1, sizeof(Vertex2D));

		context->glDrawArrays(GL_TRIANGLES, 0, project->get_logic_model()->get_gates_count() * 6);

		context->glBindBuffer(GL_ARRAY_BUFFER, line_vbo);

		program->enableAttributeArray("pos");
		program->setAttributeBuffer("pos", GL_FLOAT, 0, 2, sizeof(Vertex2D));

		program->enableAttributeArray("color");
		program->setAttributeBuffer("color", GL_FLOAT, 2 * sizeof(float), 3, sizeof(Vertex2D));

		program->enableAttributeArray("alpha");
		program->setAttributeBuffer("alpha", GL_FLOAT, 5 * sizeof(float), 1, sizeof(Vertex2D));

		context->glDrawArrays(GL_LINES, 0, project->get_logic_model()->get_gates_count() * 8);

		context->glBindBuffer(GL_ARRAY_BUFFER, 0);

		program->release();

		text.draw(projection);
	}

	void WorkspaceGates::create_gate(Gate_shptr& gate, unsigned indice)
	{
		if(gate == NULL)
			return;

		context->glBindBuffer(GL_ARRAY_BUFFER, vbo);

		// Vertices and colors

		Vertex2D temp;
		temp.color = QVector3D(MASK_R(gate->get_fill_color()), MASK_G(gate->get_fill_color()), MASK_B(gate->get_fill_color()));
		temp.alpha = 0.25; // MASK_A(gate->get_fill_color())

		temp.pos = QVector2D(gate->get_min_x(), gate->get_min_y());
		context->glBufferSubData(GL_ARRAY_BUFFER, indice * 6 * sizeof(Vertex2D) + 0 * sizeof(Vertex2D), sizeof(Vertex2D), &temp);

		temp.pos = QVector2D(gate->get_max_x(), gate->get_min_y());
		context->glBufferSubData(GL_ARRAY_BUFFER, indice * 6 * sizeof(Vertex2D) + 1 * sizeof(Vertex2D), sizeof(Vertex2D), &temp);

		temp.pos = QVector2D(gate->get_min_x(), gate->get_max_y());
		context->glBufferSubData(GL_ARRAY_BUFFER, indice * 6 * sizeof(Vertex2D) + 2 * sizeof(Vertex2D), sizeof(Vertex2D), &temp);

		temp.pos = QVector2D(gate->get_max_x(), gate->get_min_y());
		context->glBufferSubData(GL_ARRAY_BUFFER, indice * 6 * sizeof(Vertex2D) + 4 * sizeof(Vertex2D), sizeof(Vertex2D), &temp);

		temp.pos = QVector2D(gate->get_min_x(), gate->get_max_y());
		context->glBufferSubData(GL_ARRAY_BUFFER, indice * 6 * sizeof(Vertex2D) + 3 * sizeof(Vertex2D), sizeof(Vertex2D), &temp);

		temp.pos = QVector2D(gate->get_max_x(), gate->get_max_y());
		context->glBufferSubData(GL_ARRAY_BUFFER, indice * 6 * sizeof(Vertex2D) + 5 * sizeof(Vertex2D), sizeof(Vertex2D), &temp);


		// Lines

		context->glBindBuffer(GL_ARRAY_BUFFER, line_vbo);

		temp.color = QVector3D(MASK_R(gate->get_frame_color()), MASK_G(gate->get_frame_color()), MASK_B(gate->get_frame_color()));
		temp.alpha = 1; // MASK_A(gate->get_frame_color())

		temp.pos = QVector2D(gate->get_min_x(), gate->get_min_y());
		context->glBufferSubData(GL_ARRAY_BUFFER, indice * 8 * sizeof(Vertex2D) + 0 * sizeof(Vertex2D), sizeof(Vertex2D), &temp);

		temp.pos = QVector2D(gate->get_max_x(), gate->get_min_y());
		context->glBufferSubData(GL_ARRAY_BUFFER, indice * 8 * sizeof(Vertex2D) + 1 * sizeof(Vertex2D), sizeof(Vertex2D), &temp);

		temp.pos = QVector2D(gate->get_min_x(), gate->get_min_y());
		context->glBufferSubData(GL_ARRAY_BUFFER, indice * 8 * sizeof(Vertex2D) + 2 * sizeof(Vertex2D), sizeof(Vertex2D), &temp);

		temp.pos = QVector2D(gate->get_min_x(), gate->get_max_y());
		context->glBufferSubData(GL_ARRAY_BUFFER, indice * 8 * sizeof(Vertex2D) + 3 * sizeof(Vertex2D), sizeof(Vertex2D), &temp);

		temp.pos = QVector2D(gate->get_max_x(), gate->get_min_y());
		context->glBufferSubData(GL_ARRAY_BUFFER, indice * 8 * sizeof(Vertex2D) + 4 * sizeof(Vertex2D), sizeof(Vertex2D), &temp);

		temp.pos = QVector2D(gate->get_max_x(), gate->get_max_y());
		context->glBufferSubData(GL_ARRAY_BUFFER, indice * 8 * sizeof(Vertex2D) + 5 * sizeof(Vertex2D), sizeof(Vertex2D), &temp);

		temp.pos = QVector2D(gate->get_min_x(), gate->get_max_y());
		context->glBufferSubData(GL_ARRAY_BUFFER, indice * 8 * sizeof(Vertex2D) + 6 * sizeof(Vertex2D), sizeof(Vertex2D), &temp);

		temp.pos = QVector2D(gate->get_max_x(), gate->get_max_y());
		context->glBufferSubData(GL_ARRAY_BUFFER, indice * 8 * sizeof(Vertex2D) + 7 * sizeof(Vertex2D), sizeof(Vertex2D), &temp);

		context->glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
}
