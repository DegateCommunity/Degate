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

#ifndef __OPENGLRENDERERBASE_H__
#define __OPENGLRENDERERBASE_H__

#include <RenderArea.h>
#include <gtkglmm.h>
#include <SingletonBase.h>

#define DEFAULT_FILTER GL_LINEAR

class OpenGLRendererBase :
  public RenderArea,
  public Gtk::GL::Widget<OpenGLRendererBase> {

private:

  // begin of helper class
  class FontRenderingHelper : public degate::SingletonBase<FontRenderingHelper> {

    friend class degate::SingletonBase<FontRenderingHelper>;

  private:
    double scale_font; // scaling factor for using glyph texture maps
    int font_height; // requested font height for rasterization
    unsigned int glyph_width[128];
    GLuint * font_textures;
    GLuint font_dlist_base;

  public:

    ~FontRenderingHelper();

    unsigned int get_font_height() const {
      return (double)font_height*scale_font;
    }

    /**
     * Draw a string.
     * @param x x-position
     * @param y y-position
     * @param str The string, that should be printed.
     * @param max_str_width The maximum string width. If the real string width is larger,
     *   then the font size is adjusted, so that the string fits. If \p max_str_width == 0
     *   no adjustment will be performed.
     */
    void draw_string(int x, int y, std::string const& str,
		     unsigned int max_str_width = 0);

  private:

    FontRenderingHelper();


    void init_font(const char * fname, unsigned int h) throw(degate::DegateRuntimeException);

    /**
     * Create a font texture for a glyph.
     * @return Returns the width of the created glyph
     */

    unsigned int create_font_textures(FT_Face face, char ch, GLuint list_base, GLuint * tex_base)
      throw(degate::DegateRuntimeException);

  };
  // end of helper class

public:

  OpenGLRendererBase();
  virtual ~OpenGLRendererBase();


protected:

  void on_realize();

  bool error_check() const;


  void set_color(degate::color_t col);

  /**
  * @param render_distant_outline If it is set, the renderer will draw
  *   another square around the centered filled square.
  */
  void draw_square(int x, int y, int diameter, degate::color_t col,
		   bool render_distant_outline = false);

  void draw_circle(int x, int y, int diameter, degate::color_t col);

  /**
   * Draw a string.
   * @param x x-position
   * @param y y-position
   * @param col font color
   * @param str The string, that should be printed.
   * @param max_str_width The maximum string width. If the real string width is larger,
   *   then the font size is adjusted, so that the string fits. If \p max_str_width == 0
   *   no adjustment will be performed.
   */
  void draw_string(int x, int y, degate::color_t col, std::string const& str,
		   unsigned int max_str_width = 0);

  unsigned int get_font_height() const;

  void draw_hline(int y, int len, degate::color_t col);
  void draw_vline(int x, int len, degate::color_t col);

};


#endif
