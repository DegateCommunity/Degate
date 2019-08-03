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
	};

	WorkspaceGates::WorkspaceGates(QWidget* new_parent) : WorkspaceElement(new_parent)
	{
	}

	WorkspaceGates::~WorkspaceGates()
	{
	}

	void WorkspaceGates::init()
	{
		WorkspaceElement::init();

		QOpenGLShader* vshader = new QOpenGLShader(QOpenGLShader::Vertex);
		const char* vsrc =
			"attribute vec2 pos;\n"
			"attribute vec3 color;\n"
			"uniform mat4 mvp;\n"
			"varying vec4 out_color;\n"
			"void main(void)\n"
			"{\n"
			"    gl_Position = mvp * vec4(pos, 0.0, 1.0);\n"
			"    out_color = vec4(color, 0.5);\n"
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

		glGenBuffers(1, &vbo);
	}

	void WorkspaceGates::update()
	{
		if(project == NULL || project->get_logic_model()->get_gates_count() == 0)
			return;

		glBindBuffer(GL_ARRAY_BUFFER, vbo);

		glBufferData(GL_ARRAY_BUFFER, project->get_logic_model()->get_gates_count() * 6 * sizeof(Vertex2D), 0, GL_STATIC_DRAW);

		unsigned indice = 0;
		for(LogicModel::gate_collection::iterator iter = project->get_logic_model()->gates_begin(); iter != project->get_logic_model()->gates_end(); ++iter)
		{
			create_gate(iter->second, indice);

			indice++;
		}

		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void WorkspaceGates::draw(const QMatrix4x4& projection)
	{
		if(project == NULL || project->get_logic_model()->get_gates_count() == 0)
			return;

		program->bind();

		glEnable(GL_BLEND);

		program->setUniformValue("mvp", projection);

		glBindBuffer(GL_ARRAY_BUFFER, vbo);

		program->enableAttributeArray("pos");
		program->setAttributeBuffer("pos", GL_FLOAT, 0, 2, sizeof(Vertex2D));

		program->enableAttributeArray("color");
		program->setAttributeBuffer("color", GL_FLOAT, 2 * sizeof(float), 3, sizeof(Vertex2D));

		glDrawArrays(GL_TRIANGLES, 0, project->get_logic_model()->get_gates_count() * 6);

		glBindBuffer(GL_ARRAY_BUFFER, 0);

		program->release();
	}

	void WorkspaceGates::create_gate(Gate_shptr& gate, unsigned indice)
	{
		if(gate == NULL)
			return;

		// Vertices and colors

		Vertex2D temp;
		temp.color = QVector3D(MASK_R(gate->get_fill_color()), MASK_G(gate->get_fill_color()), MASK_B(gate->get_fill_color()));

		temp.pos = QVector2D(gate->get_min_x(), gate->get_min_y());
		glBufferSubData(GL_ARRAY_BUFFER, indice * 6 * sizeof(Vertex2D) + 0 * sizeof(Vertex2D), sizeof(Vertex2D), &temp);

		temp.pos = QVector2D(gate->get_max_x(), gate->get_min_y());
		glBufferSubData(GL_ARRAY_BUFFER, indice * 6 * sizeof(Vertex2D) + 1 * sizeof(Vertex2D), sizeof(Vertex2D), &temp);

		temp.pos = QVector2D(gate->get_min_x(), gate->get_max_y());
		glBufferSubData(GL_ARRAY_BUFFER, indice * 6 * sizeof(Vertex2D) + 2 * sizeof(Vertex2D), sizeof(Vertex2D), &temp);

		temp.pos = QVector2D(gate->get_max_x(), gate->get_min_y());
		glBufferSubData(GL_ARRAY_BUFFER, indice * 6 * sizeof(Vertex2D) + 4 * sizeof(Vertex2D), sizeof(Vertex2D), &temp);

		temp.pos = QVector2D(gate->get_min_x(), gate->get_max_y());
		glBufferSubData(GL_ARRAY_BUFFER, indice * 6 * sizeof(Vertex2D) + 3 * sizeof(Vertex2D), sizeof(Vertex2D), &temp);

		temp.pos = QVector2D(gate->get_max_x(), gate->get_max_y());
		glBufferSubData(GL_ARRAY_BUFFER, indice * 6 * sizeof(Vertex2D) + 5 * sizeof(Vertex2D), sizeof(Vertex2D), &temp);
	}
}
