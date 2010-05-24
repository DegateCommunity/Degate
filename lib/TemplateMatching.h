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

#ifndef __TEMPLATEMATCHING_H__
#define __TEMPLATEMATCHING_H__

#include <Image.h>
#include <Project.h>
#include <Layer.h>
#include <ProgressControl.h>

namespace degate {

  /**
   * Base class for matching alorithms.
   */
  class Matching : public ProgressControl {
  public:
    virtual ~Matching() {}
    virtual void init(BoundingBox const& bounding_box, Project_shptr project) = 0;
    virtual void run() = 0;
  };


  /**
   * This class implements the matching of gate representing images on
   * a background image.
   */

  class TemplateMatching : public Matching {
  protected:

    struct prepared_template {
      TempImage_GS_BYTE_shptr tmpl_img_normal;
      TempImage_GS_BYTE_shptr tmpl_img_scaled;

      //TempImage_GS_BYTE_shptr zero_mean_template_normal; // GS_DOUBLE?
      //TempImage_GS_BYTE_shptr zero_mean_template_scaled;

      TempImage_GS_DOUBLE_shptr zero_mean_template_normal; // GS_DOUBLE?
      TempImage_GS_DOUBLE_shptr zero_mean_template_scaled;

      double sum_over_zero_mean_template_normal;
      double sum_over_zero_mean_template_scaled;

      Gate::ORIENTATION orientation;
      GateTemplate_shptr gate_template;
    };


    struct search_state {
      
      unsigned int x, y; // unscaled coordinates in the cropped image
      unsigned int step_size_search;
      BoundingBox search_area; // on unscaled uncropped image

      Grid_shptr grid;
      Grid::grid_iter iter, iter_begin, iter_end;
    };


  private:

    // params for the matching
    double threshold_hc;
    double threshold_detection;
    unsigned int max_step_size_search;
    unsigned int scale_down;
    unsigned int threshold_steps;


    // background images in greyscale
    TileImage_GS_BYTE_shptr gs_img_normal;
    TileImage_GS_BYTE_shptr gs_img_scaled;

    // summation tables
    TileImage_GS_DOUBLE_shptr sum_table_single_normal;
    TileImage_GS_DOUBLE_shptr sum_table_squared_normal;
    TileImage_GS_DOUBLE_shptr sum_table_single_scaled;
    TileImage_GS_DOUBLE_shptr sum_table_squared_scaled;

    BoundingBox bounding_box; // bounding box on original unscaled background image

    std::list<GateTemplate_shptr> tmpl_set; // templates to match
    std::list<Gate::ORIENTATION> tmpl_orientations; // template orientations to match

    clock_t start, finish;

  protected:

    Project_shptr project;

    Layer_shptr layer_matching; // matching happens on this layer
    Layer_shptr layer_insert; // found gates are inserted here

  private:



    void prepare_sum_tables(TileImage_GS_BYTE_shptr gs_img_normal,
			    TileImage_GS_BYTE_shptr gs_img_scaled);

    void precalc_sum_tables(TileImage_GS_BYTE_shptr img, 
			    TileImage_GS_DOUBLE_shptr summation_table_single,
			    TileImage_GS_DOUBLE_shptr summation_table_squared);


    BoundingBox get_scaled_bounding_box(BoundingBox const& bounding_box, 
					double scale_down) const;

    void prepare_background_images(ScalingManager_shptr sm, 
				   BoundingBox const& bounding_box, 
				   unsigned int scaling_factor);

    struct prepared_template prepare_template(GateTemplate_shptr tmpl,
					      Gate::ORIENTATION orientation);


    void hill_climbing(unsigned int start_x, unsigned int start_y, double xcorr_val,
		       unsigned int * max_corr_x_out,
		       unsigned int * max_corr_y_out,
		       double * max_xcorr_out,
		       const TileImage_GS_BYTE_shptr master,
		       const TempImage_GS_DOUBLE_shptr zero_mean_template,
		       double sum_over_zero_mean_template) const;

    /**
     * Adjust step size depending on correlation value.
     */
    void adjust_step_size(struct search_state & state, double corr_val) const;

    void match_single_template(struct prepared_template & tmpl,
			       double threshold_hc, double threshold_detection);
    
    
    /**
     * Calculate a zero mean image from an image and return
     * the variance(?).
     */
    double subtract_mean(TempImage_GS_BYTE_shptr img, 
			 TempImage_GS_DOUBLE_shptr zero_mean_img) const;

    /**
     * Calculate correlation between template and background.
     *
     * @param master The image where we look for matchings.
     * @param summation_table_single
     * @param summation_table_squared
     * @param zero_mean_template
     * @param sum_over_zero_mean_template
     * @param local_x Coordinate within \p master.
     * @param local_y Coordinate within \p master.
     */
    double calc_single_xcorr(const TileImage_GS_BYTE_shptr master,
			     const TileImage_GS_DOUBLE_shptr summation_table_single,
			     const TileImage_GS_DOUBLE_shptr summation_table_squared,
			     const TempImage_GS_DOUBLE_shptr zero_mean_template,
			     double sum_over_zero_mean_template,
			     unsigned int local_x,
			     unsigned int local_y) const;


    void add_gate(unsigned int x, unsigned int y,
		  struct prepared_template & tmpl);

  protected:

    /**
     * Calculate the next position for a template to background matching.
     * @return Returns false if there is no further position.
     */

    virtual bool get_next_pos(struct search_state * state,
			      struct prepared_template const& tmpl) const = 0;


    double get_current_threshold(double min_threshold, int num_steps, int step_i) const { 
      double delta_thresh = (1.0 - min_threshold) / (double)num_steps;
      return min_threshold + delta_thresh * (double)(num_steps - step_i);
    }


  public:

    TemplateMatching();

    virtual ~TemplateMatching();

    void set_threshold_steps(unsigned int steps) { threshold_steps = steps; }
    unsigned int get_threshold_steps() const { return threshold_steps; }

    /**
     * Get the correlation threshold for the hill climbing start phase.
     */

    double get_threshold_hc() const { return threshold_hc; }

    /**
     * Set the correlation threshold for the hill climbing.
     */

    void set_threshold_hc(double t) { threshold_hc = t; }

    /**
     * Get the correlation threshold for acepting gate recognitions.
     */
    double get_threshold_detection() const { return threshold_detection; }
    
    /**
     * Set the correlation threshold for acepting gate recognitions.
     */

    void set_threshold_detection(double t) { threshold_detection = t; }

    /**
     * Get the pixel step size.
     *
     * The correlation is not calculated for all (x,y). Instead the algorithm
     * skips pixel. The skipping depends on the correalation values. In areas
     * of higher correlation the step size is decremented. Else in correlation
     * valleys the step size is incremented. It is incremented up to a limit.
     * With this method you can set the limit.
     *
     * The image might be scaled before. This step size limit is in the
     * scale of the background image after(!) scaling.
     */

    unsigned int get_max_step_size() const { return max_step_size_search; }

    /**
     * Set the pixel step size.
     */
    
    void set_max_step_size(unsigned int s) { max_step_size_search = s; }

    /**
     * Get the scaling factor.
     * @return Returns a value >= 1 that is the factor for the down scaling.
     */

    unsigned int get_scaling_factor() const { return scale_down; }

    /**
     * Set the scaling factor.
     */

    void set_scaling_factor(unsigned int factor) { scale_down = factor; }


    /**
     * Run the template matching.
     */

    virtual void init(BoundingBox const& bounding_box, Project_shptr project);

    /**
     * Run the template matching.
     */

    virtual void run();

    /**
     * Set templates that should be matched.
     * Templates become sorted, so that larger templates are matched first.
     */

    void set_templates(std::list<GateTemplate_shptr> tmpl_set);


    /**
     * Set orientations that should be tested.
     */

    void set_orientations(std::list<Gate::ORIENTATION> tmpl_orientations);

    /**
     * Set the layers.
     * 
     */

    void set_layers(Layer_shptr layer_matching, Layer_shptr layer_insert) {
      this->layer_matching = layer_matching;
      this->layer_insert = layer_insert;
    }

  };


  typedef std::tr1::shared_ptr<TemplateMatching> TemplateMatching_shptr;


  /**
   * This class implements a template matching that basically scans line
   * by line to detect gate placements.
   */
  class TemplateMatchingNormal : public TemplateMatching {
  protected:
    bool get_next_pos(struct search_state * state,
		      struct prepared_template const& tmpl) const;
  public:
    TemplateMatchingNormal() {}
    ~TemplateMatchingNormal() {}
  };

  typedef std::tr1::shared_ptr<TemplateMatchingNormal> TemplateMatchingNormal_shptr;


  /**
   * This class is the base class for template matching along a grid.
   */

  class TemplateMatchingAlongGrid : public TemplateMatching {

  protected:

    bool initialize_state_struct(struct search_state * state,
				 int offs_min,
				 int offs_max) const;

    virtual bool get_next_pos(struct search_state * state,
			      struct prepared_template const& tmpl) const = 0;

  public:

    virtual ~TemplateMatchingAlongGrid() {}

  };


  /**
   * This class implements matching for gate template that are aligned in a row.
   */
  class TemplateMatchingInRows : public TemplateMatchingAlongGrid {

  protected:

    bool get_next_pos(struct search_state * state,
		      struct prepared_template const& tmpl) const;


  public:
    TemplateMatchingInRows() {}
    ~TemplateMatchingInRows() {}
  };

  typedef std::tr1::shared_ptr<TemplateMatchingInRows> TemplateMatchingInRows_shptr;

  /**
   * This class implements matching for gate template that are aligned in a column.
   */
  class TemplateMatchingInCols : public TemplateMatchingAlongGrid {

  protected:

    bool get_next_pos(struct search_state * state,
		      struct prepared_template const& tmpl) const;
  public:

    TemplateMatchingInCols() {}
    ~TemplateMatchingInCols() {}
  };

  typedef std::tr1::shared_ptr<TemplateMatchingInCols> TemplateMatchingInCols_shptr;

}


#endif
