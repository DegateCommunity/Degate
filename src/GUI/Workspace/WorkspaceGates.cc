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

#include "WorkspaceGates.h"

#define TEXT_PADDING 2

namespace degate
{
	struct GatesVertex2D
	{
		QVector2D pos;
		QVector3D color;
		float alpha;
	};

    WorkspaceGates::WorkspaceGates(QWidget* parent)
            : WorkspaceElement(parent),
              gate_template_name_text(parent),
              port_name_text(parent)
	{
	}

	WorkspaceGates::~WorkspaceGates()
	{
        if (context->glIsBuffer(line_vbo) == GL_TRUE)
		    context->glDeleteBuffers(1, &line_vbo);

        if (context->glIsBuffer(port_vbo) == GL_TRUE)
		    context->glDeleteBuffers(1, &port_vbo);
	}

	void WorkspaceGates::init()
	{
		WorkspaceElement::init();

        gate_template_name_text.init();
        port_name_text.init();

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

		context->glGenBuffers(1, &line_vbo);
		context->glGenBuffers(1, &port_vbo);

		context->glEnable(GL_LINE_SMOOTH);
	}

	void WorkspaceGates::update()
	{
		if (project == nullptr || project->get_logic_model()->get_gates_count() == 0)
			return;

		context->glBindBuffer(GL_ARRAY_BUFFER, vbo);

		context->glBufferData(GL_ARRAY_BUFFER, project->get_logic_model()->get_gates_count() * 6 * sizeof(GatesVertex2D), nullptr, GL_STATIC_DRAW);

		context->glBindBuffer(GL_ARRAY_BUFFER, line_vbo);

		context->glBufferData(GL_ARRAY_BUFFER, project->get_logic_model()->get_gates_count() * 8 * sizeof(GatesVertex2D), nullptr, GL_STATIC_DRAW);

		context->glBindBuffer(GL_ARRAY_BUFFER, 0);

		unsigned gate_template_name_text_size = 0;
		unsigned port_name_text_size = 0;
        ports_count = 0;

		unsigned index = 0;
		for (LogicModel::gate_collection::iterator iter = project->get_logic_model()->gates_begin(); iter != project->get_logic_model()->gates_end(); ++iter)
		{
			create_gate(iter->second, index);
			iter->second->set_index(index);

            gate_template_name_text_size += static_cast<unsigned int>(iter->second->get_gate_template()->get_name().length());

            if (!iter->second->get_name().empty())
                gate_template_name_text_size += static_cast<unsigned int>(iter->second->get_name().length()) + 3;

            ports_count += iter->second->get_ports_number();

			for (Gate::port_iterator port_iter = iter->second->ports_begin(); port_iter != iter->second->ports_end(); ++port_iter)
			{
                port_name_text_size += static_cast<unsigned int>((*port_iter)->get_name().length());
			}
			
			index++;
		}

        gate_template_name_text.update(gate_template_name_text_size);
        port_name_text.update(port_name_text_size);

		context->glBindBuffer(GL_ARRAY_BUFFER, port_vbo);

		context->glBufferData(GL_ARRAY_BUFFER, ports_count * 9 * sizeof(GatesVertex2D), nullptr, GL_STATIC_DRAW);

		context->glBindBuffer(GL_ARRAY_BUFFER, 0);

		unsigned gate_template_name_text_offset = 0;
		unsigned port_name_text_offset = 0;
		unsigned ports_index = 0;

		for (auto iter = project->get_logic_model()->gates_begin(); iter != project->get_logic_model()->gates_end(); ++iter)
		{
		    std::string text = iter->second->get_gate_template()->get_name();

            if (!iter->second->get_name().empty())
                text += " [" + iter->second->get_name() + "]";

            gate_template_name_text.add_sub_text(gate_template_name_text_offset,
                                                 iter->second->get_min_x() + TEXT_PADDING,
                                                 iter->second->get_min_y() + TEXT_PADDING,
                                                 text.c_str(),
                                                 10,
                                                 QVector3D(255, 255, 255),
                                                 1,
                                                 false,
                                                 false,
                                                 iter->second->get_max_x() - iter->second->get_min_x() - TEXT_PADDING * 2);

			create_ports(iter->second, ports_index);

            gate_template_name_text_offset += static_cast<unsigned int>(iter->second->get_gate_template()->get_name().length());

            if (!iter->second->get_name().empty())
                gate_template_name_text_offset += static_cast<unsigned int>(iter->second->get_name().length()) + 3;

			ports_index += iter->second->get_ports_number();

			for (auto port_iter = iter->second->ports_begin(); port_iter != iter->second->ports_end(); ++port_iter)
			{
				unsigned x = (*port_iter)->get_x();
				unsigned y = (*port_iter)->get_y() + (*port_iter)->get_diameter() / 2.0 + TEXT_PADDING;
                port_name_text.add_sub_text(port_name_text_offset,
                                            x,
                                            y,
                                            (*port_iter)->get_name(),
                                            5,
                                            QVector3D(255, 255, 255),
                                            1,
                                            true,
                                            false);

                port_name_text_offset += static_cast<unsigned int>((*port_iter)->get_name().length());
			}
		}
	}

	void WorkspaceGates::update(Gate_shptr& gate)
	{
		if (gate == nullptr)
			return;

		create_gate(gate, gate->get_index());
	}

	void WorkspaceGates::draw(const QMatrix4x4& projection)
	{
		if (project == nullptr || project->get_logic_model()->get_gates_count() == 0)
			return;

		program->bind();

		program->setUniformValue("mvp", projection);

		context->glBindBuffer(GL_ARRAY_BUFFER, vbo);

		program->enableAttributeArray("pos");
		program->setAttributeBuffer("pos", GL_FLOAT, 0, 2, sizeof(GatesVertex2D));

		program->enableAttributeArray("color");
		program->setAttributeBuffer("color", GL_FLOAT, 2 * sizeof(float), 3, sizeof(GatesVertex2D));

		program->enableAttributeArray("alpha");
		program->setAttributeBuffer("alpha", GL_FLOAT, 5 * sizeof(float), 1, sizeof(GatesVertex2D));

		context->glDrawArrays(GL_TRIANGLES, 0, project->get_logic_model()->get_gates_count() * 6);

		context->glBindBuffer(GL_ARRAY_BUFFER, line_vbo);

		program->enableAttributeArray("pos");
		program->setAttributeBuffer("pos", GL_FLOAT, 0, 2, sizeof(GatesVertex2D));

		program->enableAttributeArray("color");
		program->setAttributeBuffer("color", GL_FLOAT, 2 * sizeof(float), 3, sizeof(GatesVertex2D));

		program->enableAttributeArray("alpha");
		program->setAttributeBuffer("alpha", GL_FLOAT, 5 * sizeof(float), 1, sizeof(GatesVertex2D));

		context->glDrawArrays(GL_LINES, 0, project->get_logic_model()->get_gates_count() * 8);

		context->glBindBuffer(GL_ARRAY_BUFFER, 0);

		program->release();
	}

	void WorkspaceGates::draw_gates_name(const QMatrix4x4& projection)
	{
		if (project == nullptr || project->get_logic_model()->get_gates_count() == 0)
			return;

        gate_template_name_text.draw(projection);
	}

	void WorkspaceGates::draw_ports(const QMatrix4x4& projection)
	{
		if (project == nullptr || project->get_logic_model()->get_gates_count() == 0)
			return;
		
		program->bind();

		program->setUniformValue("mvp", projection);
		
		context->glBindBuffer(GL_ARRAY_BUFFER, port_vbo);

		program->enableAttributeArray("pos");
		program->setAttributeBuffer("pos", GL_FLOAT, 0, 2, sizeof(GatesVertex2D));

		program->enableAttributeArray("color");
		program->setAttributeBuffer("color", GL_FLOAT, 2 * sizeof(float), 3, sizeof(GatesVertex2D));

		program->enableAttributeArray("alpha");
		program->setAttributeBuffer("alpha", GL_FLOAT, 5 * sizeof(float), 1, sizeof(GatesVertex2D));

		context->glDrawArrays(GL_TRIANGLES, 0, ports_count * 9);

		context->glBindBuffer(GL_ARRAY_BUFFER, 0);

		program->release();
	}

	void WorkspaceGates::draw_ports_name(const QMatrix4x4& projection)
	{
		if (project == nullptr || project->get_logic_model()->get_gates_count() == 0)
			return;

        port_name_text.draw(projection);
	}

	void WorkspaceGates::create_gate(Gate_shptr& gate, unsigned index)
	{
		if (gate == nullptr)
			return;

		context->glBindBuffer(GL_ARRAY_BUFFER, vbo);


		// Vertices and colors

        color_t color = gate->get_gate_template()->get_fill_color() == 0 ? project->get_default_color(DEFAULT_COLOR_GATE) : gate->get_gate_template()->get_fill_color();

		color = highlight_color_by_state(color, gate->get_highlighted());

		GatesVertex2D temp;
		temp.color = QVector3D(MASK_R(color) / 255.0, MASK_G(color) / 255.0, MASK_B(color) / 255.0);
		temp.alpha = MASK_A(color) / 255.0;

		temp.pos = QVector2D(gate->get_min_x(), gate->get_min_y());
		context->glBufferSubData(GL_ARRAY_BUFFER, index * 6 * sizeof(GatesVertex2D) + 0 * sizeof(GatesVertex2D), sizeof(GatesVertex2D), &temp);

		temp.pos = QVector2D(gate->get_max_x(), gate->get_min_y());
		context->glBufferSubData(GL_ARRAY_BUFFER, index * 6 * sizeof(GatesVertex2D) + 1 * sizeof(GatesVertex2D), sizeof(GatesVertex2D), &temp);

		temp.pos = QVector2D(gate->get_min_x(), gate->get_max_y());
		context->glBufferSubData(GL_ARRAY_BUFFER, index * 6 * sizeof(GatesVertex2D) + 2 * sizeof(GatesVertex2D), sizeof(GatesVertex2D), &temp);

		temp.pos = QVector2D(gate->get_max_x(), gate->get_min_y());
		context->glBufferSubData(GL_ARRAY_BUFFER, index * 6 * sizeof(GatesVertex2D) + 4 * sizeof(GatesVertex2D), sizeof(GatesVertex2D), &temp);

		temp.pos = QVector2D(gate->get_min_x(), gate->get_max_y());
		context->glBufferSubData(GL_ARRAY_BUFFER, index * 6 * sizeof(GatesVertex2D) + 3 * sizeof(GatesVertex2D), sizeof(GatesVertex2D), &temp);

		temp.pos = QVector2D(gate->get_max_x(), gate->get_max_y());
		context->glBufferSubData(GL_ARRAY_BUFFER, index * 6 * sizeof(GatesVertex2D) + 5 * sizeof(GatesVertex2D), sizeof(GatesVertex2D), &temp);


		// Lines

        color = gate->get_gate_template()->get_frame_color() == 0 ? project->get_default_color(DEFAULT_COLOR_GATE_FRAME) : gate->get_gate_template()->get_frame_color();

        color = highlight_color_by_state(color, gate->get_highlighted());

		context->glBindBuffer(GL_ARRAY_BUFFER, line_vbo);

		temp.color = QVector3D(MASK_R(color) / 255.0, MASK_G(color) / 255.0, MASK_B(color) / 255.0);
		temp.alpha = MASK_A(color) / 255.0;

		temp.pos = QVector2D(gate->get_min_x(), gate->get_min_y());
		context->glBufferSubData(GL_ARRAY_BUFFER, index * 8 * sizeof(GatesVertex2D) + 0 * sizeof(GatesVertex2D), sizeof(GatesVertex2D), &temp);

		temp.pos = QVector2D(gate->get_max_x(), gate->get_min_y());
		context->glBufferSubData(GL_ARRAY_BUFFER, index * 8 * sizeof(GatesVertex2D) + 1 * sizeof(GatesVertex2D), sizeof(GatesVertex2D), &temp);

		temp.pos = QVector2D(gate->get_min_x(), gate->get_min_y());
		context->glBufferSubData(GL_ARRAY_BUFFER, index * 8 * sizeof(GatesVertex2D) + 2 * sizeof(GatesVertex2D), sizeof(GatesVertex2D), &temp);

		temp.pos = QVector2D(gate->get_min_x(), gate->get_max_y());
		context->glBufferSubData(GL_ARRAY_BUFFER, index * 8 * sizeof(GatesVertex2D) + 3 * sizeof(GatesVertex2D), sizeof(GatesVertex2D), &temp);

		temp.pos = QVector2D(gate->get_max_x(), gate->get_min_y());
		context->glBufferSubData(GL_ARRAY_BUFFER, index * 8 * sizeof(GatesVertex2D) + 4 * sizeof(GatesVertex2D), sizeof(GatesVertex2D), &temp);

		temp.pos = QVector2D(gate->get_max_x(), gate->get_max_y());
		context->glBufferSubData(GL_ARRAY_BUFFER, index * 8 * sizeof(GatesVertex2D) + 5 * sizeof(GatesVertex2D), sizeof(GatesVertex2D), &temp);

		temp.pos = QVector2D(gate->get_min_x(), gate->get_max_y());
		context->glBufferSubData(GL_ARRAY_BUFFER, index * 8 * sizeof(GatesVertex2D) + 6 * sizeof(GatesVertex2D), sizeof(GatesVertex2D), &temp);

		temp.pos = QVector2D(gate->get_max_x(), gate->get_max_y());
		context->glBufferSubData(GL_ARRAY_BUFFER, index * 8 * sizeof(GatesVertex2D) + 7 * sizeof(GatesVertex2D), sizeof(GatesVertex2D), &temp);

		context->glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void draw_port_in_out(QOpenGLFunctions* context, float x, float y, unsigned size, QVector3D color, float alpha, unsigned offset)
	{
		GatesVertex2D temp;

		temp.color = color;
		temp.alpha = alpha;

		int mid = size / 2.0;

		temp.pos = QVector2D(x - mid, y - mid);
		context->glBufferSubData(GL_ARRAY_BUFFER, offset * 9 * sizeof(GatesVertex2D) + 0 * sizeof(GatesVertex2D), sizeof(GatesVertex2D), &temp);

		temp.pos = QVector2D(x - mid, y + mid);
		context->glBufferSubData(GL_ARRAY_BUFFER, offset * 9 * sizeof(GatesVertex2D) + 1 * sizeof(GatesVertex2D), sizeof(GatesVertex2D), &temp);

		temp.pos = QVector2D(x + mid, y - mid);
		context->glBufferSubData(GL_ARRAY_BUFFER, offset * 9 * sizeof(GatesVertex2D) + 2 * sizeof(GatesVertex2D), sizeof(GatesVertex2D), &temp);

		temp.pos = QVector2D(x + mid, y - mid);
		context->glBufferSubData(GL_ARRAY_BUFFER, offset * 9 * sizeof(GatesVertex2D) + 3 * sizeof(GatesVertex2D), sizeof(GatesVertex2D), &temp);

		temp.pos = QVector2D(x, y);
		context->glBufferSubData(GL_ARRAY_BUFFER, offset * 9 * sizeof(GatesVertex2D) + 4 * sizeof(GatesVertex2D), sizeof(GatesVertex2D), &temp);

		temp.pos = QVector2D(x + mid, y + mid);
		context->glBufferSubData(GL_ARRAY_BUFFER, offset * 9 * sizeof(GatesVertex2D) + 5 * sizeof(GatesVertex2D), sizeof(GatesVertex2D), &temp);

		temp.pos = QVector2D(x, y);
		context->glBufferSubData(GL_ARRAY_BUFFER, offset * 9 * sizeof(GatesVertex2D) + 6 * sizeof(GatesVertex2D), sizeof(GatesVertex2D), &temp);

		temp.pos = QVector2D(x + mid, y + mid);
		context->glBufferSubData(GL_ARRAY_BUFFER, offset * 9 * sizeof(GatesVertex2D) + 7 * sizeof(GatesVertex2D), sizeof(GatesVertex2D), &temp);

		temp.pos = QVector2D(x - mid, y + mid);
		context->glBufferSubData(GL_ARRAY_BUFFER, offset * 9 * sizeof(GatesVertex2D) + 8 * sizeof(GatesVertex2D), sizeof(GatesVertex2D), &temp);
	}

	void draw_port_in(QOpenGLFunctions* context, float x, float y, unsigned size, QVector3D color, float alpha, unsigned offset)
	{
		GatesVertex2D temp;

		temp.color = color;
		temp.alpha = alpha;

		int mid = size / 2.0;

		temp.pos = QVector2D(x - mid, y - mid);
		context->glBufferSubData(GL_ARRAY_BUFFER, offset * 9 * sizeof(GatesVertex2D) + 0 * sizeof(GatesVertex2D), sizeof(GatesVertex2D), &temp);

		temp.pos = QVector2D(x + mid, y - mid);
		context->glBufferSubData(GL_ARRAY_BUFFER, offset * 9 * sizeof(GatesVertex2D) + 1 * sizeof(GatesVertex2D), sizeof(GatesVertex2D), &temp);

		temp.pos = QVector2D(x, y);
		context->glBufferSubData(GL_ARRAY_BUFFER, offset * 9 * sizeof(GatesVertex2D) + 2 * sizeof(GatesVertex2D), sizeof(GatesVertex2D), &temp);

		temp.pos = QVector2D(x + mid, y - mid);
		context->glBufferSubData(GL_ARRAY_BUFFER, offset * 9 * sizeof(GatesVertex2D) + 3 * sizeof(GatesVertex2D), sizeof(GatesVertex2D), &temp);

		temp.pos = QVector2D(x + mid, y + mid);
		context->glBufferSubData(GL_ARRAY_BUFFER, offset * 9 * sizeof(GatesVertex2D) + 4 * sizeof(GatesVertex2D), sizeof(GatesVertex2D), &temp);

		temp.pos = QVector2D(x, y);
		context->glBufferSubData(GL_ARRAY_BUFFER, offset * 9 * sizeof(GatesVertex2D) + 5 * sizeof(GatesVertex2D), sizeof(GatesVertex2D), &temp);

		temp.pos = QVector2D(x + mid, y + mid);
		context->glBufferSubData(GL_ARRAY_BUFFER, offset * 9 * sizeof(GatesVertex2D) + 6 * sizeof(GatesVertex2D), sizeof(GatesVertex2D), &temp);

		temp.pos = QVector2D(x - mid, y + mid);
		context->glBufferSubData(GL_ARRAY_BUFFER, offset * 9 * sizeof(GatesVertex2D) + 7 * sizeof(GatesVertex2D), sizeof(GatesVertex2D), &temp);

		temp.pos = QVector2D(x, y);
		context->glBufferSubData(GL_ARRAY_BUFFER, offset * 9 * sizeof(GatesVertex2D) + 8 * sizeof(GatesVertex2D), sizeof(GatesVertex2D), &temp);
	}

	void draw_port_out(QOpenGLFunctions* context, float x, float y, unsigned size, QVector3D color, float alpha, unsigned offset)
	{
		GatesVertex2D temp;

		temp.color = color;
		temp.alpha = alpha;

		int mid = size / 2.0;

		temp.pos = QVector2D(x - mid, y - mid);
		context->glBufferSubData(GL_ARRAY_BUFFER, offset * 9 * sizeof(GatesVertex2D) + 0 * sizeof(GatesVertex2D), sizeof(GatesVertex2D), &temp);

		temp.pos = QVector2D(x, y - mid);
		context->glBufferSubData(GL_ARRAY_BUFFER, offset * 9 * sizeof(GatesVertex2D) + 1 * sizeof(GatesVertex2D), sizeof(GatesVertex2D), &temp);

		temp.pos = QVector2D(x, y + mid);
		context->glBufferSubData(GL_ARRAY_BUFFER, offset * 9 * sizeof(GatesVertex2D) + 2 * sizeof(GatesVertex2D), sizeof(GatesVertex2D), &temp);

		temp.pos = QVector2D(x - mid, y - mid);
		context->glBufferSubData(GL_ARRAY_BUFFER, offset * 9 * sizeof(GatesVertex2D) + 3 * sizeof(GatesVertex2D), sizeof(GatesVertex2D), &temp);

		temp.pos = QVector2D(x - mid, y + mid);
		context->glBufferSubData(GL_ARRAY_BUFFER, offset * 9 * sizeof(GatesVertex2D) + 4 * sizeof(GatesVertex2D), sizeof(GatesVertex2D), &temp);

		temp.pos = QVector2D(x, y + mid);
		context->glBufferSubData(GL_ARRAY_BUFFER, offset * 9 * sizeof(GatesVertex2D) + 5 * sizeof(GatesVertex2D), sizeof(GatesVertex2D), &temp);

		temp.pos = QVector2D(x, y - mid);
		context->glBufferSubData(GL_ARRAY_BUFFER, offset * 9 * sizeof(GatesVertex2D) + 6 * sizeof(GatesVertex2D), sizeof(GatesVertex2D), &temp);

		temp.pos = QVector2D(x + mid, y);
		context->glBufferSubData(GL_ARRAY_BUFFER, offset * 9 * sizeof(GatesVertex2D) + 7 * sizeof(GatesVertex2D), sizeof(GatesVertex2D), &temp);

		temp.pos = QVector2D(x, y + mid);
		context->glBufferSubData(GL_ARRAY_BUFFER, offset * 9 * sizeof(GatesVertex2D) + 8 * sizeof(GatesVertex2D), sizeof(GatesVertex2D), &temp);
	}

	void WorkspaceGates::update(GatePort_shptr& port)
	{
		if (port == nullptr)
			return;

		context->glBindBuffer(GL_ARRAY_BUFFER, port_vbo);

		GateTemplatePort_shptr tmpl_port = port->get_template_port();
		color_t color = tmpl_port->get_fill_color() == 0 ? project->get_default_color(DEFAULT_COLOR_GATE_PORT) : tmpl_port->get_fill_color();

		color = highlight_color_by_state(color, port->get_highlighted());

		switch (tmpl_port->get_port_type())
		{
			case GateTemplatePort::PORT_TYPE_UNDEFINED:
				draw_port_in_out(context, port->get_x(), port->get_y(), port->get_diameter(), QVector3D(MASK_R(color) / 255.0, MASK_G(color) / 255.0, MASK_B(color) / 255.0), MASK_A(color) / 255.0, port->get_index());
				break;
		    case GateTemplatePort::PORT_TYPE_IN:
			    draw_port_in(context, port->get_x(), port->get_y(), port->get_diameter(), QVector3D(MASK_R(color) / 255.0, MASK_G(color) / 255.0, MASK_B(color) / 255.0), MASK_A(color) / 255.0, port->get_index());
			    break;
		    case GateTemplatePort::PORT_TYPE_OUT:
			    draw_port_out(context, port->get_x(), port->get_y(), port->get_diameter(), QVector3D(MASK_R(color) / 255.0, MASK_G(color) / 255.0, MASK_B(color) / 255.0), MASK_A(color) / 255.0, port->get_index());
			    break;
		    case GateTemplatePort::PORT_TYPE_INOUT:
			    draw_port_in_out(context, port->get_x(), port->get_y(), port->get_diameter(), QVector3D(MASK_R(color) / 255.0, MASK_G(color) / 255.0, MASK_B(color) / 255.0), MASK_A(color) / 255.0, port->get_index());
			    break;
			default:
				draw_port_in_out(context, port->get_x(), port->get_y(), port->get_diameter(), QVector3D(MASK_R(color) / 255.0, MASK_G(color) / 255.0, MASK_B(color) / 255.0), MASK_A(color) / 255.0, port->get_index());
				break;
		}

		context->glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void WorkspaceGates::create_ports(Gate_shptr& gate, unsigned index)
	{
		context->glBindBuffer(GL_ARRAY_BUFFER, port_vbo);

		for (Gate::port_iterator iter = gate->ports_begin(); iter != gate->ports_end(); ++iter)
		{
			GatePort_shptr port = *iter;
			GateTemplatePort_shptr tmpl_port = port->get_template_port();
			color_t color = tmpl_port->get_fill_color() == 0 ? project->get_default_color(DEFAULT_COLOR_GATE_PORT) : tmpl_port->get_fill_color();

			color = highlight_color_by_state(color, port->get_highlighted());

			switch (tmpl_port->get_port_type())
			{
				case GateTemplatePort::PORT_TYPE_UNDEFINED:
					draw_port_in_out(context, port->get_x(), port->get_y(), port->get_diameter(), QVector3D(MASK_R(color) / 255.0, MASK_G(color) / 255.0, MASK_B(color) / 255.0), MASK_A(color) / 255.0, index);
					break;
			    case GateTemplatePort::PORT_TYPE_IN:
				    draw_port_in(context, port->get_x(), port->get_y(), port->get_diameter(), QVector3D(MASK_R(color) / 255.0, MASK_G(color) / 255.0, MASK_B(color) / 255.0), MASK_A(color) / 255.0, index);
				    break;
			    case GateTemplatePort::PORT_TYPE_OUT:
				    draw_port_out(context, port->get_x(), port->get_y(), port->get_diameter(), QVector3D(MASK_R(color) / 255.0, MASK_G(color) / 255.0, MASK_B(color) / 255.0), MASK_A(color) / 255.0, index);
				    break;
			    case GateTemplatePort::PORT_TYPE_INOUT:
				    draw_port_in_out(context, port->get_x(), port->get_y(), port->get_diameter(), QVector3D(MASK_R(color) / 255.0, MASK_G(color) / 255.0, MASK_B(color) / 255.0), MASK_A(color) / 255.0, index);
				    break;
				default:
					draw_port_in_out(context, port->get_x(), port->get_y(), port->get_diameter(), QVector3D(MASK_R(color) / 255.0, MASK_G(color) / 255.0, MASK_B(color) / 255.0), MASK_A(color) / 255.0, index);
					break;
			}

			port->set_index(index);

			index++;
		}

		context->glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
}
