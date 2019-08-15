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

#include "WorkspaceAnnotations.h"

namespace degate
{
	struct AnnotationsVertex2D
	{
		QVector2D pos;
		QVector3D color;
		float alpha;
	};

	WorkspaceAnnotations::WorkspaceAnnotations(QWidget* new_parent) : WorkspaceElement(new_parent), text(new_parent)
	{

	}

	WorkspaceAnnotations::~WorkspaceAnnotations()
	{
		context->glDeleteBuffers(1, &line_vbo);
	}

	void WorkspaceAnnotations::init()
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

		context->glGenBuffers(1, &line_vbo);

		context->glEnable(GL_LINE_SMOOTH);
	}

	void WorkspaceAnnotations::update()
	{
		if(project == NULL || project->get_logic_model()->get_annotations_count() == 0)
			return;

		context->glBindBuffer(GL_ARRAY_BUFFER, vbo);

		context->glBufferData(GL_ARRAY_BUFFER, project->get_logic_model()->get_annotations_count() * 6 * sizeof(AnnotationsVertex2D), 0, GL_STATIC_DRAW);

		context->glBindBuffer(GL_ARRAY_BUFFER, line_vbo);

		context->glBufferData(GL_ARRAY_BUFFER, project->get_logic_model()->get_annotations_count() * 8 * sizeof(AnnotationsVertex2D), 0, GL_STATIC_DRAW);

		context->glBindBuffer(GL_ARRAY_BUFFER, 0);

		unsigned text_size = 0;

		unsigned index = 0;
		for(LogicModel::annotation_collection::iterator iter = project->get_logic_model()->annotations_begin(); iter != project->get_logic_model()->annotations_end(); ++iter)
		{
			create_annotation(iter->second, index);
			iter->second->set_index(index);

			text_size += iter->second->get_name().length();
			index++;
		}

		text.update(text_size);

		unsigned text_offset = 0;
		for(LogicModel::annotation_collection::iterator iter = project->get_logic_model()->annotations_begin(); iter != project->get_logic_model()->annotations_end(); ++iter)
		{
			text.add_sub_text(text_offset, iter->second->get_min_x() + TEXT_SPACE, iter->second->get_min_y() + TEXT_SPACE, iter->second->get_name().c_str(), 20, QVector3D(255, 255, 255), 1);

			text_offset += iter->second->get_name().length();
			index++;
		}
	}

	void WorkspaceAnnotations::update(Annotation_shptr& annotation)
	{
		if(annotation == NULL)
			return;

		create_annotation(annotation, annotation->get_index());
	}

	void WorkspaceAnnotations::draw(const QMatrix4x4& projection)
	{
		if(project == NULL || project->get_logic_model()->get_annotations_count() == 0)
			return;

		program->bind();

		context->glEnable(GL_BLEND);

		program->setUniformValue("mvp", projection);

		context->glBindBuffer(GL_ARRAY_BUFFER, vbo);

		program->enableAttributeArray("pos");
		program->setAttributeBuffer("pos", GL_FLOAT, 0, 2, sizeof(AnnotationsVertex2D));

		program->enableAttributeArray("color");
		program->setAttributeBuffer("color", GL_FLOAT, 2 * sizeof(float), 3, sizeof(AnnotationsVertex2D));

		program->enableAttributeArray("alpha");
		program->setAttributeBuffer("alpha", GL_FLOAT, 5 * sizeof(float), 1, sizeof(AnnotationsVertex2D));

		context->glDrawArrays(GL_TRIANGLES, 0, project->get_logic_model()->get_annotations_count() * 6);

		context->glBindBuffer(GL_ARRAY_BUFFER, line_vbo);

		program->enableAttributeArray("pos");
		program->setAttributeBuffer("pos", GL_FLOAT, 0, 2, sizeof(AnnotationsVertex2D));

		program->enableAttributeArray("color");
		program->setAttributeBuffer("color", GL_FLOAT, 2 * sizeof(float), 3, sizeof(AnnotationsVertex2D));

		program->enableAttributeArray("alpha");
		program->setAttributeBuffer("alpha", GL_FLOAT, 5 * sizeof(float), 1, sizeof(AnnotationsVertex2D));

		context->glDrawArrays(GL_LINES, 0, project->get_logic_model()->get_annotations_count() * 8);

		context->glBindBuffer(GL_ARRAY_BUFFER, 0);

		program->release();

		text.draw(projection);
	}

	void WorkspaceAnnotations::create_annotation(Annotation_shptr& annotation, unsigned index)
	{
		if(annotation == NULL)
			return;

		context->glBindBuffer(GL_ARRAY_BUFFER, vbo);

		// Vertices and colors

		AnnotationsVertex2D temp;

		color_t color = highlight_color_by_state(annotation->get_fill_color(), annotation->get_highlighted());

		temp.color = QVector3D(MASK_R(color) / 255.0, MASK_G(color) / 255.0, MASK_B(color) / 255.0);
		temp.alpha = 0.25; // MASK_A(annotation->get_fill_color()) / 255.0

		temp.pos = QVector2D(annotation->get_min_x(), annotation->get_min_y());
		context->glBufferSubData(GL_ARRAY_BUFFER, index * 6 * sizeof(AnnotationsVertex2D) + 0 * sizeof(AnnotationsVertex2D), sizeof(AnnotationsVertex2D), &temp);

		temp.pos = QVector2D(annotation->get_max_x(), annotation->get_min_y());
		context->glBufferSubData(GL_ARRAY_BUFFER, index * 6 * sizeof(AnnotationsVertex2D) + 1 * sizeof(AnnotationsVertex2D), sizeof(AnnotationsVertex2D), &temp);

		temp.pos = QVector2D(annotation->get_min_x(), annotation->get_max_y());
		context->glBufferSubData(GL_ARRAY_BUFFER, index * 6 * sizeof(AnnotationsVertex2D) + 2 * sizeof(AnnotationsVertex2D), sizeof(AnnotationsVertex2D), &temp);

		temp.pos = QVector2D(annotation->get_max_x(), annotation->get_min_y());
		context->glBufferSubData(GL_ARRAY_BUFFER, index * 6 * sizeof(AnnotationsVertex2D) + 4 * sizeof(AnnotationsVertex2D), sizeof(AnnotationsVertex2D), &temp);

		temp.pos = QVector2D(annotation->get_min_x(), annotation->get_max_y());
		context->glBufferSubData(GL_ARRAY_BUFFER, index * 6 * sizeof(AnnotationsVertex2D) + 3 * sizeof(AnnotationsVertex2D), sizeof(AnnotationsVertex2D), &temp);

		temp.pos = QVector2D(annotation->get_max_x(), annotation->get_max_y());
		context->glBufferSubData(GL_ARRAY_BUFFER, index * 6 * sizeof(AnnotationsVertex2D) + 5 * sizeof(AnnotationsVertex2D), sizeof(AnnotationsVertex2D), &temp);


		// Lines

		color = highlight_color_by_state(annotation->get_frame_color(), annotation->get_highlighted());

		context->glBindBuffer(GL_ARRAY_BUFFER, line_vbo);

		temp.color = QVector3D(MASK_R(color) / 255.0, MASK_G(color) / 255.0, MASK_B(color) / 255.0);
		temp.alpha = 1; // MASK_A(annotation->get_frame_color()) / 255.0

		temp.pos = QVector2D(annotation->get_min_x(), annotation->get_min_y());
		context->glBufferSubData(GL_ARRAY_BUFFER, index * 8 * sizeof(AnnotationsVertex2D) + 0 * sizeof(AnnotationsVertex2D), sizeof(AnnotationsVertex2D), &temp);

		temp.pos = QVector2D(annotation->get_max_x(), annotation->get_min_y());
		context->glBufferSubData(GL_ARRAY_BUFFER, index * 8 * sizeof(AnnotationsVertex2D) + 1 * sizeof(AnnotationsVertex2D), sizeof(AnnotationsVertex2D), &temp);

		temp.pos = QVector2D(annotation->get_min_x(), annotation->get_min_y());
		context->glBufferSubData(GL_ARRAY_BUFFER, index * 8 * sizeof(AnnotationsVertex2D) + 2 * sizeof(AnnotationsVertex2D), sizeof(AnnotationsVertex2D), &temp);

		temp.pos = QVector2D(annotation->get_min_x(), annotation->get_max_y());
		context->glBufferSubData(GL_ARRAY_BUFFER, index * 8 * sizeof(AnnotationsVertex2D) + 3 * sizeof(AnnotationsVertex2D), sizeof(AnnotationsVertex2D), &temp);

		temp.pos = QVector2D(annotation->get_max_x(), annotation->get_min_y());
		context->glBufferSubData(GL_ARRAY_BUFFER, index * 8 * sizeof(AnnotationsVertex2D) + 4 * sizeof(AnnotationsVertex2D), sizeof(AnnotationsVertex2D), &temp);

		temp.pos = QVector2D(annotation->get_max_x(), annotation->get_max_y());
		context->glBufferSubData(GL_ARRAY_BUFFER, index * 8 * sizeof(AnnotationsVertex2D) + 5 * sizeof(AnnotationsVertex2D), sizeof(AnnotationsVertex2D), &temp);

		temp.pos = QVector2D(annotation->get_min_x(), annotation->get_max_y());
		context->glBufferSubData(GL_ARRAY_BUFFER, index * 8 * sizeof(AnnotationsVertex2D) + 6 * sizeof(AnnotationsVertex2D), sizeof(AnnotationsVertex2D), &temp);

		temp.pos = QVector2D(annotation->get_max_x(), annotation->get_max_y());
		context->glBufferSubData(GL_ARRAY_BUFFER, index * 8 * sizeof(AnnotationsVertex2D) + 7 * sizeof(AnnotationsVertex2D), sizeof(AnnotationsVertex2D), &temp);

		context->glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
}
