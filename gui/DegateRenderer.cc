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

#include <DegateRenderer.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include <ft2build.h>
#include <freetype/freetype.h>
#include <freetype/ftglyph.h>
#include <freetype/ftoutln.h>
#include <freetype/fttrigon.h>

#define DEFAULT_FILTER GL_LINEAR

using namespace degate;

static inline uint32_t highlight_color(uint32_t col) {
  uint8_t r = MASK_R(col);
  uint8_t g = MASK_G(col);
  uint8_t b = MASK_B(col);
  uint8_t a = MASK_A(col);
  
  return MERGE_CHANNELS((((255-r)>>1) + r),
			(((255-g)>>1) + g),
			(((255-b)>>1) + b), (a < 128 ? 128 : a));
}


// @todo fix
static inline uint32_t highlight_color_by_state(uint32_t col, bool state) {
  if(!state) return col;
  else return highlight_color(col);
  /*
  if(state == SELECT_STATE_NOT) return col;
  else if(state == SELECT_STATE_DIRECT) return highlight_color(highlight_color(col));
  else if(state == SELECT_STATE_ADJ) return highlight_color(col);
  return col;
  */
}


struct GLConfigUtil {

  static void print_gl_attrib(const Glib::RefPtr<const Gdk::GL::Config>& glconfig,
                              const char* attrib_str,
                              int attrib,
                              bool is_boolean);
  
  static void examine_gl_attrib(const Glib::RefPtr<const Gdk::GL::Config>& glconfig);
};

//
// Print a configuration attribute.
//
void GLConfigUtil::print_gl_attrib(const Glib::RefPtr<const Gdk::GL::Config>& glconfig,
                                   const char* attrib_str,
                                   int attrib,
                                   bool is_boolean) {
  int value;

  if(glconfig->get_attrib(attrib, value)) {
    std::cout << attrib_str << " = ";
    if (is_boolean)
      std::cout << (value == true ? "true" : "false") << std::endl;
    else
      std::cout << value << std::endl;
  }
  else {
    std::cout << "*** Cannot get "
              << attrib_str
              << " attribute value\n";
  }
}
//
// Print configuration attributes.
//
void GLConfigUtil::examine_gl_attrib(const Glib::RefPtr<const Gdk::GL::Config>& glconfig) {
  std::cout << "\nOpenGL visual configurations :\n\n";

  std::cout << "glconfig->is_rgba() = "
            << (glconfig->is_rgba() ? "true" : "false")
            << std::endl;
  std::cout << "glconfig->is_double_buffered() = "
            << (glconfig->is_double_buffered() ? "true" : "false")
            << std::endl;
  std::cout << "glconfig->is_stereo() = "
            << (glconfig->is_stereo() ? "true" : "false")
            << std::endl;
  std::cout << "glconfig->has_alpha() = "
            << (glconfig->has_alpha() ? "true" : "false")
            << std::endl;
  std::cout << "glconfig->has_depth_buffer() = "
            << (glconfig->has_depth_buffer() ? "true" : "false")
            << std::endl;
  std::cout << "glconfig->has_stencil_buffer() = "
            << (glconfig->has_stencil_buffer() ? "true" : "false")
            << std::endl;
  std::cout << "glconfig->has_accum_buffer() = "
            << (glconfig->has_accum_buffer() ? "true" : "false")
            << std::endl;

  std::cout << std::endl;

  print_gl_attrib(glconfig, "Gdk::GL::USE_GL",           Gdk::GL::USE_GL,           true);
  print_gl_attrib(glconfig, "Gdk::GL::BUFFER_SIZE",      Gdk::GL::BUFFER_SIZE,      false);
  print_gl_attrib(glconfig, "Gdk::GL::LEVEL",            Gdk::GL::LEVEL,            false);
  print_gl_attrib(glconfig, "Gdk::GL::RGBA",             Gdk::GL::RGBA,             true);
  print_gl_attrib(glconfig, "Gdk::GL::DOUBLEBUFFER",     Gdk::GL::DOUBLEBUFFER,     true);
  print_gl_attrib(glconfig, "Gdk::GL::STEREO",           Gdk::GL::STEREO,           true);
  print_gl_attrib(glconfig, "Gdk::GL::AUX_BUFFERS",      Gdk::GL::AUX_BUFFERS,      false);
  print_gl_attrib(glconfig, "Gdk::GL::RED_SIZE",         Gdk::GL::RED_SIZE,         false);
  print_gl_attrib(glconfig, "Gdk::GL::GREEN_SIZE",       Gdk::GL::GREEN_SIZE,       false);
  print_gl_attrib(glconfig, "Gdk::GL::BLUE_SIZE",        Gdk::GL::BLUE_SIZE,        false);
  print_gl_attrib(glconfig, "Gdk::GL::ALPHA_SIZE",       Gdk::GL::ALPHA_SIZE,       false);
  print_gl_attrib(glconfig, "Gdk::GL::DEPTH_SIZE",       Gdk::GL::DEPTH_SIZE,       false);
  print_gl_attrib(glconfig, "Gdk::GL::STENCIL_SIZE",     Gdk::GL::STENCIL_SIZE,     false);
  print_gl_attrib(glconfig, "Gdk::GL::ACCUM_RED_SIZE",   Gdk::GL::ACCUM_RED_SIZE,   false);
  print_gl_attrib(glconfig, "Gdk::GL::ACCUM_GREEN_SIZE", Gdk::GL::ACCUM_GREEN_SIZE, false);
  print_gl_attrib(glconfig, "Gdk::GL::ACCUM_BLUE_SIZE",  Gdk::GL::ACCUM_BLUE_SIZE,  false);
  print_gl_attrib(glconfig, "Gdk::GL::ACCUM_ALPHA_SIZE", Gdk::GL::ACCUM_ALPHA_SIZE, false);

  std::cout << std::endl;

}

DegateRenderer::DegateRenderer() : last_scaling(0), realized(false), 
				   idle_hook_enabled(false), is_idle(true), lock_state(false) {
  //
  // Query OpenGL extension version.
  //

  int major, minor;
  Gdk::GL::query_version(major, minor);
  std::cout << "OpenGL extension version - "
            << major << "." << minor << std::endl;



  // Try double-buffered visual
  Glib::RefPtr<Gdk::GL::Config> glconfig = Gdk::GL::Config::create(Gdk::GL::MODE_RGBA   |
								   Gdk::GL::MODE_DEPTH  |
								   Gdk::GL::MODE_DOUBLE);
  if(!glconfig) {
    std::cerr << "*** Cannot find the double-buffered visual.\n"
              << "*** Trying single-buffered visual.\n";
    
    // Try single-buffered visual
    glconfig = Gdk::GL::Config::create(Gdk::GL::MODE_RGBA | Gdk::GL::MODE_DEPTH);
    if(!glconfig) {
      std::cerr << "*** Cannot find any OpenGL-capable visual.\n";
      std::exit(1);
    }
  }

  // print frame buffer attributes.
  GLConfigUtil::examine_gl_attrib(glconfig);

  //
  // Set OpenGL-capability to the widget.
  //

  set_gl_capability(glconfig);


  show();
  update_viewport_dimension();
  update_virtual_dimension();
}

DegateRenderer::~DegateRenderer() {
  glDeleteLists(font_dlist_base,128);
  glDeleteTextures(128, font_textures);
  delete [] font_textures;

}

void DegateRenderer::on_realize() {

  // We need to call the base on_realize()
  Gtk::DrawingArea::on_realize();

  Glib::RefPtr<Gdk::GL::Window> glwindow = get_gl_window();

  if(!glwindow->gl_begin(get_gl_context())) return;

  std::cout
    << "Vendor: " << glGetString(GL_VENDOR) << std::endl
    << "Renderer: " << glGetString(GL_RENDERER) << std::endl
    << "Version: " << glGetString(GL_VERSION) << std::endl
    << std::endl;

  glDisable(GL_LIGHTING);
  glDisable(GL_LIGHT0);
  glDisable(GL_COLOR_MATERIAL);
  glDisable(GL_DEPTH_TEST);

  //glDisbale(GL
  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
	    
  glClearColor(0, 0, 0, 0);
  //glClearColor(0, 0, 0, 0);
  //glClearDepth(10);
  
  glEnable(GL_TEXTURE_2D);


  // init fonts
  char font_file[PATH_MAX];
  snprintf(font_file, PATH_MAX, "%s/FreeSans.ttf", getenv("DEGATE_HOME"));
  init_font(font_file, 16);


  background_dlist = glGenLists(1);
  assert(error_check());

  gates_dlist = glGenLists(1);
  assert(error_check());

  gate_details_dlist = glGenLists(1);
  assert(error_check());

  vias_dlist = glGenLists(1);
  assert(error_check());

  wires_dlist = glGenLists(1);
  assert(error_check());

  annotations_dlist = glGenLists(1);
  assert(error_check());

  annotation_details_dlist = glGenLists(1);
  assert(error_check());

  grid_dlist = glGenLists(1);
  assert(error_check());

  tool_dlist = glGenLists(1);
  assert(error_check());
 
  glwindow->gl_end();

  realized = true;
}

void DegateRenderer::on_idle() {
  is_idle = true;
  if(realized) {
    if(get_scaling() <= 2) {
      render_details = true;
    }
    update_screen();
  }
  idle_hook_enabled = false;
}

void DegateRenderer::update_screen() {

  if(lock_state == false) {

    if(!idle_hook_enabled) {
      idle_hook_enabled = true;
      is_idle = false;
      Glib::signal_idle().connect_once(sigc::mem_fun(*this, &DegateRenderer::on_idle), 
				       Glib::PRIORITY_LOW);
    }
    
    if(is_idle || should_update_gates) {
      render_background();

      // render gates with and without details into two different display lists
      render_gates(false);
      render_gates(true);
      should_update_gates = false;

      // same with annotations
      render_annotations(false);
      render_annotations(true);

      render_vias();
      render_wires();

      render_grid();


    }
  }

  Glib::RefPtr<Gdk::GL::Window> glwindow = get_gl_window();

  if(glwindow->gl_begin(get_gl_context())) {

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    glCallList(background_dlist);
    assert(error_check());

    glCallList(gates_dlist);
    assert(error_check());

    glCallList(annotations_dlist);
    assert(error_check());

    glCallList(grid_dlist);
    assert(error_check());

    if(!lock_state && render_details) {
      glCallList(gate_details_dlist);
      assert(error_check());

      glCallList(annotation_details_dlist);
      assert(error_check());

      render_details = false;
    }

    if(is_idle) {
      glCallList(wires_dlist);
      assert(error_check());

      glCallList(vias_dlist);
      assert(error_check());
    }

    glCallList(tool_dlist);
    assert(error_check());

    // Swap buffers.
    if(glwindow->is_double_buffered()) glwindow->swap_buffers();
    else glFlush();

    glwindow->gl_end();
  }

}

void DegateRenderer::update_viewport_dimension() {

  clock_t start, finish;
  start = clock();

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

  finish = clock();
  debug(TM, "rendering time: %f ms", 1000*(double(finish - start)/CLOCKS_PER_SEC));

}


GLuint DegateRenderer::create_and_add_tile(degate::BackgroundImage_shptr img, 
					   unsigned int x, unsigned int y, 
					   unsigned int tile_width, 
					   unsigned int pre_scaling) const {
  
  //if(img == NULL) return;
  
  guint32 * data = new guint32[tile_width * tile_width];
  img->raw_copy(data, x, y);

  GLuint texture = 0;

  
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
	       tile_width, tile_width,
	       0, // border
	       GL_RGBA,
	       GL_UNSIGNED_BYTE,
	       data);
  assert(glGetError() == GL_NO_ERROR);

  delete[] data;

  unsigned int min_x = x * pre_scaling;
  unsigned int min_y = y * pre_scaling;
  unsigned int max_x = min_x + tile_width * pre_scaling + 1;
  unsigned int max_y = min_y + tile_width * pre_scaling + 1;

  glBindTexture(GL_TEXTURE_2D, texture);
  assert(error_check());

  glBegin(GL_QUADS);
  assert(error_check());

  glTexCoord2i(0, 0);
  glVertex3i(min_x, min_y, 0);

  glTexCoord2i(1, 0);
  glVertex3i(max_x, min_y, 0);

  glTexCoord2i(1, 1);
  glVertex3i(max_x, max_y, 0);

  glTexCoord2i(0, 1);
  glVertex3i(min_x, max_y, 0);
  glEnd();
  assert(error_check());

  //glDisable(GL_TEXTURE_2D);
  return texture;
}

void DegateRenderer::render_vias() {
  if(lmodel == NULL) return;

  glNewList(vias_dlist, GL_COMPILE);

  for(Layer::object_iterator iter = layer->objects_begin();
      iter != layer->objects_end(); ++iter) {

    if(Via_shptr via = std::tr1::dynamic_pointer_cast<Via>(*iter)) {
      unsigned int diameter = via->get_diameter();
      uint32_t col = via->get_direction() == Via::DIRECTION_UP ? 0xff12ffff : 0xff0000ff;

      if(via->is_selected()) {
	col = highlight_color_by_state(col, true);
	diameter <<= 2;
      }
      
      draw_circle(via->get_x(), via->get_y(), diameter, col);
    }

  }
  glEndList();
}

void DegateRenderer::render_grid() {
  glNewList(grid_dlist, GL_COMPILE);

  render_grid(regular_horizontal_grid);
  render_grid(irregular_horizontal_grid);
  render_grid(regular_vertical_grid);  
  render_grid(irregular_vertical_grid);  

  glEndList();
}

void DegateRenderer::render_grid(degate::Grid_shptr grid) {
  if(grid == NULL || !grid->is_enabled()) return;

  for(Grid::grid_iter iter = grid->begin(); iter != grid->end(); ++iter) {

    set_color(0xffff1200);
    glBegin(GL_QUADS);

    if(grid->is_vertical()) { // vertical spacing == horizontal lines
      int y = *iter;
      glVertex2i(0, y);
      glVertex2i(get_virtual_width() - 1, y);
      glVertex2i(get_virtual_width() - 1, y + 1);
      glVertex2i(0, y + 1);
    }
    else {
      int x = *iter;
      glVertex2i(x, 0);
      glVertex2i(x+1, 0);
      glVertex2i(x+1, get_virtual_height() - 1);
      glVertex2i(x, get_virtual_height() - 1);
    }
    glEnd();
  }
}

void DegateRenderer::render_wires() {
  if(lmodel == NULL) return;

  glNewList(wires_dlist, GL_COMPILE);
  for(Layer::object_iterator iter = layer->objects_begin();
      iter != layer->objects_end(); ++iter) {

    if(Wire_shptr wire = std::tr1::dynamic_pointer_cast<Wire>(*iter)) {
      color_t col = wire->get_frame_color();
      if(col == 0) col = 0xffff1200;

      set_color(highlight_color_by_state(col, wire->is_selected()));

      
      glLineWidth((double)wire->get_diameter() / get_scaling());
      glBegin(GL_LINES);
      glVertex2i(wire->get_from_x(), wire->get_from_y());
      glVertex2i(wire->get_to_x(), wire->get_to_y());
      glEnd();      
      

      /*
      int radius = wire->get_diameter() >> 1;

      glBegin(GL_POLYGON);
      glVertex2i((int)wire->get_from_x() - radius, (int)wire->get_from_y() - radius);
      glVertex2i((int)wire->get_to_x() + radius, (int)wire->get_from_y() - radius);
      glVertex2i((int)wire->get_to_x() + radius, (int)wire->get_to_y() + radius);
      glVertex2i((int)wire->get_from_x() - radius, (int)wire->get_to_y() + radius);
      glEnd();
      */
    }
  }
  glEndList();
}

void DegateRenderer::render_annotations(bool details) {

  if(lmodel == NULL) return;

  glNewList(details ? annotation_details_dlist : annotations_dlist, GL_COMPILE);

  for(Layer::object_iterator iter = layer->objects_begin();
      iter != layer->objects_end(); ++iter) {

    if(Annotation_shptr a = std::tr1::dynamic_pointer_cast<Annotation>(*iter)) {
      color_t fill_col = a->get_fill_color();
      color_t frame_col = a->get_frame_color();

      if(fill_col == 0) fill_col = 0xa0303030;
      if(frame_col == 0) frame_col = fill_col;

      if(!details) {
	set_color(highlight_color_by_state(fill_col, a->is_selected()));
	glLineWidth(1);
	glBegin(GL_QUADS);
	glVertex2i(a->get_min_x(), a->get_min_y());
	glVertex2i(a->get_max_x(), a->get_min_y());
	glVertex2i(a->get_max_x(), a->get_max_y());
	glVertex2i(a->get_min_x(), a->get_max_y());
	glEnd();
    
	set_color(highlight_color_by_state(frame_col, a->is_selected()));
	glBegin(GL_LINE_LOOP);
	glVertex2i(a->get_min_x(), a->get_min_y());
	glVertex2i(a->get_max_x(), a->get_min_y());
	glVertex2i(a->get_max_x(), a->get_max_y());
	glVertex2i(a->get_min_x(), a->get_max_y());
	glEnd();
      }
      else {
	if(a->has_name())
	  draw_string(a->get_min_x()+2, a->get_min_y()+2 + get_font_height(), a->get_name(), 
		      a->get_width() > 4 ? a->get_width() - 4 : a->get_width());
      }

    }
  }
  glEndList(); 
}


void DegateRenderer::render_gates(bool details) {

  if(lmodel == NULL) return;

  glNewList(details ? gate_details_dlist : gates_dlist, GL_COMPILE);

  for(LogicModel::gate_collection::iterator iter = lmodel->gates_begin();
      iter != lmodel->gates_end(); ++iter) {
    render_gate(iter->second, details);
  }

  glEndList(); 
}

void DegateRenderer::render_gate(degate::Gate_shptr gate, bool details) {

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

}

void DegateRenderer::set_color(color_t col) {
  glColor4ub(MASK_R(col), MASK_G(col), MASK_B(col), MASK_A(col));
}

void DegateRenderer::draw_circle(int x, int y, int diameter, color_t col) {
  set_color(col);
  int r = diameter >> 1;

  glBegin(GL_QUADS);
  glVertex2i(x - r, y - r);
  glVertex2i(x + r, y - r);
  glVertex2i(x + r, y + r);
  glVertex2i(x - r, y + r);
  glEnd();
    /*
  glBegin(GL_POLYGON);
  for(float angle = 0; angle < 2 * M_PI; angle += 2 * M_PI / 6.0)
    glVertex2f(x + r*cos(angle), y + r*sin(angle));
  glEnd();
    */
}




void DegateRenderer::render_background() {

  if(layer == NULL || !layer->has_background_image()) return;

  degate::ScalingManager_shptr smgr = layer->get_scaling_manager();
  assert(smgr != NULL);

  degate::ScalingManager<degate::BackgroundImage>::image_map_element elem = 
    smgr->get_image(get_scaling());

  if(last_scaling != elem.first ||
     !background_bbox.complete_within(get_viewport()) ) {

    drop_tiles();
    last_scaling = elem.first;
  
    unsigned int pre_scaling = elem.first;
    degate::BackgroundImage_shptr img = elem.second;
    
    unsigned int tile_width = img->get_tile_size();
  
    unsigned int // scaled coordinates
      min_x = to_lower_tile_offset(std::max(get_viewport_min_x(), 0), tile_width) / pre_scaling,
      max_x = to_upper_tile_offset(std::min((unsigned int)std::max(get_viewport_max_x(), 0), 
					    get_virtual_width()), tile_width) / pre_scaling,
      min_y = to_lower_tile_offset(std::max(get_viewport_min_y(), 0), tile_width) / pre_scaling,
      max_y = to_upper_tile_offset(std::min((unsigned int)std::max(get_viewport_max_y(), 0), 
					    get_virtual_height()), tile_width) / pre_scaling;

    // screen coordinates
    background_bbox.set(min_x * pre_scaling, 
			max_x * pre_scaling, 
			min_y * pre_scaling, 
			max_y * pre_scaling);

    glNewList(background_dlist, GL_COMPILE);
    assert(error_check());
    
    glColor4ub(0, 0, 0, 0xff);

    // iterate over possible tile positions
    for(unsigned int x = min_x; x < max_x; x+=tile_width)
    
      for(unsigned int y = min_y; y < max_y; y+=tile_width) {
	GLuint texture = create_and_add_tile(img, x, y, tile_width, elem.first);
	rendered_bg_tiles.push_back(boost::make_tuple(x, y, texture));
      }
  
    glEndList();
  
  }
}

void DegateRenderer::drop_tiles() {

  BOOST_FOREACH(bg_tiles_type const & t, rendered_bg_tiles) {
    GLuint i = t.get<2>();
    glDeleteTextures(1, &i);
    assert(error_check());
  }

  rendered_bg_tiles.clear();
}

bool DegateRenderer::error_check() const {
  GLenum errCode = glGetError();
  if(errCode != GL_NO_ERROR) {
    std::cout << "OpenGL Error: " << gluErrorString(errCode) << std::endl;
    return false;
  }
  return true;
}



/* ---------------------------------------------------------

   The following methods are derived from NeHe's OpenGL
   Lesson 43 on texture mapped font rendering.

   http://nehe.gamedev.net/data/lessons/lesson.asp?lesson=43

   --------------------------------------------------------- */

void DegateRenderer::draw_string(int x, int y, std::string const& str, unsigned int max_str_width) {

  double adjusted_scaling = 1;

  if(max_str_width != 0) {

    unsigned int string_width = 0;

    BOOST_FOREACH(unsigned char c, str) {
      string_width += glyph_width[(int)c];
    }
  
    string_width = round(scale_font * string_width);

    if(string_width >= max_str_width)
      adjusted_scaling = (double)max_str_width / (double)string_width;
  }

  glColor4f(0,0,0,1);

  glMatrixMode(GL_PROJECTION);
  glPushMatrix();

  glPushAttrib(GL_LIST_BIT | GL_CURRENT_BIT  | GL_ENABLE_BIT | GL_TRANSFORM_BIT); 
  glListBase(font_dlist_base);
  
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();

  float modelview_matrix[16];
  glGetFloatv(GL_MODELVIEW_MATRIX, modelview_matrix);
  glLoadIdentity();

  glTranslatef(x, y + (float)font_height * scale_font * adjusted_scaling, 0);
  glMultMatrixf(modelview_matrix);
  glScalef(scale_font * adjusted_scaling, scale_font * adjusted_scaling, 1);

  glCallLists(str.length(), GL_UNSIGNED_BYTE, str.c_str());

  glPopMatrix();
  glPopAttrib();          


  glMatrixMode(GL_PROJECTION);
  glPopMatrix();

}

int next_p2(int a) {
  int rval = 2;
  while(rval < a) rval <<= 1;
  return rval;
}

void DegateRenderer::init_font(const char * fname, unsigned int h) {
  FT_Library library;
  FT_Face face;

  font_textures = new GLuint[128];

  scale_font = 0.5;
  font_height = h;

  if(FT_Init_FreeType( &library )) throw std::runtime_error("FT_Init_FreeType failed");
  if(FT_New_Face( library, fname, 0, &face )) 
    throw std::runtime_error("FT_New_Face failed (there is probably a problem with your font file)");
  FT_Set_Char_Size(face, font_height << 6, font_height << 6, 96, 96);

  font_dlist_base = glGenLists(128);
  assert(error_check());

  glGenTextures(128, font_textures);
  assert(error_check());

  for(unsigned char i=0; i < 128; i++) {
    glyph_width[(int)i] = create_font_textures(face, i, font_dlist_base, font_textures);
  }

  FT_Done_Face(face);
  FT_Done_FreeType(library);
}

unsigned int DegateRenderer::create_font_textures(FT_Face face, char ch, GLuint list_base, GLuint * tex_base) {

  if(FT_Load_Glyph( face, FT_Get_Char_Index( face, ch ), FT_LOAD_DEFAULT ))
    throw std::runtime_error("FT_Load_Glyph failed");

  FT_Glyph glyph;
  if(FT_Get_Glyph(face->glyph, &glyph)) throw std::runtime_error("FT_Get_Glyph failed");

  FT_Glyph_To_Bitmap( &glyph, ft_render_mode_normal, 0, 1 );
  FT_BitmapGlyph bitmap_glyph = (FT_BitmapGlyph)glyph;

  FT_Bitmap& bitmap=bitmap_glyph->bitmap;

  int width = next_p2( bitmap.width );
  int height = next_p2( bitmap.rows );

  GLubyte* expanded_data = new GLubyte[ 2 * width * height];

  for(int j=0; j < height; j++) {
    for(int i=0; i < width; i++){
      unsigned int dst_offs = 2*(i+j*width);
      expanded_data[dst_offs] = 255;
      expanded_data[dst_offs + 1] =  (i>=bitmap.width || j>=bitmap.rows) ?
	0 : bitmap.buffer[i + bitmap.width*j];
    }
  }

  
  glBindTexture( GL_TEXTURE_2D, tex_base[(int)ch]);
  assert(error_check());

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, DEFAULT_FILTER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, DEFAULT_FILTER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  assert(glGetError() == GL_NO_ERROR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
  assert(glGetError() == GL_NO_ERROR);

  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
  assert(glGetError() == GL_NO_ERROR);


  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0,
	       GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, expanded_data);
  assert(error_check());

  delete [] expanded_data;

  glNewList(list_base+ch, GL_COMPILE);
  assert(error_check());

  glBindTexture(GL_TEXTURE_2D, tex_base[(int)ch]);
  glPushMatrix();
  glTranslatef(bitmap_glyph->left, - bitmap_glyph->top, 0);

  float
    x=((float)bitmap.width / (float)width),
    y=((float)(bitmap.rows) / (float)height);
  glBegin(GL_QUADS);
  glTexCoord2f(0, 0); glVertex2f(0,0);
  glTexCoord2f(x, 0); glVertex2f(bitmap.width, 0);
  glTexCoord2f(x, y); glVertex2f(bitmap.width, bitmap.rows);
  glTexCoord2f(0, y); glVertex2f(0, bitmap.rows);
  glEnd();
  glPopMatrix();

  glTranslatef(face->glyph->advance.x >> 6 ,0,0);

  glEndList();

  FT_Done_Glyph(glyph);

  return width;
}
