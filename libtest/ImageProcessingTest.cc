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


#include <ImageProcessingTest.h>
#include <Image.h>
#include <IPPipe.h>
#include <IPCopy.h>
#include <IPConvolve.h>
#include <IPNormalize.h>
#include <IPThresholding.h>
#include <ImageManipulation.h>
#include <IPImageWriter.h>

#include <ImageHelper.h>
#include <CannyEdgeDetection.h>
#include <ZeroCrossingEdgeDetection.h>

#include <ImageHistogram.h>
#include <BackgroundClassifier.h>
#include <MedianFilter.h>
#include <AnalyzeWireGradient.h>
#include <LineSegmentExtraction.h>

#include <globals.h>
#include <stdlib.h>


CPPUNIT_TEST_SUITE_REGISTRATION (ImageProcessingTest);

using namespace std;
using namespace degate;


void ImageProcessingTest::setUp(void) { 
}

void ImageProcessingTest::tearDown(void) {
}


void ImageProcessingTest::test_pipe(void) {

  /*
   * Set up images
   */
  BackgroundImage_shptr in(new BackgroundImage(100, 100, 8));
 
  
  /*
   * Set up processing element
   */
  
  std::shared_ptr<IPCopy<BackgroundImage, TileImage_GS_DOUBLE> > copy_rgba_to_gs
    (new IPCopy<BackgroundImage, TileImage_GS_DOUBLE>() );
  
  std::shared_ptr<IPCopy<TileImage_GS_DOUBLE, BackgroundImage> > copy_gs_to_rgba
    (new IPCopy<TileImage_GS_DOUBLE, BackgroundImage>() );

  
  // Get plugin default params
  //IPParams_shptr plugin_params = plugin_rgba_to_gs->get_params();
  
  /*
   * Set up a pipe.
   */
  
  // IPPipe derives from ProgressControl.
  IPPipe pipe;
  CPPUNIT_ASSERT(pipe.is_empty() == true);

  // fill the pipe
  pipe.add(copy_rgba_to_gs);
  CPPUNIT_ASSERT(pipe.size() == 1);

  // remove a plug in frm the pipe
  //pipe->remove(copy_rgba_to_gs);

  // add again
  //pipe->add(copy_rgba_to_gs);
  //CPPUNIT_ASSERT(pipe.size() == 1);

  // add other plugin
  pipe.add(copy_gs_to_rgba);
  CPPUNIT_ASSERT(pipe.size() == 2);
  
  //ImageBase_shptr out = pipe.run(std::dynamic_pointer_cast<ImageBase>(in));
  ImageBase_shptr out = pipe.run(in);
 
}


void ImageProcessingTest::test_wire_matching(void) {

  //std::string testfile("libtest/testfiles/wire_matching_samples/mifare/contrast/good.tif");
  //std::string testfile("libtest/testfiles/wire_matching_samples/c_unknown/good/contrast/good.tif");
  std::string testfile("libtest/testfiles/wire_matching_samples/dect/dect.tif");
  //std::string testfile("libtest/testfiles/wire_matching_samples/legic/m2_part.tif");

  // load a background image
  TempImage_RGBA_shptr img_in = load_image<TempImage_RGBA>(testfile);

  CPPUNIT_ASSERT(img_in != NULL);
  CPPUNIT_ASSERT(img_in->get_width() > 0);
  CPPUNIT_ASSERT(img_in->get_height() > 0);

  CannyEdgeDetection ed(0, img_in->get_width(), 0, img_in->get_height(), 5);

  //wm.run(img_in);
  
}



void ImageProcessingTest::test_background_classification_dect(void) {
  /*
  unsigned int wire_diameter = 6;
  std::string directory("libtest/testfiles/wire_matching_samples/dect");
  std::string testfile(join_pathes(directory, "dect.tif"));

  TempImage_RGBA_shptr img_in = load_image<TempImage_RGBA>(testfile);

  TempImage_RGBA_shptr i3(new TempImage_RGBA(img_in->get_width(), img_in->get_height()));
  TempImage_RGBA_shptr img(new TempImage_RGBA(img_in->get_width(), img_in->get_height()));
  
  std::list<BoundingBox> bg_areas, fg_areas;
  //bg_areas.push_back(BoundingBox(370, 580, 0, 25));
  //bg_areas.push_back(BoundingBox(714, 888, 356, 400));
  //bg_areas.push_back(BoundingBox(256, 1013, 498, 541));

  bg_areas.push_back(BoundingBox(536,540,91,135));
  bg_areas.push_back(BoundingBox(659,656,82,136));
  bg_areas.push_back(BoundingBox(800,806,57,127));
  bg_areas.push_back(BoundingBox(129,244,105,127));
  bg_areas.push_back(BoundingBox(211,244,148,169));
  bg_areas.push_back(BoundingBox(377,419,553,569));
  bg_areas.push_back(BoundingBox(289,385,314,319));

  fg_areas.push_back(BoundingBox(305, 417, 288, 308));
  fg_areas.push_back(BoundingBox(0, 185, 454, 476));
  fg_areas.push_back(BoundingBox(991, 1157, 190, 211));
  fg_areas.push_back(BoundingBox(478,1289,289,301));

  median_filter<TempImage_RGBA, TempImage_RGBA>(img, img_in, wire_diameter);


  BackgroundClassifier bg_c(wire_diameter);
  bg_c.add_background_areas(img, bg_areas);
  bg_c.add_foreground_areas(img, fg_areas);
  bg_c.save_histograms(directory);


  TempImage_GS_DOUBLE_shptr prob(new TempImage_GS_DOUBLE(img_in->get_width(), img_in->get_height()));

  for(unsigned int y = 0; y < img->get_height(); y++)
    for(unsigned int x = 0; x < img->get_width(); x++) {

      rgba_pixel_t p = img->get_pixel(x, y);

      prob->set_pixel(x, y, bg_c.get_probability(p));

      if(bg_c.get_probability(img->get_pixel(x, y)) > 0)
	i3->set_pixel(x, y, img_in->get_pixel(x, y));
      else
	i3->set_pixel(x, y, 0);
    }

  save_image<TempImage_RGBA>(join_pathes(directory, "removed_background.tif"), i3);
  save_normalized_image<TempImage_GS_DOUBLE>(join_pathes(directory, "removed_background_prob.tif"), prob);

  //WireMatchingCanny wm(0, img_in->get_width(), 0, img_in->get_height(), wire_diameter);
  //wm.run(img_in, prob, directory);
 
  */
}





void ImageProcessingTest::test_background_classification_legic(void) {

  unsigned int wire_diameter = 10;
  std::string directory("libtest/testfiles/wire_matching_samples/legic");
  std::string testfile(join_pathes(directory, "m2_part.tif"));

  TempImage_RGBA_shptr img_in = load_image<TempImage_RGBA>(testfile);

  unsigned int width = img_in->get_width();
  unsigned int height = img_in->get_height();
  TempImage_RGBA_shptr i3(new TempImage_RGBA(width, height));
  TempImage_RGBA_shptr img(new TempImage_RGBA(width, height));
  
  std::list<BoundingBox> bg_areas, fg_areas;;

  /*
    for m2.tif
  bg_areas.push_back(BoundingBox(978,0,1056,124));

  fg_areas.push_back(BoundingBox(929,948,1227,1323));
  fg_areas.push_back(BoundingBox(926,962,416,511));
  fg_areas.push_back(BoundingBox(1048,1131,347,376));
  fg_areas.push_back(BoundingBox(1060,1152,29,37));
  fg_areas.push_back(BoundingBox(84,223,1542,1548));
  fg_areas.push_back(BoundingBox(266,362,1434,1443));
  fg_areas.push_back(BoundingBox(1404,1487,1433,1440));
  */

  fg_areas.push_back(BoundingBox(173,268,77,92));
  fg_areas.push_back(BoundingBox(62,80,35,166));
  fg_areas.push_back(BoundingBox(76,82,318,399));
  fg_areas.push_back(BoundingBox(401,408,257,433));
  fg_areas.push_back(BoundingBox(349,367,269,342));

  bg_areas.push_back(BoundingBox(146,168,454, 516));
  bg_areas.push_back(BoundingBox(369,397,279,419));
  bg_areas.push_back(BoundingBox(14,61,459,516));
  median_filter<TempImage_RGBA, TempImage_RGBA>(img, img_in, 2 /*wire_diameter >> 1*/);


  //BackgroundClassifier bg_c(wire_diameter);
  //bg_c.add_background_areas(img, bg_areas);
  //bg_c.add_foreground_areas(img, fg_areas);
  //bg_c.save_histograms(directory);

  // classifier

  unsigned int window_width = wire_diameter;
  unsigned int window_threshold = (wire_diameter * wire_diameter) >> 1;

  BackgroundClassifier<TempImage_RGBA, 
    HueImageHistogram> hue_classifier(img, window_width, window_threshold, "hue");  
  BackgroundClassifier<TempImage_RGBA, SaturationImageHistogram> 
    sat_classifier(img, window_width, window_threshold, "sat");
  BackgroundClassifier<TempImage_RGBA, LightnessImageHistogram> 
    value_classifier(img, window_width, window_threshold, "value");

  BackgroundClassifier<TempImage_RGBA, RedChannelImageHistogram> 
    red_classifier(img, window_width, window_threshold, "red");
  BackgroundClassifier<TempImage_RGBA, GreenChannelImageHistogram> 
    green_classifier(img, window_width, window_threshold, "green");
  BackgroundClassifier<TempImage_RGBA, BlueChannelImageHistogram> 
    blue_classifier(img, window_width, window_threshold, "blue");


  classifier_list_type classifiers;
  classifiers.push_back(&hue_classifier);
  classifiers.push_back(&sat_classifier);
  classifiers.push_back(&value_classifier);

  classifiers.push_back(&red_classifier);
  classifiers.push_back(&green_classifier);
  classifiers.push_back(&blue_classifier);


  for(classifier_list_type::iterator iter = classifiers.begin(); iter != classifiers.end(); ++iter) {
    if(BackgroundClassifierBase * b = dynamic_cast<BackgroundClassifierBase*>(*iter)) {
      b->add_background_areas(bg_areas);
      b->add_foreground_areas(fg_areas);
    }
  }
  
  coord_list positive_list;
  coord_list negative_list;

  positive_list.push_back(coord_type(152,10));
  positive_list.push_back(coord_type(38,153));
  positive_list.push_back(coord_type(112,223));
  positive_list.push_back(coord_type(159,217));
  positive_list.push_back(coord_type(180,234));
  positive_list.push_back(coord_type(162,323));
  positive_list.push_back(coord_type(46,324));
  positive_list.push_back(coord_type(347,338));
  positive_list.push_back(coord_type(214,434));
  positive_list.push_back(coord_type(172,420));
  positive_list.push_back(coord_type(93,458));
  positive_list.push_back(coord_type(38,190));
  positive_list.push_back(coord_type(38,162));
  positive_list.push_back(coord_type(14,228));
  positive_list.push_back(coord_type(203,327));
  positive_list.push_back(coord_type(57,351));

  negative_list.push_back(coord_type(23,23));
  negative_list.push_back(coord_type(164,488));
  negative_list.push_back(coord_type(162,392));
  negative_list.push_back(coord_type(54,388));
  negative_list.push_back(coord_type(163,372));
  negative_list.push_back(coord_type(164,294));
  negative_list.push_back(coord_type(53,278));
  negative_list.push_back(coord_type(35,338));
  negative_list.push_back(coord_type(109,236));
  negative_list.push_back(coord_type(205,170));
  negative_list.push_back(coord_type(38,279));
  negative_list.push_back(coord_type(160,374));
  negative_list.push_back(coord_type(190,293));
  negative_list.push_back(coord_type(144,408));
  negative_list.push_back(coord_type(35,464));
  negative_list.push_back(coord_type(416,306));
  negative_list.push_back(coord_type(416,349));
  negative_list.push_back(coord_type(348,358));
  negative_list.push_back(coord_type(376,367));
  negative_list.push_back(coord_type(316,318));
  negative_list.push_back(coord_type(316,362));
  negative_list.push_back(coord_type(378,394));
  negative_list.push_back(coord_type(165,294));


  vector<coord_type*> training_data;
  vector<int> training_label;
  


  std::cout << "training" << std::endl;

  for(coord_list::iterator samples_iter = positive_list.begin(); 
      samples_iter != positive_list.end(); ++samples_iter) {

    for(classifier_list_type::iterator iter = classifiers.begin(); iter != classifiers.end(); ++iter)
      if((*iter)->recognize(*samples_iter) != 1)
	std::cout << "warning: bad classification for +1 sample in " << (*iter)->get_name() << "." << std::endl;

    training_data.push_back(&(*samples_iter));
    training_label.push_back(1);
  }

  for(coord_list::iterator samples_iter = negative_list.begin(); 
      samples_iter != negative_list.end(); ++samples_iter) {

    for(classifier_list_type::iterator iter = classifiers.begin(); iter != classifiers.end(); ++iter)
      if((*iter)->recognize(*samples_iter) != -1)
	std::cout << "warning: bad classification for -1 sample in " << (*iter)->get_name() << "." << std::endl;

    training_data.push_back(&(*samples_iter));
    training_label.push_back(-1);
  }
  std::cout << "done" << std::endl;




  // use ada-boost to train the strong classifier from the weak classifiers 
  vector<float> weights = Classifier<coord_type>::adaboost(classifiers,
							   training_data,
							   training_label, 
							   200);

  unsigned int i = 0;
  std::cout << std::endl;
  for(vector<float>::const_iterator iter = weights.begin(); iter != weights.end(); ++iter, i++) {
    std::cout << "weight for " << classifiers[i]->get_name() << " : " << *iter << std::endl;
  }
  std::cout << std::endl;
  
  // construct a strong classifier
  MultiClassifier<coord_type> multi(weights, classifiers);


  TempImage_GS_DOUBLE_shptr prob(new TempImage_GS_DOUBLE(width, height));
  TempImage_GS_DOUBLE_shptr i4(new TempImage_GS_DOUBLE(width, height));
  
  
  std::cout << "classify image pixel" << std::endl;
  // classify
  for(unsigned int y = 0; y < img->get_height(); y++) {
    std::cout << "y = " << y << std::endl;

    for(unsigned int x = 0; x < img->get_width(); x++) {
      coord_type c(x, y);
      if(multi.recognize(c) == 1) {
	//i3->set_pixel(x, y, img_in->get_pixel(x, y));
	i4->set_pixel_as<double>(x, y, img_in->get_pixel_as<double>(x, y));
	prob->set_pixel(x, y, 1);
      }
      else {
	uint8_t gs = RGBA_TO_GS_BY_VAL(img_in->get_pixel(x, y));
	//i3->set_pixel(x, y, MERGE_CHANNELS(gs, gs, gs, 255));
	i4->set_pixel(x, y, 0);
	prob->set_pixel(x, y, 0);
      }
    }
  }
  

  save_normalized_image<TempImage_GS_DOUBLE>(join_pathes(directory, "removed_background.tif"), i4);


  ZeroCrossingEdgeDetection ed(0, img->get_width(), 0, img->get_height(), wire_diameter, 
			       10, 2.1, 
			       3, 15);

  //CannyEdgeDetection ed(0, img_in->get_width(), 0, img_in->get_height(), wire_diameter, 10, 
  //2.1, 0.35, 0.5);

  TempImage_GS_DOUBLE_shptr edge_image = ed.run(img_in, prob, directory);

  /*
  LineSegmentExtraction extraction(edge_image, wire_diameter/2, 3, ed.get_border());
  LineSegmentMap_shptr line_segments = extraction.run();


  ContourTracer tracer(line_segments, wire_diameter, 0.8*(double)wire_diameter);
  tracer.run();
  */

}

void ImageProcessingTest::test_line_segment_extraction(void) {

  unsigned int wire_diameter = 10;
  std::string directory("libtest/testfiles/wire_matching_samples/legic");
  std::string testfile(join_pathes(directory, "04_non_max_suppression.tif"));

  TempImage_RGBA_shptr img = load_image<TempImage_RGBA>(testfile);
  TempImage_GS_DOUBLE_shptr img2(new TempImage_GS_DOUBLE(img->get_width(), img->get_height()));
  
  copy_image<TempImage_GS_DOUBLE, TempImage_RGBA>(img2, img);

  LineSegmentExtraction<TempImage_GS_DOUBLE> extraction(img2, wire_diameter, wire_diameter/2, 10);
  extraction.run();
}

void ImageProcessingTest::test_mifare(void) {
  std::string directory("libtest/testfiles/wire_matching_samples/mifare/normal");
  //std::string directory("libtest/testfiles/wire_matching_samples/c_unknown/good");
  std::string testfile(join_pathes(directory, "good.tif"));

  int wire_diameter = 6;
  int median_size = wire_diameter;
  // load a background image
  TempImage_RGBA_shptr img_in = load_image<TempImage_RGBA>(testfile);

  CPPUNIT_ASSERT(img_in != NULL);
  CPPUNIT_ASSERT(img_in->get_width() > 0);
  CPPUNIT_ASSERT(img_in->get_height() > 0);

  TempImage_RGBA_shptr img(new TempImage_RGBA(img_in->get_width(), img_in->get_height()));
  median_filter<TempImage_RGBA, TempImage_RGBA>(img, img_in, median_size);

  /*
  CannyEdgeDetection ed(0, img->get_width(), 0, img->get_height(), wire_diameter, 10,
			2.1,
			0.30, 0.35);
  */

  ZeroCrossingEdgeDetection ed(0, img->get_width(), 0, img->get_height(), wire_diameter,
			       10, 2.1,
			       3, 10);

  TempImage_GS_DOUBLE_shptr i = ed.run(img, TempImage_GS_DOUBLE_shptr(), directory);


  
  LineSegmentExtraction<TempImage_GS_DOUBLE> extraction(i, wire_diameter/2, 2, ed.get_border());
  LineSegmentMap_shptr line_segments = extraction.run();

  /*
  ContourTracer tracer(line_segments, wire_diameter, 1.5 * (double)wire_diameter);
  tracer.run();
  */
}

void ImageProcessingTest::test_c_unknown(void) {
  std::string directory("libtest/testfiles/wire_matching_samples/c_unknown/good");
  std::string testfile(join_pathes(directory, "good.tif"));

  int wire_diameter = 3;
  int median_size = 3;
  // load a background image
  TempImage_RGBA_shptr img = load_image<TempImage_RGBA>(testfile);


  //TempImage_RGBA_shptr img(new TempImage_RGBA(img_in->get_width(), img_in->get_height()));
  //median_filter<TempImage_RGBA, TempImage_RGBA>(img, img_in, median_size);


  ZeroCrossingEdgeDetection ed(0, img->get_width(), 0, img->get_height(), wire_diameter, 
			       0, 0.8, 
			       1, 10);

  TempImage_GS_DOUBLE_shptr i = ed.run(img, TempImage_GS_DOUBLE_shptr(), directory);
  /*

  LineSegmentExtraction extraction(i, wire_diameter/2, 2, ed.get_border());
  LineSegmentMap_shptr line_segments = extraction.run();


  ContourTracer tracer(line_segments, wire_diameter,  1*(double)wire_diameter);
  std::list<ContourPath_shptr> pathes = tracer.run();
  */

}
