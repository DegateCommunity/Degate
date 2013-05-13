/*

This file is part of the IC reverse engineering tool degate.

Copyright 2008, 2009, 2010 by Martin Schobert
Copyright 2012 Robert Nitsch

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
#include "RenderWindow.h"
#include "Editor.h"
#include "DegateRenderer.h"
#include "InProgressWin.h"
#include "GridConfigWin.h"
#include "ObjectMatchingWin.h"
#include "ConnectionInspectorWin.h"
#include "RCViolationsWin.h"
#include "ModuleWin.h"
#include "AnnotationListWin.h"
#include <HlObjectSet.h>
#include <ObjectSet.h>
#include "MenuManager.h"
#include "LayerConfigWin.h"
#include <EMarker.h>
#include <degate.h>
#include <AutoNameGates.h>
#include <BoundingBox.h>

#include <set>
#include <utility>
#include <vector>

class MainWin : public Gtk::Window  {

  friend class MenuManager;


 public:
  MainWin();
  virtual ~MainWin();
  void open_project(Glib::ustring project_dir);
  void set_project_to_open(char * project_dir);

  //@{
  degate::ProjectSnapshot_shptr create_snapshot(const std::string &title);
  degate::ProjectSnapshot_shptr create_automatic_snapshot();
  std::vector<degate::ProjectSnapshot_shptr> get_snapshots();
  void clear_snapshots();
  void remove_snapshot(degate::ProjectSnapshot_shptr &ss);
  void revert_to_snapshot(degate::ProjectSnapshot_shptr &ss);
  //@}

 private:
  void open_popup_menu(GdkEventButton * event);
  void on_popup_menu_place_emarker();
  void on_popup_menu_set_name();
  void on_popup_menu_set_description();
  void on_popup_menu_set_port();
  void on_popup_menu_add_horizontal_grid_line();
  void on_popup_menu_add_vertical_grid_line();


 protected:
  char * project_to_open;


  void zoom(unsigned int center_x, unsigned int center_y, double zoom_factor);
  void center_view(unsigned int center_x, unsigned int center_y, unsigned int layer);
  void set_layer(degate::Layer_shptr layer);
  void set_layer(unsigned int layer);


  //Signal handlers:

  virtual void on_menu_project_new();
  virtual void on_menu_project_open();
  virtual void on_menu_project_close();
  virtual void on_menu_project_quit();
  virtual void on_menu_project_save();
  virtual void on_menu_project_settings();
  virtual void on_menu_project_export_archive();

  virtual void on_menu_project_recent_projects();
  virtual void on_menu_project_create_subproject();
  virtual void on_menu_project_open_parent();
  virtual void on_menu_project_push_changes();
  virtual void on_menu_project_pull_changes();

  virtual void on_menu_snapshot_create();
  virtual void on_menu_snapshot_view();
  
  virtual void on_menu_view_zoom_in();
  virtual void on_menu_view_zoom_out();
  virtual void on_menu_view_next_layer();
  virtual void on_menu_view_prev_layer();
  virtual void on_menu_view_grid_config();
  virtual void on_menu_view_toggle_all_info_layers();
  virtual void on_menu_view_fullscreen();


  // Layer menu
  virtual void on_menu_layer_import_background();
  virtual void on_menu_layer_configuration();
  virtual void on_menu_layer_clear_background_image();
  virtual void on_menu_layer_export_background_image();

  // Logic menu
  virtual void on_menu_logic_interconnect();
  virtual void on_menu_logic_autointerconnect_interlayer();
  virtual void on_menu_logic_isolate();
  virtual void on_menu_logic_remove_entire_net();
  virtual void on_menu_logic_autointerconnect();
  virtual void on_menu_logic_clear_logic_model();
  virtual void remove_objects();
  virtual void on_menu_logic_connection_inspector();
  virtual void on_menu_logic_rc();
  virtual void on_menu_logic_auto_name_gates(degate::AutoNameGates::ORIENTATION orientation);
  virtual void on_menu_logic_create_annotation();
  virtual void on_menu_logic_show_annotations();
  virtual void on_menu_logic_show_modules();
  virtual void on_menu_move_gate_into_module();

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
  virtual void on_menu_tools_via(degate::Via::DIRECTION dir);

  // Help menu
  virtual void on_menu_help_about();

  void object_clicked(unsigned int real_x, unsigned int real_y);
  void selection_tool_clicked(unsigned int real_x, unsigned int real_y, unsigned int button);

  void selection_tool_double_clicked(unsigned int real_x, unsigned int real_y,
				     unsigned int button);


  void via_up_tool_clicked(unsigned int real_x, unsigned int real_y, unsigned int button);
  void via_down_tool_clicked(unsigned int real_x, unsigned int real_y, unsigned int button);
  void via_tool_clicked(unsigned int real_x, unsigned int real_y, degate::Via::DIRECTION dir);

  virtual void on_wire_added(unsigned int from_x, unsigned int from_y,
			     unsigned int to_x, unsigned int to_y);
  virtual void on_area_selection_activated(degate::BoundingBox const& bbox);
  virtual void on_area_selection_resized(degate::BoundingBox const& bbox);
  virtual void on_area_selection_revoked();

  virtual void goto_last_emarker();
  virtual void goto_object(degate::PlacedLogicModelObject_shptr obj_ptr);

  bool on_key_press_event_received(GdkEventKey * event);
  bool on_key_release_event_received(GdkEventKey * event);

  //virtual bool on_expose_event(GdkEventExpose * event);
  bool on_timer();


  //Child widgets:

  Gtk::VBox m_Box;



  Gtk::Statusbar m_statusbar;

  GfxEditor<DegateRenderer> editor;
  RenderWindow<GfxEditor<DegateRenderer> > render_window;


  std::shared_ptr<InProgressWin> ipWin;
  std::shared_ptr<ConnectionInspectorWin> ciWin;
  std::shared_ptr<RCViolationsWin> rcWin;
  std::shared_ptr<ModuleWin> modWin;
  std::shared_ptr<AnnotationListWin> alWin;
  std::shared_ptr<GridConfigWin> gcWin;
  std::shared_ptr<LayerConfigWin> lcWin;

  Gtk::HBox m_displayBox;

  std::shared_ptr<MenuManager> menu_manager;

  degate::Project_shptr main_project;

 private:

  std::vector<degate::ProjectSnapshot_shptr> snapshots;
  
  bool shift_key_pressed;
  bool control_key_pressed;

  Glib::Thread * thread;

  degate::ObjectSet selected_objects;
  degate::HlObjectSet highlighted_objects;

  unsigned int last_click_on_real_x, last_click_on_real_y;
  degate::EMarker_shptr last_emarker;

  std::string thread_error_msg;

  bool is_fullscreen;

 private:

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

  void on_project_load_finished();
  void on_background_import_finished();
  void on_algorithm_finished(int slot_pos);
  void on_export_finished(bool success);

  Glib::Dispatcher signal_project_open_finished_;
  Glib::Dispatcher signal_bg_import_finished_;
  std::shared_ptr<Glib::Dispatcher> signal_algorithm_finished_;
  sigc::signal<void, bool> signal_export_finished_;

  void update_gui_for_loaded_project(bool reverted);

  void initialize_menu();

  void initialize_menu_render_funcs();
  void initialize_menu_algorithm_funcs();
  void set_image_for_toolbar_widget(Glib::ustring toolbar_widget_path, Glib::ustring file_name);

  void on_algorithms_func_clicked(int pos);

  void on_view_info_layer_toggled(int slot_pos);
  void on_grid_config_changed();



  void error_dialog(const char * const title, const char * const message);
  void warning_dialog(const char * const title, const char * const message);
  bool yes_no_dialog(const char * const title, const char * const message);


  void update_gui_on_selection_change();
  void clear_selection();

  void project_changed();
};

#endif
