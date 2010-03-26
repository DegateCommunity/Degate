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

class DegateRenderer : public RenderArea {

 private:

  degate::LogicModel_shptr lmodel;
  degate::Layer_shptr layer;
  double last_scaling;

  typedef boost::tuple<unsigned int, unsigned int, Glib::RefPtr<Goocanvas::Image> > bg_tiles_type;
  std::list<bg_tiles_type> rendered_bg_tiles;

  typedef std::map<degate::PlacedLogicModelObject_shptr, Glib::RefPtr<Goocanvas::Item> > rendered_objects_type;
  rendered_objects_type rendered_objects;

 public:

  DegateRenderer() : last_scaling(0) {
  }
  

  void set_logic_model(degate::LogicModel_shptr lmodel) {
    this->lmodel = lmodel;
  }

  void set_layer(degate::Layer_shptr layer) {
    this->layer = layer;
    clear_objects();
    drop_tiles();
  }

  void set_grid(void) {}

  virtual void clear_objects() {
    BOOST_FOREACH(rendered_objects_type::value_type const & p, rendered_objects) {
      p.second->remove();
    }
    rendered_objects.clear();
  }

  virtual void clear_object(degate::PlacedLogicModelObject_shptr o) {
  }

  virtual void update_screen() {
    RenderArea::update_screen();

    update_viewport_dimension();
    render_gates();
  }

 protected:

  /**
   * Method is called after changes to the viewport.
   */
  virtual void update_viewport_dimension() {
    RenderArea::update_viewport_dimension();
    
    render_background();
  }


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


  void create_and_add_tile(degate::BackgroundImage_shptr img, 
			   unsigned int x, unsigned int y, unsigned int tile_width, double pre_scaling);


  void drop_tiles() {
    std::cout << "drop tiles" << std::endl;
    BOOST_FOREACH(bg_tiles_type const & t, rendered_bg_tiles) {
      t.get<2>()->remove();
    }
    rendered_bg_tiles.clear();
  }

  void render_background() {

    if(layer == NULL || !layer->has_background_image()) return;

  clock_t start, finish;
  start = clock();

    degate::ScalingManager_shptr smgr = layer->get_scaling_manager();
    assert(smgr != NULL);
    std::cout << "Requested scaling is " << get_scaling() << std::endl;
    degate::ScalingManager<degate::BackgroundImage>::image_map_element  elem = smgr->get_image(get_scaling());
    std::cout << "Tile scaling is: " << elem.first << std::endl;
    if(last_scaling > 0 && last_scaling != elem.first) drop_tiles();
    last_scaling = elem.first;

    double pre_scaling = get_scaling() / elem.first;
    std::cout << "Prescaling: " << pre_scaling << std::endl;
    degate::BackgroundImage_shptr img = elem.second;

    unsigned int tile_width = img->get_tile_size();

    unsigned int 
      min_x = to_lower_tile_offset(std::max(get_viewport_min_x(), 0), tile_width),
      max_x = to_upper_tile_offset(std::min((unsigned int)std::max(get_viewport_max_x(), 0), 
					    get_virtual_width()), tile_width),
      min_y = to_lower_tile_offset(std::max(get_viewport_min_y(), 0), tile_width),
      max_y = to_upper_tile_offset(std::min((unsigned int)std::max(get_viewport_max_y(), 0), 
					    get_virtual_height()), tile_width);
    
    std::cout << "Check for tiles in range: " 
	      << get_viewport_min_x() << ".." << get_viewport_max_x() << " / "
	      << get_viewport_min_y() << ".." << get_viewport_max_y() << std::endl;
    
    // iterate over possible tile positions
      for(unsigned int x = min_x; x < max_x; x+=tile_width) {

	for(unsigned int y = min_y; y < max_y; y+=tile_width) {

	std::cout << "\tCheck if there is a tile at " << x << " / " << y << std::endl;
	// if there is not already a tile, create one
	if(!check_tile_is_present(x, y)) {
	  std::cout << "No. Create a tile." << std::endl;
	  create_and_add_tile(img, x, y, tile_width, elem.first);
	}
      }
    }
      
  finish = clock();
  debug(TM, "rendering time: %f ms", 1000*(double(finish - start)/CLOCKS_PER_SEC));
   
  }

  void render_gates();

  Glib::RefPtr<Goocanvas::Rect> render_gate(degate::Gate_shptr gate);

};


#endif
