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


#ifndef __GLOBALS_H__
#define __GLOBALS_H__

#include <stdio.h>
#include <stdint.h>
#include <tr1/memory>
#include <assert.h>

#include "degate_exceptions.h"

/**
 * @namespace degate The namespace 'degate' is the namespace for everything that belongs to libdegate.
 */
namespace degate {

#define DEBUG 1

#define SIGNUM(x) ((x > 0) ? 1 : (x < 0) ? -1 : 0)

enum ret_t {
  RET_OK = 0,
  RET_ERR = 1,
  RET_INV_PTR = 2,
  RET_MALLOC_FAILED = 3,
  RET_INV_PATH = 4,
  RET_MATH_ERR = 5,
  RET_CANCEL = 6
};

#define RET_IS_OK(call_res) ((call_res) == RET_OK)
#define RET_IS_NOT_OK(call_res) ((call_res) != RET_OK)


#define degate_mime_type "application/degate"

#if (__SIZEOF_POINTER__ == 4) || (__WORDSIZE == 32)
#define ARCH_32
#define MAP_FILES_ON_DEMAND
#elif (__SIZEOF_POINTER__ == 8) || (__WORDSIZE == 64)
#define ARCH_64
#else
#error "Unknown architecture"
#endif

	
#define DEGATE_VERSION "0.0.7"

}

#define TM __FILE__,__LINE__

#ifdef DEBUG
void debug(const char * const module, int line, const char * const format, ...);
#else
#define debug(module, line, format, ...) ;
#endif


namespace degate {
  // we need some forward declerations here

  class ConnectedLogicModelObject;
  typedef std::tr1::shared_ptr<ConnectedLogicModelObject> ConnectedLogicModelObject_shptr;

  class PlacedLogicModelObject;
  typedef std::tr1::shared_ptr<PlacedLogicModelObject> PlacedLogicModelObject_shptr;

  class RemoteObject;
  typedef std::tr1::shared_ptr<RemoteObject> RemoteObject_shptr;

  class Net;
  typedef std::tr1::shared_ptr<Net> Net_shptr;  

  class Gate;

  class GatePort;
  typedef std::tr1::shared_ptr<GatePort> GatePort_shptr;

  class GateTemplate;
  typedef std::tr1::shared_ptr<GateTemplate> GateTemplate_shptr;

  class GateLibrary;
  typedef std::tr1::shared_ptr<GateLibrary> GateLibrary_shptr;

  class Layer;
  typedef std::tr1::shared_ptr<Layer> Layer_shptr;  

  class Wire;
  typedef std::tr1::shared_ptr<Wire> Wire_shptr;

  class Via;
  typedef std::tr1::shared_ptr<Via> Via_shptr;

  class Annotation;
  typedef std::tr1::shared_ptr<Annotation> Annotation_shptr;

  class Module;
  typedef std::tr1::shared_ptr<Module> Module_shptr;


  typedef unsigned int layer_position_t;


  /** defines the type for colors */
  typedef uint32_t color_t;
  
  typedef unsigned long long object_id_t;
  typedef unsigned long long transaction_id_t;
  typedef unsigned int diameter_t;
  typedef unsigned int length_t;
  typedef int pos_t;


  std::string gen_tabs(int n);
}

#endif
