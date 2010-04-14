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

#ifndef __DEGATERENDERER_H__
#define __DEGATERENDERER_H__

#include <RenderArea.h>
#include <LogicModel.h>
#include <Layer.h>
#include <LogicModelHelper.h>
#include <ScalingManager.h>

#include <list>
#include <set>
#include <algorithm>
#include <boost/tuple/tuple.hpp>
#include <boost/foreach.hpp>

#include <gtkglmm.h>
#include <Editor.h>

class DegateRenderer : 
  public RenderArea,
  public Gtk::GL::Widget<DegateRenderer> {

  friend class GfxEditorTool<DegateRenderer>;

 private:

  degate::LogicModel_shptr lmodel;
  degate::Layer_shptr layer;
  double last_scaling;

  
  typedef boost::tuple<unsigned int, unsigned int, GLuint> bg_tiles_type;
  std::list<bg_tiles_type> rendered_bg_tiles;

  /*
  typedef std::map<degate::PlacedLogicModelObject_shptr, 
    Glib::RefPtr<Goocanvas::Item> > rendered_objects_type;
  rendered_objects_type rendered_objects;
  */

  degate::BoundingBox background_bbox;

  bool realized;


  double scale_font; // scaling factor for using glyph texture maps
  int font_height; // requested font height for rasterization
  GLuint * font_textures;
  GLuint font_dlist_base;

  GLuint background_dlist, gates_dlist, gate_details_dlist, 
    vias_dlist, wires_dlist, 
    annotations_dlist, annotation_details_dlist,
    grid_dlist,
    tool_dlist;

  bool should_update_gates;
  bool render_details;

  bool idle_hook_enabled;
  bool is_idle;
  bool lock_state;

  degate::RegularGrid_shptr regular_horizontal_grid;
  degate::RegularGrid_shptr regular_vertical_grid;
  degate::IrregularGrid_shptr irregular_horizontal_grid;
  degate::IrregularGrid_shptr irregular_vertical_grid;

protected:

  void on_realize();
  void update_viewport_dimension();

  void start_tool() {
    glNewList(tool_dlist, GL_COMPILE);
    //assert(error_check());
  }

  void stop_tool() {
    glEndList();
    assert(error_check());
    update_screen();
  }

  /**
   * Set the lock state of the renderer.
   */

  void set_lock(bool lock_state) {
    this->lock_state = lock_state;
  }

public:

  DegateRenderer();
  virtual ~DegateRenderer();

  

  void set_logic_model(degate::LogicModel_shptr lmodel) {
    this->lmodel = lmodel;
    should_update_gates = true;
  }

  void set_layer(degate::Layer_shptr layer) {
    this->layer = layer;
    clear_objects();
    last_scaling = 0; // reset scaling
    drop_tiles();
  }

  void set_grid(degate::RegularGrid_shptr regular_horizontal_grid,
		degate::RegularGrid_shptr regular_vertical_grid,
		degate::IrregularGrid_shptr irregular_horizontal_grid,
		degate::IrregularGrid_shptr irregular_vertical_grid) {

    this->regular_horizontal_grid = regular_horizontal_grid;
    this->regular_vertical_grid = regular_vertical_grid;
    this->irregular_horizontal_grid = irregular_horizontal_grid;
    this->irregular_vertical_grid = irregular_vertical_grid;
  }

  virtual void clear_objects() {
    /*
    BOOST_FOREACH(rendered_objects_type::value_type const & p, rendered_objects) {
      p.second->remove();
    }
    rendered_objects.clear();
    */
  }

  virtual void clear_object(degate::PlacedLogicModelObject_shptr o) {
  }

  
  virtual void update_screen();


  void render_vias();
  void render_wires();
  void render_grid();
  
 private:

  /**
   * Calculate the lower offset to top or left for a tile.
   */
  unsigned int to_lower_tile_offset(unsigned int pos, unsigned int tile_width) const {
    return (pos & ~(tile_width - 1));
  }

  /**
   * Calculate the upper offset to top or left for a tile.
   */
  unsigned int to_upper_tile_offset(unsigned int pos, unsigned int tile_width) const {
    return (pos & ~(tile_width - 1)) + (tile_width - 1);
  }

  /**
   * Check if a tile is present for the upper/left position given by x,y.
   */

  bool check_tile_is_present(unsigned int x, unsigned int y) const {
    
    BOOST_FOREACH(bg_tiles_type const & t, rendered_bg_tiles) {
      if(x == t.get<0>() && y == t.get<1>()) return true;
    }
    
    return false;
  }


  GLuint create_and_add_tile(degate::BackgroundImage_shptr img, 
			     unsigned int x, unsigned int y, 
			     unsigned int tile_width, 
			     unsigned int pre_scaling) const;


  void drop_tiles();

  void render_background();

  void render_gates(bool detail = false);
  void render_gate(degate::Gate_shptr gate, bool details);

  void render_annotations(bool detail = false);

  bool error_check() const;

  void draw_string(int x, int y, std::string const& str);
  void init_font(const char * fname, unsigned int h);
  void create_font_textures(FT_Face face, char ch, GLuint list_base, GLuint * tex_base);

  unsigned int get_font_height() const {
    return (double)font_height*scale_font;
  }

  void set_color(degate::color_t col);
  void draw_circle(int x, int y, int diameter, degate::color_t col);

  void on_idle();

  void render_grid(degate::Grid_shptr grid);

};


#endif
