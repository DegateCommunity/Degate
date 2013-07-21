/* -*-c++-*-

 This file is part of the IC reverse engineering tool degate.

 Copyright 2013 by Taekgwan Kim 

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

#ifndef __REGIONLIST_H__
#define __REGIONLIST_H__

#include <Region.h>
#include <Image.h>
#include <FileSystem.h>

namespace degate {

  class RegionList {

    private:

    unsigned int width;
    unsigned int height;
    unsigned int count;
    Region_shptr_list mainList;

    public:

    bool isEmpty() {
      if(mainList.empty()) return true;
      else return false;
    }

    void free_all_region() {

      Region_shptr_list::iterator iter_Region_shptr;
      unsigned int cnt;

      debug(TM, "free all region start");
      for(iter_Region_shptr = mainList.begin(); iter_Region_shptr != mainList.end(); ++iter_Region_shptr) {
	++cnt;
	(*iter_Region_shptr)->free_region();
      }

      mainList.erase(mainList.begin(), mainList.end());
      print_region();
    }

    void set_region(Region_shptr region) {

      Region_shptr_list::iterator iter_Region_shptr, iter_merged;
      Region_shptr tmpRegion = NULL;
      unsigned int region_gap;
      unsigned int tmpY, tmpX_start, tmpX_end;
      unsigned int cnt = 0;

debug(TM, "set_region start");
      if(isEmpty()) {
	debug(TM, "first region created");
	tmpY = region->get_top();
	tmpX_start = ((((region->get_y_list()).begin())->x_list).begin())->x_start;
	tmpX_end = ((((region->get_y_list()).begin())->x_list).begin())->x_end;	
	debug(TM, "create list: y = %u, x1 = %u, x2 = %u", tmpY, tmpX_start, tmpX_end);
	mainList.push_back(region);
debug(TM, "set_region end with creating first region");
	return;
      }

      //made of one line region
      if(region->get_top() == region->get_bottom()) {
	tmpY = region->get_top();
	tmpX_start = ((((region->get_y_list()).begin())->x_list).begin())->x_start;
	tmpX_end = ((((region->get_y_list()).begin())->x_list).begin())->x_end;
	debug(TM, "y = %u, x1 = %u, x2 = %u", tmpY, tmpX_start, tmpX_end);
	//to check all region object
	for(iter_Region_shptr = mainList.begin();
		iter_Region_shptr != mainList.end(); ) {
	  ++cnt;
	  region_gap = region->get_top() - (*iter_Region_shptr)->get_bottom();
	  debug(TM, "region_gap : %u", region_gap);
	  //between region's y_bottom and line gap is 0 or 1
	  if(region_gap == 0 || region_gap == 1) {
	    //determine whether the region can includes line or not  
	    if((*iter_Region_shptr)->isOverlap(tmpY, tmpX_start, tmpX_end)) {
	      //if there are more overlapping regions, save current region to merge them 
	      if(tmpRegion == NULL) {
		debug(TM, "line added first detected region");
		tmpRegion = *iter_Region_shptr;
		tmpRegion->add_line(tmpY, tmpX_start, tmpX_end);
		++iter_Region_shptr;
	      }else {
		//one line involve two region, should merge them
		debug(TM, "region merged");
		tmpRegion->merge(*iter_Region_shptr);
		(*iter_Region_shptr)->free_region();
		iter_merged = iter_Region_shptr;
		++iter_Region_shptr;
		mainList.erase(iter_merged);
		//tmpRegion->add_line(tmpY, tmpX_start, tmpX_end);
		
	      }
	    }else {
	      //nothing to do. return to loop
	      debug(TM, "not overlap region");
	      ++iter_Region_shptr;
	    }
	  }else {
	    //nothing to do. return to loop
	    ++iter_Region_shptr;
	  }
	}
debug(TM, "number of checked regions : %u", cnt);
	//check all region, but there is no matching region. create new region
	if(tmpRegion == NULL) {
	  debug(TM, "Region's first line");
	  mainList.push_back(region);
	}
      //set region that is made of 2 or more lines.
      }else {
	//there is no case reach here
	debug(TM, "else3");
      }
debug(TM, "set_region end");
    }

    void set_region(unsigned int Y, unsigned int Xs, unsigned int Xe) {

      Region_shptr tmpRegion(new Region(Y, Xs, Xe));

      set_region(tmpRegion);
    }

    void print_region() {

      Region_shptr_list::iterator iter_region_shptr;
      unsigned int cnt = 0;

      for(iter_region_shptr = mainList.begin();
		iter_region_shptr != mainList.end();
		++iter_region_shptr) {
	++cnt;
	if(cnt == 1) debug(TM, "1st Region");
	else if(cnt == 2) debug(TM, "2nd Region");
	else if(cnt == 3) debug(TM, "3rd Region");
	else debug(TM, "%dth Region", cnt);
	(*iter_region_shptr)->print();
      }

      if(cnt == 0) debug(TM, "0 Region list");
    }

    unsigned int get_count() {

      Region_shptr_list::iterator iter_Region_shptr;
      unsigned int cnt;

      for(iter_Region_shptr = mainList.begin(); iter_Region_shptr != mainList.end(); ++iter_Region_shptr) {
	++cnt;
      }

      count = cnt;
      return count;
    }

    void save_region() {

      Region_shptr_list::iterator iter_Region_shptr;
      unsigned int chk = count;
      unsigned int i=0, k = 0;

      do {
	chk %= 10;
	++i;
      }while(chk > 9);
      for(iter_Region_shptr = mainList.begin(); iter_Region_shptr != mainList.end(); ++iter_Region_shptr) {
	TileImage_GS_DOUBLE_shptr j(new TileImage_GS_DOUBLE(width, height));
	char *buf = new char[i];
	sprintf(buf, "%d", k);
	(*iter_Region_shptr)->draw_region(join_pathes("/tmp", buf), j);
	k++;
      }
    }

    RegionList(unsigned int _width, unsigned int _height) {

      width = _width;
      height = _height;
      count = 0;
      debug(TM, "object constructor");
    }

    RegionList() {}

    ~RegionList() {debug(TM, "object destructor");}

  };

  typedef std::shared_ptr<RegionList> RegionList_shptr;
}

#endif
