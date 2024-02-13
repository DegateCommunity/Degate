/**
 * This file is part of the IC reverse engineering tool Degate.
 *
 * Copyright 2008, 2009, 2010 by Martin Schobert
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

#include "Core/LogicModel/Gate/AutoNameGates.h"
#include "Core/LogicModel/LogicModelHelper.h"
#include "Core/LogicModel/Layer.h"

using namespace degate;

bool compare_min_x(const Gate_shptr lhs, const Gate_shptr rhs)
{
    return lhs->get_min_x() < rhs->get_min_x();
}

bool compare_min_y(const Gate_shptr lhs, const Gate_shptr rhs)
{
    return lhs->get_min_y() < rhs->get_min_y();
}

AutoNameGates::AutoNameGates(LogicModel_shptr lmodel, ORIENTATION orientation) :
        lmodel(lmodel),
        orientation(orientation)
{
    layer = get_first_logic_layer(lmodel);
}

void AutoNameGates::run()
{
    std::vector<unsigned int> histogram(std::max(lmodel->get_width(), lmodel->get_height()));

    fill_histogram(lmodel, histogram);
    std::list<int> scan_lines = scan_histogram(histogram);
    rename_gates(histogram, scan_lines);
}

void AutoNameGates::rename_gates(std::vector<unsigned int> const& histogram, std::list<int> const& scan_lines) const
{
    unsigned int col_num = 1;
    unsigned int row_num = 1;

    for (auto i : scan_lines)
    {
        // naming = along-rows => histogram along y-axis, following scanlines along x-axis


        // collect all gates placed along scanline

        std::list<Gate_shptr> gate_list;

        BoundingBox bbox(orientation == ALONG_ROWS ? 0 : i,
                         orientation == ALONG_ROWS ? layer->get_width() - 1 : i,
                         orientation == ALONG_COLS ? 0 : i,
                         orientation == ALONG_COLS ? layer->get_height() - 1 : i);

        for (Layer::qt_region_iterator iter = layer->region_begin(bbox); iter != layer->region_end(); ++iter)
        {
            if (Gate_shptr gate = std::dynamic_pointer_cast<Gate>(*iter))
                gate_list.push_back(gate);
        }

        // sort gate list according to their min_x or min_y
        if (orientation == ALONG_ROWS)
            gate_list.sort(compare_min_x);
        else
            gate_list.sort(compare_min_y);

        // rename gates
        for (auto gate : gate_list)
        {
            boost::format f("%1%.%2%");
            f % row_num % col_num;
            gate->set_name(f.str());

            // next row or col
            if (orientation == ALONG_ROWS)
                col_num++;
            else
                row_num++;
        }

        // next row or col
        if (orientation == ALONG_ROWS)
        {
            row_num++;
            col_num = 1;
        }
        else
        {
            col_num++;
            row_num = 1;
        }
    }
}

void AutoNameGates::fill_histogram(LogicModel_shptr lmodel, std::vector<unsigned int>& histogram) const
{
    // iterate over gates
    for (LogicModel::gate_collection::iterator iter = lmodel->gates_begin(); iter != lmodel->gates_end(); ++iter)
    {
        Gate_shptr gate = (*iter).second;
        assert(gate != nullptr);

        if (orientation == ALONG_COLS)
        {
            for (int x = gate->get_min_x(); x < gate->get_max_x(); x++)
                histogram[x]++;
        }

        if (orientation == ALONG_ROWS)
        {
            for (int y = gate->get_min_y(); y < gate->get_max_y(); y++)
                histogram[y]++;
        }
    }
}

std::list<int> AutoNameGates::scan_histogram(std::vector<unsigned int> const& histogram) const
{
    std::list<int> scan_lines;

    unsigned int from = 0;
    unsigned int i;
    for (i = 0; i < histogram.size(); i++)
    {
        if (histogram[i] > 0 && from == 0)
            from = i;

        if (histogram[i] == 0 && from > 0)
        {
            scan_lines.push_back(from + (i - from) / 2);
            from = 0;
        }
    }

    return scan_lines;
}
