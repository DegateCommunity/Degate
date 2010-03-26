#ifndef __RENDERWINDOW_H__
#define __RENDERWINDOW_H__

#include <gtkmm.h>
#include <iostream>
#include <cstdlib>
#include <assert.h>


/**
 * A container that have a "real" renderer and is able to
 * dispatch events to it.
 *
 * @todo Introduce templates
 */
template <typename RendererType>
class RenderWindow : public Gtk::Frame {

private:

  const static guint default_padding = 2;

  RendererType & renderer;

  Gtk::Adjustment v_adjustment;
  Gtk::Adjustment h_adjustment;
  Gtk::VScrollbar v_scrollbar;
  Gtk::HScrollbar h_scrollbar;

  Gtk::Table table;

  Gtk::HRuler h_ruler;
  Gtk::VRuler v_ruler;

  static const double zoom_step = 1.0/1.3;

  unsigned int win_width, win_height;

  sigc::connection v_adjustment_signal, h_adjustment_signal;

private:

  /**
   * Initialize the render window.
   */
  void init_window();

  /**
   * Recalculate and reconfigure the settings for the scrollbars.
   */
  void adjust_scrollbars();

  /**
   * This method is called, if the vertical adjustment is changed.
   */
  void on_v_adjustment_changed() {
    int y_offset = (int) v_adjustment.get_value();
    renderer.set_viewport_y_range(y_offset, renderer.get_viewport_height() + y_offset);
  }

  /**
   * This method is called, if the horizontal adjustment is changed.
   */
  void on_h_adjustment_changed() {
    std::cout << "h-adj changed" << std::endl;
    int x_offset = (int) h_adjustment.get_value();
    renderer.set_viewport_x_range(x_offset, 
				  renderer.get_viewport_width() + x_offset);
  }


  void on_mouse_scroll_down(unsigned int clicked_real_x, unsigned int clicked_real_y) {
    mouse_zoom(clicked_real_x, clicked_real_y, 1.0/zoom_step);
  }

  void on_mouse_scroll_up(unsigned int clicked_real_x, unsigned int clicked_real_y) {
    mouse_zoom(clicked_real_x, clicked_real_y, zoom_step);
  }


  void mouse_zoom(unsigned int clicked_real_x, unsigned int clicked_real_y, double zoom_factor);

  virtual bool on_expose_event(GdkEventExpose * event) {
    //std::cout << "RW::on_expose_event()" << std::endl;

    if((int)win_width != get_width() || (int)win_height != get_height()) {
      win_width = get_width();
      win_height = get_height();

      renderer.set_drawing_window_width(win_width);
      renderer.set_drawing_window_height(win_height);
    }

    //adjust_scrollbars();
    propagate_expose(table, event);

    return true;
  }


  void enable_adjustment_events();
  void disable_adjustment_events();

public:

  //@todo resize-event -> scroller

  /**
   * The constructor.
   * @todo Pass render params via this constructor to the ctor of RenderArea.
   */
  RenderWindow(RendererType & _renderer) : 
    renderer(_renderer),
    // value, lower, upper, step_increment, page_increment, page_size                
    v_adjustment(0.0, 0.0, 101.0, 0.1, 1.0, 1.0), 
    h_adjustment(0.0, 0.0, 101.0, 0.1, 1.0, 1.0),
    v_scrollbar(v_adjustment),
    h_scrollbar(h_adjustment),
    table(3, 3),
    win_width(0),
    win_height(0) {

      adjust_scrollbars();
      init_window();
  }

  /**
   * The destructor.
   */
  virtual ~RenderWindow() {}

  /**
   * Zoom in.
   */
  virtual void zoom_in() {
    zoom(renderer.get_viewport_center_x(), 
	 renderer.get_viewport_center_y(), 
	 zoom_step);
  }

  /**
   * Zoom out.
   */
  virtual void zoom_out() {
    zoom(renderer.get_viewport_center_x(), 
	 renderer.get_viewport_center_y(), 
	 1.0/zoom_step);
  }

  /**
   * Zoom and keep a center. Parameter \p center_x and \p center_y are
   * given in "real" pixel units.
   *
   * @param center_x The x coordinate of the center that should be kept.
   * @param center_y The y coordinate of the center that should be kept.
   * @param zoom_factor Values larger then 1 have the meaning of zoom out.
   *   Values less then 1 means zoom in.
   */
  virtual void zoom(double center_x, double center_y, double zoom_factor);

  /**
   * Center the view. Parameter \p center_x and \p center_y are
   * given in "real" pixel units.
   * @param center_x The x coordinate of the center that should be kept.
   * @param center_y The y coordinate of the center that should be kept.
   */
  virtual void center_view(unsigned int center_x, unsigned int center_y);


  virtual void set_virtual_size(unsigned int w, unsigned int h) {
    renderer.set_virtual_width(w);
    renderer.set_virtual_height(h);
  }

  
};





template <typename RendererType>
void RenderWindow<RendererType>::adjust_scrollbars() {

  unsigned int min_h = renderer.get_viewport_min_x();
  unsigned int max_h = min_h + renderer.get_viewport_width();
  unsigned int min_v = renderer.get_viewport_min_y();
  unsigned int max_v = min_v + renderer.get_viewport_height();

  std::cout << "adjust scrollbars, min_h = " << min_h << " max_h = " << max_h << std::endl;

  disable_adjustment_events();

  h_adjustment.configure( min_h, // value
			  0, // lower 
			  renderer.get_virtual_width(), // upper
			  (double)renderer.get_viewport_width() * 0.1, // step_increment
			  renderer.get_viewport_width(), // page_increment
			  renderer.get_viewport_width()); // page_size

  v_adjustment.configure( min_v, // value
			  0, // lower 
			  renderer.get_virtual_height(), // upper
			  (double)renderer.get_viewport_height() * 0.1, // step_increment
			  renderer.get_viewport_height(), // page_increment
			  renderer.get_viewport_height()); // page_size

  enable_adjustment_events();

  h_ruler.set_range(min_h, max_h, min_h, max_h);
  v_ruler.set_range(min_v, max_v, min_v, max_v);
}


template <typename RendererType>
void RenderWindow<RendererType>::mouse_zoom(unsigned int clicked_real_x, unsigned int clicked_real_y, 
			      double zoom_factor) {
  int real_dist_to_center_x = (int)clicked_real_x - (int)renderer.get_viewport_center_x();
  int real_dist_to_center_y = (int)clicked_real_y - (int)renderer.get_viewport_center_y();
  int new_center_x = (int)renderer.get_viewport_center_x() + real_dist_to_center_x -
    (double)real_dist_to_center_x * zoom_factor;

  int new_center_y = (int)renderer.get_viewport_center_y() + real_dist_to_center_y -
    (double)real_dist_to_center_y * zoom_factor;

  if(new_center_x < 0) new_center_x = 0;
  if(new_center_y < 0) new_center_y = 0;

  std::cout << "center: " << new_center_x << std::endl;
  zoom(new_center_x, new_center_y, zoom_factor);
}


template <typename RendererType>
void RenderWindow<RendererType>::init_window() {

  add(table);

  table.attach(h_ruler, 1, 2, 0, 1, 
	       Gtk::FILL,
	       Gtk::SHRINK,
	       default_padding, default_padding);
  
  table.attach(v_ruler, 0, 1, 1, 2, 
	       Gtk::SHRINK,
	       Gtk::FILL,
	       default_padding, default_padding);
  
  table.attach(renderer, 1, 2, 1, 2, 
	       Gtk::FILL | Gtk::EXPAND,
	       Gtk::FILL | Gtk::EXPAND,
	       default_padding, default_padding);
  
  table.attach(v_scrollbar, 2, 3, 1, 2, 
	       Gtk::SHRINK,
	       Gtk::FILL,
	       default_padding, default_padding);
  
  table.attach(h_scrollbar, 1, 2, 2, 3, 
	       Gtk::FILL,
	       Gtk::SHRINK,
	       default_padding, default_padding);
  
  // set scrollbar update policy
  h_scrollbar.set_update_policy(Gtk::UPDATE_CONTINUOUS);
  v_scrollbar.set_update_policy(Gtk::UPDATE_CONTINUOUS);
  
  // connect signals
  v_adjustment_signal = v_adjustment.signal_value_changed().connect
    (sigc::mem_fun(*this, &RenderWindow::on_v_adjustment_changed));

  h_adjustment_signal = h_adjustment.signal_value_changed().connect
    (sigc::mem_fun(*this, &RenderWindow::on_h_adjustment_changed));

  // connect signals
  renderer.signal_adjust_scrollbars().connect
    (sigc::mem_fun(*this, &RenderWindow::adjust_scrollbars));
  
  renderer.signal_mouse_scroll_up().connect
    (sigc::mem_fun(*this, &RenderWindow::on_mouse_scroll_up));
  renderer.signal_mouse_scroll_down().connect
    (sigc::mem_fun(*this, &RenderWindow::on_mouse_scroll_down));
  
  
  // configure ruler
  h_ruler.set_metric(Gtk::PIXELS);
  v_ruler.set_metric(Gtk::PIXELS);
  
  show_all_children();
}

template <typename RendererType>
void RenderWindow<RendererType>::enable_adjustment_events() {
  
  v_adjustment_signal.unblock();
  h_adjustment_signal.unblock();

}

template <typename RendererType>
void RenderWindow<RendererType>::disable_adjustment_events() {

  v_adjustment_signal.block();
  h_adjustment_signal.block();

}

template <typename RendererType>
void RenderWindow<RendererType>::zoom(double center_x, double center_y, double zoom_factor) {
  double delta_x = renderer.get_viewport_width();
  double delta_y = renderer.get_viewport_height();

  std::cout << "Zoom to center " << center_x << " / " << center_y 
	    << " factor: " << zoom_factor << std::endl;

  unsigned int max_edge_length = std::max(renderer.get_virtual_width(), 
					  renderer.get_virtual_height())*2;

  if( ((delta_x < max_edge_length && delta_y < max_edge_length) && zoom_factor >= 1) ||
      (delta_x > 10 && delta_y > 10 && zoom_factor <= 1)  ) {
    double min_x = center_x - zoom_factor * (delta_x/2.0);
    double min_y = center_y - zoom_factor * (delta_y/2.0);
    double max_x = center_x + zoom_factor * (delta_x/2.0);
    double max_y = center_y + zoom_factor * (delta_y/2.0);
    if(min_x < 0) { max_x -= min_x; min_x = 0; }
    if(min_y < 0) { max_y -= min_y; min_y = 0; }

    renderer.set_viewport(min_x, min_y, max_x, max_y);
    adjust_scrollbars();
  }
  else {
    std::cout << "no scroll" << std::endl;
  }

}

template <typename RendererType>
void RenderWindow<RendererType>::center_view(unsigned int center_x, unsigned int center_y) {
  unsigned int width_half = renderer.get_viewport_width() / 2;
  unsigned int height_half = renderer.get_viewport_height() / 2;
  
  unsigned int min_x = center_x > width_half ? center_x - width_half : 0;
  unsigned int min_y = center_y > height_half ? center_y - height_half : 0;
  
  renderer.set_viewport(min_x, min_y, 
			min_x + (width_half << 1), min_y + (height_half << 1));
  //adjust_scrollbars();
}

#endif
