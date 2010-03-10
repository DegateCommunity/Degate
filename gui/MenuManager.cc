#include "gui_globals.h"

#include "MenuManager.h"
#include <RecognitionManager.h>

#include <degate.h>
#include <iostream>
#include <assert.h>

using namespace Gtk::Menu_Helpers;
using namespace degate;

MenuManager::MenuManager(MainWin * wnd) {
  window = wnd;

  toolbar_visible = true;
  info_layers_visible = true;
  info_layers_checkbox_ignore_sig = false;

  create_popup_menu();
  create_menu();

  set_toolbar_images();
  set_widget_sensitivity(false);

}


void MenuManager::create_popup_menu() {

  Gtk::Menu::MenuList & l = popup_menu.items();

  l.push_back( MenuElem("Set port on gate", 
			sigc::mem_fun(*window, &MainWin::on_popup_menu_set_port) ));
  
  l.push_back( MenuElem("Set _name for object", 
			sigc::mem_fun(*window, &MainWin::on_popup_menu_set_name) ));
  
  
  l.push_back( SeparatorElem());
  
  l.push_back( MenuElem("Add a horizontal grid line", 
			sigc::mem_fun(*window, &MainWin::on_popup_menu_add_horizontal_grid_line) ));
  
  l.push_back( MenuElem("Add a vertical grid line", 
			sigc::mem_fun(*window, &MainWin::on_popup_menu_add_vertical_grid_line) ));
  
  
 
  popup_menu.accelerate(*window);
}

void MenuManager::show_popup_menu(guint button, guint32  activate_time) {
  popup_menu.popup(button, activate_time);

}



void MenuManager::create_menu() {

  m_refActionGroup = Gtk::ActionGroup::create();

  create_and_bind_project_menu();
  create_and_bind_view_menu();
  create_and_bind_tools_menu();
  create_and_bind_layer_menu();
  create_and_bind_logic_menu();
  create_and_bind_gate_menu();
  create_and_bind_recognition_menu();
  create_and_bind_help_menu();
  
  m_refUIManager = Gtk::UIManager::create();
  m_refUIManager->insert_action_group(m_refActionGroup);
  
  window->add_accel_group(m_refUIManager->get_accel_group());

  setup_menu_structure();
  initialize_menu_algorithm_funcs();
}


void MenuManager::create_and_bind_project_menu() {
  //Project
  m_refActionGroup->add(Gtk::Action::create("ProjectMenu", "Project"));

  m_refActionGroup->add(Gtk::Action::create("ProjectNew",
					    Gtk::Stock::NEW, "_New", "Create a new project"),
			sigc::mem_fun(*window, &MainWin::on_menu_project_new));

  m_refActionGroup->add(Gtk::Action::create("ProjectOpen",
					    Gtk::Stock::OPEN, "_Open", "Open an existing project"),
			sigc::mem_fun(*window, &MainWin::on_menu_project_open));

  m_refActionGroup->add(Gtk::Action::create("ProjectClose",
					    Gtk::Stock::CLOSE, "_Close", "Close a project"),
			sigc::mem_fun(*window, &MainWin::on_menu_project_close));

  m_refActionGroup->add(Gtk::Action::create("ProjectSave",
					    Gtk::Stock::SAVE, "_Save", "Save a project"),
			sigc::mem_fun(*window, &MainWin::on_menu_project_save));

  m_refActionGroup->add(Gtk::Action::create("ProjectCreateSubproject",
					    "Create subproject for selection", 
					    "Create subproject for selection"),
			sigc::mem_fun(*window, &MainWin::on_menu_project_create_subproject));

  m_refActionGroup->add(Gtk::Action::create("ProjectOpenParent",
					    "Open parent project", 
					    "Open parent project"),
			sigc::mem_fun(*window, &MainWin::on_menu_project_open_parent));

  m_refActionGroup->add(Gtk::Action::create("ProjectPushChanges",
					    "Push changes to server", 
					    "Push changes to server"),
			sigc::mem_fun(*window, &MainWin::on_menu_project_push_changes));

  m_refActionGroup->add(Gtk::Action::create("ProjectPullChanges",
					    "Pull changes from server", 
					    "Pull changes from server"),
			sigc::mem_fun(*window, &MainWin::on_menu_project_pull_changes));


  m_refActionGroup->add(Gtk::Action::create("ProjectSettings",
					    Gtk::Stock::PREFERENCES,
					    "Project settings", "Project settings"),
			sigc::mem_fun(*window, &MainWin::on_menu_project_settings));

  m_refActionGroup->add(Gtk::Action::create("ProjectExportArchive",
					    Gtk::Stock::HARDDISK,
					    "Create _archive", "Create an archive"),
			sigc::mem_fun(*window, &MainWin::on_menu_project_export_archive));

  m_refActionGroup->add(Gtk::Action::create("ExportViewAsGraphics",
					    "Export current view as XPM-graphics", "Export view as graphics"),
			sigc::mem_fun(*window, &MainWin::on_menu_project_export_view));

  m_refActionGroup->add(Gtk::Action::create("ExportLayerAsGraphics",
					    "Export current layer as XPM-graphics", "Export layer as graphics"),
			sigc::mem_fun(*window, &MainWin::on_menu_project_export_layer));

  
  Glib::RefPtr<Gtk::RecentAction> recent_action_ptr = Gtk::RecentAction::create("ProjectRecentProjects", "_Recent projects");
  recent_action_ptr->signal_item_activated().connect(sigc::mem_fun(*window, &MainWin::on_menu_project_recent_projects));
  m_refActionGroup->add(recent_action_ptr);

  Gtk::RecentFilter * recent_filter_ptr = Gtk::manage(new Gtk::RecentFilter());
  recent_filter_ptr->add_mime_type(degate_mime_type);
  recent_action_ptr->set_filter(*recent_filter_ptr);
  recent_action_ptr->set_show_numbers(true);
  recent_action_ptr->set_sort_type(Gtk::RECENT_SORT_MRU);
  

  m_refActionGroup->add(Gtk::Action::create("ProjectQuit", Gtk::Stock::QUIT),
			sigc::mem_fun(*window, &MainWin::on_menu_project_quit));
}

std::string MenuManager::get_recent_project_uri() {
  return Glib::RefPtr<Gtk::RecentAction>::cast_static
    (m_refActionGroup->get_action("ProjectRecentProjects"))->get_current_uri();
}

void MenuManager::create_and_bind_view_menu() {

  // View
  m_refActionGroup->add(Gtk::Action::create("ViewMenu", "View"));

  m_refActionGroup->add(Gtk::Action::create("ViewZoomIn",
					    Gtk::Stock::ZOOM_IN, "Zoom in", "Zoom in"),
			Gtk::AccelKey("plus"),
			sigc::mem_fun(*window, &MainWin::on_menu_view_zoom_in));

  m_refActionGroup->add(Gtk::Action::create("ViewZoomOut",
					    Gtk::Stock::ZOOM_OUT, "Zoom out", "Zoom out"),
			Gtk::AccelKey("minus"),
			sigc::mem_fun(*window, &MainWin::on_menu_view_zoom_out));

  m_refActionGroup->add(Gtk::Action::create("ViewNextLayer",
					    Gtk::Stock::GO_UP, "Layer up", "Layer up"),
			Gtk::AccelKey("<control>2"),
			sigc::mem_fun(*window, &MainWin::on_menu_view_next_layer));

  m_refActionGroup->add(Gtk::Action::create("ViewPrevLayer",
					    Gtk::Stock::GO_DOWN, "Layer down", "Layer down"),
			Gtk::AccelKey("<control>1"),
			sigc::mem_fun(*window, &MainWin::on_menu_view_prev_layer));

  m_refActionGroup->add(Gtk::Action::create("ViewGridConfiguration", Gtk::Stock::PREFERENCES,
					    "Grid configuration", "Grid configuration"),
			sigc::mem_fun(*window, &MainWin::on_menu_view_grid_config));

  m_refActionGroup->add(Gtk::Action::create("ViewToggleInfoLayer", 
					    "Disable all info layers", "Disable all info layers"),
			Gtk::AccelKey("<control>3"),
			sigc::mem_fun(*window, &MainWin::on_menu_view_toggle_all_info_layers));

  m_refActionGroup->add(Gtk::Action::create("ViewToggleToolbar", 
					    "Hide Toolbar", "Toggle toolbar visibility"),
			Gtk::AccelKey("<control>T"),
			sigc::mem_fun(*this, &MenuManager::toggle_toolbar_visibility));

  /*  m_refActionGroup->add(Gtk::Action::create("ViewDistanceToColor", "Define color for similarity filter", 
					    "Define color for similarity filter"),
			sigc::mem_fun(*window, &MainWin::on_menu_view_distance_to_color));
  */

}

void MenuManager::toggle_select_move_tool() {
  if(m_refChoice_Select->get_active()) m_refChoice_Move->activate();
  else if(m_refChoice_Move->get_active()) m_refChoice_Select->activate();
}

void MenuManager::create_and_bind_tools_menu() {

  // Choices menu, to demonstrate Radio items
  m_refActionGroup->add( Gtk::Action::create("ToolsMenu", "Tools") );
  Gtk::RadioAction::Group group_tools;
  
  m_refChoice_Select = Gtk::RadioAction::create(group_tools, "ToolSelect", "Select");
  m_refActionGroup->add(m_refChoice_Select, sigc::mem_fun(*window, &MainWin::on_menu_tools_select) );

  m_refChoice_Move = Gtk::RadioAction::create(group_tools, "ToolMove", "Move");
  m_refActionGroup->add(m_refChoice_Move, sigc::mem_fun(*window, &MainWin::on_menu_tools_move) );

  m_refChoice_Wire = Gtk::RadioAction::create(group_tools, "ToolWire", "Wire");
  m_refActionGroup->add(m_refChoice_Wire, sigc::mem_fun(*window, &MainWin::on_menu_tools_wire) );

  m_refChoice_via_up = Gtk::RadioAction::create(group_tools, "ToolViaUp", "Via up");
  m_refActionGroup->add(m_refChoice_via_up, sigc::mem_fun(*window, &MainWin::on_menu_tools_via_up) );

  m_refChoice_via_down = Gtk::RadioAction::create(group_tools, "ToolViaDown", "Via down");
  m_refActionGroup->add(m_refChoice_via_down, sigc::mem_fun(*window, &MainWin::on_menu_tools_via_down) );
}

void MenuManager::create_and_bind_layer_menu() {
  // Layer Menu
  m_refActionGroup->add( Gtk::Action::create("LayerMenu", "Layer"));

  m_refActionGroup->add(Gtk::Action::create("LayerImportBackground",
					    Gtk::Stock::MISSING_IMAGE,
					    "_Import background image", 
					    "Import background for current layer"),
			sigc::mem_fun(*window, &MainWin::on_menu_layer_import_background));

  m_refActionGroup->add(Gtk::Action::create("LayerClearBackgroundImage",
					    Gtk::Stock::CLEAR, "Clear background image", 
					    "Clear background image for current layer"),
			sigc::mem_fun(*window, &MainWin::on_menu_layer_clear_background_image));


  m_refActionGroup->add(Gtk::Action::create("LayerConfiguration",
					    "Layer configuration", "Layer configuration"),
			sigc::mem_fun(*window, &MainWin::on_menu_layer_configuration));

}

void MenuManager::create_and_bind_logic_menu() {

  // Logic menu
  m_refActionGroup->add( Gtk::Action::create("LogicMenu", "Logic"));

  m_refActionGroup->add(Gtk::Action::create("LogicInterconnect",
					    Gtk::Stock::CONNECT, "Interconnect objects", 
					    "Interconnect objects"),
			Gtk::AccelKey("<control>A"),
			sigc::mem_fun(*window, &MainWin::on_menu_logic_interconnect));

  m_refActionGroup->add(Gtk::Action::create("LogicIsolate",
					    Gtk::Stock::DISCONNECT, "Isolate object(s)", 
					    "Isolate object(s)"),
			Gtk::AccelKey("<control>U"),
			sigc::mem_fun(*window, &MainWin::on_menu_logic_isolate));

  m_refActionGroup->add(Gtk::Action::create("LogicClearLogicModel",
					    Gtk::Stock::CLEAR, "Clear logic model", 
					    "Clear logic model for current layer"),
			sigc::mem_fun(*window, &MainWin::on_menu_logic_clear_logic_model));

  m_refActionGroup->add(Gtk::Action::create("LogicClearLogicModelInSelection",
					    Gtk::Stock::CLEAR, "Remove selected objects", 
					    "Remove selected objects"),
			Gtk::AccelKey("<control>C"),
			sigc::mem_fun(*window, &MainWin::remove_objects));

  m_refActionGroup->add(Gtk::Action::create("LogicConnectionInspector", 
					    Gtk::Stock::EXECUTE, 
					    "Connection inspector", 
					    "Connection inspector"),
			Gtk::AccelKey("<control>I"),
			sigc::mem_fun(*window, &MainWin::on_menu_logic_connection_inspector));

  m_refActionGroup->add(Gtk::Action::create("LogicCreateAnnotation", 
					    "Create an annotation", 
					    "Create an annotaion"),
			sigc::mem_fun(*window, &MainWin::on_menu_logic_create_annotation));

  m_refActionGroup->add(Gtk::Action::create("LogicShowAnnotations", 
					    "Annotations", 
					    "Show list of annotations"),
			sigc::mem_fun(*window, &MainWin::on_menu_logic_show_annotations));

  m_refActionGroup->add(Gtk::Action::create("LogicShowModules", 
					    "Modules", 
					    "Show list of modules"),
			sigc::mem_fun(*window, &MainWin::on_menu_logic_show_modules));

  m_refActionGroup->add(Gtk::Action::create("LogicMoveGateIntoModule", 
					    "Move gate(s) into module", 
					    "Move gate(s) into module"),
			Gtk::AccelKey("<control>M"),
			sigc::mem_fun(*window, &MainWin::on_menu_move_gate_into_module));



}

void MenuManager::create_and_bind_gate_menu() {

  // Gate menu
  m_refActionGroup->add( Gtk::Action::create("GateMenu", "Gate"));
  m_refActionGroup->add(Gtk::Action::create("GateCreateBySelection",
					    "Create gate by selection", 
					    "Create a new gate"),
			sigc::mem_fun(*window, &MainWin::on_menu_gate_create_by_selection));

  m_refActionGroup->add(Gtk::Action::create("GateList",
					    Gtk::Stock::DND_MULTIPLE,
					    "Gate library", 
					    "Gate Library"),
			Gtk::AccelKey("<control>L"),
			sigc::mem_fun(*window, &MainWin::on_menu_gate_list));

  m_refActionGroup->add(Gtk::Action::create("GatePortColors",
					    Gtk::Stock::SELECT_COLOR,
					    "Port colors", 
					    "Define port colors"),
			sigc::mem_fun(*window, &MainWin::on_menu_gate_port_colors));

  m_refActionGroup->add(Gtk::Action::create("GateGotoGateByName",
					    Gtk::Stock::JUMP_TO,
					    "Goto gate by name", 
					    "Goto gate by name"),
			Gtk::AccelKey("<control>G"),
			sigc::mem_fun(*window, &MainWin::on_menu_goto_gate_by_name));

  m_refActionGroup->add(Gtk::Action::create("GateGotoGateByID",
					    Gtk::Stock::JUMP_TO,
					    "Goto gate by ID", 
					    "Goto gate by ID"),
			sigc::mem_fun(*window, &MainWin::on_menu_goto_gate_by_id));

  m_refActionGroup->add(Gtk::Action::create("GateSet",
					    "Set gate for selection", 
					    "Set gate for selection"),
			sigc::mem_fun(*window, &MainWin::on_menu_gate_set));

  m_refActionGroup->add(Gtk::Action::create("GateOrientation",
					    Gtk::Stock::ORIENTATION_REVERSE_LANDSCAPE,
					    "Set gate orientation", 
					    "Set gate orientation"),
			sigc::mem_fun(*window, &MainWin::on_menu_gate_orientation));

  m_refActionGroup->add(Gtk::Action::create("GateSetAsMaster",
					    "Set gate as master", 
					    "Set gate as master"),
			sigc::mem_fun(*window, &MainWin::on_menu_gate_set_as_master));

  m_refActionGroup->add(Gtk::Action::create("GateRemoveGateByType",
					    Gtk::Stock::CLEAR, "Remove gates by type ...", 
					    "Remove gates by type ..."),
			sigc::mem_fun(*window, &MainWin::on_menu_gate_remove_gate_by_type));


  m_refActionGroup->add(Gtk::Action::create("LogicAutoNameGatesAlongRows",
					    "Generate names for gates along rows",
					    "Generate names for gates along rows"),
			sigc::bind<AutoNameGates::ORIENTATION>(sigc::mem_fun(*window, 
									     &MainWin::on_menu_logic_auto_name_gates), 
							       AutoNameGates::ALONG_ROWS) );

  m_refActionGroup->add(Gtk::Action::create("LogicAutoNameGatesAlongCols",
					    "Generate names for gates along columns",
					    "Generate names for gates along columns"),
			sigc::bind<AutoNameGates::ORIENTATION>(sigc::mem_fun(*window, 
									     &MainWin::on_menu_logic_auto_name_gates), 
							       AutoNameGates::ALONG_COLS) );

}

void MenuManager::create_and_bind_recognition_menu() {
  // Recognition menu
  m_refActionGroup->add(Gtk::Action::create("RecognitionMenu", "Recognition"));
}


void MenuManager::create_and_bind_help_menu() {
  // Help menu:
  m_refActionGroup->add( Gtk::Action::create("HelpMenu", "Help") );
  m_refActionGroup->add( Gtk::Action::create("HelpAbout", Gtk::Stock::ABOUT, "About"),
			 sigc::mem_fun(*window, &MainWin::on_menu_help_about) );
  
}

void MenuManager::setup_menu_structure() {
  
  // Layout the actions in a menubar and toolbar:
  Glib::ustring ui_info = 
        "<ui>"
        "  <menubar name='MenuBar'>"
        "    <menu action='ProjectMenu'>"
        "      <menuitem action='ProjectNew'/>"
        "      <menuitem action='ProjectOpen'/>"
	"      <menuitem action='ProjectRecentProjects'/>"
        "      <menuitem action='ProjectClose'/>"
        "      <menuitem action='ProjectSave'/>"
        "      <menuitem action='ProjectExportArchive'/>"
        "      <separator/>"
        "      <menuitem action='ProjectCreateSubproject'/>"
        "      <menuitem action='ProjectOpenParent'/>"
        "      <separator/>"
        "      <menuitem action='ProjectPushChanges'/>"
        "      <menuitem action='ProjectPullChanges'/>"
        "      <separator/>"
        "      <menuitem action='ProjectSettings'/>"
        "      <separator/>"
        "      <menuitem action='ExportViewAsGraphics'/>"
        "      <menuitem action='ExportLayerAsGraphics'/>"
        "      <separator/>"
        "      <menuitem action='ProjectQuit'/>"
        "    </menu>"
        "    <menu action='ViewMenu'>"
        "      <menuitem action='ViewZoomIn'/>"
        "      <menuitem action='ViewZoomOut'/>"
        "      <separator/>"
        "      <menuitem action='ViewNextLayer'/>"
        "      <menuitem action='ViewPrevLayer'/>"
        "      <separator/>"
        "      <menuitem action='ViewGridConfiguration'/>"  
        "      <separator/>"
        "      <menuitem action='ViewToggleInfoLayer'/>"  
        "      <menuitem action='ViewToggleToolbar'/>"  
        "    </menu>"
        "    <menu action='ToolsMenu'>"
        "      <menuitem action='ToolSelect'/>"
        "      <menuitem action='ToolMove'/>"
        "      <menuitem action='ToolWire'/>"
        "      <menuitem action='ToolViaUp'/>"
        "      <menuitem action='ToolViaDown'/>"
        "    </menu>"

        "    <menu action='LayerMenu'>"
        "      <menuitem action='LayerImportBackground'/>"
        "      <menuitem action='LayerClearBackgroundImage'/>"
        "      <separator/>"
        "      <menuitem action='LayerConfiguration'/>"
        "    </menu>"
        "    <menu action='LogicMenu'>"
        "      <menuitem action='LogicInterconnect'/>"
        "      <menuitem action='LogicIsolate'/>"
        "      <separator/>"
        "      <menuitem action='LogicClearLogicModel'/>"
        "      <menuitem action='LogicClearLogicModelInSelection'/>"
        "      <separator/>"
        "      <menuitem action='LogicCreateAnnotation'/>"
        "      <menuitem action='LogicShowAnnotations'/>"
        "      <separator/>"
        "      <menuitem action='LogicShowModules'/>"
        "      <menuitem action='LogicMoveGateIntoModule'/>"
        "      <separator/>"
        "      <menuitem action='LogicConnectionInspector'/>"  
        "    </menu>"
        "    <menu action='GateMenu'>"
        "      <menuitem action='GateCreateBySelection'/>"
        "      <menuitem action='GateSet'/>"
        "      <menuitem action='GateOrientation'/>"
        "      <menuitem action='GateSetAsMaster'/>"
        "      <separator/>"
        "      <menuitem action='GatePortColors'/>"
        "      <separator/>"
        "      <menuitem action='GateGotoGateByName'/>"
        "      <menuitem action='GateGotoGateByID'/>"
        "      <separator/>"
        "      <menuitem action='GateRemoveGateByType'/>"
        "      <separator/>"
        "      <menuitem action='LogicAutoNameGatesAlongRows'/>"
        "      <menuitem action='LogicAutoNameGatesAlongCols'/>"
        "      <separator/>"
        "      <menuitem action='GateList'/>"
        "    </menu>"
        "    <menu action='RecognitionMenu'/>"
        "    <menu action='HelpMenu'>"
        "      <menuitem action='HelpAbout'/>"
        "    </menu>"
        "  </menubar>"
        "  <toolbar  name='ToolBar'>"
        "    <toolitem action='ViewZoomIn'/>"
        "    <toolitem action='ViewZoomOut'/>"
        "    <separator/>"
        "    <toolitem action='ViewPrevLayer'/>"
        "    <toolitem action='ViewNextLayer'/>"
        "    <separator/>"
        "    <toolitem action='ToolSelect'/>"
        "    <toolitem action='ToolMove'/>"
        "    <toolitem action='ToolWire'/>"
        "    <toolitem action='ToolViaUp'/>"
        "    <toolitem action='ToolViaDown'/>"
        "    <separator/>"
        "    <toolitem action='GateList'/>"
        "    <toolitem action='LogicConnectionInspector'/>"
        "    <toolitem action='LogicShowModules'/>"
        "    <toolitem action='LogicShowAnnotations'/>"
        "  </toolbar>"
    "</ui>";

#ifdef GLIBMM_EXCEPTIONS_ENABLED
  try {
    m_refUIManager->add_ui_from_string(ui_info);
  }
  catch(const Glib::Error& ex) {
    std::cerr << "building menus failed: " <<  ex.what();
  }
#else
  std::auto_ptr<Glib::Error> ex;
  m_refUIManager->add_ui_from_string(ui_info, ex);
  if(ex.get()) {
    std::cerr << "building menus failed: " <<  ex->what();
  }
#endif //GLIBMM_EXCEPTIONS_ENABLED

  // Get the menubar and toolbar widgets, and add them to a container widget:
  

}

Gtk::Widget* MenuManager::get_menubar() {

  return m_refUIManager->get_widget("/MenuBar");
}

Gtk::Widget* MenuManager::get_toolbar() {

  return m_refUIManager->get_widget("/ToolBar");
}


void MenuManager::set_toolbar_images() {

  set_image_for_toolbar_widget("/ToolBar/ToolSelect",  "tools_select.png");
  set_image_for_toolbar_widget("/ToolBar/ToolMove",    "tools_move.png");
  set_image_for_toolbar_widget("/ToolBar/ToolWire",    "tools_wire.png");
  set_image_for_toolbar_widget("/ToolBar/ToolViaUp",   "tools_via_up.png");
  set_image_for_toolbar_widget("/ToolBar/ToolViaDown", "tools_via_down.png");
  //set_image_for_toolbar_widget("/ToolBar/LogicShowAnnotations", "annotation.png");
}




void MenuManager::set_image_for_toolbar_widget(Glib::ustring toolbar_widget_path, 
					       Glib::ustring image_file_name) {

  char path[PATH_MAX];
  snprintf(path, PATH_MAX, "%s/icons/%s", 
	   getenv("DEGATE_HOME"), image_file_name.c_str());

  assert(file_exists(path) == true);

  Gtk::ToolButton* pToolbarItem;
  pToolbarItem = dynamic_cast<Gtk::ToolButton*>(m_refUIManager->get_widget(toolbar_widget_path));

  assert(pToolbarItem != NULL);
  if(pToolbarItem == NULL) return;
  
  Gtk::Image * m_Image = Gtk::manage(new Gtk::Image(path));
  assert(m_Image != NULL);
  if(m_Image == NULL) return;

  pToolbarItem->set_icon_widget(*m_Image);
}

void MenuManager::initialize_menu_algorithm_funcs() {

  Gtk::MenuItem * pMenuItem = dynamic_cast<Gtk::MenuItem*>(m_refUIManager->get_widget("/MenuBar/RecognitionMenu"));
  assert(pMenuItem != NULL);

  if(pMenuItem) {

    Gtk::Menu* pMenu = pMenuItem->get_submenu(); 
    assert(pMenu != NULL);

    if(pMenu) {
      (pMenu->items()).remove(pMenu->items()[1]); // XXX

      // create
      unsigned int i = 0;
      RecognitionManager * rm = RecognitionManager::get_instance();
      RecognitionManager::plugin_list plugins = rm->get_plugins();
      for(RecognitionManager::plugin_list::iterator iter = plugins.begin();
	  iter != plugins.end(); ++iter, i++) {

	RecognitionGUIBase * ui = *iter;
	assert(ui != NULL);

	Gtk::MenuItem *menuItem = new Gtk::MenuItem(ui->get_name());
	menuItem->show();
	menuItem->signal_activate().connect(sigc::bind(sigc::mem_fun(*window, 
								     &MainWin::on_algorithms_func_clicked), i));
    
	pMenu->add(*menuItem);
	pMenu->show();
	pMenuItem->show();

      }
    }
  }
}



void MenuManager::initialize_menu_render_funcs(const std::vector<Glib::ustring> & func_names, 
					       const std::vector<bool> & func_states) {

  assert(func_names.size() != 0);
  
  Gtk::MenuItem * pMenuItem = dynamic_cast<Gtk::MenuItem*>(m_refUIManager->get_widget("/MenuBar/ViewMenu/ViewPrevLayer"));
  if(pMenuItem) {
    Gtk::Menu * pMenu = dynamic_cast<Gtk::Menu*>(pMenuItem->get_parent());
    assert(pMenu != NULL);

    if(pMenu) {

      Gtk::SeparatorMenuItem * sepMenuItem = new Gtk::SeparatorMenuItem();
      pMenu->append(*sepMenuItem);
      sepMenuItem->show();

      slot_states.reserve(func_names.size());

      std::vector<Glib::ustring>::const_iterator i;
      int slot_pos = 0;
      for(i = func_names.begin(); i != func_names.end(); i++, slot_pos++) {
	//bool rf_initial_state = imgWin.get_renderer_func_enabled(slot_pos);

	Gtk::CheckMenuItem *menuItem = new Gtk::CheckMenuItem(*i, true);
	menuItem->set_active(func_states[slot_pos]);
	pMenu->append(*menuItem);
	menuItem->show();
	menuItem->signal_toggled().connect(sigc::bind<int>(sigc::mem_fun(*window, &MainWin::on_view_info_layer_toggled), 
							   slot_pos));

	std::pair<Gtk::CheckMenuItem *, bool> p(menuItem, func_states[slot_pos]);
	slot_states.push_back(p);
      }

      
    }
  }

}



void MenuManager::set_widget_sensitivity(bool state) {

  set_toolbar_item_sensitivity("/ToolBar/ViewZoomIn", state);
  set_toolbar_item_sensitivity("/ToolBar/ViewZoomOut", state);
  set_toolbar_item_sensitivity("/ToolBar/ViewPrevLayer", state);
  set_toolbar_item_sensitivity("/ToolBar/ViewNextLayer", state);

  set_toolbar_item_sensitivity("/ToolBar/ToolSelect", state);
  set_toolbar_item_sensitivity("/ToolBar/ToolMove", state);
  set_toolbar_item_sensitivity("/ToolBar/ToolWire", state);
  set_toolbar_item_sensitivity("/ToolBar/ToolViaUp", state);
  set_toolbar_item_sensitivity("/ToolBar/ToolViaDown", state);

  set_toolbar_item_sensitivity("/ToolBar/GateList", state);
  set_toolbar_item_sensitivity("/ToolBar/LogicConnectionInspector", state);
  set_toolbar_item_sensitivity("/ToolBar/LogicShowAnnotations", state);
  set_toolbar_item_sensitivity("/ToolBar/LogicShowModules", state);

  set_menu_item_sensitivity("/MenuBar/ProjectMenu/ProjectClose", state);
  set_menu_item_sensitivity("/MenuBar/ProjectMenu/ProjectSave", state);
  set_menu_item_sensitivity("/MenuBar/ProjectMenu/ProjectExportArchive", state);
  set_menu_item_sensitivity("/MenuBar/ProjectMenu/ProjectOpenParent", state);
  set_menu_item_sensitivity("/MenuBar/ProjectMenu/ProjectPushChanges", state);
  set_menu_item_sensitivity("/MenuBar/ProjectMenu/ProjectPullChanges", state);

  set_menu_item_sensitivity("/MenuBar/ProjectMenu/ProjectSettings", state);
  set_menu_item_sensitivity("/MenuBar/ProjectMenu/ExportViewAsGraphics", state);
  set_menu_item_sensitivity("/MenuBar/ProjectMenu/ExportLayerAsGraphics", state);
  set_menu_item_sensitivity("/MenuBar/ProjectMenu/ProjectCreateSubproject", state);

  set_menu_item_sensitivity("/MenuBar/ViewMenu/ViewZoomIn", state);
  set_menu_item_sensitivity("/MenuBar/ViewMenu/ViewZoomOut", state);
  set_menu_item_sensitivity("/MenuBar/ViewMenu/ViewNextLayer", state);
  set_menu_item_sensitivity("/MenuBar/ViewMenu/ViewPrevLayer", state);
  set_menu_item_sensitivity("/MenuBar/ViewMenu/ViewGridConfiguration", state);
  set_menu_item_sensitivity("/MenuBar/ViewMenu/ViewToggleInfoLayer", state);

  set_menu_item_sensitivity("/MenuBar/ToolsMenu/ToolSelect", state);
  set_menu_item_sensitivity("/MenuBar/ToolsMenu/ToolMove", state);
  set_menu_item_sensitivity("/MenuBar/ToolsMenu/ToolWire", state);
  set_menu_item_sensitivity("/MenuBar/ToolsMenu/ToolViaUp", state);
  set_menu_item_sensitivity("/MenuBar/ToolsMenu/ToolViaDown", state);

  set_menu_item_sensitivity("/MenuBar/LayerMenu/LayerImportBackground", state);
  set_menu_item_sensitivity("/MenuBar/LayerMenu/LayerClearBackgroundImage", state);
  set_menu_item_sensitivity("/MenuBar/LayerMenu/LayerConfiguration", state);

  if(state == false) {
    set_menu_item_sensitivity("/MenuBar/LogicMenu/LogicInterconnect", state);
    set_menu_item_sensitivity("/MenuBar/LogicMenu/LogicIsolate", state);
  }

  set_menu_item_sensitivity("/MenuBar/LogicMenu/LogicClearLogicModel", state);
  set_menu_item_sensitivity("/MenuBar/LogicMenu/LogicConnectionInspector", state);
  set_menu_item_sensitivity("/MenuBar/LogicMenu/LogicClearLogicModelInSelection", state);
  set_menu_item_sensitivity("/MenuBar/LogicMenu/LogicCreateAnnotation", state);
  set_menu_item_sensitivity("/MenuBar/LogicMenu/LogicShowAnnotations", state);
  set_menu_item_sensitivity("/MenuBar/LogicMenu/LogicShowModules", state);
  set_menu_item_sensitivity("/MenuBar/LogicMenu/LogicMoveGateIntoModule", state);

  set_menu_item_sensitivity("/MenuBar/GateMenu/GateList", state);
  set_menu_item_sensitivity("/MenuBar/GateMenu/GatePortColors", state);
  set_menu_item_sensitivity("/MenuBar/GateMenu/GateGotoGateByName", state);
  set_menu_item_sensitivity("/MenuBar/GateMenu/GateGotoGateByID", state);
  set_menu_item_sensitivity("/MenuBar/GateMenu/GateRemoveGateByType", state);

  set_menu_item_sensitivity("/MenuBar/GateMenu/GateCreateBySelection", state);
  set_menu_item_sensitivity("/MenuBar/GateMenu/GateSet", state);
  set_menu_item_sensitivity("/MenuBar/GateMenu/GateOrientation", state);
  set_menu_item_sensitivity("/MenuBar/GateMenu/GateSetAsMaster", state);
  set_menu_item_sensitivity("/MenuBar/GateMenu/GatePortColors", state);
  set_menu_item_sensitivity("/MenuBar/GateMenu/LogicAutoNameGatesAlongRows", state);
  set_menu_item_sensitivity("/MenuBar/GateMenu/LogicAutoNameGatesAlongCols", state);

}

void MenuManager::set_menu_item_sensitivity(const Glib::ustring& widget_path, bool state) {
  Gtk::MenuItem * pItem = dynamic_cast<Gtk::MenuItem*>(m_refUIManager->get_widget(widget_path));
#ifdef DEBUG
  if(pItem == NULL) debug(TM, "widget lookup failed for path: %s", widget_path.c_str());
#endif
  assert(pItem != NULL);
  if(pItem) pItem->set_sensitive(state);
}

void MenuManager::set_toolbar_item_sensitivity(const Glib::ustring& widget_path, bool state) {
  Gtk::Widget * pItem = m_refUIManager->get_widget(widget_path);
#ifdef DEBUG
  if(pItem == NULL) debug(TM, "widget lookup failed for path: %s", widget_path.c_str());
#endif
  assert(pItem != NULL);
  if(pItem) pItem->set_sensitive(state);
}


void MenuManager::toggle_toolbar_visibility() {
    Gtk::Widget* pToolbar = m_refUIManager->get_widget("/ToolBar") ;
    assert(pToolbar != NULL);
    if(pToolbar) {
      if(toolbar_visible == true)
	pToolbar->hide();
      else 
	pToolbar->show();

      toolbar_visible = toggle_menu_item("/MenuBar/ViewMenu/ViewToggleToolbar", toolbar_visible,
					 "Show toolbar", "Hide Toolbar");
    }

}


bool MenuManager::toggle_menu_item(Glib::ustring path, bool state, 
			       Glib::ustring text1, Glib::ustring text2) {

  Gtk::MenuItem * item = dynamic_cast<Gtk::MenuItem *>(m_refUIManager->get_widget(path));
  assert(item != NULL);
  if(item != NULL) {
    Gtk::Label * lab = dynamic_cast<Gtk::Label*>( item->get_child() ); 
    assert(lab != NULL);
    if(lab != NULL) lab->set_text(state == true ? text1 : text2);
  }

  return !state;
}



const std::vector<bool> MenuManager::toggle_info_layer_visibility() {

  int i = 0;
  std::vector<bool> new_states(slot_states.size());

  assert(slot_states.size() > 0);

  std::vector< std::pair<Gtk::CheckMenuItem *, bool> >::iterator it = slot_states.begin();
  if(slot_states.size() > 2) {
    new_states[i] = slot_states[i].second; i++;
    new_states[i] = slot_states[i].second; i++;
    it++; it++; 
  }

  info_layers_checkbox_ignore_sig = true; 

  while(it != slot_states.end()) {

    bool new_state = info_layers_visible == true ? false : (*it).second;
    ((*it).first)->set_active(new_state);

    new_states[i] = new_state;
    i++;
    it++;
  }
  info_layers_checkbox_ignore_sig = false;

  info_layers_visible = toggle_menu_item("/MenuBar/ViewMenu/ViewToggleInfoLayer", info_layers_visible,
					 "Enable all info layers", "Disable all info layers");

  return new_states;
}


/**
 *
 * @return true, if the checkbox was toggled
 */
bool MenuManager::toggle_info_layer(int slot_pos) {
  if(info_layers_checkbox_ignore_sig == false) {
    slot_states[slot_pos].second = !slot_states[slot_pos].second;
    return true;
  }
  return false;
}
