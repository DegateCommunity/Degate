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

#include "Core/Primitive/Region.h"
#include "Core/Image/Image.h"
#include "Core/Utils/FileSystem.h"

namespace degate
{
	class RegionList
	{
	private:

		unsigned int      width;
		unsigned int      height;
		unsigned int      count;
		Region_shptr_list main_list;

	public:

		unsigned int get_width()
		{
			return width;
		}

		unsigned int get_height()
		{
			return height;
		}

		bool is_empty()
		{
			if (main_list.empty())
				return true;
			else
				return false;
		}

		void free_all_region()
		{
			Region_shptr_list::iterator iter_region_shptr;
			unsigned int                cnt;

			debug(TM, "free all region start");
			for (iter_region_shptr = main_list.begin(); iter_region_shptr != main_list.end(); ++iter_region_shptr)
			{
				++cnt;
				(*iter_region_shptr)->free_region();
			}

			main_list.erase(main_list.begin(), main_list.end());
			print_region();
		}

		void set_region(Region_shptr region)
		{
			Region_shptr_list::iterator iter_region_shptr, iter_merged;
			Region_shptr                tmp_region = nullptr;
			unsigned int                region_gap;
			unsigned int                tmp_y, tmp_x_start, tmp_x_end;
			unsigned int                cnt        = 0;

			if (is_empty())
			{
                tmp_y       = region->get_y_min();
                tmp_x_start = ((((region->get_y_list()).begin())->x_list).begin())->x_start;
                tmp_x_end   = ((((region->get_y_list()).begin())->x_list).begin())->x_end;
				main_list.push_back(region);
				debug(TM, "create list: y = %u, x1 = %u, x2 = %u", tmp_y, tmp_x_start, tmp_x_end);
				return;
			}

			//made of one line region
			if (region->get_y_min() == region->get_y_max())
			{
                tmp_y                  = region->get_y_min();
                tmp_x_start            = ((((region->get_y_list()).begin())->x_list).begin())->x_start;
                tmp_x_end              = ((((region->get_y_list()).begin())->x_list).begin())->x_end;
				//debug(TM, "y = %u, x1 = %u, x2 = %u", tmp_y, tmp_x_start, tmp_x_end);
				//to check all region object
				for (iter_region_shptr = main_list.begin();
                     iter_region_shptr != main_list.end();)
				{
					++cnt;
					region_gap = region->get_y_min() - (*iter_region_shptr)->get_y_max();
					//debug(TM, "region_gap : %u", region_gap);
					//between region's y_bottom and line gap is 0 or 1
					if (region_gap == 0 || region_gap == 1)
					{
						//determine whether the region can includes line or not  
						if ((*iter_region_shptr)->is_overlap(tmp_y, tmp_x_start, tmp_x_end))
						{
							//if there are more overlapping regions, save current region to merge them 
							if (tmp_region == nullptr)
							{
								//debug(TM, "line added first detected region");
								tmp_region = *iter_region_shptr;
								tmp_region->add_line(tmp_y, tmp_x_start, tmp_x_end);
								++iter_region_shptr;
							}
							else
							{
								//one line involve two region, should merge them
								//debug(TM, "region merged");
								tmp_region->merge(*iter_region_shptr);
								(*iter_region_shptr)->free_region();
								iter_merged = iter_region_shptr;
								++iter_region_shptr;
								main_list.erase(iter_merged);
								//tmp_region->add_line(tmp_y, tmp_x_start, tmp_x_end);
							}
						}
						else
						{
							//nothing to do. return to loop
							//debug(TM, "not overlap region");
							++iter_region_shptr;
						}
					}
					else
					{
						//nothing to do. return to loop
						++iter_region_shptr;
					}
				}
				//debug(TM, "number of checked regions : %u", cnt);
				//check all region, but there is no matching region. create new region
				if (tmp_region == nullptr)
				{
					//debug(TM, "Region's first line");
					main_list.push_back(region);
				}
				//set region that is made of 2 or more lines.
			}
			else
			{
				//there is no case reach here
				//debug(TM, "else3");
			}
			//debug(TM, "set_region end");
		}

		void set_region(unsigned int Y, unsigned int Xs, unsigned int Xe)
		{
			Region_shptr tmp_region(new Region(Y, Xs, Xe));

			set_region(tmp_region);
		}

		void print_region()
		{
			Region_shptr_list::iterator iter_region_shptr;
			unsigned int cnt = 0;

			for (iter_region_shptr = main_list.begin();
                 iter_region_shptr != main_list.end();
			     ++iter_region_shptr)
			{
				++cnt;
				if (cnt == 1) debug(TM, "1st Region");
				else if (cnt == 2) debug(TM, "2nd Region");
				else if (cnt == 3) debug(TM, "3rd Region");
				else debug(TM, "%dth Region", cnt);
				(*iter_region_shptr)->print();
			}

			if (cnt == 0) debug(TM, "0 Region list");
		}

		unsigned int get_count()
		{
			Region_shptr_list::iterator iter_region_shptr;
			unsigned int                cnt = 0;

			for (iter_region_shptr = main_list.begin(); iter_region_shptr != main_list.end(); ++iter_region_shptr)
			{
				++cnt;
			}

			return cnt;
		}

		void save_region()
		{
			Region_shptr_list::iterator iter_region_shptr;
			unsigned int                chk = count;
			unsigned int                i   = 0, k = 0;

			do
			{
				chk %= 10;
				++i;
			}
			while (chk > 9);
			for (iter_region_shptr          = main_list.begin(); iter_region_shptr != main_list.end(); ++iter_region_shptr)
			{
				TileImage_GS_DOUBLE_shptr j(new TileImage_GS_DOUBLE(width, height));
				char* buf = new char[i];
				sprintf(buf, "%d", k);
				(*iter_region_shptr)->draw_region(join_pathes("/tmp", buf), j);
				k++;
			}
		}

		void application_grid(unsigned int diameter)
		{
			Region_shptr_list::iterator iter_region_shptr, iter_erase;
			unsigned int                cnt = 0;

			for (iter_region_shptr = main_list.begin(); iter_region_shptr != main_list.end();)
			{
				debug(TM, "%d", cnt++);
				if (!(*iter_region_shptr)->is_wire(diameter))
				{
					(*iter_region_shptr)->free_region();
					iter_erase = iter_region_shptr;
					++iter_region_shptr;
					main_list.erase(iter_erase);
				}
				else
				{
					++iter_region_shptr;
				}
			}
		}

		TileImage_GS_DOUBLE_shptr get_binary()
		{
			Region_shptr_list::iterator iter_region_shptr;
			region_line_list::iterator  iter_region_line;
			end_point_list::iterator    iter_end_point;
			TileImage_GS_DOUBLE_shptr   binary(new TileImage_GS_DOUBLE(get_width(), get_height()));

			for (iter_region_shptr = main_list.begin(); iter_region_shptr != main_list.end(); ++iter_region_shptr)
			{
				//debug(TM, "top = %d, bot = %d", (*iter_region_shptr)->get_y_min(), (*iter_region_shptr)->get_y_max());
				(*iter_region_shptr)->draw_region(binary);
			}

			return binary;
		}

		TileImage_GS_DOUBLE_shptr get_unfixed_grid_binary(unsigned int diameter)
		{
			Region_shptr_list::iterator iter_region_shptr;
			region_line_list::iterator  iter_region_line;
			end_point_list::iterator    iter_end_point;
			TileImage_GS_DOUBLE_shptr   binary(new TileImage_GS_DOUBLE(get_width(), get_height()));

			for (iter_region_shptr = main_list.begin(); iter_region_shptr != main_list.end(); ++iter_region_shptr)
			{
				(*iter_region_shptr)->draw_unfixed_grid_region(binary, diameter);
			}

			return binary;
		}

		RegionList(unsigned int width, unsigned int height)
		{
			this->width = width;
			this->height = height;
			count = 0;
			debug(TM, "object constructor");
		}

		RegionList()
		{
		}

		~RegionList() { debug(TM, "object destructor"); }
	};

	typedef std::shared_ptr<RegionList> RegionList_shptr;
}

#endif
