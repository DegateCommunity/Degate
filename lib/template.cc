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

/**
   The code performs template matching by shifting a template
   over a background image. For each step the normalized
   cross correlation is calculated.

 */

#include <iostream>
#include <gtkmm.h>
#include <assert.h>
#include <fcntl.h>
#include <time.h>
#include "globals.h"
#include "plugins.h"
#include "gui/GateSelectWin.h"
#include "gui/TemplateMatchingParamsWin.h"

Gtk::Dialog* pDialog = 0;

enum TEMPLATE_MATCHING_MODE {
  TEMPLATE_MATCHING_NORMAL = 1,
  TEMPLATE_MATCHING_ALONG_GRID_ROWS = 2,
  TEMPLATE_MATCHING_ALONG_GRID_COLS = 3
};

enum TEMPLATE_MATCHING_STATE {
  TEMPLATE_MATCHING_ERROR = 0,
  TEMPLATE_MATCHING_DONE = 1,
  TEMPLATE_MATCHING_CONTINUE = 2
};

typedef struct {
  TEMPLATE_MATCHING_MODE matching_mode;

  unsigned int min_x, min_y, max_x, max_y; // scaling applied

  lmodel_gate_template_set_t * tmpl_list;
  project_t * project;
  unsigned int placement_layer;

  memory_map_t * summation_table_single;
  memory_map_t * summation_table_squared;
  memory_map_t * summation_table_single_sd;
  memory_map_t * summation_table_squared_sd;

  unsigned int objects_found;
  unsigned int objects_added;
  int seconds;
  unsigned int stats_real_gamma_calcs;

  int stop_algorithm; // if it is set to 1 cancel algorithm
} template_matching_params_t;

ret_t cancel_algorithm(plugin_params_t * pparams) {
  template_matching_params_t * matching_params = (template_matching_params_t *) pparams->data_ptr;
  matching_params->stop_algorithm = 1;
  return RET_OK;
}

ret_t init_template(plugin_params_t * pparams) {
  debug(TM, "init(%p) called", pparams);
  if(!pparams) return RET_INV_PTR;

  pparams->data_ptr = malloc(sizeof(template_matching_params_t));
  if(!pparams->data_ptr) return RET_MALLOC_FAILED;

  memset(pparams->data_ptr, 0, sizeof(template_matching_params_t));
  return RET_OK;
}

ret_t shutdown_template(plugin_params_t * pparams) {
  ret_t ret;
  debug(TM, "shutdown(%p) called", pparams);
  if(!pparams) return RET_INV_PTR;

  template_matching_params_t * matching_params = (template_matching_params_t *) pparams->data_ptr;
  
  if(matching_params->tmpl_list != NULL &&
     RET_IS_NOT_OK(ret = lmodel_destroy_gate_template_set(matching_params->tmpl_list, 
							  DESTROY_CONTAINER_ONLY))) return ret;

  if(pparams->data_ptr) {
    memset(pparams->data_ptr, 0, sizeof(template_matching_params_t));
    free(pparams->data_ptr);
  }
  pparams->data_ptr = NULL;
  return RET_OK;
}

/* some function prototypes */
ret_t template_matching_along_grid_cols(plugin_params_t * foo);
ret_t template_matching_along_grid_rows(plugin_params_t * foo);
ret_t template_matching_normal(plugin_params_t * foo);
ret_t template_matching(plugin_params_t * foo);

ret_t raise_dialog_before(Gtk::Window *parent, plugin_params_t * foo);
ret_t raise_dialog_after(Gtk::Window *parent, plugin_params_t * foo);
ret_t imgalgo_run_template_matching(image_t * master, image_t * _template,
				    unsigned int min_x, unsigned int min_y,
				    unsigned int max_x, unsigned int max_y,

				    image_t * sd_master, image_t * sd_template,
				    unsigned int sd_min_x, unsigned int sd_min_y,
				    unsigned int sd_max_x, unsigned int sd_max_y,

				    int layer, 
				    lmodel_gate_template_t * tmpl_ptr,
				    LM_TEMPLATE_ORIENTATION orientation,
				    template_matching_params_t * matching_params);

double imgalgo_calc_single_xcorr(const image_t * const master, 
				 memory_map_t * const zero_mean_template, 
				 memory_map_t * const summation_table_single,
				 memory_map_t * const summation_table_squared,
				 double sum_over_zero_mean_template,
				 unsigned int x, unsigned int y);

double calc_mean_for_img_area(image_t * img, unsigned int min_x, unsigned int min_y, 
			      unsigned int width, unsigned int height);



/** 
    This structure defines, which functions this plugin provides. 
*/
plugin_func_descr_t plugin_func_descriptions[] = {
  { "Template matching",     // name will be displayed in menu
    &template_matching_normal, // a function that perfoms the calculation
    (plugin_raise_dialog_func_t) &raise_dialog_before, // a gui dialog to call before
    (plugin_raise_dialog_func_t) &raise_dialog_after,  // a gui dialog to call after calculation
    &init_template,
    &shutdown_template,
    &cancel_algorithm },

  { "Template matching along grid columns",     // name will be displayed in menu
    &template_matching_along_grid_cols, // a function that perfoms the calculation
    (plugin_raise_dialog_func_t) &raise_dialog_before, // a gui dialog to call before
    (plugin_raise_dialog_func_t) &raise_dialog_after,  // a gui dialog to call after calculation
    &init_template,
    &shutdown_template,
    &cancel_algorithm },

  { "Template matching along grid rows",     // name will be displayed in menu
    &template_matching_along_grid_rows, // a function that perfoms the calculation
    (plugin_raise_dialog_func_t) &raise_dialog_before, // a gui dialog to call before
    (plugin_raise_dialog_func_t) &raise_dialog_after,  // a gui dialog to call after calculation
    &init_template,
    &shutdown_template,
    &cancel_algorithm },

  { NULL, NULL, NULL, NULL, NULL, NULL, NULL}
};


inline double mm_get_double(memory_map_t * table, unsigned int x, unsigned int y) {
  return *(double *)mm_get_ptr(table, x, y);
}

inline void mm_set_double(memory_map_t * table, unsigned int x, unsigned int y, double v) {
  *(double *)mm_get_ptr(table, x, y) = v;
}


ret_t precalc_summation_tables(image_t * master_img, 
			       memory_map_t * summation_table_single, 
			       memory_map_t * summation_table_squared) {


  return RET_OK;
}

/* These functions are called back from the main application within
   a thread. 
*/

ret_t template_matching_normal(plugin_params_t * pparams) {
  assert(pparams != NULL);
  if(pparams == NULL) return RET_INV_PTR;
  ((template_matching_params_t *) pparams->data_ptr)->matching_mode = TEMPLATE_MATCHING_NORMAL;
  return template_matching(pparams);
}

ret_t template_matching_along_grid_cols(plugin_params_t * pparams) {
  assert(pparams != NULL);
  if(pparams == NULL) return RET_INV_PTR;
  ((template_matching_params_t *) pparams->data_ptr)->matching_mode = TEMPLATE_MATCHING_ALONG_GRID_COLS;
  return template_matching(pparams);
}

ret_t template_matching_along_grid_rows(plugin_params_t * pparams) {
  assert(pparams != NULL);
  if(pparams == NULL) return RET_INV_PTR;
  ((template_matching_params_t *) pparams->data_ptr)->matching_mode = TEMPLATE_MATCHING_ALONG_GRID_ROWS;
  return template_matching(pparams);
}

ret_t template_matching(plugin_params_t * pparams) {
  assert(pparams);

  ret_t ret;
  image_t * master_img_gs = NULL;
  image_t * master_img_gs_sd = NULL;
  image_t * _template = NULL;
  image_t * _template_sd = NULL;
  double total_time_ms;
  clock_t start, finish;

  LM_TEMPLATE_ORIENTATION orientation;
  template_matching_params_t * matching_params = (template_matching_params_t *) pparams->data_ptr;


  if(!pparams) return RET_INV_PTR;
  matching_params->project = pparams->project;
  matching_params->placement_layer = lmodel_get_layer_num_by_type(matching_params->project->lmodel, 
								  LM_LAYER_TYPE_LOGIC);

  unsigned int layer = pparams->project->current_layer;
  /* this is a pointer to the background image */
  double scale_down = 0;

  image_t * master_img = pparams->project->bg_images[layer];

  image_t * master_img_sd = scalmgr_get_image(pparams->project->scaling_manager,
					   layer,
					   matching_params->scale_down,
					   &scale_down);
  assert(scale_down == matching_params->scale_down);
  if(scale_down != matching_params->scale_down) return RET_ERR;

  matching_params->min_x = lrint((double)pparams->min_x / (double)scale_down);
  matching_params->max_x = lrint((double)pparams->max_x / (double)scale_down);
  matching_params->min_y = lrint((double)pparams->min_y / (double)scale_down);
  matching_params->max_y = lrint((double)pparams->max_y / (double)scale_down);
  if(matching_params->max_x >= master_img->width) matching_params->max_x = master_img->width - 1;
  if(matching_params->max_y >= master_img->height) matching_params->max_y = master_img->height - 1;

  debug(TM, "matching on layer = %d", layer);

  /************************************************************************************
   *
   * Prepare the master image. This is the backgound image.
   * (XXX: should be renamed)
   *
   ************************************************************************************/
  // we get get a lot of performance gain, if we use a grayscaled image
  if((master_img_gs_sd = gr_create_image(matching_params->max_x - matching_params->min_x, 
					 matching_params->max_y - matching_params->min_y, 
					 IMAGE_TYPE_GS)) == NULL) { ret = RET_ERR; goto error; }
  
  if(RET_IS_NOT_OK(ret = gr_map_temp_file(master_img_gs_sd, 
					  pparams->project->project_dir))) goto error;
  
  // implicit conversion to gs
  if(RET_IS_NOT_OK(ret = gr_copy_image(master_img_gs_sd, master_img_sd, 
				       matching_params->min_x, matching_params->min_y,
				       matching_params->max_x, matching_params->max_y))) goto error;

  // normal version
  if((master_img_gs = gr_create_image(pparams->max_x - pparams->min_x, 
				      pparams->max_y - pparams->min_y, 
				      IMAGE_TYPE_GS)) == NULL) { ret = RET_ERR; goto error; }
  
  if(RET_IS_NOT_OK(ret = gr_map_temp_file(master_img_gs, 
					  pparams->project->project_dir))) goto error;
  
  // implicit conversion to gs
  if(RET_IS_NOT_OK(ret = gr_copy_image(master_img_gs, master_img, 
				       pparams->min_x, pparams->min_y,
				       pparams->max_x, pparams->max_y))) goto error;

  /************************************************************************************
   *
   * Summation tables
   *
   ************************************************************************************/

  if((matching_params->summation_table_single_sd = 
      mm_create(matching_params->max_x - matching_params->min_x,
		matching_params->max_y - matching_params->min_y, 
		sizeof(double))) == NULL) { ret = RET_ERR; goto error; }
  
  if(RET_IS_NOT_OK(ret = mm_map_temp_file(matching_params->summation_table_single_sd, 
					  pparams->project->project_dir))) goto error;


  if((matching_params->summation_table_squared_sd = 
      mm_create(matching_params->max_x - matching_params->min_x, 
		matching_params->max_y - matching_params->min_y, 
		sizeof(double))) == NULL) goto error;

  if(RET_IS_NOT_OK(ret = mm_map_temp_file(matching_params->summation_table_squared_sd, 
					  pparams->project->project_dir))) goto error;


  if(RET_IS_NOT_OK(ret = precalc_summation_tables(master_img_gs_sd, 
						  matching_params->summation_table_single_sd, 
						  matching_params->summation_table_squared_sd))) goto error;


  // normal

  if((matching_params->summation_table_single = 
      mm_create(pparams->max_x - pparams->min_x, pparams->max_y - pparams->min_y, 
		sizeof(double))) == NULL) { ret = RET_ERR; goto error; }
  
  if(RET_IS_NOT_OK(ret = mm_map_temp_file(matching_params->summation_table_single, 
					  pparams->project->project_dir))) goto error;


  if((matching_params->summation_table_squared = 
      mm_create(pparams->max_x - pparams->min_x, pparams->max_y - pparams->min_y, 
		sizeof(double))) == NULL) goto error;

  if(RET_IS_NOT_OK(ret = mm_map_temp_file(matching_params->summation_table_squared,
					  pparams->project->project_dir))) goto error;

  if(RET_IS_NOT_OK(ret = precalc_summation_tables(master_img_gs, 
						  matching_params->summation_table_single,
						  matching_params->summation_table_squared))) goto error;


  start = clock();

  while(tmpl_list_ptr != NULL) {

    lmodel_gate_template_t * gate_template = tmpl_list_ptr->gate;

    /************************************************************************************
     *
     * create a temp image from the template 
     *
     ************************************************************************************/
    if(_template != NULL && RET_IS_NOT_OK(gr_image_destroy(_template))) {ret = RET_ERR; goto error;}
    if(_template_sd != NULL && RET_IS_NOT_OK(gr_image_destroy(_template_sd))) {ret = RET_ERR; goto error;}

    unsigned int tmpl_pos_min_x = lrint((double)gate_template->master_image_min_x / (double)scale_down);
    unsigned int tmpl_pos_max_x = lrint((double)gate_template->master_image_max_x / (double)scale_down);
    unsigned int tmpl_pos_min_y = lrint((double)gate_template->master_image_min_y / (double)scale_down);
    unsigned int tmpl_pos_max_y = lrint((double)gate_template->master_image_max_y / (double)scale_down);
    if(tmpl_pos_max_x >= master_img->width) tmpl_pos_max_x = master_img->width - 1;
    if(tmpl_pos_max_y >= master_img->height) tmpl_pos_max_y = master_img->height - 1;
    
    if((_template_sd = gr_extract_image_as_gs(master_img_sd,
					      tmpl_pos_min_x, tmpl_pos_min_y,
					      tmpl_pos_max_x - tmpl_pos_min_x,
					      tmpl_pos_max_y - tmpl_pos_min_y)) == NULL) {
      ret = RET_ERR; 
      goto error; 
    }

    // normal
    if((_template = gr_extract_image_as_gs(master_img,
					   gate_template->master_image_min_x, 
					   gate_template->master_image_min_y,
					   gate_template->master_image_max_x - 
					   gate_template->master_image_min_x,
					   gate_template->master_image_max_y - 
					   gate_template->master_image_min_y)) == NULL) {
      ret = RET_ERR; 
      goto error; 
    }

  
    debug(TM, "Template matching: normal");
    orientation = LM_TEMPLATE_ORIENTATION_NORMAL;
    if(RET_IS_NOT_OK(ret = imgalgo_run_template_matching(master_img_gs, _template,
							 pparams->min_x, pparams->min_y,
							 pparams->max_x - _template->width,
							 pparams->max_y - _template->height,

							 master_img_gs_sd, _template_sd,
							 matching_params->min_x, matching_params->min_y,
							 matching_params->max_x - _template_sd->width,
							 matching_params->max_y - _template_sd->height,

							 pparams->project->current_layer,
							 gate_template, orientation, 
							 matching_params))) goto error;

    if(matching_params->stop_algorithm == 1) goto finish;

    debug(TM, "Template matching: flipped up down");
    orientation = LM_TEMPLATE_ORIENTATION_FLIPPED_UP_DOWN;
    gr_flip_up_down(_template);
    gr_flip_up_down(_template_sd);
    if(RET_IS_NOT_OK(ret = imgalgo_run_template_matching(master_img_gs, _template,
							 pparams->min_x, pparams->min_y,
							 pparams->max_x - _template->width,
							 pparams->max_y - _template->height,

							 master_img_gs_sd, _template_sd,
							 matching_params->min_x, matching_params->min_y,
							 matching_params->max_x - _template_sd->width,
							 matching_params->max_y - _template_sd->height,

							 pparams->project->current_layer,
							 gate_template, orientation, 
							 matching_params))) goto error;
    if(matching_params->stop_algorithm == 1) goto finish;
    
    debug(TM, "Template matching: flipped both");
    orientation = LM_TEMPLATE_ORIENTATION_FLIPPED_BOTH;
    gr_flip_left_right(_template);
    gr_flip_left_right(_template_sd);
    if(RET_IS_NOT_OK(ret = imgalgo_run_template_matching(master_img_gs, _template,
							 pparams->min_x, pparams->min_y,
							 pparams->max_x - _template->width,
							 pparams->max_y - _template->height,

							 master_img_gs_sd, _template_sd,
							 matching_params->min_x, matching_params->min_y,
							 matching_params->max_x - _template_sd->width,
							 matching_params->max_y - _template_sd->height,

							 pparams->project->current_layer,
							 gate_template, orientation, 
							 matching_params))) goto error;
    if(matching_params->stop_algorithm == 1) goto finish;
    
    debug(TM, "Template matching: flipped left right");
    orientation = LM_TEMPLATE_ORIENTATION_FLIPPED_LEFT_RIGHT;
    gr_flip_up_down(_template);
    gr_flip_up_down(_template_sd);
    if(RET_IS_NOT_OK(ret = imgalgo_run_template_matching(master_img_gs, _template,
							 pparams->min_x, pparams->min_y,
							 pparams->max_x - _template->width,
							 pparams->max_y - _template->height,

							 master_img_gs_sd, _template_sd,
							 matching_params->min_x, matching_params->min_y,
							 matching_params->max_x - _template_sd->width,
							 matching_params->max_y - _template_sd->height,

							 pparams->project->current_layer,
							 gate_template, orientation, 
							 matching_params))) goto error;

    if(matching_params->stop_algorithm == 1) goto finish;

    tmpl_list_ptr = tmpl_list_ptr->next;
  }

 finish:
  // stats
  finish = clock();
  matching_params->seconds = double(finish - start)/CLOCKS_PER_SEC;
  total_time_ms = 1000*(double(finish - start)/CLOCKS_PER_SEC);
  
  debug(TM, "-------------------- [ matching stats ] --------------------");
  debug(TM, "region x: %d .. %d  region y %d .. %d -> %d x %d px", 
	pparams->min_x , pparams->max_x, pparams->min_y, pparams->max_y,
	pparams->max_x - pparams->min_x, pparams->max_y - pparams->min_y);
  debug(TM, "xcorr time total: %f ms", total_time_ms);
  debug(TM, "xcorr nummer of real gamma calculations: %d", matching_params->stats_real_gamma_calcs);
  debug(TM, "xcorr time per real gamma: %f ms", total_time_ms / matching_params->stats_real_gamma_calcs);
  
  debug(TM, "objects found: %d", matching_params->objects_found);
  debug(TM, "objects added: %d", matching_params->objects_added);
  debug(TM, "------------------------------------------------------------");
  
 error:
  
  // free temp image
  if(_template_sd != NULL && RET_IS_NOT_OK(gr_image_destroy(_template_sd))) 
    debug(TM, "gr_image_destroy() failed");
  if(master_img_gs_sd != NULL && RET_IS_NOT_OK(gr_image_destroy(master_img_gs_sd))) 
    debug(TM, "gr_image_destroy() failed");

  if(_template != NULL && RET_IS_NOT_OK(gr_image_destroy(_template))) 
    debug(TM, "gr_image_destroy() failed");
  if(master_img_gs != NULL && RET_IS_NOT_OK(gr_image_destroy(master_img_gs))) 
    debug(TM, "gr_image_destroy() failed");
  
  if(matching_params->summation_table_single != NULL) 
    mm_destroy(matching_params->summation_table_single);
  if(matching_params->summation_table_squared != NULL) 
    mm_destroy(matching_params->summation_table_squared);

  if(matching_params->summation_table_single_sd != NULL) 
    mm_destroy(matching_params->summation_table_single_sd);
  if(matching_params->summation_table_squared_sd != NULL) 
    mm_destroy(matching_params->summation_table_squared_sd);
  
  if(RET_IS_NOT_OK(ret)) debug(TM, "There was an error.");
  
  return ret;
}

ret_t raise_dialog_after(Gtk::Window * parent, plugin_params_t * pparams) {
  char str[1000];
  assert(pparams != NULL);
  if(pparams == NULL) return RET_INV_PTR;

  template_matching_params_t * matching_params = (template_matching_params_t *) pparams->data_ptr;

  if(matching_params->objects_added > 0) {
    snprintf(str, sizeof(str), "I found %d objects and added %d objects into the logic model. "
	     "The matching took %d seconds.", 
	     matching_params->objects_added, matching_params->objects_added, matching_params->seconds);
  }
  else {
    snprintf(str, sizeof(str), "No objects added. The matching took %d seconds.", matching_params->seconds);
  }

  Gtk::MessageDialog dialog(*parent, str, true, Gtk::MESSAGE_INFO, Gtk::BUTTONS_OK);
  dialog.set_title("Info");      
  dialog.run();

  return RET_OK;
}

ret_t raise_dialog_before(Gtk::Window * parent, plugin_params_t * pparams) {
  ret_t ret;
  assert(pparams);
  if(!pparams) return RET_INV_PTR;
  unsigned int layer = pparams->project->current_layer;

  template_matching_params_t * matching_params = (template_matching_params_t *) pparams->data_ptr;

  /* pparams->(min|max)_(x|y) is the region, that is selected. If nothing
     is selected, these variables are zero.
     If nothing is selected, we set it to the complete background image.
   */

  if(pparams->max_x == 0 && pparams->max_y == 0) {
  }

  if(pparams->max_x == 0) {
    pparams->min_x = 0;
    pparams->max_x = pparams->project->bg_images[layer]->width -1;
  }
  if(pparams->max_y == 0) {
    pparams->min_y = 0;
    pparams->max_y = pparams->project->bg_images[layer]->height -1;
  }

  assert(pparams->max_x > pparams->min_x);
  assert(pparams->max_y > pparams->min_y);

  /* This will show a dialog window with a list of available gate types. */
  GateSelectWin gsWin(parent, pparams->project->lmodel);
  matching_params->tmpl_list = gsWin.get_multiple();
  lmodel_gate_template_set_t 
    * tmpl_list_ptr = matching_params->tmpl_list,
    * tmpl_list_head = tmpl_list_ptr;


  if(tmpl_list_ptr == NULL) return RET_CANCEL;

  while(tmpl_list_ptr != NULL) {
    lmodel_gate_template_t * tmpl = tmpl_list_ptr->gate;

    if(tmpl) {
      /* Check, if there is a graphical representation for the gate template. */
      if(tmpl->master_image_min_x < tmpl->master_image_max_x &&
	 tmpl->master_image_min_y < tmpl->master_image_max_y) {

	
	if(pparams->max_x - pparams->min_x < tmpl->master_image_max_x - tmpl->master_image_min_x ||
	   pparams->max_y - pparams->min_y < tmpl->master_image_max_y - tmpl->master_image_min_y) {
	  
	  if(tmpl_list_head->next == NULL) { // if there is only one element
	    
	  }
	  
	  if(RET_IS_NOT_OK(ret = lmodel_remove_gate_template_from_template_set(&tmpl_list_head, tmpl))) 
	    return ret;
	  
	}
      }
      else {
      }
    }
    tmpl_list_ptr = tmpl_list_ptr->next;
  }

  TemplateMatchingParamsWin paramsWin(parent, 
				      pparams->project->scaling_manager,
				      0.45, 0.7,
				      MAX(1, pparams->project->lambda >> 1), 
				      2);
  
  if(RET_IS_NOT_OK(ret = paramsWin.run(&(matching_params->threshold_hc),
				       &(matching_params->threshold_detection),
				       &(matching_params->max_step_size_search),
				       &(matching_params->scale_down) ))) return ret;


  return RET_OK;
}


ret_t clear_area_in_map(memory_map_t * temp, 
			unsigned int start_x, unsigned int start_y, 
			unsigned int radius) {

  unsigned int x, y;
  assert(temp != NULL);
  if(temp == NULL) return RET_INV_PTR;
  
  for(y = start_y > radius ? start_y - radius : 0; 
      y < MIN(start_y + radius, temp->height); y++) {
    for(x = start_x > radius ? start_x - radius : 0; 
	x < MIN(start_x + radius, temp->width); x++) {
      mm_set_double(temp, x, y, -1);
    }
  }
  return RET_OK;
}

#define CALC_AND_CHECK_DIRECTION(_x, _y, v) { \
    double curr_val =  imgalgo_calc_single_xcorr(master, zero_mean_template, \
					     matching_params->summation_table_single, \
                                             matching_params->summation_table_squared, \
					     sum_over_zero_mean_template, _x, _y); \
    matching_params->stats_real_gamma_calcs++; \
    \
    if(curr_max_val < curr_val) { \
      max_corr_x2 = _x; \
      max_corr_y2 = _y; \
      curr_max_val = curr_val; \
    }}


ret_t hill_climbing(unsigned int start_x, unsigned int start_y, double xcorr_val,
		    unsigned int * max_corr_x_out, unsigned int * max_corr_y_out, double * max_xcorr_out,
		    image_t * master,
		    memory_map_t * zero_mean_template,
		    double sum_over_zero_mean_template,
		    template_matching_params_t * matching_params) {

  unsigned int max_corr_x = start_x;
  unsigned int max_corr_y = start_y;
  unsigned int max_corr_x2 = start_x, max_corr_y2 = start_y;
  double curr_max_val = xcorr_val;
  double val = xcorr_val;
  
  debug(TM, "\t+++ area of higher correlation found at %d,%d -> corr = %f", max_corr_x, max_corr_y, val);
  
  do {
    val = curr_max_val;
    
    if(max_corr_x > 1 && max_corr_y > 1) CALC_AND_CHECK_DIRECTION(max_corr_x-1, max_corr_y-1, val);
    if(max_corr_y > 1) CALC_AND_CHECK_DIRECTION(max_corr_x, max_corr_y-1, val);
    if(max_corr_y > 1 && max_corr_x < master->width) 
      CALC_AND_CHECK_DIRECTION(max_corr_x+1, max_corr_y-1, val);
    
    if(max_corr_x > 1) CALC_AND_CHECK_DIRECTION(max_corr_x - 1, max_corr_y, val);
    if(max_corr_x < master->width) CALC_AND_CHECK_DIRECTION(max_corr_x+1, max_corr_y, val);
    
    if(max_corr_x > 1 && max_corr_y < master->height) CALC_AND_CHECK_DIRECTION(max_corr_x - 1, max_corr_y + 1, val);
    if(max_corr_y < master->height) CALC_AND_CHECK_DIRECTION(max_corr_x, max_corr_y + 1, val);
    if(max_corr_x < master->width && max_corr_y < master->height) 
      CALC_AND_CHECK_DIRECTION(max_corr_x + 1, max_corr_y + 1, val);
    
    max_corr_x = max_corr_x2;
    max_corr_y = max_corr_y2;
    debug(TM, "\tclimbed up to %d,%d -> corr = %f", max_corr_x, max_corr_y, curr_max_val);
    
  } while(curr_max_val > val);
  
  *max_corr_x_out = max_corr_x;
  *max_corr_y_out = max_corr_y;
  *max_xcorr_out = curr_max_val;

  return RET_OK;
}

/** x,y are absolute coordinates */
ret_t add_gate(template_matching_params_t * matching_params,
	      lmodel_gate_template_t * tmpl_ptr,
	      LM_TEMPLATE_ORIENTATION orientation,
	      unsigned int x, unsigned int y) {

  ret_t ret;
  //  tmpl_ptr, min_x + max_corr_x, min_y + max_corr_y, orientation);
  unsigned int w = (tmpl_ptr->master_image_max_x - tmpl_ptr->master_image_min_x);
  unsigned int h = (tmpl_ptr->master_image_max_y - tmpl_ptr->master_image_min_y);
  logic_model_t * lmodel = matching_params->project->lmodel;
  
  debug(TM, "IN ADD_GATE(%d,%d)", x, y);
  matching_params->objects_found++;

  lmodel_gate_t * gate;
  debug(TM, "check if there is already a gate");
  if(RET_IS_NOT_OK(lmodel_get_gate_in_region(lmodel, matching_params->placement_layer, 
					     x, y, x+w, y+h, &gate))) return RET_ERR;
	  
  if(gate == NULL) {
    debug(TM, "GATE NOT FOUND - ADDING A NEW check in %d..%d and %d..%d", x, x+w, y, y+h);
    // create gate
    lmodel_gate_t * new_gate = lmodel_create_gate(lmodel, x, y, x+w, y+h, tmpl_ptr, NULL, 0);
    assert(new_gate != NULL);
    if(new_gate == NULL) return RET_ERR;
    if(RET_IS_NOT_OK(ret = lmodel_set_gate_orientation(new_gate, orientation))) return ret;
    if(RET_IS_NOT_OK(ret = lmodel_add_gate(lmodel, matching_params->placement_layer, new_gate))) return ret;
    matching_params->objects_added++;
  }
  else {
    debug(TM, "gate found. don't add a new gate");
  }
  return RET_OK;
}


void adjust_step_size( unsigned int * step_size_search, double val, 
		       const template_matching_params_t * const matching_params) {
  if(val > 0) {
    *step_size_search = MAX(1, rint((1.0 - (double)matching_params->max_step_size_search) * val + 
				    matching_params->max_step_size_search));
  }
  
  else {
    *step_size_search = matching_params->max_step_size_search;
  }

}

/**
 * @param x Position relative to (scaled down) search area
 * @param y Position relative to (scaled down) search area
 * @param min_x Position within complete (scaled down) background image.
 * @param min_y Position within complete (scaled down) background image.
 * @param max_x Position within complete (scaled down) background image.
 * @param max_y Position within complete (scaled down) background image.
 */
TEMPLATE_MATCHING_STATE get_next_pos(unsigned int * x, unsigned int * y, 
				     unsigned int step_size_search,
				     const image_t * const _template,
				     unsigned int min_x, unsigned int max_x,
				     unsigned int min_y, unsigned int max_y,
				     const template_matching_params_t * const matching_params) {
  
  unsigned int width = max_x - min_x;
  unsigned int height = max_y - min_y;
  lmodel_gate_t * gate = NULL;

  if(matching_params->matching_mode == TEMPLATE_MATCHING_NORMAL) {

    if( *x + step_size_search < width) *x += step_size_search;
    else {
      *x = 0;
      if(*y + step_size_search < height) *y += step_size_search;
      else return TEMPLATE_MATCHING_DONE;
    }

  }
  else if(matching_params->matching_mode == TEMPLATE_MATCHING_ALONG_GRID_COLS) {

    const grid_t * grid = matching_params->project->grid;

    if(*x == 0 && *y == 0) { // start condition
      if(RET_IS_NOT_OK(grid_get_first_v_offset(grid, min_x, min_x + width, x)))
	return TEMPLATE_MATCHING_ERROR;
      *x = *x - min_x;
      debug(TM, "start condition, x = %d", *x);	      
    }

    if(*y + step_size_search < height) *y += step_size_search;
    else {
      *y = 0;
      unsigned int next_offset;
      if(RET_IS_OK(grid_get_next_v_offset(grid, *x + min_x, min_x + width, &next_offset)))
	*x = next_offset - min_x;
      else return TEMPLATE_MATCHING_DONE;
      
      if(RET_IS_NOT_OK(lmodel_get_gate_in_region(matching_params->project->lmodel, 
						 matching_params->placement_layer, 
						 *x + min_x, *y + min_y, 
						 *x + min_x + _template->width, 
						 *y + min_y + _template->height,
						 &gate))) return TEMPLATE_MATCHING_ERROR;
      if(gate != NULL) {
	unsigned int gate_height = gate->max_y - gate->min_y;
	if(gate_height > step_size_search) gate_height -= step_size_search;
	debug(TM, "there is a gate skip y by %d", gate_height);
	*y += gate_height;
	return get_next_pos(x, y, step_size_search, _template, min_x, max_x, min_y, max_y, matching_params);
      }

    }
  }
  else if(matching_params->matching_mode == TEMPLATE_MATCHING_ALONG_GRID_ROWS) {

    const grid_t * grid = matching_params->project->grid;


    if(*x == 0 && *y == 0) { // start condition
      if(RET_IS_NOT_OK(grid_get_first_h_offset(grid, min_y, min_y + height, y)))
	return TEMPLATE_MATCHING_ERROR;
      *y = *y - min_y;
      debug(TM, "start condition, y = %d", *y);	      
    }
    
    if(*x + step_size_search < width) *x += step_size_search;
    else {
      *x = 0;
      unsigned int next_offset;
      if(RET_IS_OK(grid_get_next_h_offset(grid, *y + min_y, min_y + height, &next_offset)))
	*y = next_offset - min_y;
      else return TEMPLATE_MATCHING_DONE;

      if(RET_IS_NOT_OK(lmodel_get_gate_in_region(matching_params->project->lmodel, 
						 matching_params->placement_layer, 
						 *x + min_x, *y + min_y, 
						 *x + min_x + _template->width, 
						 *y + min_y + _template->height, 
						 &gate))) return TEMPLATE_MATCHING_ERROR;
      if(gate != NULL) {
	unsigned int gate_width = gate->max_x - gate->min_x;
	if(gate_width > step_size_search) gate_width -= step_size_search;
	debug(TM, "there is a gate skip x by %d", gate_width);
	*x += gate_width;
	return get_next_pos(x, y, step_size_search, _template, min_x, max_x, min_y, max_y, matching_params);
      }
      
    }
    
  }
  
  return TEMPLATE_MATCHING_CONTINUE;

}

double subtract_mean(image_t * img, memory_map_t * zero_mean_img) {
  
  double mean = calc_mean_for_img_area(img, 0, 0, img->width, img->height);
  double sum_over_zero_mean_img = 0;
  unsigned int x, y;

  for(y = 0; y < img->height; y++) 
    for(x = 0; x < img->width; x++) {
      double tmp = (double)gr_get_greyscale_pixval(img, x, y) - mean;
      mm_set_double( zero_mean_img, x, y, tmp);
      sum_over_zero_mean_img += tmp * tmp;
    }

  return sum_over_zero_mean_img;
}

ret_t imgalgo_run_template_matching(image_t * master, image_t * _template,
				    unsigned int min_x, unsigned int min_y,
				    unsigned int max_x, unsigned int max_y,

				    image_t * sd_master, image_t * sd_template,
				    unsigned int sd_min_x, unsigned int sd_min_y,
				    unsigned int sd_max_x, unsigned int sd_max_y,

				    int layer, lmodel_gate_template_t * tmpl_ptr,
				    LM_TEMPLATE_ORIENTATION orientation,
				    template_matching_params_t * matching_params) {

  unsigned int x = 0, y = 0;
  ret_t ret;
  double sum_over_zero_mean_template = 0;
  memory_map_t * zero_mean_template = NULL;

  double sum_over_zero_mean_template_sd = 0;
  memory_map_t * zero_mean_template_sd = NULL;

  unsigned int step_size_search = matching_params->max_step_size_search;
  TEMPLATE_MATCHING_STATE state;
  

  // prepare template
  if((zero_mean_template = mm_create(_template->width, _template->height, sizeof(double))) == NULL) { 
    debug(TM, "mm_create() for zero_mean_template failed");
    ret = RET_ERR; 
    goto error;
  }
  if(RET_IS_NOT_OK(ret = mm_alloc_memory(zero_mean_template))) goto error;


  if((zero_mean_template_sd = mm_create(sd_template->width, sd_template->height, sizeof(double))) == NULL) { 
    debug(TM, "mm_create() for zero_mean_template failed");
    ret = RET_ERR; 
    goto error;
  }
  if(RET_IS_NOT_OK(ret = mm_alloc_memory(zero_mean_template_sd))) goto error;

  sum_over_zero_mean_template = subtract_mean(_template, zero_mean_template);
  sum_over_zero_mean_template_sd = subtract_mean(sd_template, zero_mean_template_sd);


  while( matching_params->stop_algorithm == 0 && 
	 (state = get_next_pos(&x, &y, step_size_search, _template,
			       min_x, max_x, min_y, max_y, matching_params)) == TEMPLATE_MATCHING_CONTINUE) {

    double val = imgalgo_calc_single_xcorr(sd_master, zero_mean_template_sd,
					   matching_params->summation_table_single_sd,
					   matching_params->summation_table_squared_sd,
					   sum_over_zero_mean_template_sd, 
					   lrint((double)x / (double)matching_params->scale_down),
					   lrint((double)y / (double)matching_params->scale_down));
    
    matching_params->stats_real_gamma_calcs++; 
    adjust_step_size(&step_size_search, val, matching_params);  
     
    if(val >= matching_params->threshold_hc) {
      
      unsigned int max_corr_x, max_corr_y;
      double curr_max_val;
      if(RET_IS_NOT_OK(ret = hill_climbing(x, y, val, &max_corr_x, &max_corr_y, &curr_max_val,
					   master, zero_mean_template, sum_over_zero_mean_template, 
					   matching_params))) {
	debug(TM, "hill climbing failed");
	goto error;
      }
      
      if(curr_max_val >= matching_params->threshold_detection) {
	debug(TM, "\tfound a correlation hotspot at %d,%d with v = %f", max_corr_x, max_corr_y, curr_max_val);
	
	// insert
	if(RET_IS_NOT_OK(ret = add_gate(matching_params, tmpl_ptr, orientation, 
					min_x + max_corr_x, min_y + max_corr_y))) {
	  debug(TM, "add_gate() failed");
	  goto error;		
	}
      }

    }    
  
  }

 error:

  /* unmap and remove temp data */
  if(zero_mean_template != NULL && RET_IS_NOT_OK(mm_destroy(zero_mean_template))) 
    debug(TM, "mm_destroy() failed");
  if(zero_mean_template_sd != NULL && RET_IS_NOT_OK(mm_destroy(zero_mean_template_sd))) 
    debug(TM, "mm_destroy() failed");
  
  return ret;
}

double calc_mean_for_img_area(image_t * img, unsigned int min_x, unsigned int min_y, 
			      unsigned int width, unsigned int height) {
  double mean = 0;
  unsigned int x,y;
  assert(width > 0 && height > 0);
  assert(min_x + width <= img->width);
  assert(min_y + height <= img->height);

  for(y = min_y; y < min_y + height; y++)
    for(x = min_x; x < min_x + width; x++)
      mean += gr_get_greyscale_pixval(img, x, y);

  return mean / (width * height);
}


double imgalgo_calc_single_xcorr(const image_t * const master, 
				 memory_map_t * const zero_mean_template, 
				 memory_map_t * const summation_table_single,
				 memory_map_t * const summation_table_squared,
				 double sum_over_zero_mean_template,
				 unsigned int local_x, unsigned int local_y) {

  double template_size = zero_mean_template->width * zero_mean_template->height;

  unsigned int 
    x_plus_w = local_x + zero_mean_template->width -1,
    y_plus_h = local_y + zero_mean_template->height -1,
    lxm1 = local_x - 1,
    lym1 = local_y - 1;
  
  // calulate denominator
  double 
    f1 = mm_get_double(summation_table_single, x_plus_w, y_plus_h),
    f2 = mm_get_double(summation_table_squared, x_plus_w, y_plus_h);
  
  if(local_x > 0) {
    f1 -= mm_get_double(summation_table_single, lxm1, y_plus_h);
    f2 -= mm_get_double(summation_table_squared, lxm1, y_plus_h);
  }
  if(local_y > 0) {
    f1 -= mm_get_double(summation_table_single, x_plus_w, lym1);
    f2 -= mm_get_double(summation_table_squared, x_plus_w, lym1);
  }
  if(local_x > 0 && local_y > 0) {
    f1 += mm_get_double(summation_table_single, lxm1, lym1);
    f2 += mm_get_double(summation_table_squared, lxm1, lym1);
  }
  
  double denominator = sqrt((f2 - f1*f1/template_size) * sum_over_zero_mean_template);
  
  // calculate nummerator
  
  unsigned int _x, _y;
  double nummerator = 0;

  for(_y = 0; _y < zero_mean_template->height; _y ++) {
    for(_x = 0; _x < zero_mean_template->width; _x ++) {
      double f_xy = gr_get_greyscale_pixval(master, _x + local_x, _y + local_y);
      double t_xy = mm_get_double(zero_mean_template, _x, _y);
      nummerator += f_xy * t_xy;
      
    }
  }
  
  return nummerator/denominator;
}
