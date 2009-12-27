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

#ifndef __RENDERER_H__
#define __RENDERER_H__

#include <ft2build.h>
#include FT_FREETYPE_H

#include "globals.h"
#include "LogicModel.h"
//#include "graphics.h"
#include "RegularGrid.h"
#include "IrregularGrid.h"
#include "ScalingManager.h"

/* ---------------------------------------------------------

   Warning: this is old ugly code and must be rewritten.

   --------------------------------------------------------- */

namespace degate {

  // Todo: should rendering params and renderer data structures be merged?

  typedef struct renderer renderer_t;

  // rendering params
  struct render_params {
    LogicModel_shptr lmodel;
    //grid_t * grid;
    RegularGrid_shptr regular_horizontal_grid;
    RegularGrid_shptr regular_vertical_grid;
    IrregularGrid_shptr irregular_horizontal_grid;
    IrregularGrid_shptr irregular_vertical_grid;

    std::tr1::shared_ptr<ScalingManager<BackgroundImage> > scaling_manager;

    // color switches
    color_t gate_pin_color;
    color_t gate_area_color;
    color_t wire_color;
    color_t il_up_color;
    color_t il_down_color;
    color_t grid_color;

    color_t annotation_frame_color;
    color_t annotation_fill_color;
  };

  typedef struct render_params render_params_t;


#define RENDERER_REGION_FUNC_PARAMS \
  renderer_t * const renderer,	    \
  RendererImage_shptr dst_img,	    \
  unsigned int layer, \
  unsigned int min_x, unsigned int min_y, unsigned int max_x, unsigned int max_y

#define RENDERER_FUNC_PARAMS \
  RENDERER_REGION_FUNC_PARAMS, render_params_t * data_ptr

  typedef int (*render_func_t)(renderer_t * const renderer,
			       RendererImage_shptr, 
			       unsigned int, // layer
			       unsigned int, // min_x
			       unsigned int, // max_y
			       unsigned int, // max_x
			       unsigned int, // max_y
			       render_params_t *);

#define MAX_RENDERER_LAYER 30

  struct renderer {
    render_func_t funcs[MAX_RENDERER_LAYER];
    void * data_ptr[MAX_RENDERER_LAYER];
    int rendering_enabled[MAX_RENDERER_LAYER];
    char * names[MAX_RENDERER_LAYER];

    // for font rendering
    FT_Library library;
    FT_Face face;
    FT_GlyphSlot slot;

    // x/y-positions are only stored to check, if we have to recalculate the step-arrays
    unsigned int last_screen_width, last_screen_height;
    unsigned int last_map_width, last_map_height;
    double last_rel_scaling_x, last_rel_scaling_y;

    unsigned int * x_steps, * y_steps;
  

    int num; // number of rendering layers
  };

  renderer_t * renderer_create();
  void renderer_destroy(renderer_t * const renderer);
  void renderer_add_layer(renderer_t * const renderer, render_func_t function_ptr, void * data_ptr,
			  int enabled, const char * const name);
  void renderer_remove_last_layer(renderer_t * const renderer);

  void renderer_toggle_render_func(renderer_t * const renderer, int slot_pos);
  int renderer_get_num_render_func(renderer_t * const renderer);
  char * renderer_get_name_render_func(renderer_t * const renderer, int slot_pos);
  int renderer_render_func_enabled(renderer_t * const renderer, int slot_pos);

  void renderer_initialize_params(render_params_t * rend);

  void render_region(RENDERER_REGION_FUNC_PARAMS);


  ret_t render_background(RENDERER_FUNC_PARAMS);
  ret_t render_to_grayscale(RENDERER_FUNC_PARAMS);
  ret_t render_color_similarity(RENDERER_FUNC_PARAMS);
  ret_t render_gates(RENDERER_FUNC_PARAMS);
  ret_t render_wires(RENDERER_FUNC_PARAMS);
  ret_t render_vias(RENDERER_FUNC_PARAMS);
  ret_t render_annotations(RENDERER_FUNC_PARAMS);
  ret_t render_grid(RENDERER_FUNC_PARAMS);

  ret_t renderer_write_image(RendererImage_shptr img, const char * const filename);

  // helper

  void vline(ImageBase_shptr img, unsigned int x, unsigned int y, uint32_t col);
  void hline(ImageBase_shptr img, unsigned int x, unsigned int y, uint32_t col);
}

#endif
