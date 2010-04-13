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

#include <RecognitionManager.h>
#include <TemplateMatchingGUI.h>
#include <WireMatchingGUI.h>

using namespace degate;

RecognitionManager * RecognitionManager::instance = NULL;


RecognitionManager::RecognitionManager() {

  TemplateMatchingNormal_shptr tm_normal(new TemplateMatchingNormal());
  TemplateMatchingInRows_shptr tm_in_rows(new TemplateMatchingInRows());
  TemplateMatchingInCols_shptr tm_in_cols(new TemplateMatchingInCols());

  plugins.push_back(new TemplateMatchingGUI(tm_normal, 
					    "Template matching") );

  plugins.push_back(new TemplateMatchingGUI(tm_in_rows, 
					    "Template matching along grid in rows") );

  plugins.push_back(new TemplateMatchingGUI(tm_in_cols, 
					    "Template matching along grid in columns") );

  WireMatching_shptr wire_matching(new WireMatching());
  plugins.push_back(new WireMatchingGUI(wire_matching, "Wire matching") );
}

RecognitionManager::~RecognitionManager() {
  for(plugin_list::iterator iter = plugins.begin();
      iter != plugins.end(); ++iter) {
    delete *iter;
  }
}

RecognitionManager * RecognitionManager::get_instance() {
  if(instance == NULL)
    instance = new RecognitionManager();
  return instance;
}

RecognitionManager::plugin_list RecognitionManager::get_plugins() {
  return plugins;
}
