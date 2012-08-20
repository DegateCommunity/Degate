/*

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
#include "ModuleWin.h"
#include <SelectModuleWin.h>
#include "AnnotationListWin.h"
#include "GenericTextInputWin.h"
#include "RecognitionManager.h"
#include "LayerConfigWin.h"
#include "gui_globals.h"
#include <AppHelper.h>

#include <degate.h>
#include <ProjectExporter.h>
#include <ProjectImporter.h>
#include <LogicModelHelper.h>
#include <SubProjectAnnotation.h>
#include <ProjectArchiver.h>
#include <XmlRpc.h>

using namespace degate;

MainWin::MainWin() : render_window(editor), is_fullscreen(false) {

  // setup window
  set_default_size(1024, 700);
  char path[PATH_MAX];
  snprintf(path, PATH_MAX, "%s/icons/degate_logo.png", getenv("DEGATE_HOME"));
  set_icon_from_file(path);

  add(m_Box);


  // setup menu
  menu_manager = std::tr1::shared_ptr<MenuManager>(new MenuManager(this));
  Gtk::Widget* menubar = menu_manager->get_menubar();
  Gtk::Widget* toolbar = menu_manager->get_toolbar();
  assert(menubar != NULL);
  assert(toolbar != NULL);
  if(menubar != NULL && toolbar != NULL) {
    m_Box.pack_start(*menubar, Gtk::PACK_SHRINK);
    m_Box.pack_start(*toolbar, Gtk::PACK_SHRINK);
  }

  //const std::vector<Glib::ustring> render_func_names = imgWin.get_renderer_func_names();
  //const std::vector<bool> render_func_states = imgWin.get_renderer_func_states();

  //menu_manager->initialize_menu_render_funcs(render_func_names, render_func_states);


  //render_window.set_virtual_size(100, 100);
  m_Box.pack_start(render_window, Gtk::PACK_EXPAND_WIDGET);
  on_menu_tools_select();

  // setup statusbar
  m_statusbar.push("");
  m_Box.pack_start(m_statusbar, Gtk::PACK_SHRINK);


  show_all_children();

  update_title();
  control_key_pressed = false;
  shift_key_pressed = false;
  //imgWin.set_shift_key_state(false);

  render_window.grab_focus();

  project_to_open = NULL;
  Glib::signal_timeout().connect( sigc::mem_fun(*this, &MainWin::on_timer), 5000);


  if(getuid() == 0) {
    warning_dialog("Security warning",
		   "You started degate as superuser. I don't cause harm to you. "
		   "But you should think about it. You should know: \"All your base are belong to us\". "
		   "I will not drop privileges and I hope you know, what you do.");
  }


  signal_key_press_event().connect(sigc::mem_fun(*this,&MainWin::on_key_press_event_received), false);
  signal_key_release_event().connect(sigc::mem_fun(*this,&MainWin::on_key_release_event_received), false);
  signal_hide().connect(sigc::mem_fun(*this, &MainWin::on_menu_project_close), false);
  signal_project_open_finished_.connect(sigc::mem_fun(*this, &MainWin::on_project_load_finished));

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

bool MainWin::on_timer() {
  if(project_to_open != NULL) {
    open_project(project_to_open);
    project_to_open = NULL;
  }

  if(main_project != NULL) {
    try {
      if(autosave_project(main_project))
	m_statusbar.push("Autosaving project data ... done.");
    }
    catch(DegateRuntimeException const& ex) {
      error_dialog("Error", "Can't save project.");
    }
  }

  return true;
}

void MainWin::on_view_info_layer_toggled(int slot_pos) {

  error_dialog("Error", "Feature deactivated.");
  //if(menu_manager->toggle_info_layer(slot_pos)) {
    //imgWin.toggle_render_info_layer(slot_pos);
    //imgWin.update_screen();
  //}
}

void MainWin::on_menu_view_toggle_all_info_layers() {

  bool current_state =  editor.get_info_layer_state(DegateRenderer::INFO_LAYER_ALL);
  editor.set_info_layer_state(DegateRenderer::INFO_LAYER_ALL, !current_state);
  menu_manager->toggle_menu_item("/MenuBar/ViewMenu/ViewToggleInfoLayer", !current_state,
				 "Disable all info layers", "Enable all info layers");
  editor.update_screen();
}


void MainWin::on_menu_view_fullscreen() {
  if(is_fullscreen) {    
    unfullscreen();
    is_fullscreen = false; // according to gtkmm documentation, it would be better to receive a signal
  }
  else {
    fullscreen();
    is_fullscreen = true;
  }
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

  std::string recent_project = Glib::filename_from_uri(menu_manager->get_recent_project_uri());
  std::cout << "URI selected = " << recent_project << std::endl;
  open_project(recent_project);
}

void MainWin::on_menu_project_quit() {
  if(yes_no_dialog("Warning", "Do you want to quit degate?")) {
    on_menu_project_close();
    hide(); //Closes the main window to stop the Gtk::Main::run().
  }
}

void MainWin::on_menu_project_new() {
  Gtk::FileChooserDialog dialog("Please choose a project folder", Gtk::FILE_CHOOSER_ACTION_CREATE_FOLDER);
  dialog.set_transient_for(*this);
  dialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
  dialog.add_button("Select", Gtk::RESPONSE_OK);

  std::cout << "create project - raise file chooser"  << std::endl;
  int result = dialog.run();
  std::cout << "create project -get file name"  << std::endl;
  std::string project_dir = dialog.get_filename();
  std::cout << "create project " << project_dir << std::endl;
  dialog.hide();
  std::cout << "create project - hide file chooser"  << std::endl;

  if(result == Gtk::RESPONSE_OK) {
    create_new_project(project_dir);
  }
  std::cout << "create project - done \n";
}

void MainWin::create_new_project(std::string const& project_dir) {
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
    try {
      if(!file_exists(project_dir)) create_directory(project_dir);
      main_project = Project_shptr(new Project(width, height, project_dir, layers));
      update_gui_for_loaded_project();
      assert(lcWin != NULL);
      lcWin->run();
      set_layer(get_first_enabled_layer(main_project->get_logic_model()));
      on_menu_project_save();
    }
    catch(degate::FileSystemException & ex) {
      error_dialog("Error: Can't create a new project.", ex.what());
    }

  }
}

void MainWin::on_menu_project_close() {
  if(main_project) {

    if(main_project->is_changed() &&       
       yes_no_dialog("Warning", "Project data was modified. Should it be saved?"))
      on_menu_project_save();

    //imgWin.disable_renderer();

    clear_selection();

    main_project.reset();
    editor.update_screen();

    update_title();


    if(ciWin != NULL) ciWin.reset();
    if(rcWin != NULL) rcWin.reset();
    if(alWin != NULL) alWin.reset();
    if(gcWin != NULL) gcWin.reset();
    if(lcWin != NULL) lcWin.reset();

    menu_manager->set_widget_sensitivity(false);
  }
}


void MainWin::on_menu_project_settings() {
  if(main_project) {
    diameter_t old_port_size = main_project->get_default_port_diameter();
    ProjectSettingsWin psWin(this, main_project);
    if(psWin.run()) {

      diameter_t new_port_size = main_project->get_default_port_diameter();
      if(old_port_size != new_port_size) {
	if(yes_no_dialog("Warning", 
			 "Default gate port diameter has changed. "
			 "Should degate update port diameters of all gate ports?"))
	  update_port_diameters(main_project->get_logic_model(), new_port_size);
      }
      editor.set_default_colors(main_project->get_default_colors());
      editor.set_corridor_size(main_project->get_template_dimension());
      editor.set_font_size(main_project->get_font_size());
      project_changed();
    }
  }
}

void MainWin::on_menu_project_export_archive() {
  if(main_project) {
    if(main_project->is_changed()) {
      if(yes_no_dialog("Warning", "Project data was modified. Should it be saved?"))
	on_menu_project_save();
      else return;
    }
    Gtk::FileChooserDialog dialog("Export project as archive", Gtk::FILE_CHOOSER_ACTION_SAVE);
    dialog.set_transient_for(*this);

    dialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
    dialog.add_button("Select", Gtk::RESPONSE_OK);

    dialog.set_current_folder(main_project->get_project_directory() + "/..");

    std::string suffix = get_basename(main_project->get_project_directory());
    dialog.set_current_name(get_date_and_time_as_file_prefix() + suffix + ".zip");
    int result = dialog.run();
    dialog.hide();

    switch(result) {
    case Gtk::RESPONSE_OK:

      ipWin = std::tr1::shared_ptr<InProgressWin>
	(new InProgressWin(this, "Exporting", "Please wait while exporting project."));
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

void MainWin::project_export_thread(std::string project_dir,
				    std::string dst_file) {

  ProjectArchiver archiver;

  try {

    std::string filename = get_filename_from_path(dst_file);
    assert(filename.length() > (get_file_suffix(filename).length() + 1));
    std::string name = filename.substr(0, filename.length() -
				       (get_file_suffix(filename).length() + 1));

    archiver.export_data(project_dir.c_str(), dst_file.c_str(), name);
    signal_export_finished_(true);
  }
  catch(ZipException & ex) {
    signal_export_finished_(false);
  }
}


void MainWin::on_export_finished(bool success) {
  if(ipWin) {
    ipWin->close();
    ipWin.reset();
  }
  if(success == false) {
    error_dialog("Error", "Export failed.");
  }
}

void MainWin::on_menu_project_save() {
  if(main_project) {
    try {
      ProjectExporter exporter;
      exporter.export_all(main_project->get_project_directory(), main_project, false);
      main_project->set_changed(false);
      update_title();
    }
    catch(DegateRuntimeException const& ex) {
      error_dialog("Error", "Can't save project.");
    }
  }
}

void MainWin::on_menu_project_open() {

  if(main_project) on_menu_project_close();

  Gtk::FileChooserDialog dialog("Please choose a project folder",
				Gtk::FILE_CHOOSER_ACTION_SELECT_FOLDER);
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

// --------------------------------------------------------------------------
void MainWin::open_project(Glib::ustring project_dir) {
  if(main_project) on_menu_project_close();

  if(check_for_autosaved_project(project_dir.c_str())) {

    if(yes_no_dialog("Autosaved files",
		     "There are autosaved files that are newer than the project files. "
		     "Should the project data be loaded from the autosaved files instead?"))
      restore_autosaved_project(project_dir.c_str());
  }

  ipWin = std::tr1::shared_ptr<InProgressWin>
    (new InProgressWin(this, "Opening Project", "Please wait while opening project."));
  ipWin->show();

  thread = Glib::Thread::create(sigc::bind<const Glib::ustring>
				(sigc::mem_fun(*this, &MainWin::project_open_thread),
				 project_dir), false);
}


void MainWin::project_open_thread(Glib::ustring project_dir) {

  assert(main_project == NULL);

  try {
    ProjectImporter importer;
    main_project = importer.import_all(project_dir);
    debug(TM, "in project_open_thread(): project loaded");
  }
  catch(std::runtime_error const& ex) {
    debug(TM, "Exception while opening a project: %s", ex.what());
    thread_error_msg = ex.what();
  }

  debug(TM, "emit signal");
  signal_project_open_finished_();
}

// in GUI-thread
void MainWin::on_project_load_finished() {
  debug(TM, "on_project_load_finished()");

  if(ipWin) {
    ipWin->close();
    ipWin.reset();
  }

  if(main_project == NULL) {
    Gtk::MessageDialog err_dialog(*this, thread_error_msg,
				  false, Gtk::MESSAGE_ERROR, Gtk::BUTTONS_OK, true);
    err_dialog.set_title("Error");
    err_dialog.run();
  }
  else {
    Layer_shptr layer = main_project->get_logic_model()->get_current_layer();
    assert(layer != NULL);
    update_gui_for_loaded_project();
    set_layer(get_first_enabled_layer(main_project->get_logic_model()));
  }
}

// --------------------------------------------------------------------------

void MainWin::update_gui_for_loaded_project() {

  if(main_project) {


    LogicModel_shptr lmodel = main_project->get_logic_model();
    int l_pos = get_first_enabled_layer(main_project->get_logic_model())->get_layer_pos();
    lmodel->set_current_layer(l_pos);
    Layer_shptr layer = lmodel->get_current_layer();


    editor.set_virtual_size(main_project->get_width(), main_project->get_height());
    editor.set_viewport(0, 0, editor.get_width(), editor.get_height());
    editor.set_logic_model(lmodel);
    editor.set_layer(layer);

    editor.set_grid(main_project->get_regular_horizontal_grid(),
		    main_project->get_regular_vertical_grid(),
		    main_project->get_irregular_horizontal_grid(),
		    main_project->get_irregular_vertical_grid());
    editor.set_corridor_size(main_project->get_template_dimension());

    editor.set_default_colors(main_project->get_default_colors());
    editor.set_font_size(main_project->get_font_size());

    menu_manager->set_widget_sensitivity(true);
    add_to_recent_menu();

    update_title();

    on_area_selection_revoked();


    ciWin = std::tr1::shared_ptr<ConnectionInspectorWin>
      (new ConnectionInspectorWin(this, main_project->get_logic_model()));
    ciWin->signal_goto_button_clicked().connect(sigc::mem_fun(*this, &MainWin::goto_object));

    rcWin = std::tr1::shared_ptr<RCViolationsWin>
      (new RCViolationsWin(this, main_project->get_logic_model(), 
			   main_project->get_rcv_blacklist()));
    rcWin->signal_goto_button_clicked().connect(sigc::mem_fun(*this, &MainWin::goto_object));
    

    modWin = std::tr1::shared_ptr<ModuleWin>(new ModuleWin(this, main_project->get_logic_model()));
    modWin->signal_goto_button_clicked().connect(sigc::mem_fun(*this, &MainWin::goto_object));

    alWin = std::tr1::shared_ptr<AnnotationListWin>
      (new AnnotationListWin(this, main_project->get_logic_model()));
    alWin->signal_goto_button_clicked().connect(sigc::mem_fun(*this, &MainWin::goto_object));

    gcWin = std::tr1::shared_ptr<GridConfigWin>(new GridConfigWin(this,
							  main_project->get_regular_horizontal_grid(),
							  main_project->get_regular_vertical_grid(),
							  main_project->get_irregular_horizontal_grid(),
							  main_project->get_irregular_vertical_grid()));
    gcWin->signal_changed().connect(sigc::mem_fun(*this, &MainWin::on_grid_config_changed));


    lcWin = std::tr1::shared_ptr<LayerConfigWin>(new LayerConfigWin(this, main_project->get_logic_model(),
							    main_project->get_project_directory()));
    lcWin->signal_on_background_import_finished().connect
      (sigc::mem_fun(*this, &MainWin::on_background_import_finished));

    editor.update_screen();
  }
}

void MainWin::on_menu_project_create_subproject() {
  if(main_project == NULL) return;

  LogicModel_shptr lmodel = main_project->get_logic_model();
  assert(lmodel != NULL);
  Layer_shptr layer = lmodel->get_current_layer();
  assert(layer != NULL);

  boost::format f("subproject_%1%");
  f % lmodel->get_new_object_id();

  std::tr1::shared_ptr<GfxEditorToolSelection<DegateRenderer> > selection_tool =
    std::tr1::dynamic_pointer_cast<GfxEditorToolSelection<DegateRenderer> >(editor.get_tool());

  if(selection_tool != NULL && selection_tool->has_selection()) {
    SubProjectAnnotation_shptr annotation
      (new SubProjectAnnotation(selection_tool->get_bounding_box(), f.str()));
    annotation->set_fill_color(0x7f0000ff);

    lmodel->add_object(layer->get_layer_pos(), annotation);

    project_changed();
    editor.update_screen();

    if(alWin != NULL) alWin->refresh();
  }
}

void MainWin::on_menu_project_open_parent() {
  if(main_project == NULL) return;

  std::string parent_dir = join_pathes(main_project->get_project_directory(), "..");
  if(file_exists(join_pathes(parent_dir, "project.xml"))) {
    open_project(parent_dir);
  }
  else
    error_dialog("Error", "Current project has no parent project.");
}


void MainWin::set_layer(Layer_shptr layer) {
  set_layer(layer->get_layer_pos());
}

void MainWin::set_layer(unsigned int layer) {
  if(main_project == NULL) return;

  LogicModel_shptr lmodel = main_project->get_logic_model();

  if(lmodel->get_num_layers() > 0 && layer < lmodel->get_num_layers()) {

    Layer_shptr layer_ptr = lmodel->get_layer(layer);
    lmodel->set_current_layer(layer_ptr->get_layer_pos());
    editor.set_layer(layer_ptr);

    update_title();
    editor.update_screen();
  }
}

void MainWin::goto_last_emarker() {
  if(main_project != NULL && last_emarker != NULL) {
    
    const BoundingBox & bbox = last_emarker->get_bounding_box();
    Layer_shptr layer =  last_emarker->get_layer();
    highlighted_objects.add(last_emarker);
    center_view(bbox.get_center_x(), bbox.get_center_y(), layer->get_layer_pos());
  }
}

void MainWin::goto_object(PlacedLogicModelObject_shptr obj_ptr) {
  assert(obj_ptr != NULL);
  if(main_project != NULL && obj_ptr != NULL) {

    const BoundingBox & bbox = obj_ptr->get_bounding_box();
    Layer_shptr layer;

    /* Do not switch layer, if user jumps to a gate or a gate port. */
    if(std::tr1::dynamic_pointer_cast<GatePort>(obj_ptr) ||
       std::tr1::dynamic_pointer_cast<Gate>(obj_ptr))
      layer = main_project->get_logic_model()->get_current_layer();
    else 
      layer = obj_ptr->get_layer();

    clear_selection();
    selected_objects.add(obj_ptr);
    highlighted_objects.add(obj_ptr, main_project->get_logic_model()); 

    assert(selected_objects.size() == 1);
    assert(highlighted_objects.size() == 1);

    if(ciWin != NULL) ciWin->set_object(obj_ptr); // update connection inspector

    center_view(bbox.get_center_x(), bbox.get_center_y(), layer->get_layer_pos());
  }
}

void MainWin::on_menu_view_next_layer() {
  if(main_project == NULL) return;

  LogicModel_shptr lmodel = main_project->get_logic_model();
  if(lmodel->get_num_layers() != 0)
  set_layer(get_next_enabled_layer(lmodel));
}

void MainWin::on_menu_view_prev_layer() {
  if(main_project == NULL) return;

  LogicModel_shptr lmodel = main_project->get_logic_model();
  if(lmodel->get_num_layers() != 0)
    set_layer(get_prev_enabled_layer(lmodel));
}


void MainWin::on_menu_view_zoom_in() {
  if(main_project == NULL) return;
  render_window.zoom_in();
}

void MainWin::on_menu_view_zoom_out() {
  if(main_project == NULL) return;
  render_window.zoom_out();
}



void MainWin::center_view(unsigned int center_x, unsigned int center_y, unsigned int layer) {

  if(main_project == NULL) return;
  // XXX
  render_window.center_view(center_x, center_y);

  set_layer(layer);
}


void MainWin::on_menu_tools_select() {

  Glib::RefPtr<Gdk::Window> window = editor.get_window();
  if(window) window->set_cursor(Gdk::Cursor(Gdk::LEFT_PTR));

  std::tr1::shared_ptr<GfxEditorToolSelection<DegateRenderer> > selection_tool
    (new GfxEditorToolSelection<DegateRenderer>(editor));

  selection_tool->signal_selection_activated().
    connect(sigc::mem_fun(*this, &MainWin::on_area_selection_activated));

  selection_tool->signal_selection_area_resized().
    connect(sigc::mem_fun(*this, &MainWin::on_area_selection_resized));

  selection_tool->signal_selection_revoked().
    connect(sigc::mem_fun(*this, &MainWin::on_area_selection_revoked));

  selection_tool->signal_mouse_double_clicked().
    connect(sigc::mem_fun(*this, &MainWin::selection_tool_double_clicked));

  selection_tool->signal_mouse_clicked().
    connect(sigc::mem_fun(*this, &MainWin::selection_tool_clicked));

  editor.set_tool(selection_tool);
}

void MainWin::on_menu_tools_move() {
  Glib::RefPtr<Gdk::Window> window = editor.get_window();
  if(window) window->set_cursor(Gdk::Cursor(Gdk::FLEUR));

  std::tr1::shared_ptr<GfxEditorToolMove<DegateRenderer> > move_tool
    (new GfxEditorToolMove<DegateRenderer>(editor));

  editor.set_tool(move_tool);
}

void MainWin::on_menu_tools_wire() {
  Glib::RefPtr<Gdk::Window> window = editor.get_window();
  window->set_cursor(Gdk::Cursor(Gdk::TCROSS));

  std::tr1::shared_ptr<GfxEditorToolWire<DegateRenderer> > wire_tool
    (new GfxEditorToolWire<DegateRenderer>(editor));

  wire_tool->signal_wire_added().connect(sigc::mem_fun(*this, &MainWin::on_wire_added));
  wire_tool->signal_via_added().connect(sigc::mem_fun(*this, &MainWin::via_tool_clicked));
  editor.set_tool(wire_tool);
}

void MainWin::on_menu_tools_via_up() {
  on_menu_tools_via(Via::DIRECTION_UP);
}

void MainWin::on_menu_tools_via_down() {
  on_menu_tools_via(Via::DIRECTION_DOWN);
}

void MainWin::on_menu_tools_via(degate::Via::DIRECTION dir) {
  Glib::RefPtr<Gdk::Window> window = editor.get_window();
  window->set_cursor(Gdk::Cursor(Gdk::CROSS));

  std::tr1::shared_ptr<GfxEditorToolVia<DegateRenderer> > via_tool
    (new GfxEditorToolVia<DegateRenderer>(editor));

  if(dir == Via::DIRECTION_DOWN)
    via_tool->signal_mouse_clicked().connect(sigc::mem_fun(*this, &MainWin::via_down_tool_clicked));
  else
    via_tool->signal_mouse_clicked().connect(sigc::mem_fun(*this, &MainWin::via_up_tool_clicked));

  editor.set_tool(via_tool);
}


void MainWin::on_algorithm_finished(int slot_pos) {


  if(ipWin) {
    ipWin->close();
    ipWin.reset();
  }

  editor.update_screen();

  debug(TM, "Algorithm finished.");

  RecognitionManager & rm = RecognitionManager::get_instance();
  rm.after_dialog(slot_pos);

  signal_algorithm_finished_.reset();

  project_changed();

  /*
  try {
    if(autosave_project(main_project, 0))
      m_statusbar.push("Autosaving project data ... done.");
  }
  catch(DegateRuntimeException const& ex) {
    error_dialog("Error", "Can't save project.");
  }
  */
}


void MainWin::algorithm_calc_thread(int slot_pos) {

  debug(TM, "Calculating ...");
  RecognitionManager & rm = RecognitionManager::get_instance();

  rm.run(slot_pos);

  signal_algorithm_finished_->emit();
}

void MainWin::on_algorithms_func_clicked(int slot_pos) {

  if(main_project == NULL) {
    error_dialog("Error", "You need to open a project first.");
    return;
  }

  RecognitionManager & rm = RecognitionManager::get_instance();

  std::tr1::shared_ptr<GfxEditorToolSelection<DegateRenderer> > selection_tool =
    std::tr1::dynamic_pointer_cast<GfxEditorToolSelection<DegateRenderer> >(editor.get_tool());

  BoundingBox bb = selection_tool != NULL && selection_tool->has_selection() ? 
    selection_tool->get_bounding_box() : main_project->get_bounding_box();

  rm.init(slot_pos, this, bb, main_project);

  if(rm.before_dialog(slot_pos)) {
    
    ipWin = std::tr1::shared_ptr<InProgressWin>
      (new InProgressWin(this, "Calculating", "Please wait while calculating.", rm.get_progress_control(slot_pos)));
    ipWin->show();
    
    signal_algorithm_finished_ = std::tr1::shared_ptr<Glib::Dispatcher>(new Glib::Dispatcher);
    
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
    PortColorsWin pcWin(this, main_project->get_port_color_manager());
    pcWin.run();

    apply_port_color_settings(main_project->get_logic_model(),
			      main_project->get_port_color_manager());

    editor.update_screen();
    project_changed();
  }
}


void MainWin::on_menu_gate_list() {
  if(main_project != NULL) {
    GateListWin glWin(this, main_project->get_logic_model(),
		      main_project->get_default_color(DEFAULT_COLOR_GATE_FRAME),
		      main_project->get_default_color(DEFAULT_COLOR_GATE));
    glWin.run();

    apply_port_color_settings(main_project->get_logic_model(),
			      main_project->get_port_color_manager());

    project_changed();
    editor.update_screen();

    apply_colors_to_gate_ports(main_project->get_logic_model(),
			       main_project->get_port_color_manager());
  }
}



void MainWin::on_menu_gate_orientation() {
  if(main_project) {

    if(Gate_shptr gate = selected_objects.get_single_object<Gate>()) {

      SetOrientationWin oWin(this, gate->get_orientation());
      Gate::ORIENTATION new_ori = oWin.run();

      if(new_ori != gate->get_orientation()) {
	gate->set_orientation(new_ori);
	main_project->get_logic_model()->update_ports(gate);
      }
      else {
	editor.update_screen();
	project_changed();
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

    if(yes_no_dialog("Warning", "Are you sure you want to remove all gates by that type(s)?")) {

      for(std::list<GateTemplate_shptr>::iterator iter = tmpl_set.begin();
	  iter != tmpl_set.end(); ++iter) {

	GateTemplate_shptr gate_template = *iter;

	lmodel->remove_gates_by_template_type(gate_template);

	if(yes_no_dialog("Warning", "Do you want to remove the gate definition, too?"))
	  lmodel->remove_gate_template(gate_template);
      }

      editor.update_screen();
      project_changed();
    }

  }

}

void MainWin::on_menu_gate_set_as_master() {
  if(main_project == NULL) return;

  /*
  if(!selected_objects.check_for_all(&is_of_object_type<Gate>)) {
    error_dialog("Error", "Can't continue, because the selection contains non gate objects.");
    return;
  }
  */

  merge_gate_images(main_project->get_logic_model(), selected_objects);
}

/**
 * Create a gate for a region.
 * The user selected a region. We will open a gate selection dialog,
 * let the user select a gate template and we create a gate of that type.
 */
void MainWin::on_menu_gate_set() {

  if(main_project == NULL) return;

  LogicModel_shptr lmodel = main_project->get_logic_model();

  std::tr1::shared_ptr<GfxEditorToolSelection<DegateRenderer> > selection_tool =
    std::tr1::dynamic_pointer_cast<GfxEditorToolSelection<DegateRenderer> >(editor.get_tool());

  if(selection_tool != NULL && selection_tool->has_selection()) {

    BoundingBox bbox = selection_tool->get_bounding_box();

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
      tmpl->set_width(bbox.get_width());
      tmpl->set_height(bbox.get_height());
    }

    Gate_shptr new_gate(new Gate(bbox));
    assert(new_gate != NULL);

    new_gate->set_gate_template(tmpl);
    lmodel->add_object(layer->get_layer_pos(), new_gate);

    new_gate->print();
    selection_tool->reset_selection();
    editor.update_screen();
    project_changed();

  }
  else if(Gate_shptr gate = selected_objects.get_single_object<Gate>()) {

    GateSelectWin gsWin(this, main_project->get_logic_model());
    std::list<GateTemplate_shptr> tmpl_list = gsWin.get_selection(false);

    if(tmpl_list.size() == 0) return;
    GateTemplate_shptr tmpl = *(tmpl_list.begin());

    gate->set_gate_template(tmpl);

    editor.update_screen();
    project_changed();

  }
}

void MainWin::on_menu_gate_create_by_selection() {
  if(main_project == NULL) return;

  std::tr1::shared_ptr<GfxEditorToolSelection<DegateRenderer> > selection_tool =
    std::tr1::dynamic_pointer_cast<GfxEditorToolSelection<DegateRenderer> >(editor.get_tool());

  if(selection_tool != NULL && selection_tool->has_selection()) {

    LogicModel_shptr lmodel = main_project->get_logic_model();
    Layer_shptr layer;

    try{
      layer = get_first_logic_layer(lmodel);
    }
    catch(CollectionLookupException const& ex) {
	error_dialog("Error", "There is no logic layer defined. Please define layer types.");
	return;
    }

    BoundingBox bbox = selection_tool->get_bounding_box();

    bool accept_dimension = false;
    int corridor_size = main_project->get_template_dimension();
    if(corridor_size == 0) {
      if(yes_no_dialog("Snap to Grid", 
		       "You have not definded a fixed width or height for gate templates yet. "
		       "If you want to use template matching and have to deal with up-down or "
		       "left-right flipped versions of a standard cell, you will most likely "
		       "use a grid in order to exactly align the standard cell along the grid lines. "
		       "Now you create your first gate. Degate can use its height or width to "
		       "define a corridor size between the nearest grid line and the height or width "
		       "of your first gate. If the corridor size is the width or a height depends "
		       "on your grid type. You should have defined either a horizontal or a vertical "
		       "grid. Do you want to use the gate's size for the corridor definition?")) {
	if(!check_grid_either_horizontal_or_vertical(main_project)) {
	  error_dialog("Error", "For this operation you must have a grid, which is either"
		       " horizontal or vertical.");
	  return;
	}

	accept_dimension = true;
      }
    }

    if(accept_dimension || 
       (corridor_size > 0 &&
	yes_no_dialog("Snap to Grid", "Should the gate be snapped to the corridor?"))) {
      // snap upper or left edge to nearest grid line
      // if corridor dimension is undefined, the bottom or right edge of the bounding
      // box is not touched.
      Grid::ORIENTATION orientation = snap_upper_or_left_edge_to_grid(main_project, 
								      bbox, corridor_size);

      if(accept_dimension) {
	if(orientation == Grid::HORIZONTAL)
	  main_project->set_template_dimension(bbox.get_width());
	else if(orientation == Grid::VERTICAL)
	  main_project->set_template_dimension(bbox.get_height());

	editor.set_corridor_size(main_project->get_template_dimension());	
	project_changed();
      }

    }


    GateTemplate_shptr tmpl(new GateTemplate(bbox.get_width(),
					     bbox.get_height() ));

    grab_template_images(lmodel, tmpl, bbox);

    GateConfigWin gcWin(this, main_project->get_logic_model(), tmpl,
			main_project->get_default_color(DEFAULT_COLOR_GATE_FRAME),
			main_project->get_default_color(DEFAULT_COLOR_GATE));

    if(gcWin.run() == true) {

      lmodel->add_gate_template(tmpl);

      Gate_shptr gate(new Gate(bbox, Gate::ORIENTATION_NORMAL));
      gate->set_gate_template(tmpl);
      lmodel->add_object(layer->get_layer_pos(), gate);

      selection_tool->reset_selection();
      editor.update_screen();
      project_changed();

    }
  }
}

void MainWin::on_menu_help_about() {
  char filename[PATH_MAX];
  snprintf(filename, PATH_MAX, "%s/icons/degate_logo.png", getenv("DEGATE_HOME"));

  Gtk::AboutDialog about_dialog;

  about_dialog.set_version("Version " DEGATE_VERSION);
  about_dialog.set_logo(Gdk::Pixbuf::create_from_file(filename));

  about_dialog.set_comments("Martin Schobert <martin@weltregierung.de>\n"
			    "This software is released under the\nGNU General Public License Version 3.\n"
			    "2010"
			    );
  about_dialog.set_website("http://www.degate.org/");
  about_dialog.run();
}


void MainWin::on_wire_added(unsigned int from_x, unsigned int from_y,
			    unsigned int to_x, unsigned int to_y) {

  if(main_project == NULL) return;
  Wire_shptr new_wire(new Wire(from_x, from_y, to_x, to_y,
			       main_project->get_default_wire_diameter()));

  assert(new_wire);

  LogicModel_shptr lmodel = main_project->get_logic_model();
  lmodel->add_object(lmodel->get_current_layer()->get_layer_pos(), new_wire);

  project_changed();
  editor.update_screen();
}

void MainWin::on_area_selection_activated(BoundingBox const& bbox) {
  if(main_project) {

    menu_manager->set_menu_item_sensitivity("/MenuBar/GateMenu/GateCreateBySelection", true);
    menu_manager->set_menu_item_sensitivity("/MenuBar/GateMenu/GateSet", true);
    menu_manager->set_menu_item_sensitivity("/MenuBar/LogicMenu/LogicCreateAnnotation", true);
    menu_manager->set_menu_item_sensitivity("/MenuBar/ProjectMenu/ProjectCreateSubproject", true);

    LogicModel_shptr lmodel = main_project->get_logic_model();
    Layer_shptr layer = lmodel->get_current_layer();

    for(Layer::qt_region_iterator iter = layer->region_begin(bbox);
	iter != layer->region_end(); ++iter) {

      PlacedLogicModelObject_shptr plo = *iter;
      assert(plo != NULL);

      selected_objects.add(plo);
      highlighted_objects.add(plo);
    }
   }
}

void MainWin::on_area_selection_resized(BoundingBox const& bbox) {
  if(main_project) {
    double px_per_um = main_project->get_pixel_per_um();
    if(px_per_um > 0) {
      boost::format f("width=%1%px, height=%2%px (w=%3%um, h=%4%um)");
      f % bbox.get_width() % bbox.get_height() 
	% (bbox.get_width() / px_per_um) % (bbox.get_height() / px_per_um);
      m_statusbar.push(f.str());
    }
    else {
      boost::format f("width=%1%px, height=%2%px");
      f % bbox.get_width() % bbox.get_height();
      m_statusbar.push(f.str());
    }
  }
}

void MainWin::on_area_selection_revoked() {
  if(main_project) {

    menu_manager->set_menu_item_sensitivity("/MenuBar/GateMenu/GateCreateBySelection", false);
    menu_manager->set_menu_item_sensitivity("/MenuBar/GateMenu/GateSet", false);
    menu_manager->set_menu_item_sensitivity("/MenuBar/LogicMenu/LogicCreateAnnotation", false);
    menu_manager->set_menu_item_sensitivity("/MenuBar/ProjectMenu/ProjectCreateSubproject", false);
  }
}


void MainWin::via_up_tool_clicked(unsigned int real_x, unsigned int real_y, unsigned int button) {
  via_tool_clicked(real_x, real_y, Via::DIRECTION_UP);
}

void MainWin::via_down_tool_clicked(unsigned int real_x, unsigned int real_y, unsigned int button) {
  via_tool_clicked(real_x, real_y, Via::DIRECTION_DOWN);
}

void MainWin::via_tool_clicked(unsigned int real_x, unsigned int real_y, degate::Via::DIRECTION dir) {
  if(main_project == NULL) return;
  Via_shptr new_via(new Via(real_x, real_y, main_project->get_default_pin_diameter(), dir));

  LogicModel_shptr lmodel = main_project->get_logic_model();
  lmodel->add_object(lmodel->get_current_layer()->get_layer_pos(), new_via);
  project_changed();
  editor.update_screen();
}

void MainWin::selection_tool_clicked(unsigned int real_x, unsigned int real_y, unsigned int button) {
  if(button == 3) {
    menu_manager->show_popup_menu(button, 0);
    // position is only needed for popup actions
    last_click_on_real_x = real_x;
    last_click_on_real_y = real_y;
  }
  else if(button == 1) {
    //!imgWin.selection_active()
    char buf[42];
    snprintf(buf, sizeof(buf), "%d, %d", real_x, real_y);
    m_statusbar.push(buf);
    object_clicked(real_x, real_y);
  }
}


bool MainWin::on_key_release_event_received(GdkEventKey * event) {
  if((event->keyval ==  GDK_Shift_L || event->keyval == GDK_Shift_R)) {
    shift_key_pressed = false;

    std::tr1::shared_ptr<GfxEditorToolWire<DegateRenderer> > tool =
      std::tr1::dynamic_pointer_cast<GfxEditorToolWire<DegateRenderer> >(editor.get_tool());

    if(tool != NULL) tool->set_shift_state(false);

    //imgWin.set_shift_key_state(false);
    //if(tool == TOOL_WIRE) imgWin.update_screen();
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

    std::tr1::shared_ptr<GfxEditorToolWire<DegateRenderer> > tool =
      std::tr1::dynamic_pointer_cast<GfxEditorToolWire<DegateRenderer> >(editor.get_tool());

    if(tool != NULL) tool->set_shift_state(true);

  }
  //else if(event->keyval == GDK_Control_L || event->keyval == GDK_Control_R) {
  else if(!(event->state & GDK_CONTROL_MASK) && (event->keyval == GDK_Control_L)) {
    control_key_pressed = true;
    //debug(TM, "ctrl pressed");
  }
  else if(event->state & GDK_CONTROL_MASK) {
    control_key_pressed = false;
    debug(TM, "ctrl as modifier released");
  }
  else if(event->keyval == GDK_space) {
    if(last_emarker) goto_last_emarker();
  }

  else if(event->keyval == GDK_a) editor.shift_viewport_left();
  else if(event->keyval == GDK_d) editor.shift_viewport_right();
  else if(event->keyval == GDK_w) editor.shift_viewport_up();
  else if(event->keyval == GDK_s) editor.shift_viewport_down();
  else if(event->keyval >= GDK_1 && event->keyval <= GDK_9) {
    set_layer(event->keyval - GDK_1);
  }

  //debug(TM, "key press: %d %d", event->state, event->keyval);
  return false;
}

void MainWin::clear_selection() {
  highlighted_objects.clear();
  selected_objects.clear();
}

void MainWin::update_gui_on_selection_change() {

  std::tr1::shared_ptr<GfxEditorToolSelection<DegateRenderer> > selection_tool =
    std::tr1::dynamic_pointer_cast<GfxEditorToolSelection<DegateRenderer> >(editor.get_tool());

  bool selection_active = selection_tool != NULL && selection_tool->has_selection();

  if(selected_objects.size() == 1) {
    ObjectSet::const_iterator it = selected_objects.begin();

    if(Gate_shptr gate = std::tr1::dynamic_pointer_cast<Gate>(*it)) {
      menu_manager->set_menu_item_sensitivity("/MenuBar/GateMenu/GateOrientation", true);
      menu_manager->set_menu_item_sensitivity("/MenuBar/GateMenu/GateSet", true);
    }
    else {
      menu_manager->set_menu_item_sensitivity("/MenuBar/GateMenu/GateSet", selection_active);
    }

    if(ciWin != NULL) ciWin->set_object(*it);

  }
  else {
    menu_manager->set_menu_item_sensitivity("/MenuBar/GateMenu/GateOrientation", false);
    menu_manager->set_menu_item_sensitivity("/MenuBar/GateMenu/GateSet", selection_active);

    /* Do not disable inspection here. Keep last settings. */
    //if(ciWin != NULL) ciWin->disable_inspection(); 
  }

  menu_manager->set_menu_item_sensitivity("/MenuBar/LogicMenu/LogicClearLogicModelInSelection",
					  selected_objects.check_for_all(&is_removable));

  menu_manager->set_menu_item_sensitivity("/MenuBar/LogicMenu/LogicInterconnect",
					  selected_objects.size() >= 2 &&
					  selected_objects.check_for_all(&is_interconnectable));

  menu_manager->set_menu_item_sensitivity("/MenuBar/LogicMenu/LogicIsolate",
					  selected_objects.size() >= 1 &&
					  selected_objects.check_for_all(&is_interconnectable));

  menu_manager->set_menu_item_sensitivity("/MenuBar/LogicMenu/LogicRemoveEntireNet",
					  selected_objects.size() == 1 &&
					  selected_objects.check_for_all(&is_interconnectable));

  menu_manager->set_menu_item_sensitivity("/MenuBar/LogicMenu/LogicMoveGateIntoModule",
					  selected_objects.check_for_all(&is_of_object_type<Gate>));

}

void MainWin::selection_tool_double_clicked(unsigned int real_x, unsigned int real_y,
					    unsigned int button) {
  if(main_project == NULL) return;

  LogicModel_shptr lmodel = main_project->get_logic_model();
  Layer_shptr layer = lmodel->get_current_layer();
  PlacedLogicModelObject_shptr plo = layer->get_object_at_position(real_x, real_y);

  if(SubProjectAnnotation_shptr sp =
     std::tr1::dynamic_pointer_cast<SubProjectAnnotation>(plo)) {

    std::string dir = join_pathes(main_project->get_project_directory(), sp->get_path());
    debug(TM, "Will open or create project at %s", dir.c_str());

    if(file_exists(dir))
      open_project(dir);
    else
      create_new_project(dir);
  }
}

void MainWin::object_clicked(unsigned int real_x, unsigned int real_y) {

  if(main_project == NULL) return;

  bool add_to_selection = false;
  const int max_distance = 4;

  // get info about selected object
  LogicModel_shptr lmodel = main_project->get_logic_model();
  Layer_shptr layer = lmodel->get_current_layer();
  PlacedLogicModelObject_shptr plo = layer->get_object_at_position(real_x, real_y, max_distance);

  // check if there is a gate or gate port on the logic layer
  if(plo == NULL) {
    try {
      layer = get_first_logic_layer(lmodel);
      plo = layer->get_object_at_position(real_x, real_y, max_distance);
    }
    catch(CollectionLookupException const& ex) {
    }
  }

  if(plo != NULL) {
    std::cout << "Object found." << std::endl;
    //plo->print();
    add_to_selection = true;

    boost::format f("object: %1%, description: %2%");
    f % plo->get_descriptive_identifier() % plo->get_description();
    m_statusbar.push(f.str());
  }


  std::set<PlacedLogicModelObject_shptr>::const_iterator it;

  // try to remove a single object
  if(plo != NULL && control_key_pressed == true && selected_objects.contains(plo)) {
    std::cout << "remove from selection\n";
    selected_objects.remove(plo);
    highlighted_objects.remove(plo);
    add_to_selection = false;
  }

  if(control_key_pressed == false){
    clear_selection();
  }


  if(add_to_selection) { // add to selection
    if(plo != NULL) {
      selected_objects.add(plo);
      highlighted_objects.add(plo, lmodel);
    }
  }

  editor.update_screen();
  update_gui_on_selection_change();
}


void MainWin::on_popup_menu_place_emarker() {
  if(main_project) {

    LogicModel_shptr lmodel = main_project->get_logic_model();
    Layer_shptr layer = lmodel->get_current_layer();

    EMarker_shptr emarker(new EMarker(last_click_on_real_x, last_click_on_real_y));
    lmodel->add_object(layer, emarker);
    last_emarker = emarker;
  }
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

      project_changed();
      editor.update_screen();
    }
  }
}

void MainWin::on_popup_menu_set_name() {
  if(main_project) {

    Glib::ustring name;

    if(selected_objects.empty()) {
      error_dialog("Error", "Please select one or more objects.");
      return;
    }
    else if(PlacedLogicModelObject_shptr plo = selected_objects.get_single_object<PlacedLogicModelObject>()) {
      name = plo->get_name();
    }

    GenericTextInputWin input(this, "Set name", "Please set a name", name);
    Glib::ustring str;
    if(input.run(name) == true) {

      for(ObjectSet::const_iterator it = selected_objects.begin(); it != selected_objects.end(); it++) {
	(*it)->set_name(name);
      }
      project_changed();
      editor.update_screen();
    }

  }
}

void MainWin::on_popup_menu_set_description() {
  if(main_project) {

    Glib::ustring description;

    if(selected_objects.empty()) {
      error_dialog("Error", "Please select one or more objects.");
      return;
    }
    else if(PlacedLogicModelObject_shptr plo = selected_objects.get_single_object<PlacedLogicModelObject>()) {
      description = plo->get_description();
    }

    GenericTextInputWin input(this, "Set description", "Please set a description", description);
    Glib::ustring str;
    if(input.run(description) == true) {

      for(ObjectSet::const_iterator it = selected_objects.begin(); it != selected_objects.end(); it++) {
	(*it)->set_description(description);
      }
      project_changed();
      editor.update_screen();
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
      project_changed();
      editor.update_screen();
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
      project_changed();
      editor.update_screen();
    }
  }
}


void MainWin::on_menu_logic_auto_name_gates(AutoNameGates::ORIENTATION orientation) {
  if(main_project != NULL) {

    if(!yes_no_dialog("Warning", 
		      "The operation may destroy previously set names. "
		      "Are you sure you want name all gates?")) return;

    try {
      Layer_shptr layer = get_first_logic_layer(main_project->get_logic_model());
    }
    catch(CollectionLookupException const& ex) {
      error_dialog("Error", "There is no logic layer defined.");
      return;
    }

    AutoNameGates auto_name(main_project->get_logic_model(), orientation);
    auto_name.run();

    project_changed();
  }
}


void MainWin::on_menu_logic_interconnect() {
  std::set<PlacedLogicModelObject_shptr>::const_iterator it;
  if(main_project && selected_objects.size() >= 2) {

    if(!selected_objects.check_for_all(&is_interconnectable)) {
      error_dialog("Error", "One of the objects you selected can not have connections at all.");
      return;
    }

    connect_objects(main_project->get_logic_model(), selected_objects.begin(), selected_objects.end());

    project_changed();
    editor.update_screen();
  }
}

void MainWin::on_menu_logic_isolate() {
  std::set<PlacedLogicModelObject_shptr>::const_iterator it;
  if(main_project && selected_objects.size() >= 1) {

    if(!selected_objects.check_for_all(&is_interconnectable)) {
      error_dialog("Error", "One of the objects you selected can not have connections at all.");
      return;
    }


    isolate_objects<std::set<PlacedLogicModelObject_shptr>::iterator>(main_project->get_logic_model(),
      selected_objects.begin(),
      selected_objects.end());

    project_changed();
    editor.update_screen();
  }

}

void MainWin::on_menu_logic_remove_entire_net() {
  if(!main_project || selected_objects.size() < 1) return;

  bool failed = false;
  if(yes_no_dialog("Warning", 
		   "Do you want to delete the net(s), which is "
		   "associated with the selected objects(?)")) {


    std::set<Net_shptr> l;
    BOOST_FOREACH(PlacedLogicModelObject_shptr plo, selected_objects) {
      if(ConnectedLogicModelObject_shptr clo = 
	 std::tr1::dynamic_pointer_cast<ConnectedLogicModelObject>(plo)) {
	if(clo->is_connected()) l.insert(clo->get_net());
      }
      else failed = true;
    }

    if(failed) {
      error_dialog("Error", "One of the objects you selected can not have connections at all.");
      return;
    }

    BOOST_FOREACH(Net_shptr net, l)
      remove_entire_net(main_project->get_logic_model(), net);

    if(l.size() > 0) {
      project_changed();
      editor.update_screen();
    }
  }
}

void MainWin::on_menu_logic_autointerconnect() {
  if(main_project == NULL) return;

  BoundingBox bbox = get_selection_bounding_box(editor, main_project);
  LogicModel_shptr lmodel = main_project->get_logic_model();
  autoconnect_objects(lmodel, lmodel->get_current_layer(), bbox);

  project_changed();
}

void MainWin::on_menu_logic_autointerconnect_interlayer() {
  if(main_project == NULL) return;

  LogicModel_shptr lmodel = main_project->get_logic_model();

  autoconnect_interlayer_objects(lmodel,
				 lmodel->get_current_layer(),
				 get_selection_bounding_box(editor, main_project));

  project_changed();

}

void MainWin::on_menu_logic_connection_inspector() {
  if(main_project != NULL && ciWin != NULL) ciWin->show();
}

void MainWin::on_menu_logic_rc() {
  if(main_project != NULL && rcWin != NULL) {
    rcWin->show();
    rcWin->run_checks();
  }
}


void MainWin::on_menu_logic_create_annotation() {
  if(main_project == NULL) return;

  std::tr1::shared_ptr<GfxEditorToolSelection<DegateRenderer> > selection_tool =
    std::tr1::dynamic_pointer_cast<GfxEditorToolSelection<DegateRenderer> >(editor.get_tool());

  if(selection_tool == NULL || !selection_tool->has_selection()) return;

  GenericTextInputWin input(this, "Add an Annotation", "Please enter a description.", "");
  Glib::ustring str;
  if(input.run(str)) {

    Annotation_shptr annotation(new Annotation(selection_tool->get_bounding_box()));
    annotation->set_name(str);
    LogicModel_shptr lmodel = main_project->get_logic_model();
    assert(lmodel != NULL);
    Layer_shptr layer = lmodel->get_current_layer();
    assert(layer != NULL);

    lmodel->add_object(layer->get_layer_pos(), annotation);

    project_changed();
    editor.update_screen();

    if(alWin != NULL) alWin->refresh();
  }
}

void MainWin::on_menu_logic_show_annotations() {
  if(main_project != NULL && alWin != NULL) alWin->show();
}

void MainWin::on_menu_logic_show_modules() {
  if(main_project != NULL && modWin != NULL) {
    modWin->show();
    project_changed();
  }
}

void MainWin::on_menu_move_gate_into_module() {
  if(main_project == NULL && !selected_objects.check_for_all(is_of_object_type<Gate>)) return;

  LogicModel_shptr lmodel = main_project->get_logic_model();

  SelectModuleWin smWin(this, lmodel);
  Module_shptr mod = smWin.show();
  if(mod != NULL) {

    for(ObjectSet::const_iterator it = selected_objects.begin();
	it != selected_objects.end(); it++) {

      Gate_shptr gate = std::tr1::dynamic_pointer_cast<Gate>(*it);
      assert(gate != NULL);


      Module_shptr root_module = lmodel->get_main_module();
      root_module->remove_gate(gate);
      mod->add_gate(gate);
    }

    modWin->update();
    project_changed();
  }

}

void MainWin::on_menu_view_grid_config() {

  if(main_project != NULL && gcWin != NULL) {
    gcWin->show();
  }

}

void MainWin::on_grid_config_changed() {
  if(gcWin) {
    project_changed();
    editor.update_screen();
  }
}

void MainWin::on_menu_layer_import_background() {
  Gtk::FileChooserDialog dialog("Please select a background image", Gtk::FILE_CHOOSER_ACTION_OPEN);
  dialog.set_transient_for(*this);
  //dialog.set_select_multiple(true);

  dialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
  dialog.add_button("Select", Gtk::RESPONSE_OK);
  add_image_file_filter_to_file_chooser_for_reading(dialog);

  int result = dialog.run();

  //Glib::SListHandle<Glib::ustring> filenames = dialog.get_filenames();
  Glib::ustring filename = dialog.get_filename();
  dialog.hide();

  switch(result) {
  case(Gtk::RESPONSE_OK):

    assert(ipWin == NULL);
    ipWin = std::tr1::shared_ptr<InProgressWin>
      (new InProgressWin(this, "Importing",
			 "Please wait while importing background image and calculating the prescaled images."));
    ipWin->show();
    project_changed();

    signal_bg_import_finished_.connect(sigc::mem_fun(*this, &MainWin::on_background_import_finished));
    Glib::Thread::create(sigc::bind<const Glib::ustring>
			 (sigc::mem_fun(*this, &MainWin::background_import_thread), filename), false);

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
    ipWin.reset();
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
      project_changed();
      set_layer(get_first_enabled_layer(main_project->get_logic_model()));
    }
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
    project_changed();
    editor.update_screen();
    ciWin->objects_removed();
    rcWin->objects_removed();
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
      if(std::tr1::dynamic_pointer_cast<GatePort>(*it) == NULL) // gate ports can't be removed directly
	lmodel->remove_object(*it);
    }

    selected_objects.clear();
    highlighted_objects.clear();

    menu_manager->set_menu_item_sensitivity("/MenuBar/LogicMenu/LogicClearLogicModelInSelection", false);
    project_changed();
    editor.update_screen();
    ciWin->objects_removed();
    rcWin->objects_removed();
  }
}

void MainWin::on_menu_layer_export_background_image() {
  if(main_project != NULL) {
    Layer_shptr layer = main_project->get_logic_model()->get_current_layer();
    if(layer->has_background_image()) {


      Gtk::FileChooserDialog dialog("Please select a file name", Gtk::FILE_CHOOSER_ACTION_SAVE);
      dialog.set_transient_for(*this);

      dialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
      dialog.add_button("Select", Gtk::RESPONSE_OK);
      add_image_file_filter_to_file_chooser_for_writing(dialog);

      int result = dialog.run();

      Glib::ustring filename = dialog.get_filename();
      dialog.hide();

      if(result == Gtk::RESPONSE_OK) {

	ScalingManager_shptr sm = layer->get_scaling_manager();
	assert(sm != NULL);

	BackgroundImage_shptr img = sm->get_image(1).second;
	assert(img != NULL);

	std::tr1::shared_ptr<GfxEditorToolSelection<DegateRenderer> > selection_tool =
	  std::tr1::dynamic_pointer_cast<GfxEditorToolSelection<DegateRenderer> >(editor.get_tool());

	if(selection_tool != NULL && selection_tool->has_selection())
	  save_part_of_image(filename, img, selection_tool->get_bounding_box());
	else
	  save_image(filename, img);

      }
    }
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
      layer->unset_image();
      /*
      BackgroundImage_shptr img = layer->get_image();
      clear_image<BackgroundImage>(img);
      */
      project_changed();
      //editor.update_screen();
      set_layer(layer); // easiest way to update the renderer
    }
    else
      debug(TM, "Has no background image. There is nothing to clear.");
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

bool MainWin::yes_no_dialog(const char * const title, const char * const message) {

  Gtk::MessageDialog dialog(*this, message, true, Gtk::MESSAGE_QUESTION, Gtk::BUTTONS_YES_NO);
  dialog.set_title(title);
  return dialog.run() == Gtk::RESPONSE_YES;
}
    
void MainWin::project_changed() {

  if(main_project != NULL) main_project->set_changed();
  update_title();
}

void MainWin::on_menu_project_push_changes() {
  if(main_project != NULL) {
    try {
      push_changes_to_server(main_project->get_server_url(),
			     main_project->get_logic_model());
      project_changed();
      editor.update_screen();
    }
    catch(XMLRPCException const& e) {
      error_dialog("XMLRPC failed", e.what());
    }
  }
}

void MainWin::on_menu_project_pull_changes() {
  if(main_project != NULL) {
    try {
      transaction_id_t tid = pull_changes_from_server(main_project->get_server_url(),
						      main_project->get_logic_model(),
						      main_project->get_last_pulled_tid());
      main_project->set_last_pulled_tid(tid);
      project_changed();
      editor.update_screen();
    }
    catch(XMLRPCException const& e) {
      error_dialog("XMLRPC failed", e.what());
    }
  }
}
