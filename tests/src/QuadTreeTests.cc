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

#include "Core/Primitive/QuadTree.h"
#include "Core/Primitive/QuadTreeDownIterator.h"
#include "Core/Primitive/QuadTreeRegionIterator.h"
#include "Core/LogicModel/Gate/Gate.h"
#include "Core/LogicModel/Wire/Wire.h"
#include "Core/Primitive/QuadTree.h"
#include "Core/LogicModel/Via/Via.h"

#include <catch.hpp>

using namespace degate;

TEST_CASE("Test quad tree insert", "[QuadTree]")
{
    const BoundingBox bbox(0, 1000, 0, 1000);
    QuadTree<PlacedLogicModelObject_shptr> qt(bbox, 4);

    REQUIRE(qt.is_leave() == true);

    Gate_shptr g1(new Gate(10, 20, 10, 20));
    REQUIRE(RET_IS_OK(qt.insert(g1)));
    REQUIRE(qt.total_size() == 1);

    Gate_shptr g2(new Gate(90, 100, 90, 100));
    REQUIRE(RET_IS_OK(qt.insert(g2)));
    REQUIRE(qt.total_size() == 2);
}

TEST_CASE("Test quad tree iterator", "[QuadTree]")
{
    const BoundingBox bbox(0, 1000, 0, 1000);
    QuadTree<PlacedLogicModelObject_shptr> qt(bbox, 4);
    
    REQUIRE(qt.region_iter_begin(0,0,0,0) == qt.region_iter_begin(0,0,0,0));
    REQUIRE(qt.region_iter_begin(0,0,0,0) == qt.region_iter_begin(0,0,0,1));
    REQUIRE(qt.region_iter_end() == qt.region_iter_begin(0,0,0,0));
    REQUIRE(qt.region_iter_end() == qt.region_iter_end());

    Gate_shptr g1(new Gate(10, 20, 10, 20));
    REQUIRE(RET_IS_OK(qt.insert(g1)));

    Gate_shptr g2(new Gate(90, 100, 90, 100));
    REQUIRE(RET_IS_OK(qt.insert(g2)));

    Gate_shptr g3(new Gate(190, 200, 190, 200));
    REQUIRE(RET_IS_OK(qt.insert(g3)));

    Gate_shptr g4(new Gate(500, 510, 500, 510));
    REQUIRE(RET_IS_OK(qt.insert(g4)));

    Gate_shptr g5(new Gate(600, 610, 600, 610));
    REQUIRE(RET_IS_OK(qt.insert(g5)));

    Gate_shptr g6(new Gate(700, 710, 700, 710));
    REQUIRE(RET_IS_OK(qt.insert(g6)));

    Gate_shptr g7(new Gate(800, 810, 800, 810));
    REQUIRE(RET_IS_OK(qt.insert(g7)));

    Gate_shptr g8(new Gate(900, 910, 900, 910));
    REQUIRE(RET_IS_OK(qt.insert(g8)));

    REQUIRE(qt.total_size() == 8);

    qt.print(std::cout, 0, true);

    unsigned int i = 0;
    for(auto it = qt.region_iter_begin(480, 620, 480, 620); it != qt.region_iter_end(); ++it, i++)
    {
        REQUIRE(it != qt.region_iter_end());
        REQUIRE(*it != nullptr);

        PlacedLogicModelObject_shptr gate = *it;
    }

    REQUIRE(i == 2);

    i = 0;
    for(auto it = qt.region_iter_begin(); it != qt.region_iter_end(); ++it, i++) 
    {
        REQUIRE(it != qt.region_iter_end());
        REQUIRE(*it != nullptr);
    }

    REQUIRE(i == qt.total_size());
}

TEST_CASE("Test insert wire into qtree", "[QuadTree]")
{
    BoundingBox bbox(0, 100, 0, 100);
    auto qtree = new QuadTree<PlacedLogicModelObject*>(bbox);

    Wire* w = new Wire(20, 21, 30, 31, 5);
    REQUIRE(w != nullptr);
    REQUIRE(w->get_from_x() == 20);
    REQUIRE(w->get_from_y() == 21);
    REQUIRE(w->get_to_x() == 30);
    REQUIRE(w->get_to_y() == 31);
    REQUIRE(w->get_diameter() == 5);

    w->set_name("wire one");
    w->set_description("description for wire one");

    qtree->insert(dynamic_cast<PlacedLogicModelObject*>(w));

    for(RegionIterator<PlacedLogicModelObject *> i = qtree->region_iter_begin(); i != qtree->region_iter_end(); ++i)
    {
        PlacedLogicModelObject* o = *i;

        REQUIRE(i == qtree->region_iter_begin());

        REQUIRE(o != nullptr);
        REQUIRE(o->get_name() == "wire one");
        REQUIRE(o->get_description() == "description for wire one");

        Wire* wxx = dynamic_cast<Wire *>(o);
        REQUIRE(wxx != nullptr);
        REQUIRE(wxx->get_net() == nullptr);

        REQUIRE(typeid(o) == typeid(PlacedLogicModelObject *));

        Wire* w2 = dynamic_cast<Wire*>(o);
        REQUIRE(w != nullptr);
        REQUIRE(typeid(w2) == typeid(Wire*));
        REQUIRE(w2->get_name() == "wire one");

        REQUIRE(w2->get_from_x() == 20);
        REQUIRE(w2->get_from_y() == 21);

    }

    delete w;
    delete qtree;
}

TEST_CASE("Test overlapping objects", "[QuadTree]")
{
    BoundingBox bbox(0, 100, 0, 100);
    auto qtree = new QuadTree<PlacedLogicModelObject*>(bbox);

    Gate* g = new Gate(0, 30, 0, 30);
    Via* v = new Via(10, 10, 5, Via::DIRECTION_UP);

    qtree->insert(dynamic_cast<PlacedLogicModelObject*>(g));
    qtree->insert(dynamic_cast<PlacedLogicModelObject*>(v));

    int counter = 0;
    for(auto i = qtree->region_iter_begin(10,10,10,10); i != qtree->region_iter_end(); ++i, counter++)
    {
        PlacedLogicModelObject * o = *i;
        REQUIRE((dynamic_cast<Gate*>(o) != nullptr || dynamic_cast<Via*>(o) != nullptr));
    }

    REQUIRE(counter == 2);

    delete g;
    delete v;
    delete qtree;
}
