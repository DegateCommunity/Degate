#include <AutoNameGates.h>
#include <LogicModelHelper.h>
#include <Layer.h>

#include <boost/foreach.hpp>

using namespace degate;

bool compare_min_x(const Gate_shptr lhs, const Gate_shptr rhs) {
  return lhs->get_min_x() < rhs->get_min_x();
}

bool compare_min_y(const Gate_shptr lhs, const Gate_shptr rhs) {
  return lhs->get_min_y() < rhs->get_min_y();
}

AutoNameGates::AutoNameGates(LogicModel_shptr lmodel, ORIENTATION orientation) :
  _lmodel(lmodel),
  _orientation(orientation) {
  layer = get_first_logic_layer(lmodel);
}

void AutoNameGates::run() {
  std::vector<unsigned int> histogram(std::max(_lmodel->get_width(), 
					       _lmodel->get_height()));

  fill_histogram(_lmodel, histogram, _orientation);
  std::list<int> scan_lines = scan_histogram(histogram);
  rename_gates(histogram, _orientation, scan_lines);
}

void AutoNameGates::rename_gates(std::vector<unsigned int> const & histogram, 
				 ORIENTATION orientation,
				 std::list<int> const& scan_lines) const {
  
  unsigned int col_num = 0;
  unsigned int row_num = 0;

  BOOST_FOREACH(int i, scan_lines) {

    // naming = along-rows => histogram along y-axis, following scanlines along x-axis

    
    // collect all gates placed along scanline
    
    std::list<Gate_shptr> gate_list;
    
    BoundingBox bbox(orientation == ALONG_ROWS ? 0 : i, 
		     orientation == ALONG_ROWS ? layer->get_width() - 1 : i, 
		     orientation == ALONG_COLS ? 0 : i, 
		     orientation == ALONG_COLS ? layer->get_height() - 1 : i);
    
    for(Layer::qt_region_iterator iter = layer->region_begin(bbox); 
	iter != layer->region_end(); ++iter) {
      if(Gate_shptr gate = std::tr1::dynamic_pointer_cast<Gate>(*iter))
	gate_list.push_back(gate);
    }
    
    // sort gate list according to their min_x or min_y
    if(orientation == ALONG_ROWS) gate_list.sort(compare_min_x);
    else gate_list.sort(compare_min_y);
    
    // rename gates
    BOOST_FOREACH(Gate_shptr gate, gate_list) {
      boost::format f("%1%.%2%");
      f % row_num % col_num;
      gate->set_name(f.str());
      
      // next row or col
      if(_orientation == ALONG_ROWS) col_num++;
      else row_num++;      
    }
    
    // next row or col
    if(_orientation == ALONG_ROWS) { row_num++; col_num = 0; }
    else { col_num++; row_num = 0; }
  }
}

void AutoNameGates::fill_histogram(LogicModel_shptr lmodel,
				   std::vector<unsigned int> & histogram, 
				   ORIENTATION orientation) const {

  int x, y;

  // iterate over gates
  for(LogicModel::gate_collection::iterator iter = lmodel->gates_begin();
      iter != lmodel->gates_end(); ++iter) {
    Gate_shptr gate = (*iter).second;
    assert(gate != NULL);
    
    if(_orientation == ALONG_COLS) 
      for(x = gate->get_min_x(); x < gate->get_max_x(); x++)
	histogram[x]++;

    if(_orientation == ALONG_ROWS) 
      for(y = gate->get_min_y(); y < gate->get_max_y(); y++)
	histogram[y]++;
  }
}

std::list<int> AutoNameGates::scan_histogram(std::vector<unsigned int> const & histogram) const {

  std::list<int> scan_lines;

  unsigned int from = 0;
  unsigned int i;
  for(i = 0; i < histogram.size(); i++) {
    if(histogram[i] > 0 && from == 0) from = i;
    if(histogram[i] == 0 && from > 0) {
      scan_lines.push_back(from + (i - from)/2);
      from = 0;
    }
  }
  return scan_lines;
}

