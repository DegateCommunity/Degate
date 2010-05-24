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


#include <TemplateMatching.h>
#include <Image.h>
#include <globals.h>
#include <algorithm>
#include <Statistics.h>
#include <ImageHelper.h>

#include <utility>

#include <math.h>

using namespace degate;

TemplateMatching::TemplateMatching() {
  threshold_hc = 0.45;
  threshold_detection = 0.7;
  max_step_size_search = 3;
  scale_down = 2;
  threshold_steps = 1;
}

TemplateMatching::~TemplateMatching() {
}

void TemplateMatching::precalc_sum_tables(TileImage_GS_BYTE_shptr img,
					  TileImage_GS_DOUBLE_shptr summation_table_single,
					  TileImage_GS_DOUBLE_shptr summation_table_squared) {

  unsigned int x, y;

  for(y = 0; y < img->get_height(); y++)
    for(x = 0; x < img->get_width(); x++) {
      summation_table_single->set_pixel(x, y, 0);
      summation_table_squared->set_pixel(x, y, 0);
    }
  
  for(y = 0; y < img->get_height(); y++)
    for(x = 0; x < img->get_width(); x++) {

      double f = img->get_pixel_as<gs_double_pixel_t>(x, y);

      double s_l = x > 0 ? summation_table_single->get_pixel(x - 1, y) : 0;
      double s_o = y > 0 ? summation_table_single->get_pixel(x, y - 1) : 0;
      double s_lo = x > 0 && y > 0 ? summation_table_single->get_pixel(x - 1, y - 1) : 0;


      double s2_l = x > 0 ? summation_table_squared->get_pixel(x - 1, y) : 0;
      double s2_o = y > 0 ? summation_table_squared->get_pixel(x, y - 1) : 0;
      double s2_lo = x > 0 && y > 0 ? summation_table_squared->get_pixel(x - 1, y - 1) : 0;

      double f1 = f + s_l + s_o - s_lo;
      double f2 = f*f + s2_l + s2_o - s2_lo;

      summation_table_single->set_pixel(x, y, f1);
      summation_table_squared->set_pixel(x, y, f2);
    }
  
}



void TemplateMatching::init(BoundingBox const& bounding_box, Project_shptr project) {

  assert(project != NULL);
  
  this->project = project;
  this->bounding_box = bounding_box;

  ScalingManager_shptr sm = layer_matching->get_scaling_manager();
  
  prepare_background_images(sm, bounding_box, get_scaling_factor());
  prepare_sum_tables(gs_img_normal, gs_img_scaled);

  set_progress(0);
}


BoundingBox TemplateMatching::get_scaled_bounding_box(BoundingBox const& bounding_box, 
						      double scale_down) const {
  return BoundingBox(lrint(bounding_box.get_min_x() / scale_down),
		     lrint(bounding_box.get_max_x() / scale_down),
		     lrint(bounding_box.get_min_y() / scale_down),
		     lrint(bounding_box.get_max_y() / scale_down));
}

void TemplateMatching::prepare_background_images(ScalingManager_shptr sm, 
						 BoundingBox const& bounding_box, 
						 unsigned int scaling_factor) {

  // Get the normal background image and the scaled background image
  // These images are in RGBA format.
  const ScalingManager<BackgroundImage>::image_map_element i1 = 
    sm->get_image(1);
  const ScalingManager<BackgroundImage>::image_map_element i2 = 
    sm->get_image(scaling_factor);

  assert(i1.second != NULL);
  assert(i2.second != NULL);
  assert(i2.first  == get_scaling_factor());

  BackgroundImage_shptr img_normal = i1.second;
  BackgroundImage_shptr img_scaled = i2.second;


  // Create a greyscaled image for the normal
  // unscaled background image and the scaled version.
  BoundingBox scaled_bounding_box = 
    get_scaled_bounding_box(bounding_box, scaling_factor);

  gs_img_normal = TileImage_GS_BYTE_shptr(new TileImage_GS_BYTE(bounding_box.get_width(),
								bounding_box.get_height()));

  gs_img_scaled = TileImage_GS_BYTE_shptr(new TileImage_GS_BYTE(bounding_box.get_width(),
								bounding_box.get_height()));

  extract_partial_image<TileImage_GS_BYTE, BackgroundImage>(gs_img_normal, 
							    img_normal, 
							    bounding_box);

  if(scaling_factor == 1)
    gs_img_scaled = gs_img_normal;
  else
    extract_partial_image<TileImage_GS_BYTE, BackgroundImage>(gs_img_scaled, 
							      img_scaled, 
							      scaled_bounding_box);

}

void TemplateMatching::prepare_sum_tables(TileImage_GS_BYTE_shptr gs_img_normal,
					  TileImage_GS_BYTE_shptr gs_img_scaled) {

  unsigned int 
    w_n = gs_img_normal->get_width(),
    h_n = gs_img_normal->get_height(),
    w_s = gs_img_scaled->get_width(),
    h_s = gs_img_scaled->get_height();

  sum_table_single_normal = TileImage_GS_DOUBLE_shptr(new TileImage_GS_DOUBLE(w_n, h_n));
  sum_table_squared_normal = TileImage_GS_DOUBLE_shptr(new TileImage_GS_DOUBLE(w_n, h_n));
  sum_table_single_scaled = TileImage_GS_DOUBLE_shptr(new TileImage_GS_DOUBLE(w_s, h_s));
  sum_table_squared_scaled = TileImage_GS_DOUBLE_shptr(new TileImage_GS_DOUBLE(w_s, h_s));
  
  precalc_sum_tables(gs_img_normal, sum_table_single_normal, sum_table_squared_normal);
  precalc_sum_tables(gs_img_scaled, sum_table_single_scaled, sum_table_squared_scaled);
}


bool compare_template_size(const GateTemplate_shptr lhs, const GateTemplate_shptr rhs) {
  return lhs->get_width() * lhs->get_height() > rhs->get_width() * rhs->get_height();
} 

void TemplateMatching::set_templates(std::list<GateTemplate_shptr> tmpl_set) {
  this->tmpl_set = tmpl_set;
  this->tmpl_set.sort(compare_template_size);
}

void TemplateMatching::set_orientations(std::list<Gate::ORIENTATION> tmpl_orientations) {
  this->tmpl_orientations = tmpl_orientations;
  for(std::list<Gate::ORIENTATION>::const_iterator iter = tmpl_orientations.begin();
      iter != tmpl_orientations.end(); ++iter) {
    debug(TM, "Template orientation to match: %d", *iter);
  }
}

void TemplateMatching::run() {


  debug(TM, "run template matching");

  set_progress_step_size(1.0/(threshold_steps * tmpl_set.size() * tmpl_orientations.size()));

  for(unsigned int i = 1; i <= threshold_steps; i++) {

    for(std::list<GateTemplate_shptr>::const_iterator iter = tmpl_set.begin();
	iter != tmpl_set.end(); ++iter) {

      GateTemplate_shptr tmpl = *iter;
      
      for(std::list<Gate::ORIENTATION>::const_iterator iter = tmpl_orientations.begin();
	  iter != tmpl_orientations.end(); ++iter) {
	
	double 
	  t_hc = get_current_threshold(get_threshold_hc(), threshold_steps, i),
	  t_det = get_current_threshold(get_threshold_detection(), threshold_steps, i);

	debug(TM, 
	      "check template %s for orientation %d.\n"
	      "threshold for hill climbing = %f\n"
	      "threshold for detection = %f\n", tmpl->get_name().c_str(), *iter, t_hc, t_det);
	
	prepared_template prep_tmpl_img = prepare_template(tmpl, *iter);
	match_single_template(prep_tmpl_img, t_hc, t_det);


	progress_step_done();
	if(is_canceled()) return;
      }
      
    }
  }

}


double TemplateMatching::subtract_mean(TempImage_GS_BYTE_shptr img, 
				       TempImage_GS_DOUBLE_shptr zero_mean_img) const {
  
  double mean = average<TempImage_GS_BYTE>(img);

  double sum_over_zero_mean_img = 0;
  unsigned int x, y;

  for(y = 0; y < img->get_height(); y++) 
    for(x = 0; x < img->get_width(); x++) {
      double tmp = img->get_pixel_as<double>(x, y) - mean;
      zero_mean_img->set_pixel(x, y, tmp);
      sum_over_zero_mean_img += tmp * tmp;
    }


  return sum_over_zero_mean_img;
}

TemplateMatching::prepared_template TemplateMatching::prepare_template(GateTemplate_shptr tmpl, 
								       Gate::ORIENTATION orientation) {

  prepared_template prep;

  assert(layer_matching->get_layer_type() != Layer::UNDEFINED);
  assert(tmpl->has_image(layer_matching->get_layer_type()));

  prep.gate_template = tmpl;
  prep.orientation = orientation;

  // get image from template
  GateTemplateImage_shptr tmpl_img_orig = tmpl->get_image(layer_matching->get_layer_type());

  unsigned int 
    w = tmpl_img_orig->get_width(),
    h = tmpl_img_orig->get_height();

  // get image according to orientation
  GateTemplateImage_shptr tmpl_img(new GateTemplateImage(w, h));
  copy_image<GateTemplateImage, GateTemplateImage>(tmpl_img, tmpl_img_orig);

  switch(orientation) {
  case Gate::ORIENTATION_FLIPPED_UP_DOWN:
    flip_up_down<GateTemplateImage>(tmpl_img);
    break;
  case Gate::ORIENTATION_FLIPPED_LEFT_RIGHT:
    flip_left_right<GateTemplateImage>(tmpl_img);
    break;
  case Gate::ORIENTATION_FLIPPED_BOTH:
    flip_both<GateTemplateImage>(tmpl_img);
    break;
  case Gate::ORIENTATION_NORMAL:
    break;
  case Gate::ORIENTATION_UNDEFINED:
    assert(1 == 0); // it is already checked
  }

  // create a scaled version of the template image

  unsigned int
    scaled_tmpl_width = (double)w / get_scaling_factor(),
    scaled_tmpl_height = (double)h / get_scaling_factor();

  prep.tmpl_img_normal = TempImage_GS_BYTE_shptr(new TempImage_GS_BYTE(w, h));
  copy_image<TempImage_GS_BYTE, GateTemplateImage>(prep.tmpl_img_normal, tmpl_img);
  
  prep.tmpl_img_scaled = TempImage_GS_BYTE_shptr(new TempImage_GS_BYTE(scaled_tmpl_width, 
								       scaled_tmpl_height));
  
  scale_down_by_power_of_2<TempImage_GS_BYTE, GateTemplateImage>(prep.tmpl_img_scaled, tmpl_img);


  // create zero-mean templates
  prep.zero_mean_template_normal = TempImage_GS_DOUBLE_shptr(new TempImage_GS_DOUBLE(w, h));
  prep.zero_mean_template_scaled = TempImage_GS_DOUBLE_shptr(new TempImage_GS_DOUBLE(scaled_tmpl_width, 
										     scaled_tmpl_height));


  // subtract mean
  
  prep.sum_over_zero_mean_template_normal = subtract_mean(prep.tmpl_img_normal, 
							  prep.zero_mean_template_normal);
  prep.sum_over_zero_mean_template_scaled = subtract_mean(prep.tmpl_img_scaled, 
							  prep.zero_mean_template_scaled);


  assert(prep.sum_over_zero_mean_template_normal > 0);
  assert(prep.sum_over_zero_mean_template_scaled > 0);

  return prep;
}



void TemplateMatching::adjust_step_size(struct search_state & state, double corr_val) const {
  if(corr_val > 0) {
    state.step_size_search = std::max(1.0, rint((1.0 - (double)get_max_step_size()) * 
						corr_val + get_max_step_size()));
  }
  
  else state.step_size_search = get_max_step_size();
}

void TemplateMatching::add_gate(unsigned int x, unsigned int y,
				struct prepared_template & tmpl) {

  // XXXX critical section

  if(!layer_insert->exists_gate_in_region(x, x + tmpl.gate_template->get_width(),
					  y, y + tmpl.gate_template->get_height())) {

    Gate_shptr gate(new Gate(x, x + tmpl.gate_template->get_width(),
			     y, y + tmpl.gate_template->get_height(),
			     tmpl.orientation));
    gate->set_gate_template(tmpl.gate_template);
    
    LogicModel_shptr lmodel = project->get_logic_model();
    lmodel->add_object(layer_insert->get_layer_pos(), gate);
    lmodel->update_ports(gate);
  }
}

void TemplateMatching::match_single_template(struct prepared_template & tmpl,
					     double threshold_hc, double threshold_detection) {

  debug(TM, "match_single_template(): start iterating over background image");
  search_state state;
  state.x = 0;
  state.y = 0;
  state.step_size_search = get_max_step_size();
  state.search_area = bounding_box;

  double max_corr_for_search = -1;

  do { // works on unscaled, but cropped image

    double corr_val = calc_single_xcorr(gs_img_scaled, 
					sum_table_single_scaled,
					sum_table_squared_scaled,
					tmpl.zero_mean_template_scaled,
					tmpl.sum_over_zero_mean_template_scaled, 
					lrint((double)state.x / get_scaling_factor()),
					lrint((double)state.y / get_scaling_factor()));

    if(corr_val > max_corr_for_search) max_corr_for_search = corr_val;


    adjust_step_size(state, corr_val);  
    
    if(corr_val >= threshold_hc) {
      debug(TM, "start hill climbing at(%d,%d), corr=%f", state.x, state.y, corr_val);
      unsigned int max_corr_x, max_corr_y;
      double curr_max_val;
      hill_climbing(state.x, state.y, corr_val,
		    &max_corr_x, &max_corr_y, &curr_max_val,
		    gs_img_normal, tmpl.zero_mean_template_normal,
		    tmpl.sum_over_zero_mean_template_normal);

      debug(TM, "hill climbing returned for (%d,%d) corr=%f", max_corr_x, max_corr_y, curr_max_val);
      if(curr_max_val >= threshold_detection) {
	add_gate(max_corr_x + bounding_box.get_min_x(),
		 max_corr_y + bounding_box.get_min_y(),
		 tmpl);
      }

    }

  } while(get_next_pos(&state, tmpl) && !is_canceled());

  std::cout << "The maximum correlation value for the current template and orientation is " << max_corr_for_search << std::endl;

}


void TemplateMatching::hill_climbing(unsigned int start_x, unsigned int start_y, double xcorr_val,
				     unsigned int * max_corr_x_out,
				     unsigned int * max_corr_y_out,
				     double * max_xcorr_out,
				     const TileImage_GS_BYTE_shptr master,
				     const TempImage_GS_DOUBLE_shptr zero_mean_template,
				     double sum_over_zero_mean_template) const {

  unsigned int max_corr_x = start_x;
  unsigned int max_corr_y = start_y;

  double max_corr = xcorr_val;
  bool running = true;

  //std::list<std::pair<unsigned int, unsigned int> > positions;

  const unsigned int radius = get_max_step_size();
  const unsigned int size = (2 * radius + 1) * (2 * radius + 1);

  unsigned int positions_x[size], positions_y[size];

  while(running) {
    running = false;

    // first generate positions

  unsigned int
    from_x = max_corr_x >= radius ? max_corr_x - radius : 0,
    from_y = max_corr_y >= radius ? max_corr_y - radius : 0,
    to_x = max_corr_x + radius < master->get_width() ? max_corr_x + radius : master->get_width(),
    to_y = max_corr_y + radius < master->get_height() ? max_corr_y + radius : master->get_height();

    unsigned int i = 0;
    for(unsigned int _y = from_y; _y < to_y; _y++)
      for(unsigned int _x = from_x; _x < to_x; _x++) {

	if(max_corr_x != _x && max_corr_y != _y) {
	  //positions.push_back(std::pair<unsigned int, unsigned int>(_x, _y));
	  positions_x[i] = _x;
	  positions_y[i] = _y;
	  i++;
	}
      }
    
    // check for position with highest correlation value
    for(unsigned int i2 = 0; i2 < i; i2++) {

      unsigned int x = positions_x[i2], y = positions_y[i2];

      //debug(TM, "hill climbing step at (%d,%d)", x, y);

      double curr_corr_val = calc_single_xcorr(master,
					       sum_table_single_normal,
					       sum_table_squared_normal,
					       zero_mean_template,
					       sum_over_zero_mean_template,
					       x, y);

      if(curr_corr_val > max_corr) {
	max_corr_x = x;
	max_corr_y = y;
	max_corr = curr_corr_val;
	running = true;
	//positions.clear();
      }
    }
  }

  *max_corr_x_out = max_corr_x;
  *max_corr_y_out = max_corr_y;
  *max_xcorr_out = max_corr;
  
}


double TemplateMatching::calc_single_xcorr(const TileImage_GS_BYTE_shptr master, 
					   const TileImage_GS_DOUBLE_shptr summation_table_single,
					   const TileImage_GS_DOUBLE_shptr summation_table_squared,
					   const TempImage_GS_DOUBLE_shptr zero_mean_template, 
					   double sum_over_zero_mean_template,
					   unsigned int local_x, 
					   unsigned int local_y) const {

  double template_size = zero_mean_template->get_width() * zero_mean_template->get_height();
  assert(template_size > 0);
  

  unsigned int 
    x_plus_w = local_x + zero_mean_template->get_width() -1,
    y_plus_h = local_y + zero_mean_template->get_height() -1,
    lxm1 = local_x - 1, // can wrap, it's checked later
    lym1 = local_y - 1;
  
  // calculate denominator
  double 
    f1 = summation_table_single->get_pixel(x_plus_w, y_plus_h),
    f2 = summation_table_squared->get_pixel(x_plus_w, y_plus_h);
  
  if(local_x > 0) {
    f1 -= summation_table_single->get_pixel(lxm1, y_plus_h);
    f2 -= summation_table_squared->get_pixel(lxm1, y_plus_h);
  }
  if(local_y > 0) {
    f1 -= summation_table_single->get_pixel(x_plus_w, lym1);
    f2 -= summation_table_squared->get_pixel(x_plus_w, lym1);
  }
  if(local_x > 0 && local_y > 0) {
    f1 += summation_table_single->get_pixel(lxm1, lym1);
    f2 += summation_table_squared->get_pixel(lxm1, lym1);
  }
  
  double denominator = sqrt((f2 - f1*f1/template_size) * sum_over_zero_mean_template);
  
  // calculate nummerator
  if(isinf(denominator) || isnan(denominator)) {
    debug(TM, "ERROR: The denominator is not a valid number: f1=%f f2=%f template_size=%d sum=%f",
	  f1, f2, template_size, sum_over_zero_mean_template);
    return -1.0;
  }

  unsigned int _x, _y;
  double nummerator = 0;

  for(_y = 0; _y < zero_mean_template->get_height(); _y ++) {
    for(_x = 0; _x < zero_mean_template->get_width(); _x ++) {
      double f_xy = master->get_pixel(_x + local_x, _y + local_y);
      double t_xy = zero_mean_template->get_pixel(_x, _y);
      nummerator += f_xy * t_xy;
    }
  }
  
  double q = nummerator/denominator;

  //debug(TM, "x=%d,y=%d a=%f, nummerator=%f, denominator=%f, q=%f", local_x, local_y, a, nummerator, denominator, q);

  if(!(q >= -1.000001 && q <= 1.000001)) {
    debug(TM, "nummerator = %f / denominator = %f", nummerator, denominator);
  }
  assert(q >= -1.1 && q <= 1.1);
  return q;
}

bool TemplateMatchingNormal::get_next_pos(struct search_state * state,
					  struct prepared_template const& tmpl) const {

  unsigned int 
    tmpl_w = tmpl.tmpl_img_normal->get_width(),
    tmpl_h = tmpl.tmpl_img_normal->get_height();

  if(state->search_area.get_width() < tmpl_w ||
     state->search_area.get_height() < tmpl_h) return false;

  unsigned int step = state->step_size_search;

  bool there_was_a_gate = false;

  do {
    if(state->x + step < state->search_area.get_width() - tmpl_w) 
      state->x += step;
    else {
      state->x = 0;
      if(state->y + step < state->search_area.get_height() - tmpl_h) 
	state->y += step;
      else return false;
    }
    
    unsigned int dist_x = layer_insert->get_distance_to_gate_boundary(state->x + state->search_area.get_min_x(), 
								      state->y + state->search_area.get_min_y(), 
								      true, tmpl_w, tmpl_h);
    
    if(dist_x > 0) {
      debug(TM, "In the window starting at %d,%d there is already a gate. Skipping %d horizontal pixels",
	    state->x + state->search_area.get_min_x(),
	    state->y + state->search_area.get_min_y(),
	    dist_x);
      state->x += dist_x;
      there_was_a_gate = true;
      step = 1;
    }
    else there_was_a_gate = false;

  }while(there_was_a_gate);

  return true;
}


bool TemplateMatchingAlongGrid::initialize_state_struct(struct search_state * state,
							int offs_min,
							int offs_max) const {
  if(state->grid == NULL) {
    const RegularGrid_shptr rg = project->get_regular_horizontal_grid();
    const IrregularGrid_shptr ig = project->get_irregular_horizontal_grid();

    if(rg->is_enabled()) state->grid = rg;
    else if(ig->is_enabled()) state->grid = ig;

    if(state->grid != NULL) {

      for(Grid::grid_iter iter = state->grid->begin(); 
	  iter != state->grid->end(); ++iter) {
	if(*iter <= offs_min) state->iter_begin = iter;
	if(*iter <= offs_max) state->iter_end = iter;
      }

      if(state->iter_begin != state->grid->end()) state->iter_begin++;
      if(state->iter_end != state->grid->end()) state->iter_end++;

      if(state->iter_begin == state->grid->end() ||
	 state->iter_end == state->grid->end()) return false;

      state->iter = state->iter_begin;
    }
    else return false;
  }
  return true;
}

bool TemplateMatchingInRows::get_next_pos(struct search_state * state,
					  struct prepared_template const& tmpl) const {

  // check if the search area is larger then the template
  unsigned int 
    tmpl_w = tmpl.tmpl_img_normal->get_width(),
    tmpl_h = tmpl.tmpl_img_normal->get_height();

  if(state->search_area.get_width() < tmpl_w ||
     state->search_area.get_height() < tmpl_h) return false;

  unsigned int step = state->step_size_search;

  // get grid and check if we are working on regular or irregular grid
  if(state->grid == NULL && 
     initialize_state_struct(state,
			     state->search_area.get_min_y(),
			     state->search_area.get_max_y() - tmpl_h) == false) 
    return false;
  

  if(state->x == 0 && state->y == 0) { // state condition
    state->y = *(state->iter) - state->search_area.get_min_y();
  }

  bool there_was_a_gate = false;

  do {

    if(state->x + step < state->search_area.get_width()) 
      state->x += step;
    else {
      state->x = 0;
      state->y = *(state->iter) - state->search_area.get_min_y();
      ++state->iter;
      if(state->iter == state->iter_end) return false;
    }
    
    unsigned int dist_x = layer_insert->get_distance_to_gate_boundary(state->x + state->search_area.get_min_x(), 
								      state->y + state->search_area.get_min_y(), 
								      true, tmpl_w, tmpl_h);
    
    if(dist_x > 0) {
      debug(TM, "In the window starting at %d,%d there is already a gate. Skipping %d horizontal pixels",
	    state->x + state->search_area.get_min_x(),
	    state->y + state->search_area.get_min_y(),
	    dist_x);
      state->x += dist_x;
      there_was_a_gate = true;
      step = 1;
    }
    else there_was_a_gate = false;

  }while(there_was_a_gate);
  
  
  return true;
}


bool TemplateMatchingInCols::get_next_pos(struct search_state * state,
					  struct prepared_template const& tmpl) const {

  // check if the search area is larger then the template
  unsigned int 
    tmpl_w = tmpl.tmpl_img_normal->get_width(),
    tmpl_h = tmpl.tmpl_img_normal->get_height();

  if(state->search_area.get_width() < tmpl_w ||
     state->search_area.get_height() < tmpl_h) return false;

  unsigned int step = state->step_size_search;

  // get grid and check if we are working on regular or irregular grid
  if(state->grid == NULL && 
     initialize_state_struct(state,
			     state->search_area.get_min_x(),
			     state->search_area.get_max_x() - tmpl_w) == false) 
    return false;
  

  if(state->x == 0 && state->y == 0) { // state condition
    state->x = *(state->iter) - state->search_area.get_min_x();
  }

  bool there_was_a_gate = false;

  do {
    if(state->y + step < state->search_area.get_height()) 
      state->y += step;
    else {
      state->x = *(state->iter) - state->search_area.get_min_x();
      state->y = 0;
      ++state->iter;
      if(state->iter == state->iter_end) return false;
    }

    unsigned int dist_y = layer_insert->get_distance_to_gate_boundary(state->x + state->search_area.get_min_x(), 
								      state->y + state->search_area.get_min_y(), 
								      false, tmpl_w, tmpl_h);
    
    if(dist_y > 0) {
      debug(TM, "In the window starting at %d,%d there is already a gate. Skipping %d vertical pixels",
	    state->x + state->search_area.get_min_x(),
	    state->y + state->search_area.get_min_y(),
	    dist_y);
      state->y += dist_y;
      there_was_a_gate = true;
      step = 1;
    }
    else there_was_a_gate = false;

  }while(there_was_a_gate);

  return true;
}

