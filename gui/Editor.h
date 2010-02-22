#ifndef __EDITOR_H__
#define __EDITOR_H__

#include <tr1/memory>
#include <goocanvasmm.h>

#include <RenderArea.h>

class GfxEditorTool {
 public:
  GfxEditorTool() {}
  virtual ~GfxEditorTool() {}

  virtual void on_mouse_click(unsigned int real_x, unsigned int real_y, unsigned int button) = 0;
  virtual void on_mouse_release(unsigned int real_x, unsigned int real_y, unsigned int button) = 0;
  virtual void on_mouse_motion(unsigned int real_x, unsigned int real_y) = 0;

};

typedef std::tr1::shared_ptr<GfxEditorTool> GfxEditorTool_shptr;

// -------------------------------------------------------------------------------

class GfxEditorToolRectangle : public GfxEditorTool {
private:
  Glib::RefPtr<Goocanvas::Rect> rect;
  unsigned int start_x, start_y;
  RenderArea & renderer;
  bool drag_mode;

public:

  GfxEditorToolRectangle(RenderArea & _renderer) :
    renderer(_renderer),
    drag_mode(false) {
  }


  void on_mouse_click(unsigned int real_x, unsigned int real_y, unsigned int button) {
    if(button == 1) {

      if(rect != NULL) rect->remove();

      start_x = real_x;
      start_y = real_y;
      rect = Goocanvas::Rect::create(real_x, real_y, 0, 0);
      renderer.get_root_item()->add_child(rect);
      drag_mode = true;
    }

  }
  void on_mouse_release(unsigned int real_x, unsigned int real_y, unsigned int button) {
    if(button == 1) {
      drag_mode = false;
    }
  }

  void on_mouse_motion(unsigned int real_x, unsigned int real_y) {
    if(drag_mode && rect != NULL) {
      Glib::PropertyProxy<double> x = rect->property_x();
      Glib::PropertyProxy<double> y = rect->property_y();
      Glib::PropertyProxy<double> w = rect->property_width();
      Glib::PropertyProxy<double> h = rect->property_height();

      if(real_x > start_x) {
	x.set_value(start_x);
	w.set_value(real_x - start_x);
      }
      else {
	x.set_value(real_x);
	w.set_value(start_x - real_x);
      }

      if(real_y > start_y) {
	y.set_value(start_y);
	h.set_value(real_y - start_y);
      }
      else {
	y.set_value(real_y);
	h.set_value(start_y - real_y);
      }


    }
  }

};

class GfxEditorToolSelection : public GfxEditorToolRectangle {
public:
  GfxEditorToolSelection(RenderArea & _renderer) : GfxEditorToolRectangle(_renderer) {
  }

  void on_mouse_release(unsigned int real_x, unsigned int real_y, unsigned int button) {
    GfxEditorToolRectangle::on_mouse_release(real_x, real_y, button);
    
  }

};

typedef std::tr1::shared_ptr<GfxEditorToolSelection> GfxEditorToolSelection_shptr;

// -------------------------------------------------------------------------------


template<class RendererType>
class GfxEditor : public RendererType {

private:
  GfxEditorTool_shptr tool;
  
public:

  GfxEditor() {
    RendererType::signal_mouse_click().connect(sigc::mem_fun(*this, &GfxEditor::on_mouse_click));
    RendererType::signal_mouse_motion().connect(sigc::mem_fun(*this, &GfxEditor::on_mouse_motion));
    RendererType::signal_mouse_release().connect(sigc::mem_fun(*this, &GfxEditor::on_mouse_release));
  }

  virtual ~GfxEditor() {}

  void set_tool(GfxEditorTool_shptr tool) {
    //tool->set_canvas(this);
    this->tool = tool;
  }
  
  void on_mouse_click(unsigned int real_x, unsigned int real_y, unsigned int button) {
    if(tool != NULL) tool->on_mouse_click(real_x, real_y, button);
  }

  void on_mouse_release(unsigned int real_x, unsigned int real_y, unsigned int button) {
    if(tool != NULL) tool->on_mouse_release(real_x, real_y, button);
  }

  void on_mouse_motion(unsigned int real_x, unsigned int real_y) {
    if(tool != NULL) tool->on_mouse_motion(real_x, real_y);
  }

};

#endif
