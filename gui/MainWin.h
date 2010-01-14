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

#ifndef __MAINWIN_H__
#define __MAINWIN_H__

#include <gtkmm.h>
#include "ImageWin.h"
#include "InProgressWin.h"
#include "GridConfigWin.h"
#include "ObjectMatchingWin.h"
#include "ConnectionInspectorWin.h"
#include "ModuleWin.h"
#include "AnnotationListWin.h"
#include "HlObjectSet.h"
#include "MenuManager.h"
#include "LayerConfigWin.h"

#include <degate.h>
#include <AutoNameGates.h>
#include <BoundingBox.h>

#include <set>
#include <utility>

class MainWin : public Gtk::Window  {

  friend class MenuManager;


 public:
  MainWin();
  virtual ~MainWin();
  void open_project(Glib::ustring project_dir);
  void set_project_to_open(char * project_dir);


 private:
  void open_popup_menu(GdkEventButton * event);
  void on_popup_menu_set_name();
  void on_popup_menu_set_port();
  void on_popup_menu_add_horizontal_grid_line();
  void on_popup_menu_add_vertical_grid_line();


 protected:
  char * project_to_open;


  void zoom(unsigned int center_x, unsigned int center_y, double zoom_factor);
  void center_view(unsigned int center_x, unsigned int center_y, unsigned int layer);
  void set_layer(degate::Layer_shptr layer);
  void set_layer(unsigned int layer);

  bool selected_objects_are_interconnectable();
  bool selected_objects_are_removable();


  //Signal handlers:
  virtual void on_v_adjustment_changed();
  virtual void on_h_adjustment_changed();
  virtual void adjust_scrollbars();
  virtual bool on_drag_motion(const Glib::RefPtr<Gdk::DragContext> &context, int x, int y, guint time);

  virtual void on_menu_project_new();
  virtual void on_menu_project_open();
  virtual void on_menu_project_close();
  virtual void on_menu_project_quit();
  virtual void on_menu_project_save();
  virtual void on_menu_project_settings();
  virtual void on_menu_project_export_archive();
  virtual void on_menu_project_export_view();
  virtual void on_menu_project_export_layer();
  virtual void on_menu_project_recent_projects();
  virtual void on_menu_project_create_subproject();
  virtual void on_menu_project_open_parent();
  
  virtual void on_menu_view_zoom_in();
  virtual void on_menu_view_zoom_out();
  virtual void on_menu_view_next_layer();
  virtual void on_menu_view_prev_layer();
  virtual void on_menu_view_grid_config();
  virtual void on_menu_view_toggle_all_info_layers();


  // Layer menu
  virtual void on_menu_layer_import_background();
  virtual void on_menu_layer_configuration();
  virtual void on_menu_layer_clear_background_image();

  // Logic menu
  virtual void on_menu_logic_interconnect();
  virtual void on_menu_logic_isolate();
  virtual void on_menu_logic_clear_logic_model();
  virtual void remove_objects();
  virtual void on_menu_logic_connection_inspector();
  virtual void on_menu_logic_auto_name_gates(degate::AutoNameGates::ORIENTATION orientation);
  virtual void on_menu_logic_create_annotation();
  virtual void on_menu_logic_show_annotations();
  virtual void on_menu_logic_show_modules();

  // Gate menu
  virtual void on_menu_gate_create_by_selection();
  virtual void on_menu_gate_list();
  virtual void on_menu_gate_set();
  virtual void on_menu_gate_orientation();
  virtual void on_menu_gate_set_as_master();
  virtual void on_menu_gate_remove_gate_by_type();
  virtual void on_menu_gate_port_colors();
  virtual void on_menu_goto_gate_by_name();
  virtual void on_menu_goto_gate_by_id();

  // Tools menu
  virtual void on_menu_tools_select();
  virtual void on_menu_tools_move();
  virtual void on_menu_tools_wire();
  virtual void on_menu_tools_via_up();
  virtual void on_menu_tools_via_down();

  // Help menu
  virtual void on_menu_help_about();

  virtual bool on_imgwin_clicked(GdkEventButton * event);
  void object_clicked(unsigned int real_x, unsigned int real_y);
  void object_double_clicked(unsigned int real_x, unsigned int real_y);
  
  virtual void on_wire_tool_release();
  virtual void on_selection_activated(); // should be renamed to area selection
  virtual void on_selection_revoked();
  virtual void on_mouse_scroll_up(unsigned int center_x, unsigned int center_y);
  virtual void on_mouse_scroll_down(unsigned int center_x, unsigned int center_y);

  virtual void goto_object(degate::PlacedLogicModelObject_shptr obj_ptr);

  bool on_key_press_event_received(GdkEventKey * event);
  bool on_key_release_event_received(GdkEventKey * event);

  //virtual bool on_expose_event(GdkEventExpose * event);
  bool on_idle();
  

  //Child widgets:

  Gtk::VBox m_Box;



  Gtk::Statusbar m_statusbar;

  ImageWin imgWin;
  std::tr1::shared_ptr<InProgressWin> ipWin;
  std::tr1::shared_ptr<ConnectionInspectorWin> ciWin;
  std::tr1::shared_ptr<ModuleWin> modWin;
  std::tr1::shared_ptr<AnnotationListWin> alWin;
  std::tr1::shared_ptr<GridConfigWin> gcWin;
  std::tr1::shared_ptr<LayerConfigWin> lcWin;

  Gtk::HBox m_displayBox;
  Gtk::Adjustment m_VAdjustment;
  Gtk::Adjustment m_HAdjustment;
  Gtk::VScrollbar m_VScrollbar;
  Gtk::HScrollbar m_HScrollbar;
  
  std::tr1::shared_ptr<MenuManager> menu_manager;

  degate::Project_shptr main_project;

 private:


  bool shift_key_pressed;
  bool control_key_pressed;

  Glib::Thread * thread;
  TOOL tool;
  std::set<degate::PlacedLogicModelObject_shptr> selected_objects;
  HlObjectSet highlighted_objects;

  unsigned int last_click_on_real_x, last_click_on_real_y;


  void create_new_project(std::string const& project_dir);

  void update_title();
  void add_to_recent_menu();

  void set_menu_item_sensitivity(const Glib::ustring& widget_path, bool state);
  void set_toolbar_item_sensitivity(const Glib::ustring& widget_path, bool state);
  void set_widget_sensitivity(bool state);

  void project_open_thread(Glib::ustring project_dir);
  void background_import_thread(Glib::ustring bg_filename);

  void algorithm_calc_thread(int slot_pos);

  void project_export_thread(std::string project_dir, std::string dst_file);
  void auto_name_gates_thread(degate::AutoNameGates::ORIENTATION orientation);

  void on_project_load_finished();
  void on_background_import_finished();
  void on_algorithm_finished(int slot_pos);
  void on_export_finished(bool success);
  void on_auto_name_finished(degate::ret_t ret);
  
  Glib::Dispatcher signal_project_open_finished_;
  Glib::Dispatcher signal_bg_import_finished_;
  std::tr1::shared_ptr<Glib::Dispatcher> signal_algorithm_finished_;
  sigc::signal<void, bool> signal_export_finished_;
  sigc::signal<void, degate::ret_t> signal_auto_name_finished_;

  void update_gui_for_loaded_project();

  void initialize_menu();
  void initialize_image_window();
  void initialize_menu_render_funcs();
  void initialize_menu_algorithm_funcs();
  void set_image_for_toolbar_widget(Glib::ustring toolbar_widget_path, Glib::ustring file_name);

  void on_algorithms_func_clicked(int pos);

  void on_view_info_layer_toggled(int slot_pos);
  void on_grid_config_changed();



  void error_dialog(const char * const title, const char * const message);
  void warning_dialog(const char * const title, const char * const message);


  void update_gui_on_selection_change();
  void clear_selection();
  
};

#endif
