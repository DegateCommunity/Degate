#ifndef __EDITOR_H__
#define __EDITOR_H__

#include <BoundingBox.h>
#include <RenderArea.h>
#include <tr1/memory>

#include <GL/gl.h>
#include <GL/glu.h>
#include <gtkglmm.h>

template<typename RendererType>
class GfxEditorTool {

private:

  RendererType & renderer;

public:

  GfxEditorTool(RendererType & _renderer) : renderer(_renderer) {}
  virtual ~GfxEditorTool() {}

  virtual void on_mouse_click(unsigned int real_x, unsigned int real_y, unsigned int button) = 0;
  virtual void on_mouse_double_click(unsigned int real_x, unsigned int real_y, unsigned int button) = 0;
  virtual void on_mouse_release(unsigned int real_x, unsigned int real_y, unsigned int button) = 0;
  virtual void on_mouse_motion(unsigned int real_x, unsigned int real_y) = 0;

protected:

  virtual RendererType & get_renderer() { return renderer; }

  virtual void start_tool_drawing() {
    renderer.start_tool();
  }

  virtual void stop_tool_drawing() {
    renderer.stop_tool();
  }

  virtual void set_renderer_lock(bool state) {
    GfxEditorTool<RendererType>::get_renderer().set_lock(state);
  }

};


// -------------------------------------------------------------------------------

template<typename RendererType>
class GfxEditorToolRectangle : public GfxEditorTool<RendererType> {

private:
  
  unsigned int start_x, start_y, stop_x, stop_y;
  bool drag_mode;
  degate::BoundingBox bbox;

public:

  GfxEditorToolRectangle(RendererType & renderer) : 
    GfxEditorTool<RendererType>(renderer),
    drag_mode(false) {
  }

  void reset() {
    bbox = degate::BoundingBox();
    GfxEditorTool<RendererType>::start_tool_drawing();
    GfxEditorTool<RendererType>::stop_tool_drawing();
  }

  degate::BoundingBox get_bounding_box() const {
    return bbox;
  }

protected:

  void on_mouse_click(unsigned int real_x, unsigned int real_y, unsigned int button) {
    if(button == 1) {
      bbox = degate::BoundingBox(start_x, start_x, start_y, start_y);
      start_x = real_x;
      start_y = real_y;
      drag_mode = true;
      GfxEditorTool<RendererType>::set_renderer_lock(true);
      
    }
  }

  void on_mouse_release(unsigned int real_x, unsigned int real_y, unsigned int button) {
    if(button == 1) {
      drag_mode = false;
      GfxEditorTool<RendererType>::set_renderer_lock(false);
      if(start_x == real_x && start_y == real_y) reset();
    }
  }

  void on_mouse_double_click(unsigned int real_x, unsigned int real_y, 
			     unsigned int button) {
    on_mouse_release(real_x, real_y, button);
  }

  void on_mouse_motion(unsigned int real_x, unsigned int real_y) {
    
    if(drag_mode) {
      stop_x = real_x;
      stop_y = real_y;

      GfxEditorTool<RendererType>::start_tool_drawing();
      glColor4ub(0xff, 0xff, 0xff, 0xff);
      glLineWidth(1);

      glBegin(GL_LINE_LOOP);
      glVertex2i(start_x, start_y);
      glVertex2i(real_x, start_y);
      glVertex2i(real_x, real_y);
      glVertex2i(start_x, real_y);
      glEnd();

      GfxEditorTool<RendererType>::stop_tool_drawing();

      bbox = degate::BoundingBox(start_x, stop_x, start_y, stop_y);
    }
  }

};

template<typename RendererType>
class GfxEditorToolSelection : public GfxEditorToolRectangle<RendererType> {
private:

  sigc::signal<void, degate::BoundingBox const&>  signal_selection_activated_;
  sigc::signal<void>  signal_selection_revoked_;
  sigc::signal<void, unsigned int, unsigned int, unsigned int>  signal_mouse_clicked_;
  sigc::signal<void, unsigned int, unsigned int, unsigned int>  signal_mouse_double_clicked_;


public:

  GfxEditorToolSelection(RendererType & renderer) : 
    GfxEditorToolRectangle<RendererType>(renderer) {
  }

  bool has_selection() const {
    degate::BoundingBox bbox = GfxEditorToolRectangle<RendererType>::get_bounding_box();
    return bbox.get_width() != 0 && bbox.get_height() != 0;
  }

  void reset_selection() {
    GfxEditorToolRectangle<RendererType>::reset();
    if(!signal_selection_revoked_.empty()) signal_selection_revoked_();
    GfxEditorToolRectangle<RendererType>::get_renderer().update_screen();
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

protected:

  void on_mouse_double_click(unsigned int real_x, unsigned int real_y, 
			     unsigned int button) {
    GfxEditorToolRectangle<RendererType>::on_mouse_double_click(real_x, real_y, button);

    if(!signal_mouse_double_clicked_.empty())
      signal_mouse_double_clicked_(real_x, real_y, button);
  }

  void on_mouse_release(unsigned int real_x, unsigned int real_y, unsigned int button) {
    GfxEditorToolRectangle<RendererType>::on_mouse_release(real_x, real_y, button);   

    if(button == 1) {

      if(has_selection()) {

	if(!signal_selection_activated_.empty()) 
	  signal_selection_activated_(GfxEditorToolRectangle<RendererType>::get_bounding_box());
      }
      else {
	if(!signal_mouse_clicked_.empty())
	  signal_mouse_clicked_(real_x, real_y, button);
      }

      GfxEditorToolRectangle<RendererType>::get_renderer().update_screen();
    }
  }


};


// -------------------------------------------------------------------------------


template<typename RendererType>
class GfxEditorToolMove : public GfxEditorTool<RendererType> {

private:

  int start_x, start_y;
  bool drag_mode;

  void move(int x, int y) {
    GfxEditorTool<RendererType>::get_renderer().shift_viewport(start_x - x, start_y - y);
  }

  void on_mouse_double_click(unsigned int real_x, unsigned int real_y, unsigned int button) {}

public:

  GfxEditorToolMove(RendererType & renderer) : 
    GfxEditorTool<RendererType>(renderer),
    drag_mode(false) {
  }

protected:

  void on_mouse_click(unsigned int real_x, unsigned int real_y, unsigned int button) {
    if(button == 1) {
      start_x = real_x;
      start_y = real_y;
      drag_mode = true;
      GfxEditorTool<RendererType>::set_renderer_lock(true);
    }
  }

  void on_mouse_release(unsigned int real_x, unsigned int real_y, unsigned int button) {
    if(button == 1) {
      drag_mode = false;
      GfxEditorTool<RendererType>::set_renderer_lock(false);
      move(real_x, real_y);
    }
  }

  void on_mouse_motion(unsigned int real_x, unsigned int real_y) {
    if(drag_mode) move(real_x, real_y);
  }


};


// -------------------------------------------------------------------------------

template<typename RendererType>
class GfxEditorToolVia : public GfxEditorTool<RendererType> {

private:

  sigc::signal<void, unsigned int, unsigned int, unsigned int>  signal_mouse_clicked_;

  void on_mouse_double_click(unsigned int real_x, unsigned int real_y, unsigned int button) {}

public:

  GfxEditorToolVia(RendererType & renderer) : 
    GfxEditorTool<RendererType>(renderer) {
  }

  /**
   * Signal for a single mouse click.
   */
  sigc::signal<void, unsigned int, unsigned int, unsigned int> & signal_mouse_clicked() {
    return signal_mouse_clicked_;
  }

protected:

  void on_mouse_click(unsigned int real_x, unsigned int real_y, unsigned int button) { }
  void on_mouse_motion(unsigned int real_x, unsigned int real_y) {}

  void on_mouse_release(unsigned int real_x, unsigned int real_y, unsigned int button) {
    if(button == 1) {
      if(!signal_mouse_clicked_.empty())
	signal_mouse_clicked_(real_x, real_y, button);
    }
  }

};

// -------------------------------------------------------------------------------

template<typename RendererType>
class GfxEditorToolWire : public GfxEditorTool<RendererType> {

private:

  unsigned int start_x , start_y;
  bool have_start;

  sigc::signal<void, unsigned int, unsigned int, unsigned int, unsigned int>  signal_wire_added_;


public:

  GfxEditorToolWire(RendererType & renderer) : 
    GfxEditorTool<RendererType>(renderer),
    have_start(false) {
  }

  /**
   * Signal for a single mouse click.
   */
  sigc::signal<void, unsigned int, unsigned int, unsigned int, unsigned int> & signal_wire_added() {
    return signal_wire_added_;
  }

protected:

  void on_mouse_motion(unsigned int real_x, unsigned int real_y) {
    if(have_start) {

      GfxEditorTool<RendererType>::start_tool_drawing();
      glColor4ub(0xff, 0xff, 0xff, 0xff);
      glLineWidth(1);

      glBegin(GL_LINES);
      glVertex2i(start_x, start_y);
      glVertex2i(real_x, real_y);
      glEnd();

      GfxEditorTool<RendererType>::stop_tool_drawing();
    }
  }

  void on_mouse_click(unsigned int real_x, unsigned int real_y, unsigned int button) { 
    if(button == 1 && have_start == false) {
      start_x = real_x;
      start_y = real_y;
      have_start = true;

      GfxEditorTool<RendererType>::set_renderer_lock(true);
    }
    else if(button == 3) {
      have_start = false;
      GfxEditorTool<RendererType>::start_tool_drawing();
      GfxEditorTool<RendererType>::stop_tool_drawing();
      GfxEditorTool<RendererType>::set_renderer_lock(false);
    }
  }

  void on_mouse_release(unsigned int real_x, unsigned int real_y, unsigned int button) {
    if(button == 1 && have_start) {
      
      if(real_x != start_x || real_y != start_y) {

	if(!signal_wire_added_.empty()) {
	   signal_wire_added_(start_x, start_y, real_x, real_y);
	   GfxEditorTool<RendererType>::get_renderer().render_wires();
	}

      }

      start_x = real_x;
      start_y = real_y;
    }
  }

  void on_mouse_double_click(unsigned int real_x, unsigned int real_y, unsigned int button) {
    if(button == 1 && have_start == true) {
      
      GfxEditorTool<RendererType>::start_tool_drawing();
      GfxEditorTool<RendererType>::stop_tool_drawing();

      have_start = false;
      GfxEditorTool<RendererType>::set_renderer_lock(false);

    }
  }

};

// -------------------------------------------------------------------------------



template<class RendererType>
class GfxEditor : public RendererType {

private:
  
  std::tr1::shared_ptr<GfxEditorTool<RendererType> > tool;

public:

  GfxEditor() {
    RendererType::signal_mouse_click().connect(sigc::mem_fun(*this, &GfxEditor::on_mouse_click));
    RendererType::signal_mouse_double_click().connect(sigc::mem_fun(*this, &GfxEditor::on_mouse_double_click));
    RendererType::signal_mouse_motion().connect(sigc::mem_fun(*this, &GfxEditor::on_mouse_motion));
    RendererType::signal_mouse_release().connect(sigc::mem_fun(*this, &GfxEditor::on_mouse_release));
  }

  virtual ~GfxEditor() {}

  void set_tool(std::tr1::shared_ptr<GfxEditorTool<RendererType> > tool) {
    this->tool = tool;
  }
  
  std::tr1::shared_ptr<GfxEditorTool<RendererType> > get_tool() {
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
