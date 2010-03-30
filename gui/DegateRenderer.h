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

class DegateRenderer : 
  public RenderArea,
  public Gtk::GL::Widget<DegateRenderer> {

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

  GLuint background_dlist, gates_dlist, gate_details_dlist;

  bool should_update_gates;
  bool render_details;

  bool idle_hook_enabled;
  bool is_idle;

protected:

  void on_realize();
  void update_viewport_dimension();

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
    drop_tiles();
  }

  void set_grid(void) {}

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

};


#endif
