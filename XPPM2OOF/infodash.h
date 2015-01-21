// -*- C++ -*-
// $RCSfile: infodash.h,v $
// $Revision: 1.9 $
// $Author: langer $
// $Date: 2001-02-18 02:29:34 $

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */

#ifndef INFODASH_H
#define INFODASH_H

#include "dashboard.h"
class Material;
class AMTriangle;
class AMNode;

class InfoDashboard : public Dashboard {
protected:
  InfoDashboard(const CharString &name, ImageForm *iform, FL_OBJECT *grp,
		RubberBandType rbt=RB_NONE)
    : Dashboard(name, iform, grp, rbt)
    {}
  virtual ~InfoDashboard() {}
  virtual int show() = 0;
  virtual int hide();
  virtual void draw_extras() = 0;
  virtual void draw_extras_ps(FILE*, const Rectangle&) const = 0;
  MouseClick lastclick;		// where mouse was clicked on image
  virtual void clickhandler(const MouseClick&) = 0;
  void set_material_browser(const Material*, FL_OBJECT*);
};

class PixelInfoDashboard : public InfoDashboard {
private:
  PixelInfoDashboard(ImageForm*, FL_OBJECT*);
  virtual ~PixelInfoDashboard();
  virtual int show();
  virtual void draw_extras();
  virtual void draw_extras_ps(FILE*, const Rectangle&) const;

  void buildmenu(Menu*, MenuAttribute);

  Cell_coordinate where;

  virtual void clickhandler(const MouseClick&);
  void display_info(const MouseClick&);
  void display_info(const Cell_coordinate&);
  virtual void update() { display_info(where); }
  
  // Color used to mark the pixels
  Color clicked_color;
  static Color clicked_color_dflt;
  void set_clicked_color(const Color&);
  static void CB_button_infopix_color(FL_OBJECT*, long);
  static void set_clicked_color_cmd(const CharString&);
  void set_clicked_color_gui();

  TrueFalse mark_clicked;
  static TrueFalse mark_clicked_dflt;
  static void set_mark_clicked_cmd(const CharString&);
  static void CB_button_mark(FL_OBJECT*, long);
  void set_mark_clicked_gui();
  void set_mark_clicked();

  static void CB_pixel_stats(FL_OBJECT*, long);
  void pixel_stats_gui();
  static CommandFn pixel_stats_cmd();
  void pixel_stats();

  friend class ImageForm;
  friend class ImageFormGfx;
  friend class ImageFormTxt;
  friend class MeshInfoDashboard;
  friend FD_image_drawer *create_form_image_drawer();
};

enum MouseMode {MM_QUERY, MM_SELECT, MM_MOVE};
enum InfoMode {IM_CLICKED, IM_SELECTED, IM_ACTIVE, IM_ALL};

class MeshInfoDashboard : public InfoDashboard {
private:
  MeshInfoDashboard(ImageForm*, FL_OBJECT*);
  virtual ~MeshInfoDashboard();
  virtual int show();
  virtual void draw_extras();
  virtual void draw_extras_ps(FILE*, const Rectangle&) const;

  MeshCoord where;

  void buildmenu(Menu*, MenuAttribute);

  // routines called when mesh has been created or destroyed
  void mesh_created();
  void mesh_destroyed();
  void triangle_destroyed(const AMTriangle*);

  AMTriangle *clickedtriangle;	// triangle that was clicked on

  virtual void clickhandler(const MouseClick&);
  virtual void draghandler(const MouseClick&, const MouseClick&);
  virtual void movehandler(const MouseClick&, const MouseClick&);
  int moving;
  double Estart;		// E at start of move
  double Ehstart;
  double Esstart;
  AMNode *moving_node;		// node being moved

  void setrubberband();
  void setcursor();		// calls Dashboard::setcursor(const XCursor&)
  void display_info(const MouseClick&);
  void display_info(const MeshCoord&);
  virtual void update() { display_info(where); }

  // what to do when the mouse is clicked?
  Enum<MouseMode> mousemode;
  static Enum<MouseMode> mousemode_dflt;
  static void CB_mousemode(FL_OBJECT*, long);
  static void set_mousemode_cmd(const CharString&);
  void set_mousemode_gui(FL_OBJECT*);
  void set_mousemode(const Enum<MouseMode>&);

  // show info for clicked triangle, or selected triangle(s)?
  static Enum<InfoMode> infomode_dflt;
  Enum<InfoMode> infomode;
  static void CB_choice_meshinfo(FL_OBJECT*, long);
  void set_infomode_gui();
  static void set_infomode_cmd(const CharString&);
  void set_infomode();

  // highlight the clicked triangle?
  static TrueFalse highlight_dflt;
  TrueFalse highlight;
  static void CB_button_highlight(FL_OBJECT*, long);
  static void set_highlight_cmd(const CharString&);
  void set_highlight_gui();
  void set_highlight();

  // set color for highlight
  Color highlight_color;
  static Color highlight_color_dflt;
  void set_highlight_color(const Color&);
  static void set_highlight_color_cmd(const CharString&);
  static void CB_button_highlight_color(FL_OBJECT*, long);
  void set_highlight_color_gui();

  // unselect all triangles
  static void CB_button_meshinfo_clearselect(FL_OBJECT*, long);
  static CommandFn clear_selection_cmd();
  void clear_selection_gui();
  void clear_selection();

#ifdef DEBUG
  void dump_intersections() const;
#endif // DEBUG

  friend class ImageForm;
  friend class ImageFormGfx;
  friend class ImageFormTxt;
  friend class PixelInfoDashboard;
  friend FD_image_drawer *create_form_image_drawer();
};

#endif









