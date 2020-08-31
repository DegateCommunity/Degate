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

#include "Core/LogicModel/Wire/Wire.h"
#include "Core/LogicModel/LogicModel.h"

#include "catch.hpp"

using namespace degate;

TEST_CASE("Test casts", "[LogicModel]")
{
    Wire* w = new Wire(20, 21, 30, 31, 5);
    REQUIRE(w != nullptr);

    PlacedLogicModelObject* ptr = dynamic_cast<PlacedLogicModelObject*>(w);
    REQUIRE(ptr != nullptr);

    Wire* w2 = dynamic_cast<Wire*>(ptr);
    REQUIRE(w2 != nullptr);

    delete w;
}

TEST_CASE("Test shared ptr casts", "[LogicModel]")
{
    Wire_shptr w(new Wire(20, 21, 30, 31, 5));
    REQUIRE(w != nullptr);

    PlacedLogicModelObject_shptr ptr = std::dynamic_pointer_cast<PlacedLogicModelObject>(w);
    REQUIRE(ptr != nullptr);

    Wire_shptr w2 = std::dynamic_pointer_cast<Wire>(ptr);
    REQUIRE(w2 != nullptr);
}

TEST_CASE("Test add layer", "[LogicModel]")
{
    LogicModel_shptr lmodel(new LogicModel(100, 100));
    lmodel->add_layer(0);
    lmodel->add_layer(1);
    lmodel->add_layer(2);
    lmodel->add_layer(3);
    lmodel->add_layer(10);
    lmodel->add_layer(100);

    REQUIRE_THROWS_AS(lmodel->add_layer(3), std::logic_error);
    REQUIRE_THROWS_AS(lmodel->add_layer(10), std::logic_error);

    lmodel->remove_layer(3);
}

TEST_CASE("Test add and retrieve placed logic model", "[LogicModel]")
{
    LogicModel_shptr lmodel(new LogicModel(100, 100));
    REQUIRE(lmodel != nullptr);

    PlacedLogicModelObject_shptr plo(new Gate(0, 10, 0, 10));
    REQUIRE(plo != nullptr);

    REQUIRE(lmodel->objects_begin() == lmodel->objects_end());
    lmodel->add_object(0, plo);
    REQUIRE(lmodel->objects_begin() != lmodel->objects_end());

    PlacedLogicModelObject_shptr retrieved = lmodel->get_object(plo->get_object_id());
    REQUIRE(retrieved != nullptr);
}

TEST_CASE("Test add and retrieve wire", "[LogicModel]")
{
    LogicModel_shptr lmodel(new LogicModel(100, 100));

    Wire_shptr w(new Wire(20, 21, 30, 31, 5));
    REQUIRE(w->has_valid_object_id() == false);

    REQUIRE(lmodel->objects_begin() == lmodel->objects_end());
    lmodel->add_object(0, w);
    REQUIRE(w->has_valid_object_id() == true);
    REQUIRE(lmodel->objects_begin() != lmodel->objects_end());

    object_id_t oid = w->get_object_id();
    REQUIRE(oid != 0);


    PlacedLogicModelObject_shptr plo = lmodel->get_object(oid);
    REQUIRE(plo != nullptr);

    Wire_shptr retrieved_wire = std::dynamic_pointer_cast<Wire>(plo);
    REQUIRE(retrieved_wire != nullptr);

    for(int j = 0; j < 100; j++)
    {
        Wire_shptr w2(new Wire(20, 21, 30, 31, 5));

        lmodel->add_object(0, w2);
    }

    // iteration test
    int i = 0;
    for(auto iter = lmodel->objects_begin(); iter != lmodel->objects_end(); ++iter, i++)
    {

        PlacedLogicModelObject_shptr o = (*iter).second;
        REQUIRE(o != nullptr);
        REQUIRE(o->has_valid_object_id() == true);
        REQUIRE((*iter).first == o->get_object_id());
    }

    REQUIRE(i > 0);
}