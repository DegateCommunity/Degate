#include <DegateRenderer.h>

using namespace degate;



void free_bg_raw_buf(const guint8 * buf) {
  if(buf != NULL) {
    std::cout << "Free pixbuf" << std::endl;
    delete[] buf;
  } 
}


void DegateRenderer::create_and_add_tile(degate::BackgroundImage_shptr img, 
					 unsigned int x, unsigned int y, 
					 unsigned int tile_width, 
					 double pre_scaling) {
  
  if(img == NULL) return;
  
  guint8 * data = new guint8[tile_width * tile_width * 4];
  //memset(data, 0x7f, tile_width * tile_width * 4);
  img->raw_copy(data, x, y);
    
  Glib::RefPtr<Gdk::Pixbuf> pixbuf = Gdk::Pixbuf::create_from_data(data, Gdk::COLORSPACE_RGB, 
								   true, // has_alpha
								   8, // bits_per_sample
								   tile_width,
								   tile_width,
								   tile_width * 4,
								   sigc::ptr_fun(&free_bg_raw_buf));

  Glib::RefPtr<Goocanvas::Image> rect = Goocanvas::Image::create(pixbuf);
  rect->scale(pre_scaling, pre_scaling);
  rect->translate(x, y);

  rect->set_is_static();
  get_root_item()->add_child(rect);

  rect->ensure_updated();
  rect->request_update();

  get_root_item()->request_update();


  //if(!rect->is_visible()) std::cout << "image is invisible at "<< x << "/" << y << std::endl;

  /*
  std::cout << "line width: " << rect->get_line_width() << std::endl;
  Glib::Value<double> v;
  v.set(0.0);
  rect->set_property_value("line-width", v);
  std::cout << "line width: " << rect->get_line_width() << std::endl;
  */

  rect->lower();
  rendered_bg_tiles.push_back(boost::make_tuple(x, y, rect));
}


Glib::RefPtr<Goocanvas::Rect> DegateRenderer::render_gate(degate::Gate_shptr gate) {

  Glib::RefPtr<Goocanvas::Rect> rect = Goocanvas::Rect::create(gate->get_min_x(),
							       gate->get_min_y(),
							       gate->get_width(),
							       gate->get_height());

  //rect->upper();

  Glib::PropertyProxy<guint> fill_col = rect->property_fill_color_rgba();
  fill_col.set_value(gate->get_fill_color());

  Glib::PropertyProxy<guint> frame_col = rect->property_stroke_color_rgba();
  frame_col.set_value(gate->get_frame_color());

  get_root_item()->add_child(rect);
  return rect;
}

void DegateRenderer::render_gates() {

  if(lmodel == NULL) return;

  degate::Layer_shptr layer_ptr;

  try {
    layer_ptr = degate::get_first_logic_layer(lmodel);
  }
  catch(degate::CollectionLookupException) {
    return ;
  }
  
  if(layer_ptr == NULL) return;
  
  for(degate::Layer::qt_region_iterator iter = layer_ptr->region_begin(get_viewport_min_x(), 
								       get_viewport_max_x(), 
								       get_viewport_min_y(), 
								       get_viewport_max_y());
      iter != layer_ptr->region_end(); ++iter) {
    
    if(degate::Gate_shptr gate = std::tr1::dynamic_pointer_cast<Gate>(*iter))	
      if(rendered_objects.find(gate) == rendered_objects.end()) {
	rendered_objects[gate] = render_gate(gate);
      }
  }
  
}

