/* -*-c++-*-

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

#include <ProjectImporter.h>
#include <Project.h>
#include <FileSystem.h>
#include <ImageHelper.h>
#include <DegateHelper.h>

#include <string>
#include <iostream>
#include <fstream>

#include <boost/program_options.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/lexical_cast.hpp>

using namespace boost::program_options;
using namespace degate;

// function prototypes

void stats(std::ostream & os, Project_shptr prj);
void dump(std::ostream & os, Project_shptr prj, 
	  std::string const& out_dir, std::string const& rel_dir);
void render_cell_library(std::ostream & os, GateLibrary_shptr gate_lib, 
			 std::string const& out_dir, std::string const& rel_dir);
void render_cell(std::ostream & os, GateTemplate_shptr gate_tmpl, 
		 std::string const& out_dir, std::string const& rel_dir);

/**
 * Main program.
 */

int main(int argc, char ** argv) {

  // Parse program options.

  options_description desc("Options");
  desc.add_options()
    ("help", "Show help message.")
    ("project-dir", value<std::string>(), "Directory of the project to open.")
    ("out-dir", value<std::string>(), "Where to write the output files.")
    ("rel-dir", value<std::string>(), "Relative directory for images and source files.")
    ;

  variables_map vm;
  store(parse_command_line(argc, argv, desc), vm);
  notify(vm);    


  if(vm.count("help") || vm.empty() || 
     !vm.count("project-dir") || 
     !vm.count("out-dir")) {
    std::cout << desc << std::endl;
    return 1;
  }


  // Import project.

  ProjectImporter importer;
  Project_shptr prj(importer.import_all( vm["project-dir"].as<std::string>() ));


  // Create outfile.
  std::ofstream f("output.tex", std::ios::trunc | std:: ios::out);

  // Dump project data.
  stats(f, prj);
  dump(f, prj, vm["out-dir"].as<std::string>(), vm["rel-dir"].as<std::string>());
  f.close();

  return 1;
}





std::string section(std::string const& name, std::string const& label = "") {
  std::string ret;

  boost::format f("\\section{%1%}\n");
  f % name;
  ret = f.str();

  if(!label.empty()) {
    boost::format f2("\\label{%1%}\n\n");
    f2 % label;
    ret += f2.str();
  }
  return ret;
}

std::string subsection(std::string const& name, std::string const& label = "") {
  std::string ret;

  boost::format f("\\subsection{%1%}\n");
  f % name;
  ret = f.str();

  if(!label.empty()) {
    boost::format f2("\\label{%1%}\n\n");
    f2 % label;
    ret += f2.str();
  }

  return ret;
}

std::string subsubsection(std::string const& name) {
  boost::format f("\\subsubsection{%1%}\n");
  f % name;
  return f.str();  
}

std::string table_start(std::list<std::string> const& tbl_header) {

  std::list<std::string> prepared_tbl_header;
  BOOST_FOREACH(std::string const& s, tbl_header) {
    std::string s2("\\textbf{");
    s2 += s + "}";
    prepared_tbl_header.push_back(s2);
  }

  std::string cols;
  for(unsigned int i = 0; i < tbl_header.size(); i++) cols += "l";

  boost::format f("\\begin{center}\n"
		  "\\begin{longtable}{@{}%1%@{}}\n"
		  "%2% \\\\  \\midrule\n"
		  );
  f % cols 
    % boost::algorithm::join(prepared_tbl_header, "&");
  return f.str();
}

std::string table_end() {
  return 
    "\\end{longtable}\n"
    "\\end{center}\n\n";
}

std::string table_add_line(std::list<std::string> const& entry) {
  std::list<std::string> prepared_tbl_header;
  BOOST_FOREACH(std::string const& s, entry) {
    prepared_tbl_header.push_back(s);
  }

  return boost::algorithm::join(prepared_tbl_header, "&") + "\\\\\n";
}

std::string reference(std::string const& label) {
  boost::format f("\\ref{%1%}");
  f % label;
  return f.str();  
  
}

std::string newpage() {
  return "\\newpage\n";
}

std::string image(std::string const& filename, std::string const& caption) {

  boost::format f("\\begin{figure}[H]\n"
		  "\\begin{center}\n"
		  "\\includegraphics{%1%}\n"
		  "\\caption{%2%}\n"
		  //   "\\label{viaexamples}"
		  "\\end{center}\n"
		  "\\end{figure}\n\n");

  f % filename % caption;
  return f.str();
}

std::string listing(std::string const& filename, std::string const& caption, std::string const& label) {
  boost::format f("\\lstinputlisting[frame=single,caption=%1%,label=%2%]{%3%}\n");
  f % caption % label % filename;
  return f.str();
}

template<class Iterator>
size_t count(Iterator begin, Iterator end) {
  size_t i = 0;
  Iterator iter = begin;
  while(iter != end) { ++iter; i++; }
  return i;
}

void stats(std::ostream & os, Project_shptr prj) {
  os << section("Project Statistics");
  
  LogicModel_shptr lmodel = prj->get_logic_model();

  std::list<std::string> tbl_header;
  tbl_header.push_back("Object Type");
  tbl_header.push_back("Instances");

  // begin table
  os << table_start(tbl_header);

  std::list<std::string> entry;  
  entry.push_back("Number of Standard Cells");
  entry.push_back(boost::lexical_cast<std::string>(count(lmodel->gates_begin(), lmodel->gates_end())));
  os << table_add_line(entry);

  entry.clear();
  entry.push_back("Number of Nets");
  entry.push_back(boost::lexical_cast<std::string>(count(lmodel->nets_begin(), lmodel->nets_end())));
  os << table_add_line(entry);

  // end of table
  os << table_end();
  
}

void dump(std::ostream & os, Project_shptr prj, std::string const& out_dir, std::string const& rel_dir) {
  // render cell library
  render_cell_library(os, prj->get_logic_model()->get_gate_library(), out_dir, rel_dir);
}

void render_cell_library(std::ostream & os, GateLibrary_shptr gate_lib, 
			 std::string const& out_dir, std::string const& rel_dir) {

  std::list<std::string> tbl_header;
  tbl_header.push_back("Cell Name");
  tbl_header.push_back("Instances");
  tbl_header.push_back("Cell Description");
  tbl_header.push_back("Section");

  os << section("Gate Library")
     << subsection("Overview")
     << table_start(tbl_header);

  // Overview
  for(GateLibrary::template_iterator iter = gate_lib->begin();
      iter != gate_lib->end(); ++iter) {

    GateTemplate_shptr gate_tmpl((*iter).second);

    std::list<std::string> entry;
    entry.push_back(gate_tmpl->get_name());
    entry.push_back(boost::lexical_cast<std::string>(gate_tmpl->get_reference_counter()));
    entry.push_back(gate_tmpl->get_description());

    entry.push_back(reference(gate_tmpl->get_name()));

    os << table_add_line(entry);
  }
  
  os << table_end()
     << newpage();

  // Section for each standard cell
  for(GateLibrary::template_iterator iter = gate_lib->begin();
      iter != gate_lib->end(); ++iter) {

    GateTemplate_shptr gate_tmpl((*iter).second);

    render_cell(os, gate_tmpl, out_dir, rel_dir);
  }

}


std::string render_image(GateTemplate_shptr gate_tmpl,
			 std::string const& out_dir, 
			 Layer::LAYER_TYPE layer_type,
			 int resize_percent = 100) {

  
  int radius = 1;
  std::list<std::string> params;

  params.push_back("-resize");
  params.push_back(boost::lexical_cast<std::string>(resize_percent) + "%");

  params.push_back("-strokewidth");
  params.push_back(boost::lexical_cast<std::string>(1));


  // iterate over ports
  for(GateTemplate::port_iterator piter = gate_tmpl->ports_begin();
      piter != gate_tmpl->ports_end(); ++piter) {

    GateTemplatePort_shptr tmpl_port((*piter));

    int x = tmpl_port->get_x() * resize_percent / 100.0;
    int y = tmpl_port->get_y() * resize_percent / 100.0;

    //name = $port->getAttribute('name');

    params.push_back("-fill");
    params.push_back("red");
    params.push_back("-stroke");
    params.push_back("red");

    params.push_back("-draw");
    boost::format f("circle %1%,%2% %3%,%4%");
    f % x % y % x % (y + radius);
    params.push_back(f.str());

    params.push_back("-fill");
    params.push_back("white");
    params.push_back("-stroke");
    params.push_back("white");

    params.push_back("-draw");
    boost::format f2("text %1%,%2% \"%3%\"");
    f2 % (x+radius) % (y+radius) % tmpl_port->get_name();
    params.push_back(f2.str());    
  }
  

  // iterate over images
  for(GateTemplate::image_iterator img_iter = gate_tmpl->images_begin();
      img_iter != gate_tmpl->images_end(); ++img_iter) {

    Layer::LAYER_TYPE this_layer_type = (*img_iter).first;
    if(this_layer_type == layer_type) {
      GateTemplateImage_shptr img = (*img_iter).second;

      boost::format fmter("%1%_%2%.tif");
      fmter % gate_tmpl->get_object_id() % Layer::get_layer_type_as_string(layer_type);
      std::string filename(fmter.str());
      
      std::string fq_in_file = join_pathes(out_dir, filename);
      save_image<GateTemplateImage>(fq_in_file, img);
      
      std::string out_file = get_basename(filename)+ ".png";
      std::string fq_out_file = join_pathes(out_dir, out_file);
      
      std::list<std::string> cmd;
      cmd.push_back(fq_in_file);
      cmd.insert(cmd.end(), params.begin(), params.end());
      cmd.push_back(fq_out_file);
      
      execute_command("convert", cmd);
      
      remove_file(fq_in_file);

      return out_file;
    }
  }

  return "";
}


void render_cell(std::ostream & os, GateTemplate_shptr gate_tmpl, 
		 std::string const& out_dir, std::string const& rel_dir) {


  os << subsection("Standard Cell: " + gate_tmpl->get_name(), gate_tmpl->get_name())
     << subsubsection("Layout");

  // Render images

  int scale = 100;
  std::string f1 = render_image(gate_tmpl, out_dir, Layer::TRANSISTOR, scale);
  if(!f1.empty()) os << image(join_pathes(rel_dir, f1), "Layout on transistor layer");

  std::string f2 = render_image(gate_tmpl, out_dir, Layer::LOGIC, scale);
  if(!f2.empty()) os << image(join_pathes(rel_dir, f2), "Layout on M1");

  std::string f3 = render_image(gate_tmpl, out_dir, Layer::METAL, scale);
  if(!f3.empty()) os << image(join_pathes(rel_dir, f3), "Layout on M2");

  //std::string f3 = render_image(gate_tmpl, out_dir, Layer::LOGIC);


  // Render ports

  std::list<std::string> tbl_header;
  tbl_header.push_back("Port Name");
  tbl_header.push_back("Direction");
  tbl_header.push_back("Description");

  os
     << subsubsection("Ports")
     << table_start(tbl_header);
  
  for(GateTemplate::port_iterator piter = gate_tmpl->ports_begin();
      piter != gate_tmpl->ports_end(); ++piter) {

    GateTemplatePort_shptr tmpl_port((*piter));
    
    std::list<std::string> entry;
    entry.push_back(tmpl_port->get_name());
    entry.push_back(tmpl_port->get_port_type_as_string());
    entry.push_back(tmpl_port->get_description());

    os << table_add_line(entry);
  
  }
  os << table_end();


  for(GateTemplate::implementation_iter iter = gate_tmpl->implementations_begin();
      iter != gate_tmpl->implementations_end(); ++iter) {
    if(!iter->second.empty()) {

      os << subsubsection("Function");
      break;
    }
  }

  for(GateTemplate::implementation_iter iter = gate_tmpl->implementations_begin();
      iter != gate_tmpl->implementations_end(); ++iter) {

    GateTemplate::IMPLEMENTATION_TYPE t = iter->first;
    std::string const& code = iter->second;

    if(t != GateTemplate::UNDEFINED && !code.empty()) {

      boost::format fmter("%1%%2%.%3%");
      object_id_t id = gate_tmpl->get_object_id();
      std::string code_type;

      switch(t) {
      case GateTemplate::TEXT: 
	code_type = "Text"; 
	fmter % "" % id % "txt"; 
	break;
      case GateTemplate::VHDL: 
	code_type = "VHDL"; 
	fmter % "" % id % "vhdl"; 
	break;
      case GateTemplate::VHDL_TESTBENCH: 
	code_type = "VHDL Testbench"; 
	fmter % "test_" % id % "vhdl"; 
	break;
      case GateTemplate::VERILOG: 
	code_type = "Verilog";
	fmter % "" % id % "v";
	break;
      case GateTemplate::VERILOG_TESTBENCH: 
	code_type = "Verilog Testbench";
	fmter % "test_" % id % "v";
	break;
      default:
	assert(1 == 0);
      }

      std::string filename(fmter.str());

      write_string_to_file(join_pathes(out_dir, filename), code);
      os << listing(join_pathes(rel_dir, filename), code_type, filename);

    }
  }

  os << newpage();

}


