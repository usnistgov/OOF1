// -*- C++ -*-
// $RCSfile: infodash.C,v $
// $Revision: 1.19 $
// $Author: langer $
// $Date: 2001-02-16 18:06:10 $

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov.
 */

#include "adaptmesh.h"
#include "amtriangle.h"
#include "cell_coordinate.h"
#include "checkcursor.h"
#include "colorutils.h"
#include "elector.h"
#include "gfx.h"
#include "goof.h"
#include "imagecanvas.h"
#include "imageform.h"
#include "infodash.h"
#include "material.h"
#include "menuDef.h"
#include "meshcmds.h"
#include "pixelgroups.h"
#include "ppm2oof.h"

Color MeshInfoDashboard::highlight_color_dflt(141, 222, 86);
Color PixelInfoDashboard::clicked_color_dflt(0, 145, 0);

TrueFalse PixelInfoDashboard::mark_clicked_dflt(1);

PixelInfoDashboard::PixelInfoDashboard(ImageForm *iform, FL_OBJECT *grp)
  : InfoDashboard("Pixel", iform, grp),
    mark_clicked(mark_clicked_dflt),
    clicked_color(clicked_color_dflt)
{
  if(!textmode) 
    fl_set_button(fdui()->button_mark, mark_clicked);

  buildmenu(menu, MOPT_NORMAL);
  buildmenu(imageformmenu(), MOPT_NOPRINT); // for backwards compatibility
}

void PixelInfoDashboard::buildmenu(Menu *carte, MenuAttribute attr) {
  AddVariable(carte, "mark_clicked", "mark pixel under mouse?",
	      mark_clicked_dflt, set_mark_clicked_cmd, attr);
  AddVariable(carte, "clicked_color", "color of clicked pixel",
	      clicked_color_dflt, set_clicked_color_cmd, attr);
  AddVariable(carte, "pixel", "mouse position", where, attr);
//    carte->AddCommand("pixel_stats", "get statistics on current image",
//  		    pixel_stats_cmd, attr);
}

PixelInfoDashboard::~PixelInfoDashboard() {
  menu->RemoveOption("mark_clicked");
  menu->RemoveOption("clicked_color");
  menu->RemoveOption("pixel");
  delete menu;
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

int PixelInfoDashboard::show() {
  fl_set_object_label(fdui()->info_image_size,
		      "width=" + to_charstring(goof()->query_width()) +
		      "   height=" + to_charstring(goof()->query_height()));
  
  setcursor(*infocursor);
  return 1;
}

int InfoDashboard::hide() {
  unsetcursor();
  return 1;
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

void PixelInfoDashboard::display_info(const MouseClick &click) {
  if(textmode) return;
  lastclick = click;
  display_info(canvas()->mouselocation(click));
}

void PixelInfoDashboard::display_info(const Cell_coordinate &pt) {
  if(textmode) return;
  ImageFormGfx *form = dynamic_cast<ImageFormGfx*>(imageform());
  where = pt;
  int w = goof()->query_width();
  int h = goof()->query_height();

  fl_freeze_form(form->xform());
  fl_set_object_label(fdui()->info_image_size,
		      "width=" + to_charstring(w) +
		      "   height=" + to_charstring(h));

  fl_clear_browser(fdui()->browser_material);
  fl_clear_browser(fdui()->browser_groups);

  if(form->beenclicked) {
    if(where.x < 0 || where.x >= w || where.y < 0 || where.y >= h) {
      // Mouse click is out of bounds
      fl_set_object_label(fdui()->info_pixel_xy, "?, ?");
      fl_set_object_label(fdui()->info_mouse_xy, "?, ?");
      fl_set_object_label(fdui()->info_color, "?");
    }
    else { // Mouse click is on image
      fl_set_object_label(fdui()->info_pixel_xy, to_charstring(where));
      fl_set_object_label(fdui()->info_mouse_xy,
			  to_charstring(lastclick.coord));
      fl_set_object_label(fdui()->info_color,
			  to_charstring(form->current_picture()[where]));

      Material *material = goof()->material[where];
      if(material)
	set_material_browser(material, fdui()->browser_material);
      else
	fl_add_browser_line(fdui()->browser_material, "@bdefault");
      // list pixel's groups
      const LinkList<PixelGroup*> &glist(goof()->pixelgrouplist[where]);
      for(LinkListIterator<PixelGroup*> i=glist.begin(); !i.end(); ++i)
	fl_add_browser_line(fdui()->browser_groups, glist[i]->query_name());
    }
  }
  fl_unfreeze_form(form->xform());
}

void PixelInfoDashboard::clickhandler(const MouseClick &click) {
  if(textmode) return;
  imageform()->beenclicked = 1;
  display_info(click);
  imageform()->meshinfodash->display_info(click);
  if(fl_get_button(fdui()->button_mark))
    goof()->redraw();
}

void PixelInfoDashboard::draw_extras() {
  if(textmode) return;
  ImageFormGfx *ifg= dynamic_cast<ImageFormGfx*>(imageform());
  ifg->getpixel(clicked_color, ImageFormGfx::CLICKEDPIXEL);
  if(imageform()->beenclicked && fl_get_button(fdui()->button_mark))
    canvas()->draw_pixel(where, clicked_color);
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

void PixelInfoDashboard::CB_button_mark(FL_OBJECT *obj, long) {
  ((PixelInfoDashboard*) ImageFormGfx::objdash(obj))->set_mark_clicked_gui();
}

void PixelInfoDashboard::set_mark_clicked_gui() {
  if(active()) {
    mark_clicked = fl_get_button(fdui()->button_mark);
    mark_clicked_dflt = mark_clicked;
    garcon()->log(menucommand() + "mark_clicked=" +
		  to_charstring(mark_clicked));
    imageform()->draw();
  }
}

void PixelInfoDashboard::set_mark_clicked_cmd(const CharString&) {
  ((PixelInfoDashboard*) get_dashboard())->set_mark_clicked();
}

void PixelInfoDashboard::set_mark_clicked() {
  mark_clicked = mark_clicked_dflt;
  if(textmode) return;
  fl_set_button(fdui()->button_mark, mark_clicked);
  imageform()->draw();
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

void PixelInfoDashboard::set_clicked_color(const Color &color) {
  clicked_color = clicked_color_dflt = color;
  imageform()->getpixel(clicked_color, ImageFormGfx::CLICKEDPIXEL);
  imageform()->draw();
}

void PixelInfoDashboard::CB_button_infopix_color(FL_OBJECT *obj, long) {
  ((PixelInfoDashboard*) ImageFormGfx::objdash(obj))->set_clicked_color_gui();
}

void PixelInfoDashboard::set_clicked_color_gui() {
  if(active()) {
    set_clicked_color(colorbrowser(clicked_color));
    garcon()->log(menucommand() + "clicked_color=" +
		  to_charstring(clicked_color));
  }
}

void PixelInfoDashboard::set_clicked_color_cmd(const CharString&) {
  ((PixelInfoDashboard*) get_dashboard())->
    set_clicked_color(clicked_color_dflt);
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//
//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

Enum<MouseMode> MeshInfoDashboard::mousemode_dflt(MM_QUERY);
Enum<InfoMode> MeshInfoDashboard::infomode_dflt(IM_CLICKED);

TrueFalse MeshInfoDashboard::highlight_dflt(1);

MeshInfoDashboard::MeshInfoDashboard(ImageForm *iform, FL_OBJECT *grp) 
  : InfoDashboard("Mesh", iform, grp),
    mousemode(mousemode_dflt),
    infomode(infomode_dflt),
    clickedtriangle(0),
    highlight(highlight_dflt),
    highlight_color(highlight_color_dflt),
    moving(0),
    moving_node(0)
{
  static int dothisonce = 1;
  if(dothisonce) {
    dothisonce = 0;
    Enum<MouseMode>::name(MM_QUERY, "query");
    Enum<MouseMode>::name(MM_SELECT, "select");
    Enum<MouseMode>::name(MM_MOVE, "move");
    Enum<InfoMode>::name(IM_CLICKED, "clicked");
    Enum<InfoMode>::name(IM_SELECTED, "selected");
    Enum<InfoMode>::name(IM_ACTIVE, "active");
    Enum<InfoMode>::name(IM_ALL, "all");
  }
  if(!textmode) {
    fl_set_button(fdui()->button_highlight, highlight);
    fl_set_choice_fontstyle(fdui()->choice_meshinfo, FL_BOLD_STYLE);
    fl_set_bitmapbutton_data(fdui()->button_select_triangle,
			     checkcursor_width, checkcursor_height,
			     checkcursor_bits);
    fl_addto_choice(fdui()->choice_meshinfo, "selected");
    fl_addto_choice(fdui()->choice_meshinfo, "clicked");
    fl_addto_choice(fdui()->choice_meshinfo, "active");
    fl_addto_choice(fdui()->choice_meshinfo, "all");
    fl_set_choice(fdui()->choice_meshinfo, 2);
  }
  buildmenu(menu, MOPT_NORMAL);
  buildmenu(imageformmenu(), MOPT_NOPRINT); // for backwards compatibility
  
  if(goof()->mesh())
    mesh_created();
  else
    mesh_destroyed();
}

void MeshInfoDashboard::buildmenu(Menu *carte, MenuAttribute attr) {
  AddVariable(carte, "mousemode", "what to do with the mouse", mousemode_dflt,
	      set_mousemode_cmd, attr);
  AddVariable(carte, "infomode", "which info to display", infomode_dflt,
	      set_infomode_cmd, attr);
  AddVariable(carte, "highlight", "highlight clicked triangle?",
	      highlight_dflt, set_highlight_cmd, attr);
  AddVariable(carte, "highlight_color", "color of clicked triangle",
	      highlight_color_dflt, set_highlight_color_cmd, attr);
  carte->AddCommand("unselectmesh", "unselect all mesh triangles",
		     clear_selection_cmd, attr);
  AddVariable(carte, "where", "mouse position", where, attr);
}

MeshInfoDashboard::~MeshInfoDashboard() {
  menu->RemoveOption("mousemode");
  menu->RemoveOption("infomode");
  menu->RemoveOption("highlight");
  menu->RemoveOption("highlight_color");
  menu->RemoveOption("unselectmesh");
  menu->RemoveOption("where");
  delete menu;
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

// routines that let the dashboard know that the mesh has been created
// or destroyed

void MeshInfoDashboard::mesh_created() {
  if(textmode) return;
  fl_activate_object(fdui()->meshinfo);
  fl_set_object_lcol(fdui()->button_meshinfo, FL_BLACK);
  fl_set_object_lcol(fdui()->button_select_triangle, FL_BLACK);
  fl_set_object_lcol(fdui()->button_movenode, FL_BLACK);
  fl_set_object_lcol(fdui()->choice_meshinfo, FL_BLACK);
  fl_set_object_lcol(fdui()->button_highlight, FL_BLACK);
  fl_set_object_lcol(fdui()->button_highlight_color, FL_BLACK);
  fl_set_object_lcol(fdui()->button_meshinfo_selection, FL_BLACK);
  fl_set_object_lcol(fdui()->button_meshinfo_selection_color, FL_BLACK);
  fl_set_object_lcol(fdui()->meshinfo_text_mouse, FL_BLACK);
}

void MeshInfoDashboard::mesh_destroyed() {
  clickedtriangle = 0;
  if(textmode) return;
  fl_deactivate_object(fdui()->meshinfo);
  fl_set_object_lcol(fdui()->button_meshinfo, FL_INACTIVE_COL);
  fl_set_object_lcol(fdui()->button_select_triangle, FL_INACTIVE_COL);
  fl_set_object_lcol(fdui()->button_movenode, FL_INACTIVE_COL);
  fl_set_object_lcol(fdui()->choice_meshinfo, FL_INACTIVE_COL);
  fl_set_object_lcol(fdui()->button_highlight, FL_INACTIVE_COL);
  fl_set_object_lcol(fdui()->button_highlight_color, FL_INACTIVE_COL);
  fl_set_object_lcol(fdui()->button_meshinfo_selection, FL_INACTIVE_COL);
  fl_set_object_lcol(fdui()->button_meshinfo_selection_color, FL_INACTIVE_COL);
  fl_set_object_lcol(fdui()->meshinfo_text_mouse, FL_INACTIVE_COL);
}

void MeshInfoDashboard::triangle_destroyed(const AMTriangle *tri) {
  if(tri == clickedtriangle)
    clickedtriangle = 0;
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

int MeshInfoDashboard::show() {
  setrubberband();
  setcursor();
  return 1;
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

void MeshInfoDashboard::CB_mousemode(FL_OBJECT *obj, long) {
  ((MeshInfoDashboard*) ImageFormGfx::objdash(obj))->set_mousemode_gui(obj);
}

void MeshInfoDashboard::set_mousemode_gui(FL_OBJECT *obj) {
  if(active()) {
    if(obj == fdui()->button_meshinfo)
      set_mousemode(MM_QUERY);
    else if(obj == fdui()->button_select_triangle)
      set_mousemode(MM_SELECT);
    else if(obj == fdui()->button_movenode)
      set_mousemode(MM_MOVE);
    garcon()->log(menucommand() + "mousemode=" + to_charstring(mousemode));
  }
}

void MeshInfoDashboard::set_mousemode(const Enum<MouseMode> &mode) {
  mousemode_dflt = mousemode = mode;
  if(textmode) return;
  setrubberband();
  setcursor();
  fl_freeze_form(imageform()->xform());
  switch(mousemode) {
  case MM_QUERY:
    fl_set_button(fdui()->button_meshinfo, 1);
    fl_set_button(fdui()->button_select_triangle, 0);
    fl_set_button(fdui()->button_movenode, 0);
    break;
  case MM_SELECT:
    fl_set_button(fdui()->button_meshinfo, 0);
    fl_set_button(fdui()->button_select_triangle, 1);
    fl_set_button(fdui()->button_movenode, 0);
    break;
  case MM_MOVE:
    fl_set_button(fdui()->button_meshinfo, 0);
    fl_set_button(fdui()->button_select_triangle, 0);
    fl_set_button(fdui()->button_movenode, 1);
    break;
  }
  fl_unfreeze_form(imageform()->xform());
}

void MeshInfoDashboard::set_mousemode_cmd(const CharString&) {
  ((MeshInfoDashboard*) get_dashboard())->set_mousemode(mousemode_dflt);
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

void MeshInfoDashboard::CB_choice_meshinfo(FL_OBJECT *obj, long) {
  ((MeshInfoDashboard*) ImageFormGfx::objdash(obj))->set_infomode_gui();
}

void MeshInfoDashboard::set_infomode_gui() {
  int which = fl_get_choice(fdui()->choice_meshinfo);
  if(which == 1)		// selected
    infomode = infomode_dflt = IM_SELECTED;
  else if(which == 2)		// clicked
    infomode = infomode_dflt = IM_CLICKED;
  else if(which == 3)
    infomode = infomode_dflt = IM_ACTIVE;
  else if(which == 4)
    infomode = infomode_dflt = IM_ALL;
  garcon()->log(menucommand() + "infomode=" + to_charstring(infomode));
  display_info(lastclick);
}

void MeshInfoDashboard::set_infomode_cmd(const CharString&) {
  ((MeshInfoDashboard*) get_dashboard())->set_infomode();
}

void MeshInfoDashboard::set_infomode() {
  infomode = infomode_dflt;
  if(imageform()->beenclicked)
    display_info(where);
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

// Highlight the clicked triangle

void MeshInfoDashboard::CB_button_highlight(FL_OBJECT *obj, long) {
  ((MeshInfoDashboard*) ImageFormGfx::objdash(obj))->set_highlight_gui();
}

void MeshInfoDashboard::set_highlight_gui() {
  if(active()) {
    highlight = fl_get_button(fdui()->button_highlight);
    highlight_dflt = highlight;
    garcon()->log(menucommand() + "highlight=" + to_charstring(highlight));
    imageform()->draw();
  }
}

void MeshInfoDashboard::set_highlight_cmd(const CharString&) {
  ((MeshInfoDashboard*) get_dashboard())->set_highlight();
}

void MeshInfoDashboard::set_highlight() {
  highlight = highlight_dflt;
  if(!textmode)
    fl_set_button(fdui()->button_highlight, highlight);
  imageform()->draw();
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

// Set color for highlighting the clicked triangle

void MeshInfoDashboard::set_highlight_color(const Color &color) {
  highlight_color = highlight_color_dflt = color;
  imageform()->getpixel(highlight_color, ImageFormGfx::HIGHLIGHTPIXEL);
  imageform()->draw();
}

void MeshInfoDashboard::CB_button_highlight_color(FL_OBJECT *obj, long) {
  ((MeshInfoDashboard*) ImageFormGfx::objdash(obj))->set_highlight_color_gui();
}

void MeshInfoDashboard::set_highlight_color_gui() {
  set_highlight_color(colorbrowser(highlight_color));
  garcon()->log(menucommand() + "highlight_color=" +
		to_charstring(highlight_color));
}

void MeshInfoDashboard::set_highlight_color_cmd(const CharString&) {
  ((MeshInfoDashboard*) get_dashboard())->
    set_highlight_color(highlight_color_dflt);
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

void MeshInfoDashboard::clear_selection() {
  goof()->mesh()->unselect_all();
  goof()->redraw();
}

void MeshInfoDashboard::CB_button_meshinfo_clearselect(FL_OBJECT *obj, long) {
  ((MeshInfoDashboard*) ImageFormGfx::objdash(obj))->clear_selection_gui();
}

void MeshInfoDashboard::clear_selection_gui() {
  clear_selection();
  garcon()->log(menucommand() + "unselectmesh");
}

CommandFn MeshInfoDashboard::clear_selection_cmd() {
  ((MeshInfoDashboard*) get_dashboard())->clear_selection();
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

void MeshInfoDashboard::clickhandler(const MouseClick &click) {
  switch(mousemode) {
  case MM_QUERY:
    imageform()->beenclicked = 1;
    display_info(click);
    imageform()->pixelinfodash->display_info(click);
#ifdef DEBUG
    dump_intersections();
#endif // DEBUG
    break;
  case MM_SELECT:
    select_mesh_triangle(click.coord, click.shiftkey);
    break;
  case MM_MOVE:
    return;
  }
  goof()->redraw();
}

// completed drag (mouse down, move, up)
void MeshInfoDashboard::draghandler(const MouseClick &dn, const MouseClick &up)
{
  switch(mousemode) {
  case MM_QUERY:
    return;
  case MM_SELECT:
    select_mesh_triangles(up.coord, dn.coord, dn.shiftkey);
    break;
  case MM_MOVE:
    if(meshexists()) {
      moving = 0;
      if(!textmode) {
	fl_set_object_label(fdui()->meshinfo_text_E, "E");
	fl_set_object_label(fdui()->meshinfo_text_Eshape, "E shape");
	fl_set_object_label(fdui()->meshinfo_text_Ehomog, "E homog.");
	fl_redraw_object(fdui()->meshinfo_text_E);
	fl_set_object_lcol(fdui()->meshinfo_E, FL_BLACK);
	fl_set_object_lcol(fdui()->meshinfo_E_shape, FL_BLACK);
	fl_set_object_lcol(fdui()->meshinfo_E_homog, FL_BLACK);
	update();		// refreshes E display
      }
      goof()->dup_mesh();
      if(!goof()->mesh()->move_node(dn.coord, up.coord))
	goof()->prev_mesh();	// unsuccesful move
    }
    break;
  }
  goof()->redraw();
}


// drag in progress (mouse down, move, not yet up)
void MeshInfoDashboard::movehandler(const MouseClick &dn,
				    const MouseClick &clck)
{
  if(textmode) return;
  if(mousemode == MM_MOVE) {
    if(meshexists()) {
      if(!moving) {		// start of a move
	moving = 1;
	moving_node = goof()->mesh()->closest_node(dn.coord);
	Estart = moving_node->E();
	Ehstart = moving_node->Ehomogeneity();
	Esstart = moving_node->Eangle();
	fl_set_object_label(fdui()->meshinfo_text_E, "dE");
	fl_set_object_label(fdui()->meshinfo_text_Eshape, "dE shape");
	fl_set_object_label(fdui()->meshinfo_text_Ehomog, "dE homog");
	fl_set_object_lcol(fdui()->meshinfo_E, FL_RED);
	fl_set_object_lcol(fdui()->meshinfo_E_shape, FL_RED);
	fl_set_object_lcol(fdui()->meshinfo_E_homog, FL_RED);
      }
      moving_node->move_to(clck.coord);
      CharString dE = to_charstring(moving_node->E() - Estart);
      CharString dEh = to_charstring(moving_node->Ehomogeneity() - Ehstart);
      CharString dEs = to_charstring(moving_node->Eangle() - Esstart);
      moving_node->revert();
      fl_set_object_label(fdui()->meshinfo_E, dE);
      fl_set_object_label(fdui()->meshinfo_E_homog, dEh);
      fl_set_object_label(fdui()->meshinfo_E_shape, dEs);
    }
  }
  fl_set_object_label(fdui()->meshinfo_mouse_xy, to_charstring(clck.coord));
}

void MeshInfoDashboard::setrubberband() {
  if(mousemode == MM_SELECT) {
    rubberbandtype = RB_RECTANGLE;
    canvas()->drag_rubberband();
  }
  else if(mousemode == MM_MOVE) {
    rubberbandtype = RB_NODE;
    canvas()->drag_rubberband();
  }
  else
    canvas()->drag_nothing();
}

void MeshInfoDashboard::setcursor() {
  switch(mousemode) {
  case MM_QUERY:
    Dashboard::setcursor(*infocursor);
    break;
  case MM_SELECT:
    Dashboard::setcursor(*checkcursor);
    break;
  case MM_MOVE:
    Dashboard::setcursor(XC_crosshair);
    break;
  }
}

void MeshInfoDashboard::display_info(const MouseClick &click) {
  lastclick = click;
  display_info(click.coord);
}

void MeshInfoDashboard::display_info(const MeshCoord &coord) {
  where = coord;
  if(textmode) return;
  fl_freeze_form(imageform()->xform());
  fl_clear_browser(fdui()->meshinfo_material);
  fl_clear_browser(fdui()->meshinfo_groups);

  if(goof() && goof()->mesh()) {
    AdaptiveMesh *mesh = goof()->mesh();
    fl_set_object_label(fdui()->meshinfo_size,
			to_charstring(mesh->ntriangles()) + " triangles, " +
			to_charstring(mesh->nnodes()) + " nodes");

    if(imageform()->beenclicked) {
      fl_set_object_label(fdui()->meshinfo_mouse_xy, to_charstring(where));
      //      if(mousemode == MM_QUERY)
      clickedtriangle = goof()->mesh()->smallest_triangle_containing(where);
    }

    if(infomode == IM_CLICKED) {
      if(imageform()->beenclicked) {
	if(clickedtriangle) {
	  fl_set_object_label(fdui()->meshinfo_depth,
			      to_charstring(clickedtriangle->depth()));
	  fl_set_object_label(fdui()->meshinfo_area,
			      to_charstring(clickedtriangle->area()));
	  fl_set_object_label(fdui()->meshinfo_E_shape,
			      to_charstring(clickedtriangle->Eangle()));
	  fl_set_object_label(fdui()->meshinfo_E_homog,
			      to_charstring(clickedtriangle->Ehomogeneity()));
	  fl_set_object_label(fdui()->meshinfo_E,
			      to_charstring(clickedtriangle->E()));
	  // display material
	  Material *material = clickedtriangle->material();
	  if(material)
	    set_material_browser(clickedtriangle->material(),
				 fdui()->meshinfo_material);
	  else
	    fl_add_browser_line(fdui()->meshinfo_material, "@bdefault");
	  // list mesh groups
	  const Vec<MeshGroup*> &glist = clickedtriangle->mesh_groups();
	  for(int i=0; i<glist.capacity(); i++)
	    fl_add_browser_line(fdui()->meshinfo_groups,
				glist[i]->query_name());
	}
	else {			// no triangle!
	  fl_set_object_label(fdui()->meshinfo_depth, "");
	  fl_set_object_label(fdui()->meshinfo_area, "");
	  fl_set_object_label(fdui()->meshinfo_E_shape, "");
	  fl_set_object_label(fdui()->meshinfo_E_homog, "");
	  fl_set_object_label(fdui()->meshinfo_E, "");
	}
      }
      else {			// infomode=IM_CLICKED but not yet clicked
	fl_set_object_label(fdui()->meshinfo_mouse_xy, "");
	fl_set_object_label(fdui()->meshinfo_depth, "");
	fl_set_object_label(fdui()->meshinfo_area, "");
	fl_set_object_label(fdui()->meshinfo_E_shape, "");
	fl_set_object_label(fdui()->meshinfo_E_homog, "");
	fl_set_object_label(fdui()->meshinfo_E, "");
      }
    }

    else if(infomode == IM_SELECTED || infomode == IM_ALL ||
	    infomode == IM_ACTIVE) {
      double area = 0;
      double Eshape = 0;
      double Ehomog = 0;
      double E = 0;
      for(AMIterator i(mesh, (infomode==IM_SELECTED?AMI_SELECTED:AMI_ALL));
	  !i.end(); ++i)
	{
	  AMTriangle *tri = (*mesh)[i];
	  if(infomode != IM_ACTIVE || tri->active()) {
	    area += tri->area();
	    Eshape += tri->Eangle();
	    Ehomog += tri->Ehomogeneity();
	    E += tri->E();
	  }
	}
      fl_set_object_label(fdui()->meshinfo_area, to_charstring(area));
      fl_set_object_label(fdui()->meshinfo_E_shape, to_charstring(Eshape));
      fl_set_object_label(fdui()->meshinfo_E_homog, to_charstring(Ehomog));
      fl_set_object_label(fdui()->meshinfo_E, to_charstring(E));
      fl_set_object_label(fdui()->meshinfo_depth, "");
    }

  }

  else {			// mesh doesn't exist!
    fl_set_object_label(fdui()->meshinfo_size, "No mesh!");
    fl_set_object_label(fdui()->meshinfo_mouse_xy, "");
    fl_set_object_label(fdui()->meshinfo_depth, "");
    fl_set_object_label(fdui()->meshinfo_area, "");
    fl_set_object_label(fdui()->meshinfo_E_shape, "");
    fl_set_object_label(fdui()->meshinfo_E_homog, "");
    fl_set_object_label(fdui()->meshinfo_E, "");
  }
  fl_unfreeze_form(imageform()->xform());
}

void MeshInfoDashboard::draw_extras() {
  imageform()->getpixel(highlight_color, ImageFormGfx::HIGHLIGHTPIXEL);
  if(imageform()->beenclicked)
      if(clickedtriangle && highlight)
	clickedtriangle->outline(*canvas(), highlight_color);
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//
//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

// display a Material and its parameters in an XForms browser.

static void set_mb(const CharString &params, FL_OBJECT *browser) {
  fl_addto_browser_chars(browser, params);
  fl_set_browser_topline(browser, 1);
  return;
}

void InfoDashboard::set_material_browser(const Material *material, 
					 FL_OBJECT *browser)
{
  int index=0;
  CharString params("@b"+material->parameters);

  // Have to replace selected spaces with newlines, so that the
  // argument list looks good in the browser.

  // First, skip over the material name...
  while(params[index] != '\0' && params[index] != ' ') index++;
  if(params[index] == '\0') {
    set_mb(params, browser);
    return;
  }
  params[index++] = '\n';

  // Now, terminate each A=B pair with a newline. Since B can contain
  // spaces, look for the next '=' and work back from there.
  int eqsign = index;		// position of this '='
  // find first '='
  while(params[eqsign] != '=' && params[eqsign] != '\0') eqsign++;
  if(params[eqsign] == '\0') {	// no args at all!
    set_mb(params, browser);
    return;
  }
  for(;;) {
    // find next '='
    int nexteq = eqsign+1;
    while(params[nexteq] != '=' &&  params[nexteq] != '\0') nexteq++;
    if(params[nexteq] == '\0') {
      set_mb(params, browser);
      return;
    }
    // work back from nexteq to find a space
    index = nexteq-1;
    while(params[index] != ' ') index--;
    params[index] = '\n';
    // go on to next arg
    eqsign = nexteq;
  }
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

#ifdef DEBUG
void MeshInfoDashboard::dump_intersections() const {
  if(clickedtriangle)
    clickedtriangle->dump_intersections();
}
#endif

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

#include "enumio.C"
