#ifndef __EDITOR_H__
#define __EDITOR_H__

#include <BoundingBox.h>

#include <tr1/memory>

#include <RenderArea.h>

class GfxEditorTool {
 public:
  GfxEditorTool() {}
  virtual ~GfxEditorTool() {}

  virtual void on_mouse_click(unsigned int real_x, unsigned int real_y, unsigned int button) = 0;
  virtual void on_mouse_double_click(unsigned int real_x, unsigned int real_y, unsigned int button) = 0;
  virtual void on_mouse_release(unsigned int real_x, unsigned int real_y, unsigned int button) = 0;
  virtual void on_mouse_motion(unsigned int real_x, unsigned int real_y) = 0;

};

typedef std::tr1::shared_ptr<GfxEditorTool> GfxEditorTool_shptr;

// -------------------------------------------------------------------------------

class GfxEditorToolRectangle : public GfxEditorTool {
private:
  //Glib::RefPtr<Goocanvas::Rect> rect;
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

      reset();

      start_x = real_x;
      start_y = real_y;
      //rect = Goocanvas::Rect::create(real_x, real_y, 0, 0);
      //renderer.get_root_item()->add_child(rect);
      drag_mode = true;
    }

  }
  void on_mouse_release(unsigned int real_x, unsigned int real_y, unsigned int button) {
    if(button == 1) {
      drag_mode = false;
      
      //assert(rect != NULL);
      degate::BoundingBox bbox = get_bounding_box();
      if(bbox.get_width() == 0 && bbox.get_height() == 0) reset();
    }
  }

  void on_mouse_motion(unsigned int real_x, unsigned int real_y) {
    //if(drag_mode && rect != NULL) {
      /*
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
      */
    //    }
  }


  degate::BoundingBox get_bounding_box() const {
    if(!has_rectangle()) return degate::BoundingBox();
    else {
      /*
      Glib::PropertyProxy<double> x = rect->property_x();
      Glib::PropertyProxy<double> y = rect->property_y();
      Glib::PropertyProxy<double> w = rect->property_width();
      Glib::PropertyProxy<double> h = rect->property_height();
      
      return degate::BoundingBox(x.get_value(),
				 x.get_value() + w.get_value(),
				 y.get_value(),
				 y.get_value() + h.get_value());
      */

    }
  }

 protected:

  virtual bool has_rectangle() const {
    //return rect != NULL;
    return false;
  }

  virtual void reset() {
    //if(rect != NULL) {
      //rect->remove();
      //rect.reset();      
    //}
  }


};

class GfxEditorToolSelection : public GfxEditorToolRectangle {
 private:

  sigc::signal<void, degate::BoundingBox const&>  signal_selection_activated_;
  sigc::signal<void>  signal_selection_revoked_;
  sigc::signal<void, unsigned int, unsigned int, unsigned int>  signal_mouse_clicked_;
  sigc::signal<void, unsigned int, unsigned int, unsigned int>  signal_mouse_double_clicked_;


 public:

  GfxEditorToolSelection(RenderArea & _renderer) : GfxEditorToolRectangle(_renderer) {
  }

  void on_mouse_double_click(unsigned int real_x, unsigned int real_y, unsigned int button) {
    //GfxEditorToolRectangle::on_mouse_double_click(real_x, real_y, button);   

    if(!signal_mouse_double_clicked_.empty())
      signal_mouse_double_clicked_(real_x, real_y, button);
  }

  void on_mouse_release(unsigned int real_x, unsigned int real_y, unsigned int button) {
    GfxEditorToolRectangle::on_mouse_release(real_x, real_y, button);   

    if(button == 1) {

      if(has_selection()) {

	if(!signal_selection_activated_.empty()) 
	  signal_selection_activated_(get_bounding_box());
      }
      else {
	if(!signal_mouse_clicked_.empty())
	  signal_mouse_clicked_(real_x, real_y, button);
      }

    }
  }

  void reset_selection() {
    GfxEditorToolRectangle::reset();
    if(!signal_selection_revoked_.empty()) signal_selection_revoked_();

  }

  bool has_selection() const {
    degate::BoundingBox bbox = get_bounding_box();
    return bbox.get_width() != 0 && bbox.get_height() != 0;
  }

  sigc::signal<void>& signal_selection_revoked() {
    return signal_selection_revoked_;
  }

  sigc::signal<void, degate::BoundingBox const& >& signal_selection_activated() {
    return signal_selection_activated_;
  }

  /**
   * Signal for a single, non-area-selecting mouse click.
   */
  sigc::signal<void, unsigned int, unsigned int, unsigned int> & signal_mouse_clicked() {
    return signal_mouse_clicked_;
  }

  /**
   * Signal for a single, non-area-selecting mouse click.
   */
  sigc::signal<void, unsigned int, unsigned int, unsigned int> & signal_mouse_double_clicked() {
    return signal_mouse_double_clicked_;
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
    RendererType::signal_mouse_double_click().connect(sigc::mem_fun(*this, &GfxEditor::on_mouse_double_click));
    RendererType::signal_mouse_motion().connect(sigc::mem_fun(*this, &GfxEditor::on_mouse_motion));
    RendererType::signal_mouse_release().connect(sigc::mem_fun(*this, &GfxEditor::on_mouse_release));
  }

  virtual ~GfxEditor() {}

  void set_tool(GfxEditorTool_shptr tool) {
    this->tool = tool;
  }
  
  GfxEditorTool_shptr get_tool() {
    return tool;
  }

  void on_mouse_click(unsigned int real_x, unsigned int real_y, unsigned int button) {
    if(tool != NULL) tool->on_mouse_click(real_x, real_y, button);
  }

  void on_mouse_double_click(unsigned int real_x, unsigned int real_y, unsigned int button) {
    if(tool != NULL) tool->on_mouse_double_click(real_x, real_y, button);
  }

  void on_mouse_release(unsigned int real_x, unsigned int real_y, unsigned int button) {
    if(tool != NULL) tool->on_mouse_release(real_x, real_y, button);
  }

  void on_mouse_motion(unsigned int real_x, unsigned int real_y) {
    if(tool != NULL) tool->on_mouse_motion(real_x, real_y);
  }

};

#endif
