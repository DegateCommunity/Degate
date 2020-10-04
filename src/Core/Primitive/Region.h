/**
 * This file is part of the IC reverse engineering tool Degate.
 *
 * Copyright 2013 by Taekgwan Kim
 * Copyright 2019-2020 Dorian Bachelot
 *
 * Degate is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * Degate is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with degate. If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef __REGION_H__
#define __REGION_H__

#include "Globals.h"
#include "Core/Image/ImageHelper.h"

namespace degate
{
    struct end_point
    {
        unsigned int x_start;
        unsigned int x_end;
    };

    typedef std::list<end_point> end_point_list;

    struct region_line
    {
        unsigned int   y;
        end_point_list x_list;
    };

    typedef std::list<region_line> region_line_list;

    class Region
    {
    private:

        unsigned int     y_min;
        unsigned int     y_max;
        unsigned int     x_min;
        unsigned int     x_max;
        region_line_list y_list;

    private:

        void set_y_min(unsigned int y)
        {
            y_min = y;
        }

        void set_y_max(unsigned int y)
        {
            y_max = y;
        }

    public:

        unsigned int get_y_min()
        {
            return y_min;
        }

        unsigned int get_y_max()
        {
            return y_max;
        }

        region_line_list get_y_list()
        {
            return y_list;
        }

        void init_region(unsigned int y, unsigned int x1, unsigned int x2)
        {
            set_y_min(y);
            set_y_max(y);

            end_point      tmp_x = {x1, x2};
            end_point_list list_x;
            list_x.push_back(tmp_x);

            region_line tmp_y = {y, list_x};
            y_list.push_back(tmp_y);
        }

        void add_line(unsigned int Y, unsigned int Xs, unsigned int Xe)
        {
            region_line_list::iterator iter_region_line;
            region_line                tmp_region_line;
            end_point                  tmp_end_point = {Xs, Xe};


            if (y_max == Y)
            {
                iter_region_line = y_list.end();
                --iter_region_line;
                (iter_region_line->x_list).push_back(tmp_end_point);
            }
            else if (y_max + 1 == Y)
            {
                tmp_region_line.y = Y;
                (tmp_region_line.x_list).push_back(tmp_end_point);
                y_list.push_back(tmp_region_line);
                set_y_max(Y);
            }
            else
            {
                //add_line failed. but this case is not possible
                debug(TM, "add_line failed.");
                return;
            }

            return;
        }

        void merge(std::shared_ptr<Region> tmp_region)
        {
            region_line_list           tmp_region_line_list;
            end_point_list             tmp_end_point_list;
            region_line_list::iterator iter_tmp_region_line, iter_region_line;
            end_point_list::iterator   iter_tmp_end_point, iter_end_point;
            //debug(TM, "y_max : %u, tmp_region->get_y_max() : %u", y_max, tmp_region->get_y_max());
            //assert(y_max - 1 == tmp_region->get_y_max());

            tmp_region_line_list = tmp_region->get_y_list();
            iter_tmp_region_line = tmp_region_line_list.end();
            iter_region_line     = y_list.end();
            if (y_max - tmp_region->get_y_max() == 1)
                --iter_region_line;
            // case of second region is higher than first region.
            if (y_min > tmp_region->get_y_min())
            {
                --iter_tmp_region_line;
                --iter_region_line;
                while (iter_tmp_region_line->y != y_min - 1)
                {
                    tmp_end_point_list = iter_tmp_region_line->x_list;
                    iter_tmp_end_point = tmp_end_point_list.begin();
                    iter_end_point     = (iter_region_line->x_list).begin();
                    while (iter_end_point != (iter_region_line->x_list).end())
                    {
                        while (iter_tmp_end_point != tmp_end_point_list.end())
                        {
                            if (iter_end_point->x_start < iter_tmp_end_point->x_start)
                            {
                                break;
                            }
                            else
                            {
                                (iter_region_line->x_list).insert(iter_end_point, *iter_tmp_end_point);
                                ++iter_tmp_end_point;
                            }
                        }
                        ++iter_end_point;
                    }

                    if (iter_tmp_end_point != tmp_end_point_list.end())
                    {
                        while (iter_tmp_end_point != tmp_end_point_list.end())
                        {
                            (iter_region_line->x_list).push_back(*iter_tmp_end_point);
                            ++iter_tmp_end_point;
                        }
                    }
                    --iter_tmp_region_line;
                    --iter_region_line;
                }

                while (iter_tmp_region_line != tmp_region_line_list.begin())
                {
                    y_list.push_front(*iter_tmp_region_line);
                    --iter_tmp_region_line;
                }
                if (iter_tmp_region_line == tmp_region_line_list.begin())
                    y_list.push_front(*iter_tmp_region_line);
            }
            else
            {
                do
                {
                    --iter_tmp_region_line;
                    --iter_region_line;
                    tmp_end_point_list = iter_tmp_region_line->x_list;
                    iter_tmp_end_point = tmp_end_point_list.begin();
                    iter_end_point     = (iter_region_line->x_list).begin();
                    while (iter_end_point != (iter_region_line->x_list).end())
                    {
                        while (iter_tmp_end_point != tmp_end_point_list.end())
                        {
                            if (iter_end_point->x_start < iter_tmp_end_point->x_start)
                            {
                                break;
                            }
                            else
                            {
                                (iter_region_line->x_list).insert(iter_end_point, *iter_tmp_end_point);
                                ++iter_tmp_end_point;
                            }
                        }
                        ++iter_end_point;
                    }

                    if (iter_tmp_end_point != tmp_end_point_list.end())
                    {
                        while (iter_tmp_end_point != tmp_end_point_list.end())
                        {
                            (iter_region_line->x_list).push_back(*iter_tmp_end_point);
                            ++iter_tmp_end_point;
                        }
                    }
                }
                while (iter_tmp_region_line != tmp_region_line_list.begin());
            }
        }

        bool is_overlap(unsigned int Y, unsigned int Xs, unsigned int Xe)
        {
            region_line_list::iterator iter_region_line;
            end_point_list::iterator   iter_end_point;
            unsigned int               ref_point_1, ref_point_2;

            iter_region_line = y_list.end();
            --iter_region_line;
            if (iter_region_line->y == Y)
            {
                if (iter_region_line != y_list.begin())
                    --iter_region_line;
            }

            if (iter_region_line->y != Y - 1)
            {
                //debug(TM, "not possible");
                return false;
            }

            for (iter_end_point = (iter_region_line->x_list).begin(); iter_end_point != (iter_region_line->x_list).end(); ++
                 iter_end_point)
            {
                ref_point_1 = iter_end_point->x_start;
                ref_point_2 = iter_end_point->x_end;

                if ((ref_point_1 <= Xs && Xs <= ref_point_2) || (Xs <= ref_point_1 && ref_point_1 <= Xe))
                {
                    //debug(TM, "line involved");
                    return true;
                }
            }
            //debug(TM, "line not involved");
            return false;
        }

        void print()
        {
            region_line_list::iterator iter_region_line;
            end_point_list::iterator   iter_end_point;
            unsigned int               cnt = 0;

            debug(TM, "\n");
            for (iter_region_line = y_list.begin(); iter_region_line != y_list.end(); ++iter_region_line)
            {
                ++cnt;
                debug(TM, "y : %d", iter_region_line->y);
                for (iter_end_point = (iter_region_line->x_list).begin(); iter_end_point != (iter_region_line->x_list).end()
                     ; ++iter_end_point)
                {
                    debug(TM, "\tx_start : %d", iter_end_point->x_start);
                    debug(TM, "\tx_end : %d", iter_end_point->x_end);
                }
            }

            if (cnt == 0) debug(TM, "0 line region");
        }

        void free_region()
        {
            region_line_list::iterator iter_region_line;
            end_point_list::iterator   iter_end_point;

            for (iter_region_line = y_list.begin(); iter_region_line != y_list.end(); ++iter_region_line)
            {
                (iter_region_line->x_list).erase((iter_region_line->x_list).begin(), (iter_region_line->x_list).end());
            }

            y_list.erase(y_list.begin(), y_list.end());
            print();
        }

        void draw_region(TileImage_GS_DOUBLE_shptr& region)
        {
            region_line_list::iterator iter_region_line;
            end_point_list::iterator   iter_end_point;

            for (iter_region_line = y_list.begin(); iter_region_line != y_list.end(); ++iter_region_line)
            {
                for (iter_end_point = (iter_region_line->x_list).begin(); iter_end_point != (iter_region_line->x_list).end()
                     ; ++iter_end_point)
                {
                    for (unsigned int i = iter_end_point->x_start; i <= iter_end_point->x_end; ++i)
                    {
                        region->set_pixel(i, iter_region_line->y, 1);
                    }
                }
            }
        }

        void draw_region(std::string const& path, TileImage_GS_DOUBLE_shptr& region)
        {
            region_line_list::iterator iter_region_line;
            end_point_list::iterator   iter_end_point;

            for (iter_region_line = y_list.begin(); iter_region_line != y_list.end(); ++iter_region_line)
            {
                for (iter_end_point = (iter_region_line->x_list).begin(); iter_end_point != (iter_region_line->x_list).end()
                     ; ++iter_end_point)
                {
                    for (unsigned int i = iter_end_point->x_start; i <= iter_end_point->x_end; ++i)
                    {
                        region->set_pixel(i, iter_region_line->y, 1);
                    }
                }
            }

            save_normalized_image<TileImage_GS_DOUBLE>(path, region);
        }

        void draw_unfixed_grid_region(TileImage_GS_DOUBLE_shptr& region, unsigned int diameter)
        {
            region_line_list::iterator iter_region_line;
            end_point_list::iterator   iter_end_point;
            unsigned int               x_base, x_remainder;
            bool                       starting_point = true;

            for (iter_region_line = y_list.begin(); iter_region_line != y_list.end(); ++iter_region_line)
            {
                for (iter_end_point = (iter_region_line->x_list).begin(); iter_end_point != (iter_region_line->x_list).end()
                     ; ++iter_end_point)
                {
                    for (unsigned int i = iter_end_point->x_start; i <= iter_end_point->x_end; ++i)
                    {
                        region->set_pixel(i, iter_region_line->y, 1);
                    }
                }
            }

            for (iter_region_line = y_list.begin(); iter_region_line != y_list.end(); ++iter_region_line)
            {
                if ((iter_region_line->y - y_min) % diameter == diameter / 2)
                {
                    for (iter_end_point = (iter_region_line->x_list).begin(); iter_end_point != (iter_region_line->x_list).
                         end(); ++iter_end_point)
                    {
                        if (starting_point)
                        {
                            x_remainder = (iter_end_point->x_start + diameter / 2) % diameter;
                            starting_point = false;
                        }
                        //x_base = iter_end_point->x_start + (x_remainder - iter_end_point->x_start % diameter);
                        x_base = iter_end_point->x_start + (diameter - iter_end_point->x_start % diameter) + x_remainder;
                        for (unsigned int x = x_base; x < iter_end_point->x_end; x = x + diameter)
                        {
                            region->set_pixel(x, iter_region_line->y, 0);
                        }
                    }
                }
            }
        }

        bool is_point(unsigned int x, unsigned int y)
        {
            region_line_list::iterator iter_region_line;
            end_point_list::iterator   iter_end_point;

            for (iter_region_line = y_list.begin(); iter_region_line != y_list.end(); ++iter_region_line)
            {
                if (iter_region_line->y == y)
                {
                    for (iter_end_point = (iter_region_line->x_list).begin(); iter_end_point != (iter_region_line->x_list).
                         end(); ++iter_end_point)
                    {
                        if (iter_end_point->x_start <= x && iter_end_point->x_end >= x)
                            return true;
                        else
                            continue;
                    }
                    return false;
                }
                else
                    continue;
            }

            return false;
        }

        bool is_wire(unsigned int diameter)
        {
            region_line_list::iterator iter_region_line;
            end_point_list::iterator   iter_end_point;
            unsigned int               x_base, x_remainder;
            unsigned int               num_grid = 0;
            bool                       starting_point = true;

            for (iter_region_line = y_list.begin(); iter_region_line != y_list.end(); ++iter_region_line)
            {
                //if((iter_region_line->y)%diameter == 0) {
                if (((iter_region_line->y) - y_min) % diameter == diameter / 2)
                {
                    for (iter_end_point = iter_region_line->x_list.begin(); iter_end_point != iter_region_line->x_list.end()
                         ; ++iter_end_point)
                    {
                        if (starting_point)
                        {
                            x_remainder = (iter_end_point->x_start + diameter / 2) % diameter;
                            starting_point = false;
                        }
                        //x_base = iter_end_point->x_start + (x_remainder - iter_end_point->x_start % diameter);
                        x_base = iter_end_point->x_start + (diameter - iter_end_point->x_start % diameter) + x_remainder;
                        while (is_point(x_base, iter_region_line->y))
                        {
                            num_grid++;
                            if (is_point(x_base + diameter, iter_region_line->y) &&
                                is_point(x_base, iter_region_line->y + diameter) &&
                                is_point(x_base + diameter, iter_region_line->y + diameter))
                            {
                                debug(TM, "not wire!!!");
                                return false;
                            }
                            else
                                x_base += diameter;
                        }
                    }
                }
            }

            if (num_grid < 3)
                return false;

            return true;
        }

        unsigned int get_height(unsigned int x, unsigned int y)
        {
            assert(is_point(x, y));
            unsigned int height = 1;

            unsigned int tmp_y = y;
            while (is_point(x, --tmp_y))
            {
                height++;
            }

            tmp_y = y;
            while (is_point(x, ++tmp_y))
            {
                height++;
            }

            return height;
        }

        Region(unsigned int Y, unsigned int Xs, unsigned int Xe) :
            y_min(Y),
            y_max(Y),
            x_min(Xs),
            x_max(Xe)
        {
            end_point      tmp_x = {Xs, Xe};
            end_point_list list_x;
            list_x.push_back(tmp_x);

            region_line tmp_y = {Y, list_x};
            y_list.push_back(tmp_y);
        }

        Region()
        {
        }

        ~Region()
        {
        }
    };

    typedef std::shared_ptr<Region> Region_shptr;
    typedef std::list<Region_shptr> Region_shptr_list;
}

#endif
