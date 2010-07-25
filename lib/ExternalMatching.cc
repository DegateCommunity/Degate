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

#include <ExternalMatching.h>
#include <BoundingBox.h>
#include <ImageHelper.h>
#include <DegateHelper.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>

using namespace degate;

ExternalMatching::ExternalMatching() {}


void ExternalMatching::init(BoundingBox const& bounding_box, Project_shptr project) 
  throw(InvalidPointerException, DegateRuntimeException) {
  
  this->bounding_box = bounding_box;

  if(project == NULL)
    throw InvalidPointerException("Invalid pointer for parameter project.");
  
  lmodel = project->get_logic_model();
  assert(lmodel != NULL); // always has a logic model
  
  layer = lmodel->get_current_layer();
  if(layer == NULL) throw DegateRuntimeException("No current layer in project.");
  
  
  ScalingManager_shptr sm = layer->get_scaling_manager();
  assert(sm != NULL);

  img = sm->get_image(1).second;
  assert(img != NULL);
}


void ExternalMatching::set_command(std::string const& cmd) {
  this->cmd = cmd;
}

std::string ExternalMatching::get_command() const {
  return cmd;
}

void ExternalMatching::run() {
  BackgroundImage_shptr part(new BackgroundImage(bounding_box.get_width(),
						 bounding_box.get_height()));

  // create a temp dir
  std::string dir = create_temp_directory();
  assert(is_directory(dir));
  
  std::string image_file = dir;
  image_file.append("/image.tiff");

  std::string results_file = dir;
  results_file.append("/results.dat");


  extract_partial_image(part, img, bounding_box);
  save_image(image_file, part);


  boost::format f("%1% --image %2% --results %3% "
		  "--start-x %4% --start-y %5% --width %6% --height %7%");
  f % cmd
    % image_file
    % results_file
    % bounding_box.get_min_x()
    % bounding_box.get_min_y()
    % bounding_box.get_width()
    % bounding_box.get_height();
		     

  debug(TM, "start external command: %s", f.str().c_str());
  exit_code = system(f.str().c_str());
  if(exit_code == -1) {
    debug(TM, "system() failed");
  }
  else {
    BOOST_FOREACH(PlacedLogicModelObject_shptr plo, 
		  parse_file(results_file)) {
      lmodel->add_object(layer, plo);
    }
  }

  // cleanup
  remove_directory(dir);
}

int ExternalMatching::get_exit_code() const {
  return WEXITSTATUS(exit_code);
}


std::list<PlacedLogicModelObject_shptr> ExternalMatching::parse_file(std::string const& filename) const {

  std::list<PlacedLogicModelObject_shptr> list;
  std::string line;
  std::ifstream file(filename.c_str());

  if(file.is_open()) {

    while(!file.eof()) {

      getline(file, line);
      
      PlacedLogicModelObject_shptr plo = parse_line(line);
      if(plo != NULL) list.push_back(plo);
    }
    file.close();
  }
  return list;
}

PlacedLogicModelObject_shptr ExternalMatching::parse_line(std::string const& line) const
  throw(DegateRuntimeException) {

  std::vector<std::string> tokens = tokenize(line);

  PlacedLogicModelObject_shptr plo;

  if(tokens.size() == 0) return plo;
  else if(tokens[0].at(0) == '#') return plo;
  else if(tokens[0] == "wire" and tokens.size() >= 6) {

    int 
      x1 = boost::lexical_cast<int>(tokens[1]),
      y1 = boost::lexical_cast<int>(tokens[2]),
      x2 = boost::lexical_cast<int>(tokens[3]),
      y2 = boost::lexical_cast<int>(tokens[4]),
      diameter = boost::lexical_cast<unsigned int>(tokens[5]);
    
    return Wire_shptr(new Wire(x1, y1, x2, y2, diameter));
  }
  else if(tokens[0] == "via" and tokens.size() >= 6) {

    int 
      x = boost::lexical_cast<int>(tokens[1]),
      y = boost::lexical_cast<int>(tokens[2]),
      diameter = boost::lexical_cast<unsigned int>(tokens[3]);
    
    Via::DIRECTION dir = tokens[4] == "up" ? 
      Via::DIRECTION_UP : Via::DIRECTION_DOWN;
    
    return Via_shptr(new Via(x, y, diameter, dir));
  }

  else {
    std::string err("Can't parse line: ");
    throw DegateRuntimeException(err + line);
  }
}


