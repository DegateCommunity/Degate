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


#ifndef __GLOBALS_H__
#define __GLOBALS_H__

#include <cstdio>
#include <cstdint>
#include <memory>
#include <map>
#include <cassert>
#include "Core/Utils/DegateExceptions.h"

#include <QStandardPaths>

/**
 * If 1 will print debug information (will allow/block all debug print).
 */
#define DEBUG 1

/**
 * If 1 will print import debug information (like gate print for every import).
 */
#define DEBUG_PROJECT_IMPORT 0

/**
 * Check if a path exist, if not it will create it.
 */
#define CHECK_PATH(path) if (!QDir(QString::fromStdString(path)).exists()) QDir().mkdir(QString::fromStdString(path));

/**
 * Default degate configuration file name.
 */
#define DEGATE_CONFIGURATION_FILE_NAME "degate.ini"

/**
 * Default degate configuration directory path.
 */
#define DEGATE_CONFIGURATION_PATH QStandardPaths::writableLocation(QStandardPaths::HomeLocation).toStdString() + "/.degate/"

/**
 * Concatenate a path (std::string) with the degate configuration path.
 */
#define DEGATE_IN_CONFIGURATION(path) DEGATE_CONFIGURATION_PATH + std::string(path)

/**
 * Default degate cache path.
 */
#define DEGATE_CACHE_PATH DEGATE_CONFIGURATION_PATH "cache/"

/**
 * Concatenate a path (std::string) with the degate cache path.
 */
#define DEGATE_IN_CACHE(path) std::string(DEGATE_CACHE_PATH) + std::string(path)

#define RET_IS_OK(call_res) ((call_res) == RET_OK)
#define RET_IS_NOT_OK(call_res) ((call_res) != RET_OK)

#if (__SIZEOF_POINTER__ == 4) || (__WORDSIZE == 32)
#define ARCH_32
#define MAP_FILES_ON_DEMAND
#elif (__SIZEOF_POINTER__ == 8) || (__WORDSIZE == 64)
#define ARCH_64
#else
//#error "Unknown architecture" TODO
#endif

#define TM __FILE__,__LINE__

#ifdef DEBUG
void debug(const char* const module, int line, const char* const format, ...);
#else
#define debug(module, line, format, ...) ;
#endif

/**
 * @namespace degate The namespace 'degate' is the namespace for everything that belongs to Degate.
 */
namespace degate
{
    // we need some forward declarations here

    class Circle;
    typedef std::shared_ptr<Circle> Circle_shptr;

    class Line;
    typedef std::shared_ptr<Line> Line_shptr;

    class Rectangle;
    typedef std::shared_ptr<Rectangle> Rectangle_shptr;

    class ConnectedLogicModelObject;
    typedef std::shared_ptr<ConnectedLogicModelObject> ConnectedLogicModelObject_shptr;

    class PlacedLogicModelObject;
    typedef std::shared_ptr<PlacedLogicModelObject> PlacedLogicModelObject_shptr;

    class RemoteObject;
    typedef std::shared_ptr<RemoteObject> RemoteObject_shptr;

    class Net;
    typedef std::shared_ptr<Net> Net_shptr;

    class Gate;

    class GatePort;
    typedef std::shared_ptr<GatePort> GatePort_shptr;

    class GateTemplate;
    typedef std::shared_ptr<GateTemplate> GateTemplate_shptr;

    class GateLibrary;
    typedef std::shared_ptr<GateLibrary> GateLibrary_shptr;

    class Layer;
    typedef std::shared_ptr<Layer> Layer_shptr;

    class Wire;
    typedef std::shared_ptr<Wire> Wire_shptr;

    class Via;
    typedef std::shared_ptr<Via> Via_shptr;

    class EMarker;
    typedef std::shared_ptr<EMarker> EMarker_shptr;

    class Annotation;
    typedef std::shared_ptr<Annotation> Annotation_shptr;

    class Module;
    typedef std::shared_ptr<Module> Module_shptr;


    class LogicModel;
    typedef std::shared_ptr<LogicModel> LogicModel_shptr;

    typedef unsigned int layer_position_t;


    /** defines the type for colors */
    typedef uint32_t color_t;

    typedef unsigned long long object_id_t;
    typedef unsigned long long layer_id_t;
    typedef unsigned long long transaction_id_t;
    typedef unsigned int diameter_t;
    typedef unsigned int length_t;
    typedef int pos_t;


    std::string gen_tabs(int n);


    enum ENTITY_COLOR
    {
        DEFAULT_COLOR_WIRE,
        DEFAULT_COLOR_VIA_UP,
        DEFAULT_COLOR_VIA_DOWN,
        DEFAULT_COLOR_GRID,
        DEFAULT_COLOR_ANNOTATION,
        DEFAULT_COLOR_ANNOTATION_FRAME,
        DEFAULT_COLOR_GATE,
        DEFAULT_COLOR_GATE_FRAME,
        DEFAULT_COLOR_GATE_PORT,
        DEFAULT_COLOR_TEXT,
        DEFAULT_COLOR_EMARKER
    };

    typedef std::map<ENTITY_COLOR, color_t> default_colors_t;

    enum ret_t
    {
        RET_OK = 0,
        RET_ERR = 1,
        RET_INV_PTR = 2,
        RET_MALLOC_FAILED = 3,
        RET_INV_PATH = 4,
        RET_MATH_ERR = 5,
        RET_CANCEL = 6
    };

    /**
     * Project type enum. Default is Normal.
     */
    enum ProjectType
    {
        Normal,
        Attached
    };
}

#endif
