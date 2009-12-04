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

#include <gdkmm/window.h>
#include <gtkmm/stock.h>
#include <libglademm.h>

#include <assert.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <iostream>
#include <unistd.h>

#include "MainWin.h"
#include "ObjectMatchingWin.h"
#include "NewProjectWin.h"
#include "GridConfigWin.h"
#include "GateListWin.h"
#include "PortColorsWin.h"
#include "GateConfigWin.h"
#include "GateSelectWin.h"
#include "PortSelectWin.h"
#include "InProgressWin.h"
#include "SplashWin.h"
#include "MenuManager.h"
#include "SetOrientationWin.h"
#include "ProjectSettingsWin.h"
#include "ConnectionInspectorWin.h"
#include "GenericTextInputWin.h"
#include "RecognitionManager.h"
#include "LayerConfigWin.h"
#include "gui_globals.h"
#include <AppHelper.h>

#include <degate.h>
#include <ProjectExporter.h>
#include <ProjectImporter.h>
#include <LogicModelHelper.h>

#define ZOOM_STEP 1.3
#define ZOOM_STEP_MOUSE_SCROLL 2.0
#define ZOOM_STEP_MOUSE_SCROLL_AND_SHIFT 1.1

using namespace degate;

MainWin::MainWin() : 
  m_VAdjustment(0.0, 0.0, 101.0, 0.1, 1.0, 1.0), // value, lower, upper, step_increment, page_increment, page_size
  m_HAdjustment(0.0, 0.0, 101.0, 0.1, 1.0, 1.0),
  m_VScrollbar(m_VAdjustment),
  m_HScrollbar(m_HAdjustment) {

  // setup window
  set_default_size(1024, 700);
  char path[PATH_MAX];
  snprintf(path, PATH_MAX, "%s/icons/degate_logo.png", getenv("DEGATE_HOME"));
  set_icon_from_file(path);

  add(m_Box);


  // setup menu
  menu_manager = new MenuManager(this);
  Gtk::Widget* menubar = menu_manager->get_menubar();
  Gtk::Widget* toolbar = menu_manager->get_toolbar();
  assert(menubar != NULL);
  assert(toolbar != NULL);
  if(menubar != NULL && toolbar != NULL) {
    m_Box.pack_start(*menubar, Gtk::PACK_SHRINK);
    m_Box.pack_start(*toolbar, Gtk::PACK_SHRINK);
  }

  const std::vector<Glib::ustring> render_func_names = imgWin.get_renderer_func_names();
  const std::vector<bool> render_func_states = imgWin.get_renderer_func_states();

  menu_manager->initialize_menu_render_funcs(render_func_names, render_func_states);
  

  initialize_image_window();

  // setup statusbar
  m_statusbar.push("");
  m_Box.pack_start(m_statusbar, Gtk::PACK_SHRINK);


  show_all_children();

  update_title();
  control_key_pressed = false;
  shift_key_pressed = false;
  imgWin.set_shift_key_state(false);
  
  imgWin.grab_focus();

  project_to_open = NULL;
  Glib::signal_idle().connect( sigc::mem_fun(*this, &MainWin::on_idle));


  if(getuid() == 0) {
    warning_dialog("Security warning", 
		   "You started degate as superuser. I don't cause harm to you. "
		   "But you should think about it. You should know: \"All your base are belong to us\". "
		   "I will not drop privileges and I hope you know, what you do.");
  }


  signal_key_press_event().connect(sigc::mem_fun(*this,&MainWin::on_key_press_event_received), false);
  signal_key_release_event().connect(sigc::mem_fun(*this,&MainWin::on_key_release_event_received), false);
  signal_hide().connect(sigc::mem_fun(*this, &MainWin::on_menu_project_close), false);

}

MainWin::~MainWin() {
}


void MainWin::update_title() {
  if(main_project == NULL) {
    set_title("degate");
  }
  else {
    char _title[1000];


    LogicModel_shptr lmodel = main_project->get_logic_model();

    snprintf(_title, sizeof(_title), "degate -- [%s%s%s%s] [%d/%d]", 
	     main_project->get_name().length() > 0 ? main_project->get_name().c_str() : "",
	     main_project->get_name().length() > 0 ? ": " : "",
	     main_project->get_project_directory().c_str(),
	     main_project->is_changed() ? "*" : "",
	     lmodel->get_current_layer()->get_layer_pos(), lmodel->get_num_layers() - 1);
    set_title(_title);
  }

}


bool MainWin::on_idle() {

  if(project_to_open != NULL) {
    open_project(project_to_open);
    project_to_open = NULL;
  }

  if(main_project != NULL) {
    if(autosave_project(main_project))
      m_statusbar.push("Autosaving project data ... done.");
  }

  return true;
}

void MainWin::on_view_info_layer_toggled(int slot_pos) {
  if(menu_manager->toggle_info_layer(slot_pos)) {
    imgWin.toggle_render_info_layer(slot_pos);
    imgWin.update_screen();
  }
}

void MainWin::on_menu_view_toggle_all_info_layers() {
 
  const std::vector<bool> new_states = menu_manager->toggle_info_layer_visibility();
  imgWin.set_renderer_info_layer_state(new_states);
  imgWin.update_screen();
}




void MainWin::initialize_image_window() {
  // @todo remove
  m_VScrollbar.set_update_policy(Gtk::UPDATE_CONTINUOUS);
  m_HScrollbar.set_update_policy(Gtk::UPDATE_CONTINUOUS);
  
  // @todo remove
  m_VAdjustment.signal_value_changed().connect(sigc::mem_fun(*this, &MainWin::on_v_adjustment_changed));
  m_HAdjustment.signal_value_changed().connect(sigc::mem_fun(*this, &MainWin::on_h_adjustment_changed));

  imgWin.signal_drag_motion().connect(sigc::mem_fun(*this, &MainWin::on_drag_motion));
  imgWin.signal_button_press_event().connect(sigc::mem_fun(*this, &MainWin::on_imgwin_clicked));
  imgWin.signal_wire_tool_released().connect(sigc::mem_fun(*this, &MainWin::on_wire_tool_release));
  imgWin.signal_selection_activated().connect(sigc::mem_fun(*this, &MainWin::on_selection_activated));
  imgWin.signal_selection_revoked().connect(sigc::mem_fun(*this, &MainWin::on_selection_revoked));
  imgWin.signal_mouse_scroll_up().connect(sigc::mem_fun(*this, &MainWin::on_mouse_scroll_up));
  imgWin.signal_mouse_scroll_down().connect(sigc::mem_fun(*this, &MainWin::on_mouse_scroll_down));

  // @todo remove
  imgWin.signal_adjust_scrollbars().connect(sigc::mem_fun(*this, &MainWin::adjust_scrollbars));

  m_displayBox.pack_start(imgWin, Gtk::PACK_EXPAND_WIDGET);
  m_displayBox.pack_start(m_VScrollbar, Gtk::PACK_SHRINK);

  m_Box.pack_start(m_displayBox, Gtk::PACK_EXPAND_WIDGET);

  m_Box.pack_start(m_HScrollbar, Gtk::PACK_SHRINK);

  tool = TOOL_SELECT;
  imgWin.set_tool(tool);
}

bool MainWin::on_drag_motion(const Glib::RefPtr<Gdk::DragContext> &context, int x, int y, guint time) {
  //debug(TM, "drag");
  return true;
}

void MainWin::add_to_recent_menu() {

  Glib::ustring str(main_project->get_project_directory());
  str += "/project.xml";
  
  Gtk::RecentManager::Data data;

  data.app_exec = "degate %u";
  data.app_name  ="degate";

  data.groups.push_back("degate");
  data.description ="degate project";

  data.display_name = str;
  data.mime_type = degate_mime_type;

  Glib::RefPtr<Gtk::RecentManager> recent_manager = Gtk::RecentManager::get_default();
  recent_manager->add_item ("file://" + str, data);
}

void MainWin::on_menu_project_recent_projects() {
  debug(TM, "on_menu_project_recent_projects()");
  /*
  Glib::RefPtr<Gtk::RecentManager> m_refRecentManager;


  Gtk::RecentChooserDialog dialog(*this, "Recent Files", m_refRecentManager);
  dialog.add_button("Select File", Gtk::RESPONSE_OK);
  dialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);

  const int response = dialog.run();
  dialog.hide();
  if(response == Gtk::RESPONSE_OK)
    {
      std::cout << "URI selected = " << dialog.get_current_uri() << std::endl;
    }
  */
}

void MainWin::on_menu_project_quit() {
  on_menu_project_close();
  hide(); //Closes the main window to stop the Gtk::Main::run().
}

void MainWin::on_menu_project_new() {

  if(main_project) on_menu_project_close();

  NewProjectWin npw_dialog(this);
  Gtk::Main::run(npw_dialog);
  unsigned int width = npw_dialog.get_width();
  unsigned int height = npw_dialog.get_height();
  unsigned int layers = npw_dialog.get_layers();

  if(width == 0 || height == 0 || layers == 0) {
    Gtk::MessageDialog dialog(*this, "Invalid value", true, Gtk::MESSAGE_ERROR);
    dialog.set_title("The values you entered are invalid.");
    dialog.run();
  }
  else {
    Gtk::FileChooserDialog dialog("Please choose a project folder", Gtk::FILE_CHOOSER_ACTION_CREATE_FOLDER);
    dialog.set_transient_for(*this);
    dialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
    dialog.add_button("Select", Gtk::RESPONSE_OK);
    
    int result = dialog.run();
    std::string project_dir = dialog.get_filename();
    dialog.hide();

    // create the project

    switch(result) {
    case(Gtk::RESPONSE_OK): {
      main_project = Project_shptr(new Project(width, height, project_dir, layers));
      
      
      update_gui_for_loaded_project();

      lcWin->run();

      set_layer(get_first_enabled_layer(main_project->get_logic_model()));
      
      on_menu_project_save();
    }
      break;
    case(Gtk::RESPONSE_CANCEL):
      break;
    }

  }

}

void MainWin::on_menu_project_close() {
  if(main_project) {

    if(main_project->is_changed()) {

      Gtk::MessageDialog dialog(*this, "Project data was modified. Should it be saved?", 
				true, Gtk::MESSAGE_QUESTION, Gtk::BUTTONS_YES_NO);
      dialog.set_title("Warning");      
      if(dialog.run() == Gtk::RESPONSE_YES) {
	on_menu_project_save();
      }
    }

    imgWin.disable_renderer();

    clear_selection();

    main_project.reset();
    imgWin.update_screen();

    update_title();
    
    delete ciWin; 
    ciWin = NULL;

    delete gcWin;
    gcWin = NULL;

    delete lcWin;
    lcWin = NULL;

    menu_manager->set_widget_sensitivity(false);
  }
}


void MainWin::on_menu_project_settings() {
  if(main_project) {
    ProjectSettingsWin psWin(this, main_project);
    if(psWin.run()) {
      main_project->set_changed();
    }
  }
}

void MainWin::on_menu_project_export_archive() {
  if(main_project) {
    if(main_project->is_changed()) {
      Gtk::MessageDialog dialog_ask(*this, "Project data was modified. Should it be saved?", 
				    true, Gtk::MESSAGE_QUESTION, Gtk::BUTTONS_YES_NO);
      dialog_ask.set_title("Warning");      
      if(dialog_ask.run() == Gtk::RESPONSE_YES) {
	on_menu_project_save();
      }
      else return;
    }
    Gtk::FileChooserDialog dialog("Export project as archive", Gtk::FILE_CHOOSER_ACTION_SAVE );
    dialog.set_transient_for(*this);

    dialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
    dialog.add_button("Select", Gtk::RESPONSE_OK);

    std::string suffix = get_basename(main_project->get_project_directory());
    dialog.set_current_name(get_date_and_time_as_file_prefix() + suffix + ".zip");
    int result = dialog.run();
    dialog.hide();

    switch(result) {
    case Gtk::RESPONSE_OK:

      ipWin = new InProgressWin(this, "Exporting", "Please wait while exporting project.");
      ipWin->show();

      signal_export_finished_.connect(sigc::mem_fun(*this, &MainWin::on_export_finished));
      thread = Glib::Thread::create(sigc::bind<std::string, std::string>
				    (sigc::mem_fun(*this, &MainWin::project_export_thread), 
				     main_project->get_project_directory(), 
				     dialog.get_filename()), false);


      break;
    case Gtk::RESPONSE_CANCEL:
      break;
    }
    
  }
}

void MainWin::project_export_thread(std::string project_dir, std::string dst_file) {

  pid_t pid = fork();
  if(pid == 0) {
    // child
    if(execlp("zip", "-j", "-r", dst_file.c_str(), project_dir.c_str(), NULL) == -1) {
      debug(TM, "exec failed");
    }
    debug(TM, "sth. failed");
    exit(1);
  }
  else if(pid < 0) {
    // fork failed
    debug(TM, "fork() failed");
    signal_export_finished_(false);
  }
  else {
    // parent
    int exit_code;
    if(waitpid(pid, &exit_code, 0) != pid) {
      debug(TM, "failed");
      signal_export_finished_(false);
    }
    else {
      signal_export_finished_(WEXITSTATUS(exit_code) == 0 ? true : false);
    }
  }
}


void MainWin::on_export_finished(bool success) {
  if(ipWin) {
    ipWin->close();
    delete ipWin;
    ipWin = NULL;
  }
  if(success == false) {
    error_dialog("Error", "Export failed. Maybe you have no zip utility installed?");
  }
}

void MainWin::on_menu_project_save() {
  if(main_project) {
    ProjectExporter exporter;
    exporter.export_all(main_project->get_project_directory(), main_project);
    main_project->set_changed(false);
    update_title();
  }
}

void MainWin::on_menu_project_open() {

  if(main_project) on_menu_project_close();

  Gtk::FileChooserDialog dialog("Please choose a project folder", Gtk::FILE_CHOOSER_ACTION_SELECT_FOLDER);
  dialog.set_transient_for(*this);

  dialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
  dialog.add_button("Select", Gtk::RESPONSE_OK);

  int result = dialog.run();
  Glib::ustring project_dir = dialog.get_filename();
  dialog.hide();

  if(result == Gtk::RESPONSE_OK) open_project(project_dir);
}

void MainWin::set_project_to_open(char * project_dir) {
  project_to_open = project_dir;
}

void MainWin::open_project(Glib::ustring project_dir) {
  if(main_project) on_menu_project_close();

  ipWin = new InProgressWin(this, "Opening Project", "Please wait while opening project.");
  ipWin->show();

  signal_project_open_finished_.connect(sigc::mem_fun(*this, &MainWin::on_project_load_finished));
  thread = Glib::Thread::create(sigc::bind<const Glib::ustring>(sigc::mem_fun(*this, &MainWin::project_open_thread), 
								project_dir), false);
}

// in GUI-thread
void MainWin::on_project_load_finished() {

  //thread->join();
  
  if(ipWin) {
    ipWin->close();
    delete ipWin;
    ipWin = NULL;
  }
  
  if(main_project == NULL) {
    Gtk::MessageDialog err_dialog(*this, "Can't open project", false, Gtk::MESSAGE_ERROR, Gtk::BUTTONS_OK, true);
    err_dialog.run();
  }
  else {
    Layer_shptr layer = main_project->get_logic_model()->get_current_layer();
    assert(layer != NULL);
    menu_manager->set_layer_type_in_menu(layer->get_layer_type());
    update_gui_for_loaded_project();
    set_layer(get_first_enabled_layer(main_project->get_logic_model()));
  }
}

void MainWin::project_open_thread(Glib::ustring project_dir) {

  ProjectImporter importer;
  Project_shptr prj(importer.import_all(project_dir));

  main_project = prj;

  signal_project_open_finished_();
}

void MainWin::on_menu_project_export_view() {
  if(main_project) {
    Gtk::FileChooserDialog dialog("Please choose a file name", Gtk::FILE_CHOOSER_ACTION_SAVE);
    dialog.set_transient_for(*this);
    dialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
    dialog.add_button("Export", Gtk::RESPONSE_OK);
    int result = dialog.run();
    Glib::ustring filename = dialog.get_filename();
    dialog.hide();

    if(result == Gtk::RESPONSE_OK)
      if(!imgWin.render_to_file(filename.c_str(), imgWin.get_min_x(),
				imgWin.get_min_y(),
				imgWin.get_max_x(),
				imgWin.get_max_y())) {
	Gtk::MessageDialog err_dialog(*this, "Can't export graphics", false, Gtk::MESSAGE_ERROR, Gtk::BUTTONS_OK, true);
	err_dialog.run();
      }
  }   
}

void MainWin::on_menu_project_export_layer() {
  if(main_project) {
    Gtk::FileChooserDialog dialog("Please choose a file name", Gtk::FILE_CHOOSER_ACTION_SAVE);
    dialog.set_transient_for(*this);
    dialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
    dialog.add_button("Export", Gtk::RESPONSE_OK);
    int result = dialog.run();
    Glib::ustring filename = dialog.get_filename();
    dialog.hide();

    if(result == Gtk::RESPONSE_OK)
      if(!imgWin.render_to_file(filename.c_str(), 0, 0, main_project->get_width(), main_project->get_height())) {
	Gtk::MessageDialog err_dialog(*this, "Can't export graphics", false, Gtk::MESSAGE_ERROR, Gtk::BUTTONS_OK, true);
	err_dialog.run();
      }
  }   
}

void MainWin::update_gui_for_loaded_project() {

  if(main_project) {
    
    imgWin.set_view(0, 0, imgWin.get_width(), imgWin.get_height());
    
    LogicModel_shptr lmodel = main_project->get_logic_model();
    int l_pos = get_first_enabled_layer(main_project->get_logic_model())->get_layer_pos();
    lmodel->set_current_layer(l_pos);
    Layer_shptr layer = lmodel->get_current_layer();

    imgWin.set_render_logic_model(lmodel);
    imgWin.reset_selection();
    imgWin.set_current_layer(l_pos);
    
    imgWin.set_grid(main_project->get_regular_horizontal_grid(),
		    main_project->get_regular_vertical_grid(),
		    main_project->get_irregular_horizontal_grid(),
		    main_project->get_irregular_vertical_grid());
    
    menu_manager->set_widget_sensitivity(true);
    add_to_recent_menu();

    update_title();

    adjust_scrollbars();
    
    ciWin = new ConnectionInspectorWin(this, main_project->get_logic_model());
    ciWin->signal_goto_button_clicked().connect(sigc::mem_fun(*this, &MainWin::goto_object));
    
    gcWin = new GridConfigWin(this, 
			      main_project->get_regular_horizontal_grid(),
			      main_project->get_regular_vertical_grid(),
			      main_project->get_irregular_horizontal_grid(),
			      main_project->get_irregular_vertical_grid() );

    gcWin->signal_changed().connect(sigc::mem_fun(*this, &MainWin::on_grid_config_changed));


    lcWin = new LayerConfigWin(this, main_project->get_logic_model(), 
			       main_project->get_project_directory());
    lcWin->signal_on_background_import_finished().connect(sigc::mem_fun(*this, &MainWin::on_background_import_finished));
    imgWin.update_screen();  
  }
}

void MainWin::set_layer(Layer_shptr layer) {
  set_layer(layer->get_layer_pos());
}

void MainWin::set_layer(unsigned int layer) {
  if(main_project == NULL) return;

  LogicModel_shptr lmodel = main_project->get_logic_model();

  if(lmodel->get_num_layers() == 0) return;

  Layer_shptr layer_ptr = lmodel->get_layer(layer);

  lmodel->set_current_layer(layer_ptr->get_layer_pos());

  imgWin.set_current_layer(layer_ptr->get_layer_pos());
  
  menu_manager->set_layer_type_in_menu(layer_ptr->get_layer_type());

  update_title();
  imgWin.update_screen();
}

void MainWin::goto_object(PlacedLogicModelObject_shptr obj_ptr) {
  assert(obj_ptr != NULL);
  if(main_project != NULL && obj_ptr != NULL) {

    const BoundingBox & bbox = obj_ptr->get_bounding_box();
    Layer_shptr layer = obj_ptr->get_layer();

    highlighted_objects.add(obj_ptr);

    center_view(bbox.get_center_x(), bbox.get_center_y(), layer->get_layer_pos());

    highlighted_objects.remove(obj_ptr);

    
  }
}

void MainWin::on_menu_view_next_layer() {
  if(main_project == NULL) return;
  
  LogicModel_shptr lmodel = main_project->get_logic_model();
  if(lmodel->get_num_layers() == 0) return;
  set_layer(get_next_enabled_layer(lmodel));
}

void MainWin::on_menu_view_prev_layer() {
  if(main_project == NULL) return;

  LogicModel_shptr lmodel = main_project->get_logic_model();
  if(lmodel->get_num_layers() == 0) return;
  set_layer(get_prev_enabled_layer(lmodel));
}

// @todo just dispatch to RenderWindow
void MainWin::on_menu_view_zoom_in() {
  if(main_project == NULL) return;

  unsigned int center_x = imgWin.get_center_x();
  unsigned int center_y = imgWin.get_center_y();

  zoom(imgWin.get_real_width() > main_project->get_width() ? main_project->get_width() /2: center_x, 
       imgWin.get_real_height() > main_project->get_height() ? main_project->get_height() / 2 : center_y,
       1.0/ZOOM_STEP);
  
}

// @todo just dispatch to RenderWindow
void MainWin::on_menu_view_zoom_out() {
  if(main_project == NULL) return;

  unsigned int center_x = imgWin.get_center_x();
  unsigned int center_y = imgWin.get_center_y();

  zoom(imgWin.get_real_width() > main_project->get_width() ? main_project->get_width() /2: center_x, 
       imgWin.get_real_height() > main_project->get_height() ? main_project->get_height() / 2 : center_y,
       ZOOM_STEP);
}


// @todo just dispatch to RenderWindow
void MainWin::zoom(unsigned int center_x, unsigned int center_y, double zoom_factor) {

  if(main_project == NULL) return;

  double delta_x = imgWin.get_max_x() - imgWin.get_min_x();
  double delta_y = imgWin.get_max_y() - imgWin.get_min_y();

  unsigned int max_edge_length = MAX(main_project->get_width(), main_project->get_height());

  if( ((delta_x < max_edge_length || delta_y < max_edge_length) && zoom_factor >= 1) ||
      ((delta_x > 100 || delta_y > 100) && zoom_factor <= 1)  ) {
    

    double min_x = (double)center_x - zoom_factor * (delta_x/2.0);
    double min_y = (double)center_y - zoom_factor * (delta_y/2.0);
    double max_x = (double)center_x + zoom_factor * (delta_x/2.0);
    double max_y = (double)center_y + zoom_factor * (delta_y/2.0);
    if(min_x < 0) { max_x -= min_x; min_x = 0; }
    if(min_y < 0) { max_y -= min_y; min_y = 0; }
    
    imgWin.set_view(min_x, min_y, max_x, max_y);
    adjust_scrollbars();
    imgWin.update_screen();
  }
}

// @todo just dispatch to RenderWindow, preserve the layer param
void MainWin::center_view(unsigned int center_x, unsigned int center_y, unsigned int layer) {

  if(main_project == NULL) return;

  unsigned int width_half = (imgWin.get_max_x() - imgWin.get_min_x()) / 2;
  unsigned int height_half = (imgWin.get_max_y() - imgWin.get_min_y()) / 2;

  unsigned int min_x = center_x > width_half ? center_x - width_half : 0;
  unsigned int min_y = center_y > height_half ? center_y - height_half : 0;
  imgWin.set_view(min_x, min_y, min_x + (width_half << 1), min_y + (height_half << 1));
  adjust_scrollbars();
  
  set_layer(layer);
}

// @todo remove
void MainWin::adjust_scrollbars() {

  m_VAdjustment.set_lower(0);
  m_HAdjustment.set_lower(0);

  m_HAdjustment.set_upper(main_project ? main_project->get_width(): 0);
  m_VAdjustment.set_upper(main_project ? main_project->get_height(): 0);

  //m_VAdjustment.set_page_size(main_project ? main_project->width: 0);
  //m_HAdjustment.set_page_size(main_project ? main_project->height: 0);

  int delta_x = (imgWin.get_max_x() - imgWin.get_min_x());
  int delta_y = (imgWin.get_max_y() - imgWin.get_min_y());

  m_VAdjustment.set_page_size(delta_y);
  m_HAdjustment.set_page_size(delta_x);

  m_VAdjustment.set_step_increment((double)delta_y * 0.1);
  m_HAdjustment.set_step_increment((double)delta_x * 0.1);

  m_VAdjustment.set_page_increment(delta_y);
  m_HAdjustment.set_page_increment(delta_x);

  m_HAdjustment.set_value(main_project ? imgWin.get_min_x() : 0);
  m_VAdjustment.set_value(main_project ? imgWin.get_min_y() : 0);

}

// @todo remove
void MainWin::on_v_adjustment_changed() {
  unsigned int val = (unsigned int) m_VAdjustment.get_value();
  imgWin.set_view(imgWin.get_min_x(), val, 
		  imgWin.get_max_x(), val + imgWin.get_real_height());
  imgWin.update_screen();
}

// @todo remove
void MainWin::on_h_adjustment_changed() {
  unsigned int val = (unsigned int)m_HAdjustment.get_value();
  imgWin.set_view(val, imgWin.get_min_y(),
		  val + imgWin.get_real_width(), imgWin.get_max_y());

  imgWin.update_screen();
}


// here is a list og images for each mouse cursor type:
// http://www.pygtk.org/docs/pygtk/gdk-constants.html

void MainWin::on_menu_tools_select() {
  Glib::RefPtr<Gdk::Window> window = imgWin.get_window();
  window->set_cursor(Gdk::Cursor(Gdk::LEFT_PTR));
  tool = TOOL_SELECT;
  imgWin.set_tool(tool);
}

void MainWin::on_menu_tools_move() {
  Glib::RefPtr<Gdk::Window> window = imgWin.get_window();
  window->set_cursor(Gdk::Cursor(Gdk::FLEUR));
  tool = TOOL_MOVE;
  imgWin.set_tool(tool);
}

void MainWin::on_menu_tools_wire() {
  Glib::RefPtr<Gdk::Window> window = imgWin.get_window();
  window->set_cursor(Gdk::Cursor(Gdk::TCROSS));
  tool = TOOL_WIRE;
  imgWin.set_tool(tool);
}

void MainWin::on_menu_tools_via_up() {
  Glib::RefPtr<Gdk::Window> window = imgWin.get_window();
  window->set_cursor(Gdk::Cursor(Gdk::CROSS));
  tool = TOOL_VIA_UP;
  imgWin.set_tool(tool);
}

void MainWin::on_menu_tools_via_down() {
  Glib::RefPtr<Gdk::Window> window = imgWin.get_window();
  window->set_cursor(Gdk::Cursor(Gdk::CROSS));
  tool = TOOL_VIA_DOWN;
  imgWin.set_tool(tool);
}


void MainWin::on_algorithm_finished(int slot_pos) {


  if(ipWin) {
    ipWin->close();
    delete ipWin;
    ipWin = NULL;
  }

  imgWin.update_screen();

  debug(TM, "Algorithm finished.");

  RecognitionManager * rm = RecognitionManager::get_instance();
  rm->after_dialog(slot_pos);

  delete signal_algorithm_finished_;

  main_project->set_changed();
}


void MainWin::algorithm_calc_thread(int slot_pos) {

  debug(TM, "Calculating ...");
  RecognitionManager * rm = RecognitionManager::get_instance();

  rm->run(slot_pos);

  (*signal_algorithm_finished_)();
}

void MainWin::on_algorithms_func_clicked(int slot_pos) {

  if(main_project == NULL) {
    error_dialog("Error", "You need to open a project first.");
    return;
  }

  debug(TM, "algorithm clicked %d", slot_pos);

  RecognitionManager * rm = RecognitionManager::get_instance();

  BoundingBox bbox(imgWin.get_selection_min_x(),
		   imgWin.get_selection_max_x(),
		   imgWin.get_selection_min_y(),
		   imgWin.get_selection_max_y());

  rm->init(slot_pos, this, bbox, main_project);

  if(rm->before_dialog(slot_pos)) {

    ipWin = new InProgressWin(this, "Calculating", "Please wait while calculating.");
    ipWin->show();
    
    signal_algorithm_finished_ = new Glib::Dispatcher;

    signal_algorithm_finished_->connect(sigc::bind(sigc::mem_fun(*this, 
								 &MainWin::on_algorithm_finished),
						   slot_pos));
    
    thread = Glib::Thread::create(sigc::bind(sigc::mem_fun(*this, &MainWin::algorithm_calc_thread), 
					     slot_pos), false);
  }

}


void MainWin::on_menu_goto_gate_by_name() {
  if(main_project != NULL) {
    GenericTextInputWin input(this, "Goto gate by name", "Gate name", "");
    Glib::ustring str;
    if(input.run(str)) {
    
      Gate_shptr gate = get_gate_by_name(main_project->get_logic_model(), str);
      if(gate == NULL) error_dialog("Error", "There is no gate with that name or the name is not unique.");
      else goto_object(gate);
    }
  }
}

void MainWin::on_menu_goto_gate_by_id() {
  if(main_project != NULL) {
    GenericTextInputWin input(this, "Goto gate by ID", "Gate ID", "");
    Glib::ustring str;
    if(input.run(str)) {
    
      unsigned int id = atol(str.c_str());

      LogicModel_shptr lmodel = main_project->get_logic_model();
      Gate_shptr gate;

      try {
	PlacedLogicModelObject_shptr plo = lmodel->get_object(id);
	gate = std::tr1::dynamic_pointer_cast<Gate>(plo);
      }
      catch(CollectionLookupException const& ex) { }


      if(gate == NULL) error_dialog("Error", "There is no gate with that ID.");
      else goto_object(gate);
	 
    }
  }
}


void MainWin::on_menu_gate_port_colors() {
  if(main_project != NULL) {
    PortColorsWin pcWin(this, 
			main_project->get_logic_model(), 
			main_project->get_port_color_manager());
    pcWin.run();

    imgWin.update_screen();
    main_project->set_changed();
  }
}


void MainWin::on_menu_gate_list() {
  if(main_project != NULL) {
    GateListWin glWin(this, main_project->get_logic_model());
    glWin.run();

    imgWin.update_screen();
    main_project->set_changed();

    apply_colors_to_gate_ports(main_project->get_logic_model(), 
			       main_project->get_port_color_manager());
  }
}


void MainWin::on_menu_gate_orientation() {
  if(main_project && (selected_objects.size() == 1)) {
    std::set<PlacedLogicModelObject_shptr>::const_iterator it = selected_objects.begin();
    if(Gate_shptr gate = std::tr1::dynamic_pointer_cast<Gate>(*it)) {
      
      SetOrientationWin oWin(this, gate->get_orientation());
      Gate::ORIENTATION new_ori = oWin.run();

      if(new_ori != gate->get_orientation()) {
	gate->set_orientation(new_ori);
	main_project->get_logic_model()->update_ports(gate);
      }
      else {
	imgWin.update_screen();
	main_project->set_changed();
      }
    }
    
  }
}

void MainWin::on_menu_gate_remove_gate_by_type() {

  if(main_project != NULL) {

    LogicModel_shptr lmodel = main_project->get_logic_model();

    GateSelectWin gsWin(this, lmodel);
    std::list<degate::GateTemplate_shptr> tmpl_set = gsWin.get_selection(true);

    if(tmpl_set.size() == 0) return;

    Gtk::MessageDialog dialog(*this, "Are you sure you want to remove all gates by that type(s)?", 
			      true, Gtk::MESSAGE_QUESTION, Gtk::BUTTONS_YES_NO);
    dialog.set_title("Warning");
    if(dialog.run() == Gtk::RESPONSE_YES) {
      dialog.hide();
      
      for(std::list<GateTemplate_shptr>::iterator iter = tmpl_set.begin();
	  iter != tmpl_set.end(); ++iter) {

	GateTemplate_shptr gate_template = *iter;

	lmodel->remove_gates_by_template_type(gate_template);

	
	Gtk::MessageDialog dialog2(*this, "Do you want to remove the gate definition, too?",
				  true, Gtk::MESSAGE_QUESTION, Gtk::BUTTONS_YES_NO);
	dialog2.set_title("Warning");
	dialog2.hide();
	if(dialog2.run() == Gtk::RESPONSE_YES)
	  lmodel->remove_gate_template(gate_template);

      }

      
      imgWin.update_screen();
      main_project->set_changed();
    }
    
  }
  
}

void MainWin::on_menu_gate_set_as_master() {
  if(main_project && (selected_objects.size() == 1)) {
    std::set<PlacedLogicModelObject_shptr>::const_iterator it = selected_objects.begin();
    if(Gate_shptr gate = std::tr1::dynamic_pointer_cast<Gate>(*it)) {
    
      GateTemplate_shptr tmpl = gate->get_gate_template();

      if(tmpl == NULL) {
	error_dialog("Error", "The gate should be the master template, but you have not defined of which type. "
		     "Please set a gate type for the gate.");
	return;
      }

      Gate::ORIENTATION orig_orient = gate->get_orientation();
      if(orig_orient == Gate::ORIENTATION_UNDEFINED) {
	error_dialog("Error", "The gate orientation is undefined. Please define it.");
	return;
      }

      // aquire an image an flip it so that the image has normal orientation
      LogicModel_shptr lmodel = main_project->get_logic_model();
      
      grab_template_images(lmodel, 
			   tmpl, gate->get_bounding_box(),
			   gate->get_orientation());

      imgWin.update_screen();
      main_project->set_changed();
    }
  }
}

/**
 * Create a gate for a region.
 * The user selected a region. We will open a gate selection dialog,
 * let the user select a gate template and we create a gate of that type.
 */
void MainWin::on_menu_gate_set() {

  if(main_project == NULL) return;

  LogicModel_shptr lmodel = main_project->get_logic_model();

  if(imgWin.selection_active()) {

    GateSelectWin gsWin(this, main_project->get_logic_model());
    std::list<GateTemplate_shptr> tmpl_list = gsWin.get_selection(false);

    if(tmpl_list.size() != 1) return;

    Layer_shptr layer;

    try{
      layer = get_first_logic_layer(lmodel);
    }
    catch(CollectionLookupException const& ex) {
	error_dialog("Error", "There is no logic layer defined. Please define layer types.");
	return;
    }

    GateTemplate_shptr tmpl = *(tmpl_list.begin());

    if(tmpl->get_width() == 0 && tmpl->get_height() == 0) {
      tmpl->set_width(imgWin.get_selection_max_x() - imgWin.get_selection_min_x());
      tmpl->set_height(imgWin.get_selection_max_y() - imgWin.get_selection_min_y());
    }


    Gate_shptr new_gate(new Gate(imgWin.get_selection_min_x(), 
				 imgWin.get_selection_min_x() + tmpl->get_width(),
				 imgWin.get_selection_min_y(), 
				 imgWin.get_selection_min_y() + tmpl->get_height() ));
    assert(new_gate != NULL);


    new_gate->set_gate_template(tmpl);
    lmodel->add_object(layer->get_layer_pos(), new_gate);

    new_gate->print();
    imgWin.reset_selection();
    imgWin.update_screen();
    main_project->set_changed();

  }
  else if(selected_objects.size() == 1) {
    
    std::set<PlacedLogicModelObject_shptr>::const_iterator it = selected_objects.begin();
    if(Gate_shptr gate = std::tr1::dynamic_pointer_cast<Gate>(*it)) {
    
      GateSelectWin gsWin(this, main_project->get_logic_model());
      std::list<GateTemplate_shptr> tmpl_list = gsWin.get_selection(false);

      if(tmpl_list.size() == 0) return;
      GateTemplate_shptr tmpl = *(tmpl_list.begin());
      
      gate->set_gate_template(tmpl);

      imgWin.update_screen();
      main_project->set_changed();

    }
  }
}

void MainWin::on_menu_gate_create_by_selection() {
  if(imgWin.selection_active() && main_project) {

    LogicModel_shptr lmodel = main_project->get_logic_model();
    Layer_shptr layer;

    try{
      layer = get_first_logic_layer(lmodel);
    }
    catch(CollectionLookupException const& ex) {
	error_dialog("Error", "There is no logic layer defined. Please define layer types.");
	return;
    }


    BoundingBox bbox(imgWin.get_selection_min_x(), 
		     imgWin.get_selection_max_x(), 
		     imgWin.get_selection_min_y(), 
		     imgWin.get_selection_max_y());

    GateTemplate_shptr tmpl(new GateTemplate(bbox.get_width(),
					     bbox.get_height() ));

    bbox.print();
    grab_template_images(lmodel, tmpl, bbox);


    GateConfigWin gcWin(this, main_project->get_logic_model(), tmpl);
    
    if(gcWin.run() == true) {
      
      lmodel->add_gate_template(tmpl);
      
      Gate_shptr gate(new Gate(bbox));
      gate->set_gate_template(tmpl);
      lmodel->add_object(layer->get_layer_pos(), gate);
    
      imgWin.reset_selection();
      imgWin.update_screen();
      main_project->set_changed();

    }
  }
}

void MainWin::on_menu_help_about() {
  char filename[PATH_MAX];
  snprintf(filename, PATH_MAX, "%s/icons/degate_logo.png", getenv("DEGATE_HOME"));

  Gtk::AboutDialog about_dialog;

  about_dialog.set_version("Version 0.0.6");
  about_dialog.set_logo(Gdk::Pixbuf::create_from_file(filename));

  about_dialog.set_comments("Martin Schobert <martin@weltregierung.de>\n"
			    "This software is released under the\nGNU General Public License Version 3.\n"
			    "2009"
			    );
  about_dialog.set_website("http://degate.zfch.de/");
  about_dialog.run();
}

void MainWin::on_wire_tool_release() {
  if(imgWin.get_wire_min_x() ==  imgWin.get_wire_max_x() &&
     imgWin.get_wire_min_y() ==  imgWin.get_wire_max_y()) return;

  Wire_shptr new_wire(new Wire(imgWin.get_wire_min_x(), imgWin.get_wire_min_y(),
			       imgWin.get_wire_max_x(), imgWin.get_wire_max_y(),
			       main_project->get_default_wire_diameter()));

  assert(new_wire);


  LogicModel_shptr lmodel = main_project->get_logic_model();
  Layer_shptr layer = lmodel->get_current_layer();

  lmodel->add_object(layer->get_layer_pos(), new_wire);

  main_project->set_changed();
  imgWin.update_screen();
}

void MainWin::on_selection_activated() {
  if(main_project) {

    menu_manager->set_menu_item_sensitivity("/MenuBar/GateMenu/GateCreateBySelection", true);
    menu_manager->set_menu_item_sensitivity("/MenuBar/GateMenu/GateSet", true);
  }
}

void MainWin::on_selection_revoked() {
  if(main_project) {

    menu_manager->set_menu_item_sensitivity("/MenuBar/GateMenu/GateCreateBySelection", false);
    menu_manager->set_menu_item_sensitivity("/MenuBar/GateMenu/GateSet", false);
  }
}

void MainWin::on_mouse_scroll_down(unsigned int clicked_real_x, unsigned int clicked_real_y) {
  if(main_project != NULL) {

    int real_dist_to_center_x = (int)clicked_real_x - (int)imgWin.get_center_x();
    int real_dist_to_center_y = (int)clicked_real_y - (int)imgWin.get_center_y();

    double zoom_factor = shift_key_pressed == true ? ZOOM_STEP_MOUSE_SCROLL_AND_SHIFT : ZOOM_STEP_MOUSE_SCROLL;

    unsigned int new_center_x = (int)imgWin.get_center_x() + real_dist_to_center_x -
      (double)real_dist_to_center_x * zoom_factor;

    unsigned int new_center_y = (int)imgWin.get_center_y() + real_dist_to_center_y -
      (double)real_dist_to_center_y * zoom_factor;

    zoom(new_center_x, new_center_y, zoom_factor);

    //zoom_out(center_x, center_y);
  }
}

void MainWin::on_mouse_scroll_up(unsigned int clicked_real_x, unsigned int clicked_real_y) {
  if(main_project != NULL) {

    int real_dist_to_center_x = (int)clicked_real_x - (int)imgWin.get_center_x();
    int real_dist_to_center_y = (int)clicked_real_y - (int)imgWin.get_center_y();

    double zoom_factor = shift_key_pressed == true ? ZOOM_STEP_MOUSE_SCROLL_AND_SHIFT : ZOOM_STEP_MOUSE_SCROLL;

    unsigned int new_center_x = (int)imgWin.get_center_x() + real_dist_to_center_x -
      (double)real_dist_to_center_x / zoom_factor;

    unsigned int new_center_y = (int)imgWin.get_center_y() + real_dist_to_center_y -
      (double)real_dist_to_center_y / zoom_factor;

    zoom(new_center_x, new_center_y, 1.0/zoom_factor);

  }
}

// @todo should have two params with real coords. the prevents, that we need a coord transformation
bool MainWin::on_imgwin_clicked(GdkEventButton * event) {

  if(main_project && (event->type == GDK_BUTTON_PRESS)) {
    if(event->button == 3) {
      imgWin.coord_screen_to_real((unsigned int)(event->x), (unsigned int)(event->y), &last_click_on_real_x, &last_click_on_real_y);
      menu_manager->show_popup_menu(event->button, event->time);
    }
    else if(event->button == 1) {
      unsigned int real_x, real_y;
      imgWin.coord_screen_to_real((unsigned int)(event->x), (unsigned int)(event->y), &real_x, &real_y);

      if(tool == TOOL_SELECT && !imgWin.selection_active()) {
	debug(TM, "call object_clicked()");
	object_clicked(real_x, real_y);
      }
      else if(tool == TOOL_VIA_UP || tool == TOOL_VIA_DOWN) {

	Via_shptr new_via(new Via(real_x, real_y,
				  main_project->get_default_pin_diameter(),
				  tool == TOOL_VIA_UP ? Via::DIRECTION_UP : Via::DIRECTION_DOWN));

	assert(new_via);
	
	LogicModel_shptr lmodel = main_project->get_logic_model();

	lmodel->add_object(lmodel->get_current_layer()->get_layer_pos(), 
			   new_via);

	main_project->set_changed();
	imgWin.update_screen();

      }
    }
      
  }
  return true;
}

bool MainWin::on_key_release_event_received(GdkEventKey * event) {
  if((event->keyval ==  GDK_Shift_L || event->keyval == GDK_Shift_R)) {
    shift_key_pressed = false;
    imgWin.set_shift_key_state(false);
    if(tool == TOOL_WIRE) imgWin.update_screen();
  }
  else if(event->state & GDK_CONTROL_MASK || 
	  event->keyval == GDK_Control_L || 
	  event->keyval == GDK_Control_R) {
    control_key_pressed = false;
    //debug(TM, "ctrl release");
  }

  //debug(TM, "key release: %d %d", event->state, event->keyval);
  return false;
}

bool MainWin::on_key_press_event_received(GdkEventKey * event) {
  if(event->keyval == GDK_Shift_L || event->keyval == GDK_Shift_R) {
    shift_key_pressed = true;
    imgWin.set_shift_key_state(true);
    if(tool == TOOL_WIRE) imgWin.update_screen();
  }
  //else if(event->keyval == GDK_Control_L || event->keyval == GDK_Control_R) {
  else if(!(event->state & GDK_CONTROL_MASK) && (event->keyval == GDK_Control_L)) {
    control_key_pressed = true;
    //debug(TM, "ctrl pressed");
  }
  else if(event->state & GDK_CONTROL_MASK) {
    control_key_pressed = false;
    //debug(TM, "ctrl as modifier pressed");
  }
  //else {
  //debug(TM, "any key  pressed");
  //}

  //debug(TM, "key press: %d %d", event->state, event->keyval);
  return false;
}

void MainWin::clear_selection() {
  highlighted_objects.clear();
  selected_objects.clear();
}

void MainWin::update_gui_on_selection_change() {
  std::set<PlacedLogicModelObject_shptr>::const_iterator it;

  if(selected_objects.size() == 1) {
    it = selected_objects.begin();

    if(Gate_shptr gate = std::tr1::dynamic_pointer_cast<Gate>(*it)) {
      menu_manager->set_menu_item_sensitivity("/MenuBar/GateMenu/GateOrientation", true);
      menu_manager->set_menu_item_sensitivity("/MenuBar/GateMenu/GateSetAsMaster", true);
      menu_manager->set_menu_item_sensitivity("/MenuBar/GateMenu/GateSet", true);
    }
    else {
      menu_manager->set_menu_item_sensitivity("/MenuBar/GateMenu/GateSet", imgWin.selection_active() ? true : false);
    }
    
    if(ciWin != NULL) ciWin->set_object(*it);

  }
  else {
    menu_manager->set_menu_item_sensitivity("/MenuBar/GateMenu/GateOrientation", false);
    menu_manager->set_menu_item_sensitivity("/MenuBar/GateMenu/GateSetAsMaster", false);
    menu_manager->set_menu_item_sensitivity("/MenuBar/GateMenu/GateSet", imgWin.selection_active() ? true : false);

    if(ciWin != NULL) ciWin->disable_inspection();
  }

  menu_manager->set_menu_item_sensitivity("/MenuBar/LogicMenu/LogicClearLogicModelInSelection", 
					  selected_objects_are_removable() ? true : false);

  menu_manager->set_menu_item_sensitivity("/MenuBar/LogicMenu/LogicInterconnect", 
					  selected_objects.size() >= 2 && 
					  selected_objects_are_interconnectable() ? true : false);

  menu_manager->set_menu_item_sensitivity("/MenuBar/LogicMenu/LogicIsolate", 
					  selected_objects.size() >= 1 && 
					  selected_objects_are_interconnectable() ? true : false);

}

void MainWin::object_clicked(unsigned int real_x, unsigned int real_y) {

  bool add_to_selection = false;

  // get info about selected object
  LogicModel_shptr lmodel = main_project->get_logic_model();
  Layer_shptr layer = lmodel->get_current_layer();
  debug(TM, "try to run get_object_at_position");
  PlacedLogicModelObject_shptr plo = layer->get_object_at_position(real_x, real_y);
  debug(TM, "get_object_at_position returned");

  // check if there is a gate on the logic layer
  if(plo == NULL) {
    try {
      layer = get_first_logic_layer(lmodel);
      plo = layer->get_object_at_position(real_x, real_y);
    }
    catch(CollectionLookupException const& ex) {
    }
  }

  if(plo != NULL) add_to_selection = true;


  std::set<PlacedLogicModelObject_shptr>::const_iterator it;

  // try to remove a single object
  if(plo != NULL && control_key_pressed == true) {
    //debug(TM, "remove single object from selection");      
    it = selected_objects.find(plo);
    if(it != selected_objects.end()) {

      selected_objects.erase(*it);
      highlighted_objects.remove(plo);
      add_to_selection = false;
    }
  }

  if(control_key_pressed == false){
    clear_selection();
    highlighted_objects.clear();
  }
  
  
  if(add_to_selection) {
    // add to selection
    if(plo != NULL) {
      selected_objects.insert(plo);
      highlighted_objects.add(plo);
    }
  }
 
  imgWin.update_screen();
  update_gui_on_selection_change();
}

// XXX put into a selection class
bool MainWin::selected_objects_are_interconnectable() {
  std::set<PlacedLogicModelObject_shptr>::const_iterator it;

  for(it = selected_objects.begin(); it != selected_objects.end(); it++) {
    if(std::tr1::dynamic_pointer_cast<ConnectedLogicModelObject>(*it) == NULL) {
      return false;
    }
  }
  return true;
}

// XXX put into a selection class
bool MainWin::selected_objects_are_removable() {
  std::set<PlacedLogicModelObject_shptr>::const_iterator it;

  if(selected_objects.size() == 0) return false;
  for(it = selected_objects.begin(); it != selected_objects.end(); it++) {
    if(std::tr1::dynamic_pointer_cast<GatePort>(*it) != NULL) {
      return false;
    }
  }
  return true;
}


void MainWin::on_popup_menu_set_port() {
  if(main_project) {

    LogicModel_shptr lmodel = main_project->get_logic_model();
    Layer_shptr layer;
    try {
      layer = get_first_logic_layer(lmodel);
    }
    catch(CollectionLookupException const& ex) {
      error_dialog("Error", "There is no logic layer defined.");
      return;
    }

    PlacedLogicModelObject_shptr plo = layer->get_object_at_position(last_click_on_real_x, last_click_on_real_y);

    if(GatePort_shptr gate_port = std::tr1::dynamic_pointer_cast<GatePort>(plo)) {
      // user clicked on an alredy placed gate port -> derive gate object from it
      plo = gate_port->get_gate();
    }

    if(plo == NULL) {
      error_dialog("Error", "There is no gate. Maybe you are not working on the logic layer?");
      return;
    }

    Gate_shptr gate = std::tr1::dynamic_pointer_cast<Gate>(plo);
    if(gate == NULL) {
      error_dialog("Error", "You clicked on something that is not a gate.");
      return;
    }

    if(!gate->has_template()) {
      error_dialog("Error", "Please define a template type for that gate.");
      return;
    }

    GateTemplate_shptr gate_template = gate->get_gate_template();

    // check, if the gate has defined ports
    if(gate_template->get_number_of_ports() == 0) {
      error_dialog("Error", "Please define ports before you place them.");
      return;
    }

    assert(gate->in_shape(last_click_on_real_x, last_click_on_real_y));


    if(!gate->has_orientation()) {
      error_dialog("Error", "Please define gate's orientation relative to the template gate.");
      return;
    }

    unsigned int 
      x = gate->get_relative_x_position_within_gate(last_click_on_real_x - gate->get_min_x()),
      y = gate->get_relative_y_position_within_gate(last_click_on_real_y - gate->get_min_y());
    
    PortSelectWin psWin(this, gate);
    GateTemplatePort_shptr template_port = psWin.run();
    if(template_port != NULL) {
      debug(TM, "x=%d y=%d", x, y);
      Point p(x, y);
      template_port->set_point(p);
      lmodel->update_ports(gate_template);

      main_project->set_changed();
      imgWin.update_screen();
    }
  }
}

void MainWin::on_popup_menu_set_name() {
  if(main_project) {

    std::set<PlacedLogicModelObject_shptr>::const_iterator it;
    Glib::ustring name;

    if(selected_objects.size() == 0) {
      error_dialog("Error", "Please select one or more objects.");
      return;
    }
    else if(selected_objects.size() == 1) {
      it = selected_objects.begin();
      name = (*it)->get_name();
    }

    GenericTextInputWin input(this, "Set name", "Please set a name", name);
    Glib::ustring str;
    if(input.run(name) == true) {
    
      for(it = selected_objects.begin(); it != selected_objects.end(); it++) {
	(*it)->set_name(name);
      }
      main_project->set_changed();
      imgWin.update_screen();
    }

  }
}

void MainWin::on_popup_menu_add_vertical_grid_line() {
  if(main_project != NULL) {

    IrregularGrid_shptr g = main_project->get_irregular_horizontal_grid();

    if(!g->is_enabled())
      error_dialog("Error", "Please set the unregular grid mode in the grid configuration.");
    else {
      g->add_offset(last_click_on_real_x);
      gcWin->update_grid_entries();
      main_project->set_changed();
      imgWin.update_screen();
    }
  }
}

void MainWin::on_popup_menu_add_horizontal_grid_line() {
  if(main_project != NULL) {

    IrregularGrid_shptr g = main_project->get_irregular_vertical_grid();

    if(!g->is_enabled())
      error_dialog("Error", "Please set the unregular grid mode in the grid configuration.");
    else {
      g->add_offset(last_click_on_real_y);
      gcWin->update_grid_entries();
      main_project->set_changed();
      imgWin.update_screen();
    }
  }
}


void MainWin::on_menu_logic_auto_name_gates(AutoNameGates::ORIENTATION orientation) {
  if(main_project != NULL) {

    Gtk::MessageDialog dialog(*this, "The operation may destroy previously set names. Are you sure you want name all gates?", 
			      true, Gtk::MESSAGE_QUESTION, Gtk::BUTTONS_YES_NO);
    dialog.set_title("Warning");
    if(dialog.run() == Gtk::RESPONSE_NO) return;
    dialog.hide();

    Layer_shptr layer;
    try {
      layer = get_first_logic_layer(main_project->get_logic_model());
    }
    catch(CollectionLookupException const& ex) {
      error_dialog("Error", "There is no logic layer defined.");
      return;
    }

    ipWin = new InProgressWin(this, "Naming gates", "Please wait while generating names.");
    ipWin->show();

    signal_auto_name_finished_.connect(sigc::mem_fun(*this, &MainWin::on_auto_name_finished));
    thread = Glib::Thread::create(sigc::bind<AutoNameGates::ORIENTATION>
				  (sigc::mem_fun(*this, 
						 &MainWin::auto_name_gates_thread), 
				   orientation), false);

  }
}


void MainWin::auto_name_gates_thread(AutoNameGates::ORIENTATION orientation) {
  try {
    AutoNameGates auto_name(get_first_logic_layer(main_project->get_logic_model()),
			    orientation);
    auto_name.run();
    signal_auto_name_finished_(RET_OK);
  }
  catch(CollectionLookupException const& ex) {
    signal_auto_name_finished_(RET_ERR);
  }
}


void MainWin::on_auto_name_finished(ret_t ret) {
  if(ipWin) {
    ipWin->close();
    delete ipWin;
    ipWin = NULL;
  }
  if(RET_IS_NOT_OK(ret)) {
    error_dialog("Error", "naming failed");
  }
  else {
    main_project->set_changed();
    //imgWin.update_screen(); // XXX results in a Fatal IO error 11 (Resource temporarily unavailable) on X server :0.0.
  }
}

void MainWin::on_menu_logic_interconnect() {
  std::set<PlacedLogicModelObject_shptr>::const_iterator it;
  if(selected_objects.size() >= 2) {

    if(!selected_objects_are_interconnectable()) {
      error_dialog("Error", "One of the objects you selected can not have connections at all.");
      return;
    }


    connect_objects<std::set<PlacedLogicModelObject_shptr>::iterator>(main_project->get_logic_model(),
      selected_objects.begin(),
      selected_objects.end());


    main_project->set_changed();
    imgWin.update_screen();
  }
}

void MainWin::on_menu_logic_isolate() {
  std::set<PlacedLogicModelObject_shptr>::const_iterator it;
  if(selected_objects.size() >= 1) {

    if(!selected_objects_are_interconnectable()) {
      error_dialog("Error", "One of the objects you selected can not have connections at all.");
      return;
    }
    

    isolate_objects<std::set<PlacedLogicModelObject_shptr>::iterator>(main_project->get_logic_model(),
      selected_objects.begin(),
      selected_objects.end());

    main_project->set_changed();
    imgWin.update_screen();
  }

}

void MainWin::on_menu_logic_connection_inspector() {
  if(main_project != NULL && ciWin != NULL) ciWin->show();
}


void MainWin::on_menu_view_grid_config() {

  if(main_project != NULL && gcWin != NULL) {
    gcWin->show();
  }
 
}

void MainWin::on_grid_config_changed() {
  if(gcWin) {
    main_project->set_changed();
    imgWin.update_screen();
  }
}

void MainWin::on_menu_layer_import_background() {
  Gtk::FileChooserDialog dialog("Please select a background image", Gtk::FILE_CHOOSER_ACTION_OPEN);
  dialog.set_transient_for(*this);
  //dialog.set_select_multiple(true);

  dialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
  dialog.add_button("Select", Gtk::RESPONSE_OK);
  add_image_file_filter_to_file_chooser(dialog);  

  int result = dialog.run();

  //Glib::SListHandle<Glib::ustring> filenames = dialog.get_filenames();
  Glib::ustring filename = dialog.get_filename();
  dialog.hide();
  
  switch(result) {
  case(Gtk::RESPONSE_OK):

    ipWin = new InProgressWin(this, "Importing", 
			      "Please wait while importing background image and calculate the prescaled images.");
    ipWin->show();
    main_project->set_changed();
    //imgWin.lock_renderer();

    signal_bg_import_finished_.connect(sigc::mem_fun(*this, &MainWin::on_background_import_finished));
    Glib::Thread::create(sigc::bind<const Glib::ustring>(sigc::mem_fun(*this, &MainWin::background_import_thread), filename), false);

    break;
  case(Gtk::RESPONSE_CANCEL):
    break;
  }
  
}


// in GUI-thread
void MainWin::on_background_import_finished() {

  debug(TM, "BG import finished");
  if(ipWin) {
    ipWin->close();
    delete ipWin;
    ipWin = NULL;
  }
  //imgWin.unlock_renderer();
  //imgWin.update_screen();
  LogicModel_shptr lmodel = main_project->get_logic_model();
  Layer_shptr layer = lmodel->get_current_layer();
  assert(layer != NULL);
  
  if(!layer->is_enabled()) set_layer(get_first_enabled_layer(lmodel));
  else set_layer(layer->get_layer_pos());
  
}


void MainWin::background_import_thread(Glib::ustring bg_filename) {
  debug(TM, "Load background image.");
  load_background_image(main_project->get_logic_model()->get_current_layer(), 			
			main_project->get_project_directory(),
			bg_filename);
  debug(TM, "Background image loaded.");
  signal_bg_import_finished_();
}


void MainWin::on_menu_layer_configuration() {
  if(main_project) {
    
    if(lcWin->run()) {
      main_project->set_changed();
      set_layer(get_first_enabled_layer(main_project->get_logic_model()));
    }
  }
}

void MainWin::on_menu_layer_set_transistor() { 
  if(main_project) {
    main_project->get_logic_model()->get_current_layer()->set_layer_type(Layer::TRANSISTOR);
    main_project->set_changed();
  }
}

void MainWin::on_menu_layer_set_logic() { 
  if(main_project) {
    main_project->get_logic_model()->get_current_layer()->set_layer_type(Layer::LOGIC);
    main_project->set_changed();
  }
}

void MainWin::on_menu_layer_set_metal() { 
  if(main_project) {
    main_project->get_logic_model()->get_current_layer()->set_layer_type(Layer::METAL);
    main_project->set_changed();
  }
}



void MainWin::on_menu_logic_clear_logic_model() {
  Gtk::MessageDialog dialog(*this, "Clear logic model",
			    false, Gtk::MESSAGE_QUESTION,
			    Gtk::BUTTONS_OK_CANCEL);

  LogicModel_shptr lmodel = main_project->get_logic_model();

  dialog.set_secondary_text("Are you sure you want to clear the complete logic model for the current layer?");
  int result = dialog.run();
  switch(result) {
  case(Gtk::RESPONSE_OK):
    clear_logic_model(lmodel, lmodel->get_current_layer());
    main_project->set_changed();
    imgWin.update_screen();
    ciWin->objects_removed();
    break;
  case(Gtk::RESPONSE_CANCEL):
  default:
    break;
  }

}

void MainWin::remove_objects() {

  if(main_project != NULL) {

    std::set<PlacedLogicModelObject_shptr>::const_iterator it;
    LogicModel_shptr lmodel = main_project->get_logic_model();

    for(it = selected_objects.begin(); it != selected_objects.end(); it++) {
      lmodel->remove_object(*it);
    }
      
    selected_objects.clear();
    highlighted_objects.clear();

    menu_manager->set_menu_item_sensitivity("/MenuBar/LogicMenu/LogicClearLogicModelInSelection", false);
    imgWin.update_screen(); 
    ciWin->objects_removed();
  }
}

void MainWin::on_menu_layer_clear_background_image() {
  Gtk::MessageDialog dialog(*this, "Clear background image",
			    false /* use_markup */, Gtk::MESSAGE_QUESTION,
			    Gtk::BUTTONS_OK_CANCEL);

  dialog.set_secondary_text("Are you sure you want to clear the background image for the current layer?");
  int result = dialog.run();
  if(result == Gtk::RESPONSE_OK) {
    Layer_shptr layer = main_project->get_logic_model()->get_current_layer();
    if(layer->has_background_image()) {
      BackgroundImage_shptr img = layer->get_image();
      clear_image<BackgroundImage>(img);
      main_project->set_changed();
      imgWin.update_screen();
    }
  }
  
}




void MainWin::error_dialog(const char * const title, const char * const message) {

  Gtk::MessageDialog dialog(*this, message, true, Gtk::MESSAGE_ERROR);
  dialog.set_title(title);
  dialog.run();
}

void MainWin::warning_dialog(const char * const title, const char * const message) {

  Gtk::MessageDialog dialog(*this, message, true, Gtk::MESSAGE_WARNING);
  dialog.set_title(title);
  dialog.run();
}

