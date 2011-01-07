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

#include <GateRenderer.h>
#include <DegateHelper.h>

#include <GL/gl.h>
#include <GL/glu.h>

#include <ft2build.h>
#include <freetype/freetype.h>
#include <freetype/ftglyph.h>
#include <freetype/ftoutln.h>
#include <freetype/fttrigon.h>

#include <utility>

using namespace degate;


GateRenderer::GateRenderer() : realized(false) {

  show();
  update_viewport_dimension();
  update_virtual_dimension();
}

GateRenderer::~GateRenderer() {

  BOOST_FOREACH(layer_list_type::value_type & p, layers)
    if(glIsTexture(p.second) == GL_TRUE) glDeleteTextures(1, &p.second);
}

void GateRenderer::on_realize() {

  OpenGLRendererBase::on_realize();

  glDisable(GL_LIGHTING);
  glDisable(GL_LIGHT0);
  glDisable(GL_COLOR_MATERIAL);
  glDisable(GL_DEPTH_TEST);

  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);

  glClearColor(0, 0, 0, 0);
  glEnable(GL_TEXTURE_2D);

  dlist = glGenLists(1);
  assert(error_check());

  realized = true;
  update_screen();
}

void GateRenderer::add_layer_type(Layer::LAYER_TYPE layer_type) {
  layers.push_back(std::make_pair(layer_type, 0));
}

void GateRenderer::set_gate_template(degate::GateTemplate_shptr tmpl)
  throw(degate::InvalidPointerException) {

  if(tmpl == NULL)
    throw InvalidPointerException("Invalid parameter for GateRenderer::set_gate_template().");
  this->tmpl = tmpl;

  /* if the gate template is new it has no size and no image. */
  if(tmpl->get_width() > 0 && tmpl->get_height() > 0) {
    set_virtual_size(tmpl->get_width(), tmpl->get_height());
    set_viewport(0, 0,
		 LENGTH_TO_MAX(tmpl->get_width()),
		 LENGTH_TO_MAX(tmpl->get_height()));
  }

  //update_screen();
}


void GateRenderer::update_screen() {

  if(!realized) {
    debug(TM, "not realized %p", this);
    return;
  }


  Glib::RefPtr<Gdk::GL::Window> glwindow = get_gl_window();

  if(glwindow->gl_begin(get_gl_context())) {

    if(tmpl != NULL) {
      debug(TM, "render tmpl %p", this);
      render_gate_template(tmpl, layers);
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_BLEND);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //glBlendFunc(GL_SRC_ALPHA, GL_DST_ALPHA);
    glBlendFunc(GL_DST_COLOR, GL_ONE);

    glCallList(dlist);
    assert(error_check());

    // Swap buffers.
    if(glwindow->is_double_buffered()) glwindow->swap_buffers();
    else glFlush();

    glwindow->gl_end();
  }

}

void GateRenderer::update_viewport_dimension() {

  if(realized) {

    Glib::RefPtr<Gdk::GL::Window> glwindow = get_gl_window();
    if(glwindow->gl_begin(get_gl_context())) {

      glViewport(0, 0, get_width(), get_height());

      glMatrixMode(GL_PROJECTION);
      glLoadIdentity();
      glScalef(1, -1, 1);
      glOrtho(get_viewport_min_x(), get_viewport_max_x(),
	      get_viewport_min_y(), get_viewport_max_y(),
	      -20, // near plane
	      20); // far plane
      assert(glGetError() == GL_NO_ERROR);


      glMatrixMode(GL_MODELVIEW);
      glLoadIdentity();

      glwindow->gl_end();
      update_screen();
    }
  }
  RenderArea::update_viewport_dimension();

}


GLuint GateRenderer::create_texture(degate::GateTemplateImage_shptr img, uint8_t alpha) const {

  GLuint texture = 0;
  assert(img != NULL);
  if(img == NULL) return 0;

  unsigned int
    width = next_power_of_two(img->get_width()),
    height = next_power_of_two(img->get_height());

  guint32 * data = new guint32[width * height];
  if(data == NULL) return 0;
  memset(data, 0, width * height * sizeof(guint32));

  for(unsigned int y = 0; y < img->get_height(); y++)
    for(unsigned int x = 0; x < img->get_width(); x++) {
      color_t c = img->get_pixel(x, y);
      data[y * width + x] = MERGE_CHANNELS(MASK_R(c), MASK_G(c), MASK_B(c), alpha);
    }


  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  assert(error_check());

  glGenTextures(1, &texture);
  assert(glGetError() == GL_NO_ERROR);

  glBindTexture(GL_TEXTURE_2D, texture);
  assert(glGetError() == GL_NO_ERROR);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, DEFAULT_FILTER);
  assert(glGetError() == GL_NO_ERROR);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, DEFAULT_FILTER);
  assert(glGetError() == GL_NO_ERROR);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  assert(glGetError() == GL_NO_ERROR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
  assert(glGetError() == GL_NO_ERROR);

  glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_FALSE);
  assert(glGetError() == GL_NO_ERROR);

  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
  assert(glGetError() == GL_NO_ERROR);

  glTexImage2D(GL_TEXTURE_2D,
	       0, // level
	       GL_RGBA, // BGRA,
	       width, height,
	       0, // border
	       GL_RGBA,
	       GL_UNSIGNED_BYTE,
	       data);
  assert(glGetError() == GL_NO_ERROR);

  delete[] data;

  return texture;
}

void GateRenderer::render_texture(degate::GateTemplateImage_shptr img, GLuint texture) const {

  assert(img != NULL);
  if(img == NULL) return;

  unsigned int
    width = next_power_of_two(img->get_width()),
    height = next_power_of_two(img->get_height());

  double
    w = (double)(LENGTH_TO_MAX(img->get_width())) / (double)LENGTH_TO_MAX(width),
    h = (double)(LENGTH_TO_MAX(img->get_height())) / (double)LENGTH_TO_MAX(height);

  glBindTexture(GL_TEXTURE_2D, texture);
  assert(error_check());

  glBegin(GL_QUADS);
  assert(error_check());

  glTexCoord2i(0, 0);
  glVertex3i(0, 0, 0);

  glTexCoord2f(w, 0);
  glVertex3i(LENGTH_TO_MAX(img->get_width()), 0, 0);

  glTexCoord2f(w, h);
  glVertex3i(LENGTH_TO_MAX(img->get_width()), LENGTH_TO_MAX(img->get_height()), 0);

  glTexCoord2f(0, h);
  glVertex3i(0, LENGTH_TO_MAX(img->get_height()), 0);

  glEnd();
  assert(error_check());
}



void GateRenderer::render_gate_template(degate::GateTemplate_shptr tmpl,
					layer_list_type & layers) {

  bool first = true;

  BOOST_FOREACH(layer_list_type::value_type & p, layers) {
    Layer::LAYER_TYPE layer_type = p.first;
    if(tmpl->has_image(p.first)) {
      GateTemplateImage_shptr img = tmpl->get_image(layer_type);
      assert(img != NULL);

      if(glIsTexture(p.second) == GL_FALSE) {
	p.second = create_texture(img, layers.size() == 1 ? 0xff : 0x7f);
	first = false;
	debug(TM, "new texture: %d for (%p)", p.second, this);
      }


    }
  }

  glNewList(dlist, GL_COMPILE);


  first = true;
  BOOST_FOREACH(layer_list_type::value_type const & p, layers) {
    if(tmpl->has_image(p.first) && glIsTexture(p.second) == GL_TRUE) {
      GateTemplateImage_shptr img = tmpl->get_image(p.first);

      if(first) {
	glBlendFunc(GL_ONE, GL_ZERO);
	first = false;
      }
      else {
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(0, 0, 0, 1.0 / (double)layers.size());
      }

      render_texture(img, p.second);
      debug(TM, "texture: %d for (%p)", p.second, this);
    }
  }

  glEndList();


  /*
  if(!details) {
    color_t fill_col = gate->has_template() ? gate->get_gate_template()->get_fill_color() : 0;
    color_t frame_col = gate->has_template() ? gate->get_gate_template()->get_frame_color() : 0;

    if(fill_col == 0) fill_col = 0xa0303030;
    if(frame_col == 0) frame_col = fill_col;

    set_color(highlight_color_by_state(fill_col, gate->is_selected()));
    glLineWidth(1);
    glBegin(GL_QUADS);
    glVertex2i(gate->get_min_x(), gate->get_min_y());
    glVertex2i(gate->get_max_x(), gate->get_min_y());
    glVertex2i(gate->get_max_x(), gate->get_max_y());
    glVertex2i(gate->get_min_x(), gate->get_max_y());
    glEnd();

    set_color(highlight_color_by_state(frame_col, gate->is_selected()));
    glBegin(GL_LINE_LOOP);
    glVertex2i(gate->get_min_x(), gate->get_min_y());
    glVertex2i(gate->get_max_x(), gate->get_min_y());
    glVertex2i(gate->get_max_x(), gate->get_max_y());
    glVertex2i(gate->get_min_x(), gate->get_max_y());
    glEnd();

  }

  if(details && gate->has_name())
    draw_string(gate->get_min_x()+2,
		gate->get_min_y()+2 + get_font_height() + 1, gate->get_name(),
		gate->get_width() > 4 ? gate->get_width() - 4 : gate->get_width());

  if(gate->has_template()) {

    GateTemplate_shptr tmpl = gate->get_gate_template();

    // render names for type and instance
    if(details && gate->get_gate_template()->has_name())
      draw_string(gate->get_min_x()+2, gate->get_min_y()+2, tmpl->get_name(),
		  gate->get_width() > 4 ? gate->get_width() - 4 : gate->get_width());

    if(gate->has_orientation()) {
      for(Gate::port_iterator iter = gate->ports_begin(); iter != gate->ports_end(); ++iter) {
	GatePort_shptr port = *iter;
	GateTemplatePort_shptr tmpl_port = port->get_template_port();

	if(tmpl_port && tmpl_port->get_x() != 0 && tmpl_port->get_y() != 0) {
	  unsigned int x = port->get_x(), y = port->get_y();
	  unsigned int port_size = port->get_diameter();
	  color_t port_color = tmpl_port->get_fill_color() == 0 ? 0xff0000ff : tmpl_port->get_fill_color();

	  if(!details && port->is_selected()) {
	    port_color = highlight_color_by_state(port_color, true);
	    port_size *= 2;
	  }

	  if(!details) draw_circle(x, y, port_size, port_color);

	  if(details && tmpl_port->has_name()) draw_string(x+2, y+2, tmpl_port->get_name());

	}
      }
    }
  }
  */

}

