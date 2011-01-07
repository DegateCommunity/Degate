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

#ifndef __GATERENDERER_H__
#define __GATERENDERER_H__

#include <OpenGLRendererBase.h>

#include <list>
#include <set>
#include <algorithm>
#include <boost/tuple/tuple.hpp>
#include <boost/foreach.hpp>

#include <Editor.h>
#include <Image.h>
#include <Layer.h>

#include <list>

class GateRenderer : public OpenGLRendererBase {

  //friend class GfxEditorTool<DegateRenderer>;

 private:

  bool realized;
  GLuint dlist;

  degate::GateTemplate_shptr tmpl;

  typedef std::list<std::pair<degate::Layer::LAYER_TYPE, GLuint> > layer_list_type;
  layer_list_type layers;

protected:

  void on_realize();
  void update_viewport_dimension();


public:

  GateRenderer();
  virtual ~GateRenderer();


  void set_gate_template(degate::GateTemplate_shptr tmpl)
    throw(degate::InvalidPointerException);

  void add_layer_type(degate::Layer::LAYER_TYPE layer_type1);


  virtual void update_screen();

 private:


  GLuint create_texture(degate::GateTemplateImage_shptr img, uint8_t alpha) const;
  void render_texture(degate::GateTemplateImage_shptr img, GLuint texture) const;


  void render_gate_template(degate::GateTemplate_shptr tmpl,
			    layer_list_type & layers);

};


#endif
