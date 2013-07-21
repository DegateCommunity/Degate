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

#ifndef __REGION_H__
#define __REGION_H__

#include "globals.h"
#include <ImageHelper.h>

namespace degate {

  typedef struct _endPoint {
    unsigned int x_start;
    unsigned int x_end;
  }endPoint;

  typedef std::list<endPoint> endPoint_list;

  typedef struct _regionLine {
    unsigned int y;
    endPoint_list x_list;
  }regionLine;

  typedef std::list<regionLine> regionLine_list;

  class Region {

  private:

    unsigned int y_top;
    unsigned int y_bottom;
    regionLine_list y_list;

  private:

    void set_top(unsigned int y) {
      y_top = y;
    }

    void set_bottom(unsigned int y) {
      y_bottom = y;
    }

  public:

    unsigned int get_top() {
      return y_top;
    }

    unsigned int get_bottom() {
      return y_bottom;
    }

    regionLine_list get_y_list() {
      return y_list;
    }

    void init_region(unsigned int y, unsigned int x1, unsigned int x2) {

      set_top(y);
      set_bottom(y);

      endPoint tmpX = {x1, x2};
      endPoint_list listX;
      listX.push_back(tmpX);

      regionLine tmpY = {y, listX};
      y_list.push_back(tmpY);

    }

    void add_line(unsigned int Y, unsigned int Xs, unsigned int Xe) {

      regionLine_list::iterator iter_regionLine;
      regionLine tmp_regionLine;
      endPoint tmp_endPoint = {Xs, Xe};

      
      if(y_bottom == Y) {
	iter_regionLine = y_list.end();
	--iter_regionLine;
	(iter_regionLine->x_list).push_back(tmp_endPoint);
      }else if(y_bottom + 1 == Y) {
	tmp_regionLine.y  = Y;
	(tmp_regionLine.x_list).push_back(tmp_endPoint);
	y_list.push_back(tmp_regionLine);
	set_bottom(Y);
      }else {
	//add_line failed. but this case is not possible
	debug(TM, "add_line failed.");
	return;
      }

      return;
    }

    void merge(std::shared_ptr<Region> tmpRegion) {

      regionLine_list tmp_regionLine_list;
      endPoint_list tmp_endPoint_list;
      regionLine_list::iterator iter_tmp_regionLine, iter_regionLine;
      endPoint_list::iterator iter_tmp_endPoint, iter_endPoint;
debug(TM, "y_bottom : %u, tmpRegion->get_bottom() : %u", y_bottom, tmpRegion->get_bottom());
      assert(y_bottom - 1 == tmpRegion->get_bottom());

      tmp_regionLine_list = tmpRegion->get_y_list();
      iter_tmp_regionLine = tmp_regionLine_list.end();
      iter_regionLine = y_list.end();
      --iter_regionLine;
      // case of second region is higher than first region.
      if(y_top > tmpRegion->get_top()) {
	--iter_tmp_regionLine;
	--iter_regionLine;
	while(iter_tmp_regionLine->y != y_top - 1) {
	  tmp_endPoint_list = iter_tmp_regionLine->x_list;
	  iter_tmp_endPoint = tmp_endPoint_list.begin();
	  iter_endPoint = (iter_regionLine->x_list).begin();
	  while(iter_endPoint != (iter_regionLine->x_list).end()) {
	    while(iter_tmp_endPoint != tmp_endPoint_list.end()) {
	      if(iter_endPoint->x_start < iter_tmp_endPoint->x_start) {
		break;
	      }else {
		(iter_regionLine->x_list).insert(iter_endPoint, *iter_tmp_endPoint);
		++iter_tmp_endPoint;
	      }
	    }
	    ++iter_endPoint;
	  }

	  if(iter_tmp_endPoint != tmp_endPoint_list.end()) {
	    while(iter_tmp_endPoint != tmp_endPoint_list.end()) {
	      (iter_regionLine->x_list).push_back(*iter_tmp_endPoint);
	      ++iter_tmp_endPoint;
	    }
	  }
	  --iter_tmp_regionLine;
	  --iter_regionLine;
	}

	while(iter_tmp_regionLine != tmp_regionLine_list.begin()) {
	  y_list.push_front(*iter_tmp_regionLine);
	  --iter_tmp_regionLine;
	}
	if(iter_tmp_regionLine == tmp_regionLine_list.begin())
	  y_list.push_front(*iter_tmp_regionLine);
      }else {
	do {
	  --iter_tmp_regionLine;
	  --iter_regionLine;
	  tmp_endPoint_list = iter_tmp_regionLine->x_list;
	  iter_tmp_endPoint = tmp_endPoint_list.begin();
	  iter_endPoint = (iter_regionLine->x_list).begin();
	  while(iter_endPoint != (iter_regionLine->x_list).end()) {
	    while(iter_tmp_endPoint != tmp_endPoint_list.end()) {
	      if(iter_endPoint->x_start < iter_tmp_endPoint->x_start) {
		break;
	      }else {
		(iter_regionLine->x_list).insert(iter_endPoint, *iter_tmp_endPoint);
		++iter_tmp_endPoint;
	      }
	    }
	    ++iter_endPoint;
	  }

	  if(iter_tmp_endPoint != tmp_endPoint_list.end()) {
	    while(iter_tmp_endPoint != tmp_endPoint_list.end()) {
	      (iter_regionLine->x_list).push_back(*iter_tmp_endPoint);
	      ++iter_tmp_endPoint;
	    }
	  }
	}while(iter_tmp_regionLine != tmp_regionLine_list.begin());
      }

    }

    bool isOverlap(unsigned int Y, unsigned int Xs, unsigned int Xe) {

      regionLine_list::iterator iter_regionLine;
      endPoint_list::iterator iter_endPoint;
      unsigned int refPoint1, refPoint2;

      iter_regionLine = y_list.end();
      --iter_regionLine;
      if(iter_regionLine->y == Y) {
	--iter_regionLine;
      }

      if(iter_regionLine->y != Y-1) {
	debug(TM, "not possible");
	return false;
      }

      for(iter_endPoint = (iter_regionLine->x_list).begin(); iter_endPoint != (iter_regionLine->x_list).end(); ++iter_endPoint) {
	refPoint1 = iter_endPoint->x_start;
	refPoint2 = iter_endPoint->x_end;
	
	if((refPoint1<=Xs && Xs<=refPoint2) || (Xs<=refPoint1 && refPoint1<=Xe)) {
	  debug(TM, "line involved");
	  return true;
	}
      }
      debug(TM, "line not involved");
      return false;
    }

    void print() {

      regionLine_list::iterator iter_regionLine;
      endPoint_list::iterator iter_endPoint;
      unsigned int cnt = 0;

      debug(TM, "\n");
      for(iter_regionLine = y_list.begin(); iter_regionLine != y_list.end(); ++iter_regionLine) {
	++cnt;
	debug(TM, "y : %d", iter_regionLine->y);
	for(iter_endPoint = (iter_regionLine->x_list).begin(); iter_endPoint != (iter_regionLine->x_list).end(); ++iter_endPoint) {
	  debug(TM, "\tx_start : %d", iter_endPoint->x_start);
	  debug(TM, "\tx_end : %d", iter_endPoint->x_end);
	}
      }

      if(cnt == 0) debug(TM, "0 line region");
    }

    void free_region() {

      regionLine_list::iterator iter_regionLine;
      endPoint_list::iterator iter_endPoint;

      for(iter_regionLine = y_list.begin(); iter_regionLine != y_list.end(); ++iter_regionLine) {
	(iter_regionLine->x_list).erase((iter_regionLine->x_list).begin(), (iter_regionLine->x_list).end());
      }

      y_list.erase(y_list.begin(), y_list.end());
      print();
    }

    void draw_region(std::string const& path, TileImage_GS_DOUBLE_shptr & region) {

      regionLine_list::iterator iter_regionLine;
      endPoint_list::iterator iter_endPoint;

      for(iter_regionLine = y_list.begin(); iter_regionLine != y_list.end(); ++iter_regionLine) {
	for(iter_endPoint = (iter_regionLine->x_list).begin(); iter_endPoint != (iter_regionLine->x_list).end(); ++iter_endPoint) {
	  for(unsigned int i = iter_endPoint->x_start; i <= iter_endPoint->x_end; ++i) {
	    region->set_pixel(i, iter_regionLine->y, 1);
	  }
	}
      }

      save_normalized_image<TileImage_GS_DOUBLE>(path, region);
    }

    Region(unsigned int Y, unsigned int Xs, unsigned int Xe) :
      y_top(Y),
      y_bottom(Y) {

      endPoint tmpX = {Xs, Xe};
      endPoint_list listX;
      listX.push_back(tmpX);

      regionLine tmpY = {Y, listX};
      y_list.push_back(tmpY);
    }

    Region() {}

    ~Region() {}

  };

  typedef std::shared_ptr<Region> Region_shptr;
  typedef std::list<Region_shptr> Region_shptr_list;

}

#endif
