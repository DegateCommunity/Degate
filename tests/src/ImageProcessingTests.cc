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

#include "Core/Image/Image.h"
#include "Core/Image/Processor/IPPipe.h"
#include "Core/Image/Processor/IPCopy.h"

#include <catch.hpp>

using namespace degate;

TEST_CASE("Test pipe", "[ImageProcessingTests]")
{
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


    /*
     * Set up a pipe.
     */

    // IPPipe derives from ProgressControl.
    IPPipe pipe;
    REQUIRE(pipe.is_empty() == true);

    // fill the pipe
    pipe.add(copy_rgba_to_gs);
    REQUIRE(pipe.size() == 1);

    // add other plugin
    pipe.add(copy_gs_to_rgba);
    REQUIRE(pipe.size() == 2);

    REQUIRE_NOTHROW(pipe.run(in));
}
