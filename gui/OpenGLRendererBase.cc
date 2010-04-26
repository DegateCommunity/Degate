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

#include <OpenGLRendererBase.h>
#include <Image.h>
#include <DegateHelper.h>

#include <GL/gl.h>
#include <GL/glu.h>

#include <ft2build.h>
#include <freetype/freetype.h>
#include <freetype/ftglyph.h>
#include <freetype/ftoutln.h>
#include <freetype/fttrigon.h>

#include <boost/foreach.hpp>
#include <iostream>

using namespace degate;

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

OpenGLRendererBase::OpenGLRendererBase() {
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

}


OpenGLRendererBase::~OpenGLRendererBase() {
}


void OpenGLRendererBase::on_realize() {
  // We need to call the base on_realize()
  Gtk::DrawingArea::on_realize();

  Glib::RefPtr<Gdk::GL::Window> glwindow = get_gl_window();


  if(!glwindow->gl_begin(get_gl_context())) return;

  std::cout
    << "Vendor: " << glGetString(GL_VENDOR) << std::endl
    << "Renderer: " << glGetString(GL_RENDERER) << std::endl
    << "Version: " << glGetString(GL_VERSION) << std::endl
    << std::endl;

  // init fonts
  FontRenderingHelper::get_instance();

  glwindow->gl_end();

}

bool opengl_error_check() {
  GLenum errCode = glGetError();
  if(errCode != GL_NO_ERROR) {
    std::cout << "OpenGL Error: " << gluErrorString(errCode) << std::endl;
    return false;
  }
  return true;
}

bool OpenGLRendererBase::error_check() const {
  return opengl_error_check();
}


void OpenGLRendererBase::set_color(color_t col) {
  glColor4ub(MASK_R(col), MASK_G(col), MASK_B(col), MASK_A(col));
}

void OpenGLRendererBase::draw_circle(int x, int y, int diameter, color_t col) {
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


void OpenGLRendererBase::draw_string(int x, int y, std::string const& str, unsigned int max_str_width) {
  FontRenderingHelper::get_instance().draw_string(x, y, str, max_str_width);
}

unsigned int OpenGLRendererBase::get_font_height() const {
  return FontRenderingHelper::get_instance().get_font_height();
}

OpenGLRendererBase::FontRenderingHelper::FontRenderingHelper() {
  try {
    char font_file[PATH_MAX];
    snprintf(font_file, PATH_MAX, "%s/FreeSans.ttf", getenv("DEGATE_HOME"));
    init_font(font_file, 16);
  }
  catch(degate::DegateRuntimeException const& ex) {
    // print and ignore exceptions
    std::cout << ex.what() << std::endl;
  }
}

OpenGLRendererBase::FontRenderingHelper::~FontRenderingHelper() {
  try {
    glDeleteLists(font_dlist_base,128);
    glDeleteTextures(128, font_textures);
    delete [] font_textures;
  }
  catch(...) { } // ignore exceptions
}


/* ---------------------------------------------------------

   The following methods are derived from NeHe's OpenGL
   Lesson 43 on texture mapped font rendering.

   http://nehe.gamedev.net/data/lessons/lesson.asp?lesson=43

   --------------------------------------------------------- */

void OpenGLRendererBase::FontRenderingHelper::draw_string(int x, int y, std::string const& str, unsigned int max_str_width) {

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


void OpenGLRendererBase::FontRenderingHelper::init_font(const char * fname, 
							unsigned int h) throw(DegateRuntimeException) {
  FT_Library library;
  FT_Face face;

  font_textures = new GLuint[128];

  scale_font = 0.5;
  font_height = h;

  if(FT_Init_FreeType( &library )) throw DegateRuntimeException("FT_Init_FreeType failed");
  if(FT_New_Face( library, fname, 0, &face )) 
    throw DegateRuntimeException("FT_New_Face failed (there is probably a problem with your font file)");
  FT_Set_Char_Size(face, font_height << 6, font_height << 6, 96, 96);

  font_dlist_base = glGenLists(128);
  assert(opengl_error_check());

  glGenTextures(128, font_textures);
  assert(opengl_error_check());

  for(unsigned char i=0; i < 128; i++) {
    glyph_width[(int)i] = create_font_textures(face, i, font_dlist_base, font_textures);
  }

  FT_Done_Face(face);
  FT_Done_FreeType(library);
}

unsigned int OpenGLRendererBase::FontRenderingHelper::create_font_textures(FT_Face face, char ch, 
									   GLuint list_base, GLuint * tex_base) 
  throw(DegateRuntimeException) {

  if(FT_Load_Glyph( face, FT_Get_Char_Index( face, ch ), FT_LOAD_DEFAULT ))
    throw DegateRuntimeException("FT_Load_Glyph failed");

  FT_Glyph glyph;
  if(FT_Get_Glyph(face->glyph, &glyph)) throw DegateRuntimeException("FT_Get_Glyph failed");

  FT_Glyph_To_Bitmap( &glyph, ft_render_mode_normal, 0, 1 );
  FT_BitmapGlyph bitmap_glyph = (FT_BitmapGlyph)glyph;

  FT_Bitmap& bitmap=bitmap_glyph->bitmap;

  int width = next_power_of_two( bitmap.width );
  int height = next_power_of_two( bitmap.rows );

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
  assert(opengl_error_check());

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
  assert(opengl_error_check());

  delete [] expanded_data;

  glNewList(list_base+ch, GL_COMPILE);
  assert(opengl_error_check());

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
