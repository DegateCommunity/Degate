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

    unsigned int y_min;
    unsigned int y_max;
    unsigned int x_min;
    unsigned int x_max;
    regionLine_list y_list;

  private:

    void set_y_min(unsigned int y) {
      y_min = y;
    }

    void set_y_max(unsigned int y) {
      y_max = y;
    }

  public:

    unsigned int get_y_min() {
      return y_min;
    }

    unsigned int get_y_max() {
      return y_max;
    }

    regionLine_list get_y_list() {
      return y_list;
    }

    void init_region(unsigned int y, unsigned int x1, unsigned int x2) {

      set_y_min(y);
      set_y_max(y);

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

      
      if(y_max == Y) {
	iter_regionLine = y_list.end();
	--iter_regionLine;
	(iter_regionLine->x_list).push_back(tmp_endPoint);
      }else if(y_max + 1 == Y) {
	tmp_regionLine.y  = Y;
	(tmp_regionLine.x_list).push_back(tmp_endPoint);
	y_list.push_back(tmp_regionLine);
	set_y_max(Y);
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
//debug(TM, "y_max : %u, tmpRegion->get_y_max() : %u", y_max, tmpRegion->get_y_max());
      //assert(y_max - 1 == tmpRegion->get_y_max());

      tmp_regionLine_list = tmpRegion->get_y_list();
      iter_tmp_regionLine = tmp_regionLine_list.end();
      iter_regionLine = y_list.end();
      if(y_max - tmpRegion->get_y_max() == 1)
	--iter_regionLine;
      // case of second region is higher than first region.
      if(y_min > tmpRegion->get_y_min()) {
	--iter_tmp_regionLine;
	--iter_regionLine;
	while(iter_tmp_regionLine->y != y_min - 1) {
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
	//debug(TM, "not possible");
	return false;
      }

      for(iter_endPoint = (iter_regionLine->x_list).begin(); iter_endPoint != (iter_regionLine->x_list).end(); ++iter_endPoint) {
	refPoint1 = iter_endPoint->x_start;
	refPoint2 = iter_endPoint->x_end;
	
	if((refPoint1<=Xs && Xs<=refPoint2) || (Xs<=refPoint1 && refPoint1<=Xe)) {
	  //debug(TM, "line involved");
	  return true;
	}
      }
      //debug(TM, "line not involved");
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

    void draw_region(TileImage_GS_DOUBLE_shptr & region) {

      regionLine_list::iterator iter_regionLine;
      endPoint_list::iterator iter_endPoint;

      for(iter_regionLine = y_list.begin(); iter_regionLine != y_list.end(); ++iter_regionLine) {
	for(iter_endPoint = (iter_regionLine->x_list).begin(); iter_endPoint != (iter_regionLine->x_list).end(); ++iter_endPoint) {
	  for(unsigned int i = iter_endPoint->x_start; i <= iter_endPoint->x_end; ++i) {
	    region->set_pixel(i, iter_regionLine->y, 1);
	  }
	}
      }
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

    void draw_unfixed_grid_region(TileImage_GS_DOUBLE_shptr & region, unsigned int diameter) {

      regionLine_list::iterator iter_regionLine;
      endPoint_list::iterator iter_endPoint;
      unsigned int x_base, x_remainder;
      bool starting_point = true;

      for(iter_regionLine = y_list.begin(); iter_regionLine != y_list.end(); ++iter_regionLine) {
	for(iter_endPoint = (iter_regionLine->x_list).begin(); iter_endPoint != (iter_regionLine->x_list).end(); ++iter_endPoint) {
	  for(unsigned int i = iter_endPoint->x_start; i <= iter_endPoint->x_end; ++i) {
	    region->set_pixel(i, iter_regionLine->y, 1);
	  }
	}
      }

      for(iter_regionLine = y_list.begin(); iter_regionLine != y_list.end(); ++iter_regionLine) {
	if((iter_regionLine->y - y_min)%diameter == diameter/2) {
	  for(iter_endPoint = (iter_regionLine->x_list).begin(); iter_endPoint != (iter_regionLine->x_list).end(); ++iter_endPoint) {
	    if(starting_point) {
	      x_remainder = (iter_endPoint->x_start + diameter/2) % diameter;
	      starting_point = false;
	    }
	    //x_base = iter_endPoint->x_start + (x_remainder - iter_endPoint->x_start % diameter);
	    x_base = iter_endPoint->x_start + (diameter - iter_endPoint->x_start%diameter) + x_remainder;
	    for(unsigned int x = x_base; x < iter_endPoint->x_end; x = x + diameter) {
	      region->set_pixel(x, iter_regionLine->y, 0);
	    }
	  }
	}
      }
    }

    bool isPoint(unsigned int _x, unsigned int _y) {

      regionLine_list::iterator iter_regionLine;
      endPoint_list::iterator iter_endPoint;

      for(iter_regionLine = y_list.begin(); iter_regionLine != y_list.end(); ++iter_regionLine) {
	if(iter_regionLine->y == _y) {
	  for(iter_endPoint = (iter_regionLine->x_list).begin(); iter_endPoint != (iter_regionLine->x_list).end(); ++iter_endPoint) {
	    if(iter_endPoint->x_start <= _x && iter_endPoint->x_end >= _x)
	      return true;
	    else
	      continue;
	  }
	  return false;
	}else
	  continue;
      }

      return false;
    }

    bool isWire(unsigned int diameter) {

      regionLine_list::iterator iter_regionLine;
      endPoint_list::iterator iter_endPoint;
      unsigned int x_base, x_remainder;
      unsigned int num_grid = 0;
      bool starting_point = true;

      for(iter_regionLine = y_list.begin(); iter_regionLine != y_list.end(); ++iter_regionLine) {
	//if((iter_regionLine->y)%diameter == 0) {
	if(((iter_regionLine->y) - y_min)%diameter == diameter/2) {
	  for(iter_endPoint = iter_regionLine->x_list.begin(); iter_endPoint != iter_regionLine->x_list.end(); ++iter_endPoint) {
	    if(starting_point) {
	      x_remainder = (iter_endPoint->x_start + diameter/2) % diameter;
	      starting_point = false;
	    }
	    //x_base = iter_endPoint->x_start + (x_remainder - iter_endPoint->x_start % diameter);
	    x_base = iter_endPoint->x_start + (diameter - iter_endPoint->x_start%diameter) + x_remainder;
	    while(isPoint(x_base, iter_regionLine->y)) {
	      num_grid++;
	      if(isPoint(x_base + diameter, iter_regionLine->y) &&
		  isPoint(x_base, iter_regionLine->y + diameter) &&
		  isPoint(x_base + diameter, iter_regionLine->y + diameter)) {
		debug(TM, "not wire!!!");
		return false;
	      }
	      else
		x_base += diameter;
	    }
	  }
	}
      }

      if(num_grid < 3)
	return false;

      return true;
    }

    unsigned int get_height(unsigned int _x, unsigned int _y) {

      assert(isPoint(_x, _y));
      unsigned int height = 1;

      unsigned int tmpY = _y;
      while(isPoint(_x, --tmpY)) {
	height++;
      }

      tmpY = _y;
      while(isPoint(_x, ++tmpY)) {
	height++;
      }

      return height;
    }

    Region(unsigned int Y, unsigned int Xs, unsigned int Xe) :
      y_min(Y),
      y_max(Y),
      x_min(Xs),
      x_max(Xe) {

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
