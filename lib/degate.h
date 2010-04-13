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

#ifndef __DEGATE_H__
#define __DEGATE_H__

/*! @mainpage libdegate
 *
 * @section intro_degate_sec What is degate?
 *
 * Degates' purpose is to aid  reverse engineering of integrated circuits (ICs).
 * Degate helps you to explore images from ICs. It matches logic gates  on the
 * imagery given by graphical templates and it assists you in tracing circuit paths.
 *
 * Degate is not a completely automatic analyzing tool. Degate helps you with
 * some automation in your manual reverse engineering process. 
 *
 * Please have a look on the project website to learn more about degate.
 * @see http://degate.zfch.de
 *
 *
 * @section intro_libdegate_sec What is libdegate?
 *
 * Libdegate is a part of the degate project. It is a shared library, that implements
 * the core functionality of degate. The core functionality is basically working with
 * degate projects, with the logic model and with images. This library is documented
 * here.
 *
 * @ref example_page The example section explains how you can program with libdegate.
 */

/*! @page example_page Examples
 *
 * @section howto_sec How to use libdegate?
 *
 * This section will describe, how to use libdegate in order to access logic model related
 * data and images from project files. The examples presented here are taken from the
 * directory \p examples.
 *
 * @subsection example1 Example 1: Load a project and get information from logic model objects.
 *
 * In the following example we will load a degate project and work with the logic model.
 *
 * @include read_a_project.cc
 *
 * @subsection example2 Example 2: Work with images
 * 
 * 
 *
 * This example will load a degate project. It will iterate over all positions where
 * vias are placed. It will grab each image region where a via is placed and store them
 * into a seperate image files.
 */


#include "globals.h"
#include "degate_exceptions.h"

/*
  These include for logic model related structures have
  to be in the correct order.
*/

#include <LogicModelObjectBase.h>
#include <Net.h>
#include <Layer.h>
#include <LogicModel.h>
#include <ConnectedLogicModelObject.h>
#include <PlacedLogicModelObject.h>
#include <GateTemplate.h>

#include "BoundingBox.h"
#include "RegularGrid.h"
#include "IrregularGrid.h"

#include <PortColorManager.h>
#include <Image.h>

//#include <PluginBase.h>

#endif
