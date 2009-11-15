
/*                                                                              
                                                                                
This file is part of the IC reverse engineering tool degate.                    
                                                                                
Copyright 2008, 2009 by Martin Schobert                                         
                                                                                
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

/* ---------------------------------------------------------

   Warning: this is old ugly code and must be rewritten.

   --------------------------------------------------------- */

#include <gdkmm/window.h>
#include <iostream>

#include "ImageWin.h"
#include "MainWin.h"
#include <renderer.h>
#include <LogicModel.h>
//#include "lib/graphics.h"
#include <ScalingManager.h>
#include <list>

using namespace degate;

void ImageWin::setup_renderer() {

  debug(TM, "initialize renderer");
  renderer = renderer_create();
  //set_grid(NULL);
  renderer_initialize_params(&render_params);

  renderer_add_layer(renderer, (render_func_t) &render_background, &render_params, 1, "Background");
  renderer_add_layer(renderer, (render_func_t) &render_to_grayscale, &render_params, 0, "Background to grayscale");
  renderer_add_layer(renderer, (render_func_t) &render_grid, &render_params, 1, "Grid");
  renderer_add_layer(renderer, (render_func_t) &render_gates, &render_params, 1, "Logic Gates");
  renderer_add_layer(renderer, (render_func_t) &render_wires, &render_params, 1, "Wires");
  renderer_add_layer(renderer, (render_func_t) &render_vias, &render_params, 1, "Vias");


  current_layer = -1;
}

void ImageWin::set_render_logic_model(LogicModel_shptr lmodel) {
  render_params.lmodel = lmodel;
}

void ImageWin::set_render_background_images(std::tr1::shared_ptr<degate::ScalingManager<degate::BackgroundImage> > scaling_manager) {
  render_params.scaling_manager = scaling_manager;
}

void ImageWin::set_current_layer(int layer) {
  debug(TM, "new layer is: %d", layer);
  if(layer >= 0) {
    assert(render_params.lmodel != NULL);
    Layer_shptr l = render_params.lmodel->get_current_layer();
    set_render_background_images(l->get_scaling_manager());
  }
  this->current_layer = layer;

  //is_locked = false;
}

ImageWin::ImageWin() {
 
  setup_renderer();

  curr_width = 0;
  curr_height = 0;

  min_x = 0;
  min_y = 0;
  max_x = 100;
  max_y = 100;

  last_click_on_x = 0;
  last_click_on_y = 0;
  mouse_button_pressed = false;

  // no areas is selected
  reset_selection();
  reset_wire();

  // no object is selected
  object_selection_x = 0;
  object_selection_y = 0;
  object_selection_active = false;

  set_events(/*Gdk::POINTER_MOTION_MASK | 
	     Gdk::POINTER_MOTION_HINT_MASK | 
	     Gdk::BUTTON_PRESS_MASK | 
	     Gdk::BUTTON_RELEASE_MASK |
	     Gdk::BUTTON_MOTION_MASK |
	     Gdk::BUTTON1_MOTION_MASK |
	     Gdk::BUTTON2_MOTION_MASK |
	     Gdk::BUTTON3_MOTION_MASK |
	     Gdk::SCROLL_MASK | */
	     Gdk::ALL_EVENTS_MASK);

  set_flags(Gtk::CAN_FOCUS);
  //is_locked = true;
}


ImageWin::~ImageWin() {
  renderer_destroy(renderer);
}


void ImageWin::unlock_renderer() {
  is_locked = false;
}

void ImageWin::lock_renderer() {
  is_locked = true;
}

void ImageWin::reset_selection() {
  selection_x_start = 0;
  selection_y_start = 0;
  selection_x_end = 0;
  selection_y_end = 0;
  in_selection_mode = false;
  signal_selection_revoked_();
}

void ImageWin::reset_wire() {
  line_x_start = 0;
  line_y_start = 0;
  line_x_end = 0;
  line_y_end = 0;
  in_line_mode = false;
}


void ImageWin::set_min_x(unsigned int val) {
  min_x = val;
}

void ImageWin::set_min_y(unsigned int val) {
  min_y = val;
}

void ImageWin::set_max_x(unsigned int val) {
  max_x = val;
}

void ImageWin::set_max_y(unsigned int val) {
  max_y = val;
}

void ImageWin::set_view(unsigned int min_x, unsigned int min_y, unsigned int max_x, unsigned int max_y) {

  this->min_x = min_x;
  this->min_y = min_y;
  this->max_x = max_x;
  this->max_y = max_y;


  if(get_scaling_x() < get_scaling_y())
    max_y = (double)curr_height * (double)(max_x - min_x) / (double)curr_width + min_y;
  else if(get_scaling_x() > get_scaling_y())
    max_x = (double)curr_width * (double)(max_y - min_y) / (double)curr_height + min_x;
}

unsigned int ImageWin::get_min_x() {
  return min_x;
}

unsigned int ImageWin::get_min_y() {
  return min_y;
}

unsigned int ImageWin::get_max_x() {
  return max_x;
}

unsigned int ImageWin::get_max_y() {
  return max_y;
}

unsigned int ImageWin::get_width() {
  return curr_width;
}

unsigned int ImageWin::get_height() {
  return curr_height;
}

unsigned int ImageWin::get_real_width() {
  return curr_width * get_scaling_x();
}

unsigned int ImageWin::get_real_height() {
  return curr_height * get_scaling_y();
}


unsigned int ImageWin::get_center_x() {
  return min_x + ((max_x - min_x) >> 1);

}

unsigned int ImageWin::get_center_y() {
  return min_y + ((max_y - min_y) >> 1);
}

double ImageWin::get_scaling_x() {
  return (max_x - min_x) / (double)curr_width;
}

double ImageWin::get_scaling_y() {
  return (max_y - min_y) / (double)curr_height;
}


void ImageWin::coord_screen_to_real(unsigned int screen_x, unsigned int screen_y, 
				    unsigned int * real_x, unsigned int * real_y) {

  *real_x = (unsigned int)(screen_x * get_scaling_x() + min_x);
  *real_y = (unsigned int)(screen_y * get_scaling_y() + min_y);
}

// XXX real coord must be on screen!!!!
void ImageWin::coord_real_to_screen(unsigned int real_x, unsigned int real_y, 
				    unsigned int * screen_x, unsigned int * screen_y) {

  if(real_x >= min_x && real_x <= max_x)
    *screen_x = (unsigned int)((real_x - min_x) / get_scaling_x());
  else if(real_x < min_x)
    *screen_x = 0;
  else
    *screen_x = curr_width;
    
  if(real_y >= min_y && real_y <= max_y)
    *screen_y = (unsigned int)((real_y - min_y) / get_scaling_y());
  else if(real_y < min_y)
    *screen_y = 0;
  else
    *screen_y = curr_height;
}

bool ImageWin::selection_active() {
  return (selection_x_start != 0 || 
	  selection_y_start != 0 || 
	  selection_x_end != 0 || 
	  selection_y_end != 0) ? true : false;
}

void ImageWin::resize_rendering_buffer(unsigned int new_width, unsigned int new_height) {

  if(new_width != curr_width || new_height != curr_height) {

    if(rendering_buffer != NULL) 
      rendering_buffer.reset();

    if(rendering_buffer_backup != NULL ) {
      rendering_buffer_backup.reset();
    }

    rendering_buffer = RendererImage_shptr(new RendererImage(new_width, new_height));

    rendering_buffer_backup = RendererImage_shptr(new RendererImage(new_width, new_height));

    curr_width = new_width;
    curr_height = new_height;
    
  }
}

void ImageWin::update_screen() {

  //if(is_locked) return;
  Glib::RefPtr<Gdk::Window> window = get_window();

  if(window) {
    Gtk::Allocation allocation = get_allocation();
    const unsigned int width = allocation.get_width();
    const unsigned int height = allocation.get_height();

    Glib::RefPtr<Gdk::GC> gc = this->get_style()->get_black_gc();
    resize_rendering_buffer(width, height);

    if(current_layer >= 0) {
      render_region(renderer, rendering_buffer, current_layer, min_x, min_y, max_x, max_y);
      //gr_clone_image_data(rendering_buffer_backup, rendering_buffer);
      copy_image<RendererImage, RendererImage>(rendering_buffer_backup, rendering_buffer);
    }
    else {
      clear_image<RendererImage>(rendering_buffer);
      clear_image<RendererImage>(rendering_buffer_backup);
    }
    
    {
      // hack to get the renderer up and running
      degate::color_t * b = (degate::color_t *)malloc(width * height * 4);
      color_t * b_ptr = b;
      for(int y = 0; y < rendering_buffer->get_height(); y++)
	for(int x = 0; x < rendering_buffer->get_width(); x++) {
	  color_t p = rendering_buffer->get_pixel(x, y);
	  *b_ptr++ = p;
	}
      
      window->draw_rgb_32_image(gc, 0, 0, width, height, Gdk::RGB_DITHER_NONE, 
				(guchar *)b, rendering_buffer->get_width() * 4);
      
      free(b);
    }

    if(selection_active()) draw_selection_box();
    else if(in_line_mode) draw_wire();
    else if(object_selection_active) draw_object_info();
  }
}

bool ImageWin::on_expose_event(GdkEventExpose * event) {
  
  update_screen();
  return true;
}

bool ImageWin::render_to_file(const char * const filename, 
			      unsigned int min_x, unsigned int min_y, unsigned int max_x, unsigned int max_y) {

  unsigned int width = (max_x - min_x);
  unsigned int height = (max_y - min_y);
  RendererImage_shptr img(new RendererImage(width, height));
  if(img == NULL) return false;
  
  render_region(renderer, img, current_layer, min_x, min_y, max_x, max_y);

  if(RET_IS_NOT_OK(renderer_write_image(img, filename))) {
    return false;
  }
  else {
    return true;
  }
}

void ImageWin::draw_selection_box() {

  if(selection_active()) {

    if(selection_x_start > max_x ||
       selection_x_end < min_x ||
       selection_y_start > max_y ||
       selection_y_end < min_y) return;

    Glib::RefPtr<Gdk::Window> window = get_window();
    if(window) {
      Gtk::Allocation allocation = get_allocation();

      Glib::RefPtr<Gdk::GC> gc = this->get_style()->get_black_gc();




    {
      // hack to get the renderer up and running
      degate::color_t * b = (degate::color_t *)malloc(curr_width * curr_height * 4);
      color_t * b_ptr = b;
      for(int y = 0; y < curr_height; y++)
	for(int x = 0; x < curr_width; x++) {
	  color_t p = rendering_buffer_backup->get_pixel(x, y);
	  *b_ptr++ = p;
	}
      
      window->draw_rgb_32_image(gc, 0, 0, curr_width, curr_height, Gdk::RGB_DITHER_NONE, 
				(guchar *)b, curr_width * 4);
      
      free(b);
    }


    //window->draw_rgb_32_image(gc, 0, 0, curr_width, curr_height, Gdk::RGB_DITHER_NONE, 
    //(guchar *)rendering_buffer_backup->map->mem, curr_width * BYTES_PER_PIXEL);
      


      Cairo::RefPtr<Cairo::Context> cr = window->create_cairo_context();
      
      cr->set_source_rgb(1, 1, 1);
      cr->set_line_width(1);
      
      unsigned int screen_min_x, screen_min_y, screen_max_x, screen_max_y;

      coord_real_to_screen(selection_x_start, selection_y_start,
			   &screen_min_x, &screen_min_y);

      coord_real_to_screen(selection_x_end, selection_y_end,
			   &screen_max_x, &screen_max_y);

      cr->rectangle(screen_min_x, screen_min_y, 
		    screen_max_x - screen_min_x, screen_max_y - screen_min_y);
      
      cr->stroke();
    }
  }
}

void ImageWin::draw_wire() {

  if(in_line_mode) {

    if(line_x_start > max_x ||
       line_x_end < min_x ||
       line_y_start > max_y ||
       line_y_end < min_y) return;

    Glib::RefPtr<Gdk::Window> window = get_window();
    if(window) {
      Gtk::Allocation allocation = get_allocation();

      Glib::RefPtr<Gdk::GC> gc = this->get_style()->get_black_gc();




    {
      // hack to get the renderer up and running
      degate::color_t * b = (degate::color_t *)malloc(curr_width * curr_height * 4);
      color_t * b_ptr = b;
      for(int y = 0; y < curr_height; y++)
	for(int x = 0; x < curr_width; x++) {
	  color_t p = rendering_buffer_backup->get_pixel(x, y);
	  *b_ptr++ = p;
	}
      
      window->draw_rgb_32_image(gc, 0, 0, curr_width, curr_height, Gdk::RGB_DITHER_NONE, 
				(guchar *)b, curr_width * 4);
      
      free(b);
    }

    //      window->draw_rgb_32_image(gc, 0, 0, curr_width, curr_height, Gdk::RGB_DITHER_NONE, 
    //			(guchar *)rendering_buffer_backup->map->mem, curr_width * BYTES_PER_PIXEL);
      



      Cairo::RefPtr<Cairo::Context> cr = window->create_cairo_context();
      
      cr->set_source_rgb(1, 1, 1);
      cr->set_line_width(1);
      
      unsigned int screen_min_x, screen_min_y, screen_max_x, screen_max_y;

      coord_real_to_screen(line_x_start, line_y_start,
			   &screen_min_x, &screen_min_y);

      coord_real_to_screen(line_x_end, line_y_end,
			   &screen_max_x, &screen_max_y);

      cr->move_to(screen_min_x, screen_min_y);
      cr->line_to(screen_max_x, screen_max_y);
      
      cr->stroke();
    }
  }
}


void ImageWin::draw_object_info() {

  if(object_selection_active) {
    //std::cout << "object info" << std::endl;
  }
}

bool ImageWin::on_scroll_event(GdkEventScroll* ev) {

  unsigned int x, y;
  coord_screen_to_real(ev->x, ev->y, &x, &y);

  if(ev->direction == GDK_SCROLL_UP) {
    signal_mouse_scroll_up_(x, y);
  }
  else if(ev->direction == GDK_SCROLL_DOWN) {
    signal_mouse_scroll_down_(x, y);
  } 
  return true;
}

void ImageWin::zoom_in() {
  
}

void ImageWin::zoom_out() {
}


bool ImageWin::on_drag_motion(const Glib::RefPtr<Gdk::DragContext> &context, int x, int y, guint time) {
  return true;
}


bool ImageWin::on_key_press_event(GdkEventKey * event) {
  if(event->keyval == GDK_Escape) {
    puts("escape");
    in_line_mode = false;
    draw_wire();
    
  }
  else {
    puts("key pressed");
  }
  //return true;
  return false;
}

bool ImageWin::on_button_press_event(GdkEventButton * event) {

  if(event->button == 1) {
    last_click_on_x = (unsigned int)event->x;
    last_click_on_y = (unsigned int)event->y;
    mouse_button_pressed = true;
    drag_mode = false;
    in_selection_mode = false;
    in_line_mode = false;
  }
  return false;
}

#define CHECK_AND_SWAP(a,b) if(a > b) {unsigned int tmp = b; b = a; a = tmp;}

bool ImageWin::on_button_release_event(GdkEventButton * event) {

  if(tool == TOOL_SELECT) {
    unsigned int x, y;
    unsigned int event_x, event_y;

    if(event->x < 0) event_x = 0;
    else if(event->x > curr_width) event_x = curr_width - 1;
    else event_x = event->x;
    if(event->y < 0) event_y = 0;
    else if(event->y > curr_height) event_y = curr_height - 1;
    else event_y = event->y;

    coord_screen_to_real(event_x, event_y, &x, &y);
    
    if(selection_active() == true) {

      if(drag_mode == false) reset_selection();
      else {
	selection_x_end = x;
	selection_y_end = y;
	in_selection_mode = false;

	CHECK_AND_SWAP(selection_x_start, selection_x_end);
	CHECK_AND_SWAP(selection_y_start, selection_y_end);
	signal_selection_activated_();
      }
    }
    else {
      // XXX eventuell ins hauptfenster verlagern
      if(object_selection_x == x && object_selection_y == y) {
	object_selection_x = 0;
	object_selection_y = 0;
	object_selection_active = false;
      }
      else {
	object_selection_x = x;
	object_selection_y = y;
	object_selection_active = true;
      }
    }
    update_screen();
  }
  else if(tool == TOOL_WIRE) {
    signal_wire_tool_released_();
    reset_wire();
    update_screen();
  }
  else if(tool == TOOL_MOVE) {
    int shift_real_x = (double)(last_click_on_x - event->x) * get_scaling_x();
    int shift_real_y = (double)(last_click_on_y - event->y) * get_scaling_y();
    if(shift_real_x < 0 && (unsigned int)abs(shift_real_x) > min_x) { // cast to avoid compiler warning
      max_x = curr_width * get_scaling_x();
      min_x = 0;
    }
    else {
      min_x += shift_real_x;
      max_x += shift_real_x;
    }
    if(shift_real_y < 0 && (unsigned int)abs(shift_real_y) > min_y) { // cast to avoid compiler warning
      max_y = curr_height * get_scaling_y();
      min_y = 0;
    }
    else {
      min_y += shift_real_y;
      max_y += shift_real_y;
    }
    signal_adjust_scrollbars_();
    update_screen();
  }

  mouse_button_pressed = false;
  drag_mode = false;
  return false;
}

void ImageWin::set_shift_key_state(bool state) {
  shift_key_pressed = state;
}

#define DELTA(a, b) (a > b ? a - b : b -a)
#define MIN_DELTA_FOR_DRAG_MODE 8

bool ImageWin::on_motion_notify_event(GdkEventMotion * event) {

  if(mouse_button_pressed == true && (
				      DELTA(last_click_on_x, (unsigned int)event->x) > MIN_DELTA_FOR_DRAG_MODE ||
				      DELTA(last_click_on_y, (unsigned int)event->y) > MIN_DELTA_FOR_DRAG_MODE )) {
    drag_mode = true;
  }
  else {

    drag_mode = false;
  }

  if(drag_mode) {
    unsigned int x, y;
    unsigned int event_x, event_y;

    if(event->x < 0) event_x = 0;
    else if(event->x > curr_width) event_x = curr_width - 1;
    else event_x = event->x;
    if(event->y < 0) event_y = 0;
    else if(event->y > curr_height) event_y = curr_height - 1;
    else event_y = event->y;


    if(tool == TOOL_SELECT) {
      if(selection_active() && !in_selection_mode) {
	reset_selection();
      }
    
      if(selection_x_start == 0 && selection_y_start == 0) {
	coord_screen_to_real(last_click_on_x, last_click_on_y, &x, &y);
	selection_x_start = x;
	selection_y_start = y;
	in_selection_mode = true;
      }
      if(in_selection_mode ) {
	coord_screen_to_real(event_x, event_y, &x, &y);
	selection_x_end = x;
	selection_y_end = y;
	draw_selection_box(); 
      }
    }
    else if(tool == TOOL_WIRE) {
      if(line_x_start == 0 && line_y_start == 0) {      
	coord_screen_to_real(last_click_on_x, last_click_on_y, &x, &y);
	line_x_start = x;
	line_y_start = y;
	in_line_mode = true;
      }
      if(in_line_mode) {
	coord_screen_to_real(event_x, event_y, &x, &y);
	line_x_end = x;
	line_y_end = y;

	if(shift_key_pressed) {
	  unsigned int dx = MAX(line_x_end, line_x_start) - MIN(line_x_end, line_x_start);
	  unsigned int dy = MAX(line_y_end, line_y_start) - MIN(line_y_end, line_y_start);
	  if(dx > dy) {
	    line_y_end = line_y_start;
	  }
	  else {
	    line_x_end = line_x_start;
	  }
	}
	draw_wire(); 
      }
    }
  }
  return true;
}

bool ImageWin::on_drawarea_scroll(GdkEventScroll * event) {
  return true;
}


void ImageWin::set_tool(TOOL tool) {
  this->tool = tool;
}


//void ImageWin::on_popup_menu_lock() {
//  puts("foo");
//}


void ImageWin::toggle_render_info_layer(int slot_pos) {
  if(renderer) {
    renderer_toggle_render_func(renderer, slot_pos);
  }
}

void ImageWin::set_render_info_layer_state(int slot_pos, bool state) {
  if(renderer) {
    if(get_renderer_func_enabled(slot_pos) != state)
      renderer_toggle_render_func(renderer, slot_pos);
  }
}

void ImageWin::set_renderer_info_layer_state(const std::vector<bool> & new_states) {
  for(int i = 0; i < new_states.size(); i++)
    set_render_info_layer_state(i, new_states[i]);
}

const std::vector<Glib::ustring> ImageWin::get_renderer_func_names() {
  int s = renderer_get_num_render_func(renderer);
  std::vector<Glib::ustring> L(s);
  
  debug(TM, "number of render funcs: %d", s);

  for(int i = 0; i < s; i++) {
    L[i] = Glib::ustring(renderer_get_name_render_func(renderer, i));
    debug(TM, "render func: %s", L[i].c_str());

  }
  return L;
}

const std::vector<bool> ImageWin::get_renderer_func_states() {
  int s = renderer_get_num_render_func(renderer);
  std::vector<bool> L(s);
  
  for(int i = 0; i < s; i++) {
    L[i] = get_renderer_func_enabled(i);
  }
  return L;
}

bool ImageWin::get_renderer_func_enabled(int slot_pos) {
  int enabled = renderer_render_func_enabled(renderer, slot_pos);
  return enabled  == 1 ? true : false;
}


/*
grid_t * ImageWin::get_grid() {
  return render_params.grid;
}
*/

void ImageWin::set_grid(RegularGrid_shptr regular_horizontal_grid, 
			RegularGrid_shptr regular_vertical_grid,
			IrregularGrid_shptr irregular_horizontal_grid,
			IrregularGrid_shptr irregular_vertical_grid) {

  render_params.regular_horizontal_grid = regular_horizontal_grid;
  render_params.regular_vertical_grid = regular_vertical_grid;
  render_params.irregular_horizontal_grid = irregular_horizontal_grid;
  render_params.irregular_vertical_grid = irregular_vertical_grid;
}


render_params_t * ImageWin::get_render_params() {
  return &render_params;
}

unsigned int ImageWin::get_selection_min_x() {
  return selection_x_start;
}

unsigned int ImageWin::get_selection_min_y() {
  return selection_y_start;
}

unsigned int ImageWin::get_selection_max_x() {
  return selection_x_end;
}

unsigned int ImageWin::get_selection_max_y() {
  return selection_y_end;
}

unsigned int ImageWin::get_wire_min_x() {
  return line_x_start;
}

unsigned int ImageWin::get_wire_min_y() {
  return line_y_start;
}

unsigned int ImageWin::get_wire_max_x() {
  return line_x_end;
}

unsigned int ImageWin::get_wire_max_y() {
  return line_y_end;
}

sigc::signal<void>& ImageWin::signal_wire_tool_released() {
  return signal_wire_tool_released_;
}

sigc::signal<void>& ImageWin::signal_selection_activated() {
  return signal_selection_activated_;
}

sigc::signal<void>& ImageWin::signal_selection_revoked() {
  return signal_selection_revoked_;
}

sigc::signal<void>& ImageWin::signal_adjust_scrollbars() {
  return signal_adjust_scrollbars_;
}

sigc::signal<void, unsigned int, unsigned int>& ImageWin::signal_mouse_scroll_down() {
  return signal_mouse_scroll_down_;
}

sigc::signal<void, unsigned int, unsigned int>& ImageWin::signal_mouse_scroll_up() {
  return signal_mouse_scroll_up_;
}
