// -*- C++ -*-
// $RCSfile: displaydash.C,v $
// $Revision: 1.6 $
// $Author: langer $
// $Date: 2000-10-30 19:23:24 $

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */

#include "colorutils.h"
#include "displaydash.h"
#include "goof.h"
#include "imageform.h"
#include "menuDef.h"
#include "ppm2oof.h"
#include "stdlib.h"

Enum<ImageBackground> DisplayDashboard::background_dflt(BG_IMAGE);
Enum<MaterialDisplayMethod> DisplayDashboard::material_display_dflt(MDM_PIXEL);
TrueFalse DisplayDashboard::mesh_visible_dflt(1);
int DisplayDashboard::meshwidth_dflt(0);
TrueFalse DisplayDashboard::selected_mesh_visible_dflt(1);
TrueFalse DisplayDashboard::selected_pixels_visible_dflt(1);
TrueFalse DisplayDashboard::firewalls_visible_dflt(0);
Color DisplayDashboard::selectedcolor_dflt(153, 68, 0);
Color DisplayDashboard::meshcolor_dflt(236, 198, 17);
Color DisplayDashboard::firewallcolor_dflt(57, 181, 226);
Color DisplayDashboard::selectedmeshcolor_dflt(255, 0, 60);
Color DisplayDashboard::nomaterialcolor_dflt(100, 50, 50);

DisplayDashboard::DisplayDashboard(ImageForm *iform, FL_OBJECT *grp)
  : Dashboard("Display", iform, grp),
    background(background_dflt),
    material_display_method(material_display_dflt),
    selected_pixels_visible(selected_pixels_visible_dflt),
    selectedcolor(selectedcolor_dflt),
    mesh_visible(mesh_visible_dflt),
    meshwidth(meshwidth_dflt),
    meshcolor(meshcolor_dflt),
    selected_mesh_visible(selected_mesh_visible_dflt),
    selectedmeshcolor(selectedmeshcolor_dflt),
    nomaterialcolor(nomaterialcolor_dflt),
    firewalls_visible(firewalls_visible_dflt),
    firewallcolor(firewallcolor_dflt)
{
  if(!textmode) {
    if(background == BG_IMAGE) {
      fl_set_button(fdui()->button_image, 1);
      fl_set_button(fdui()->button_material, 0);
    }
    else {
      fl_set_button(fdui()->button_image, 0);
      fl_set_button(fdui()->button_material, 1);
    }
    
    set_display_method_buttons();

    fl_set_button(fdui()->button_mesh, mesh_visible? 1 : 0);
    fl_set_input(fdui()->input_meshwidth, to_charstring(meshwidth));

    fl_set_button(fdui()->button_selectedmesh, selected_mesh_visible);
    fl_set_button(fdui()->button_meshinfo_selection, selected_mesh_visible);
    fl_set_button(fdui()->button_selectedpixels, selected_pixels_visible);
    fl_set_button(fdui()->button_firewalls, firewalls_visible_dflt);
  
    if(goof()->mesh())
      activate_mesh_controls();
    else
      deactivate_mesh_controls();
  }
  static int dothisonce = 1;
  if(dothisonce) {
    Enum<ImageBackground>::name(BG_IMAGE, "image");
    Enum<ImageBackground>::name(BG_MATERIAL, "material");
    Enum<MaterialDisplayMethod>::name(MDM_PIXEL, "pixel");
    Enum<MaterialDisplayMethod>::name(MDM_TRIANGLE, "triangle");
    dothisonce = 0;
  }

  buildmenu(menu, MOPT_NORMAL);
  buildmenu(imageformmenu(), MOPT_NOPRINT);	// for backwards compatibility
}

void DisplayDashboard::buildmenu(Menu *carte, MenuAttribute attr) {
  AddVariable(carte, "background", "display image or material?",
	      background, background_cmd, attr);
  AddVariable(carte, "matdisplay", "display material by pixel or by triangle?",
	      material_display_dflt, set_display_method_cmd, attr);
  AddVariable(carte, "meshvisible", "display mesh?",mesh_visible,set_mesh_cmd, attr);
  AddVariable(carte, "meshwidth", "set mesh width", meshwidth, meshwidth_cmd, attr);
  AddVariable(carte, "meshcolor", "set mesh color", meshcolor,
	      set_meshcolor_cmd, attr);
  AddVariable(carte, "selectedcolor", "set color of selected pixels",
	      selectedcolor, set_selectedcolor_cmd, attr);
  AddVariable(carte, "selectedpixels", "are selected pixels visible?",
	      selected_pixels_visible, set_selpixvis_cmd, attr);
  AddVariable(carte, "firewalls", "are firewalls visible?",
	      firewalls_visible, set_firewallsvis_cmd, attr);
  AddVariable(carte, "firewallcolor", "set color of fire walls",
	      firewallcolor, set_firewallcolor_cmd, attr);
  AddVariable(carte, "selectedmesh", "are selected triangles visible?",
	      selected_mesh_visible, set_selectedmeshvis_cmd, attr);
  AddVariable(carte, "selectedmeshcolor",
	      "set color of selected mesh triangles",
	      selectedmeshcolor, set_selectedmeshcolor_cmd, attr);
  AddVariable(carte, "nomaterialcolor",
	      "set color of pixels with no assigned material",
	      nomaterialcolor, set_nomaterialcolor_cmd, attr);
}


DisplayDashboard::~DisplayDashboard() {
  menu->RemoveOption("background");
  menu->RemoveOption("matdisplay");
  menu->RemoveOption("meshvisible");
  menu->RemoveOption("meshwidth");
  menu->RemoveOption("meshcolor");
  menu->RemoveOption("selectedcolor");
  menu->RemoveOption("selectedpixels");
  menu->RemoveOption("firewalls");
  menu->RemoveOption("firewallcolor");
  menu->RemoveOption("selectedmesh");
  menu->RemoveOption("selectedmeshcolor");
  menu->RemoveOption("nomaterialcolor");
  delete menu;
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

void DisplayDashboard::activate_mesh_controls() {
  if(textmode) return;
  fl_freeze_form(fdui()->image_drawer);

  fl_activate_object(fdui()->button_mesh);
  fl_activate_object(fdui()->input_meshwidth);
  fl_activate_object(fdui()->button_selectedmesh);
  fl_activate_object(fdui()->button_selectedmeshcolor);
  fl_activate_object(fdui()->button_trimaterial);

  fl_set_object_lcol(fdui()->button_mesh, FL_BLACK);
  fl_set_object_lcol(fdui()->input_meshwidth, FL_BLACK);
  fl_set_object_lcol(fdui()->button_selectedmesh, FL_BLACK);
  fl_set_object_lcol(fdui()->button_selectedmeshcolor, FL_BLACK);
  fl_set_object_lcol(fdui()->button_trimaterial, FL_BLACK);

  fl_redraw_object(fdui()->button_mesh);
  fl_redraw_object(fdui()->input_meshwidth);
  fl_redraw_object(fdui()->button_selectedmesh);
  fl_redraw_object(fdui()->button_selectedmeshcolor);
  fl_redraw_object(fdui()->button_trimaterial);

  fl_unfreeze_form(fdui()->image_drawer);
}

void DisplayDashboard::deactivate_mesh_controls() {
  if(textmode) return;
  fl_freeze_form(fdui()->image_drawer);

  fl_deactivate_object(fdui()->button_mesh);
  fl_deactivate_object(fdui()->input_meshwidth);
  fl_deactivate_object(fdui()->button_selectedmesh);
  fl_deactivate_object(fdui()->button_selectedmeshcolor);
  fl_deactivate_object(fdui()->button_trimaterial);

  fl_set_object_lcol(fdui()->button_mesh, FL_INACTIVE_COL);
  fl_set_object_lcol(fdui()->input_meshwidth, FL_INACTIVE_COL);
  fl_set_object_lcol(fdui()->button_selectedmesh, FL_INACTIVE_COL);
  fl_set_object_lcol(fdui()->button_selectedmeshcolor, FL_INACTIVE_COL);
  fl_set_object_lcol(fdui()->button_trimaterial, FL_INACTIVE_COL);

  fl_redraw_object(fdui()->button_mesh);
  fl_redraw_object(fdui()->input_meshwidth);
  fl_redraw_object(fdui()->button_selectedmesh);
  fl_redraw_object(fdui()->button_selectedmeshcolor);
  fl_redraw_object(fdui()->button_trimaterial);

  fl_unfreeze_form(fdui()->image_drawer);
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

void DisplayDashboard::show_image() {
  background_dflt = background = BG_IMAGE;
  imageform()->draw();
}

void DisplayDashboard::show_material() {
  background_dflt = background = BG_MATERIAL;
  imageform()->draw();
}

// called by forms, image button

void DisplayDashboard::CB_button_image(FL_OBJECT *obj, long) {
  ((DisplayDashboard*) ImageFormGfx::objdash(obj))->show_image_gui();
}

void DisplayDashboard::show_image_gui() {
  if(active()) {
    show_image();
    garcon()->log(menucommand() + "background=" + to_charstring(background));
  }
}

// called by forms, material button

void DisplayDashboard::CB_button_material(FL_OBJECT *obj, long) {
  ((DisplayDashboard*) ImageFormGfx::objdash(obj))->show_material_gui();
}

void DisplayDashboard::show_material_gui() {
  if(active()) {
    show_material();
    garcon()->log(menucommand() + "background=" + to_charstring(background));
  }
}

// called by menus

void DisplayDashboard::background_cmd(const CharString&) {
  if(!textmode)
    ((DisplayDashboard*) get_dashboard())->set_background();
}

void DisplayDashboard::set_background() {
  if(background == BG_IMAGE) {
    show_image();
    fl_set_button(fdui()->button_image, 1);
  }
  else {
    show_material();
    fl_set_button(fdui()->button_material, 1);
  }
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

void DisplayDashboard::set_display_method(Enum<MaterialDisplayMethod> m) {
  material_display_method = material_display_dflt = m;
  set_display_method_buttons();
  imageform()->draw();
}

void DisplayDashboard::CB_button_pixmaterial(FL_OBJECT *obj, long) {
  ((DisplayDashboard*) ImageFormGfx::objdash(obj))
    ->set_display_method_gui(MDM_PIXEL);
}

void DisplayDashboard::CB_button_trimaterial(FL_OBJECT *obj, long) {
  ((DisplayDashboard*) ImageFormGfx::objdash(obj))
    ->set_display_method_gui(MDM_TRIANGLE);
}

void DisplayDashboard::set_display_method_gui(Enum<MaterialDisplayMethod> m) {
  if(active()) {
    garcon()->log(menucommand() + "matdisplay=" + to_charstring(m));
    set_display_method(m);
  }
}

void DisplayDashboard::set_display_method_cmd(const CharString &) {
  ((DisplayDashboard*) get_dashboard())
    ->set_display_method(material_display_dflt);
}

void DisplayDashboard::set_display_method_buttons() {
  if(textmode) return;
  fl_freeze_form(fdui()->image_drawer);
  switch(material_display_method) {
  case MDM_PIXEL:
    fl_set_button(fdui()->button_pixmaterial, 1);
    fl_set_button(fdui()->button_trimaterial, 0);
    break;
  case MDM_TRIANGLE:
    fl_set_button(fdui()->button_pixmaterial, 0);
    fl_set_button(fdui()->button_trimaterial, 1);
    break;
  }
  fl_unfreeze_form(fdui()->image_drawer);
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

void DisplayDashboard::set_mesh_visibility(const TrueFalse &mv) {
  mesh_visible = mesh_visible_dflt = mv;
  imageform()->draw();
}

void DisplayDashboard::CB_button_mesh(FL_OBJECT *obj, long) {
  ((DisplayDashboard*) ImageFormGfx::objdash(obj))->set_mesh_gui();
}

void DisplayDashboard::set_mesh_gui() {
  if(active()) {
    set_mesh_visibility(fl_get_button(fdui()->button_mesh));
    garcon()->log(menucommand() + "meshvisible="
		  + to_charstring(mesh_visible));
  }
}

void DisplayDashboard::set_mesh_cmd(const CharString&) {
  ((DisplayDashboard*) get_dashboard())->set_mesh_visibility();
}

void DisplayDashboard::set_mesh_visibility() {
  set_mesh_visibility(mesh_visible);
  if(!textmode)
    fl_set_button(fdui()->button_mesh, mesh_visible);
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

void DisplayDashboard::set_meshwidth(int w) {
  if(w < 0) {
    w = 0;
    if(!textmode)
      fl_set_input(fdui()->input_meshwidth, "0");
  }
  meshwidth = meshwidth_dflt = w;
  imageform()->draw();
}

void DisplayDashboard::CB_input_meshwidth(FL_OBJECT *obj, long) {
  ((DisplayDashboard*) ImageFormGfx::objdash(obj))->set_meshwidth_gui();
}

void DisplayDashboard::set_meshwidth_gui() {
  if(active()) {
    set_meshwidth(atoi(fl_get_input(fdui()->input_meshwidth)));
    garcon()->log(menucommand() + "meshwidth=" + to_charstring(meshwidth));
  }
}

void DisplayDashboard::meshwidth_cmd(const CharString&) {
  ((DisplayDashboard*) get_dashboard())->set_meshwidth();
}

void DisplayDashboard::set_meshwidth() {
  if(!textmode)
    fl_set_input(fdui()->input_meshwidth, to_charstring(meshwidth));
  set_meshwidth(meshwidth);
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

// set color of mesh

void DisplayDashboard::set_meshcolor(const Color &color) {
  meshcolor = meshcolor_dflt = color;
  imageform()->getpixel(meshcolor, ImageFormGfx::MESHPIXEL);
  //  imageform()->set_mesh_color(meshcolor);
  imageform()->draw();
}

void DisplayDashboard::CB_button_meshcolor(FL_OBJECT *obj, long) {
  ((DisplayDashboard*) ImageFormGfx::objdash(obj))->set_meshcolor_gui();
}

void DisplayDashboard::set_meshcolor_gui() {
  if(active()) {
    set_meshcolor(colorbrowser(meshcolor));
    garcon()->log(menucommand() + "meshcolor=" +
		  to_charstring(meshcolor));
  }
}

void DisplayDashboard::set_meshcolor_cmd(const CharString&) {
  ((DisplayDashboard*) get_dashboard())->set_meshcolor();
}

void DisplayDashboard::set_meshcolor() {
  set_meshcolor(meshcolor);
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

// set color of selected pixels

void DisplayDashboard::set_selectedcolor(const Color &color) {
  selectedcolor = selectedcolor_dflt = color;
  imageform()->getpixel(selectedcolor, ImageFormGfx::SELECTEDPIXEL);
  //  imageform()->set_selected_color(selectedcolor);
  imageform()->draw();
}

void DisplayDashboard::CB_button_pixcolor(FL_OBJECT *obj, long) {
  ((DisplayDashboard*) ImageFormGfx::objdash(obj))->set_selectedcolor_gui();
}

void DisplayDashboard::set_selectedcolor_gui() {
  if(active()) {
    set_selectedcolor(colorbrowser(selectedcolor));
    garcon()->log(menucommand() + "selectedcolor=" +
		  to_charstring(selectedcolor));
  }
}

void DisplayDashboard::set_selectedcolor_cmd(const CharString&) {
  ((DisplayDashboard*) get_dashboard())->set_selectedcolor();
}

void DisplayDashboard::set_selectedcolor() {
  set_selectedcolor(selectedcolor);
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

// display selected pixels?

void DisplayDashboard::CB_button_selpix(FL_OBJECT *obj, long) {
  ((DisplayDashboard*) ImageFormGfx::objdash(obj))->set_selpixvisible_gui(obj);
}

void DisplayDashboard::set_selpixvisible_gui(FL_OBJECT *obj) {
  if(active()) {
    selected_pixels_visible = fl_get_button(obj);
    selected_pixels_visible_dflt = selected_pixels_visible;
    garcon()->log(menucommand() + "selectedpixels=" +
		  to_charstring(selected_pixels_visible));
    imageform()->draw();
  }
}

void DisplayDashboard::set_selpixvis_cmd(const CharString&) {
  ((DisplayDashboard*) get_dashboard())->set_selpixvisible();
}

void DisplayDashboard::set_selpixvisible() {
  selected_pixels_visible_dflt = selected_pixels_visible;
  if(!textmode)
    fl_set_button(fdui()->button_selectedpixels, selected_pixels_visible);
  imageform()->draw();
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

void DisplayDashboard::CB_button_firewalls(FL_OBJECT *obj, long) {
  ((DisplayDashboard*)
   ImageFormGfx::objdash(obj))->set_firewalls_visible_gui(obj);
}

void DisplayDashboard::set_firewalls_visible_gui(FL_OBJECT *obj) {
  if(active()) {
    firewalls_visible = fl_get_button(obj);
    firewalls_visible_dflt = firewalls_visible;
    garcon()->log(menucommand() + "firewalls=" +
		  to_charstring(firewalls_visible));
    imageform()->draw();
  }
}

void DisplayDashboard::set_firewallsvis_cmd(const CharString&) {
  ((DisplayDashboard*) get_dashboard())->set_firewallsvisible();
}

void DisplayDashboard::set_firewallsvisible() {
  firewalls_visible_dflt = firewalls_visible;
  if(!textmode)
    fl_set_button(fdui()->button_firewalls, firewalls_visible);
  imageform()->draw();
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

// set color of firewalls

void DisplayDashboard::set_firewallcolor(const Color &color) {
  firewallcolor = firewallcolor_dflt = color;
  imageform()->getpixel(firewallcolor, ImageFormGfx::FIREWALLPIXEL);
  imageform()->draw();
}

void DisplayDashboard::CB_button_firewallcolor(FL_OBJECT *obj, long) {
  ((DisplayDashboard*) ImageFormGfx::objdash(obj))->set_firewallcolor_gui();
}

void DisplayDashboard::set_firewallcolor_gui() {
  if(active()) {
    set_firewallcolor(colorbrowser(firewallcolor));
    garcon()->log(menucommand() + "firewallcolor=" +
		  to_charstring(firewallcolor));
  }
}

void DisplayDashboard::set_firewallcolor_cmd(const CharString&) {
  ((DisplayDashboard*) get_dashboard())->set_firewallcolor();
}

void DisplayDashboard::set_firewallcolor() {
  set_firewallcolor(firewallcolor);
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

// are selected triangles in the mesh visible?

void DisplayDashboard::CB_button_selectedmesh(FL_OBJECT *obj, long) {
  // don't use objdash here, because this callback is used by the mesh
  // dashboard as well.
  ImageFormGfx::objform(obj)->displaydash->set_selectedmeshvis_gui(obj);
}

void DisplayDashboard::set_selectedmeshvis_gui(FL_OBJECT *obj) {
  selected_mesh_visible = selected_mesh_visible_dflt =
    fl_get_button(obj);
  fl_set_button(fdui()->button_selectedmesh, selected_mesh_visible);
  fl_set_button(fdui()->button_meshinfo_selection, selected_mesh_visible);
  garcon()->log(menucommand() + "selectedmesh=" +
		to_charstring(selected_mesh_visible));
  imageform()->draw();
}

void DisplayDashboard::set_selectedmeshvis_cmd(const CharString&) {
  ((DisplayDashboard*) get_dashboard())->set_selectedmeshvis();
}

void DisplayDashboard::set_selectedmeshvis() {
  selected_mesh_visible_dflt = selected_mesh_visible;
  if(!textmode) {
    fl_set_button(fdui()->button_selectedmesh, selected_mesh_visible);
    fl_set_button(fdui()->button_meshinfo_selection, selected_mesh_visible);
    imageform()->draw();
  }
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

// set color of selected triangles in the mesh

void DisplayDashboard::set_selectedmeshcolor(const Color &color) {
  selectedmeshcolor = selectedmeshcolor_dflt = color;
  selectedmeshfadecolor = selectedmeshcolor.fade(ActiveArea::fade);
  imageform()->getpixel(selectedmeshcolor, ImageFormGfx::SELMESHPIXEL);
  imageform()->getpixel(selectedmeshfadecolor, ImageFormGfx::SELMESHFADEPIXEL);
  imageform()->draw();
}

void DisplayDashboard::CB_button_selectedmeshcolor(FL_OBJECT *obj, long) {
  // don't use objdash here, because this callback is used by the mesh
  // dashboard as well.
  ImageFormGfx::objform(obj)->displaydash->set_selectedmeshcolor_gui();
}

void DisplayDashboard::set_selectedmeshcolor_gui() {
  // this is also called by MeshInfoDashboard, so don't check active()
  // here. In fact, there's no need to check active() for anything but
  // input objects anyway.
  set_selectedmeshcolor(colorbrowser(selectedmeshcolor));
  garcon()->log(menucommand() + "selectedmeshcolor=" +
		to_charstring(selectedmeshcolor));
}

void DisplayDashboard::set_selectedmeshcolor_cmd(const CharString&) {
  // don't use get_dashboard() here, because this callback is used by
  // the mesh dashboard as well.
  get_imageform()->displaydash->set_selectedmeshcolor();
}

void DisplayDashboard::set_selectedmeshcolor() {
  set_selectedmeshcolor(selectedmeshcolor);
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

// set color of pixels with no material assigned to them

void DisplayDashboard::set_nomaterialcolor(const Color &color) {
  nomaterialcolor = nomaterialcolor_dflt = color;
  imageform()->getpixel(nomaterialcolor, ImageFormGfx::NOMATERIALPIXEL);
  imageform()->draw();
}

void DisplayDashboard::CB_button_nomaterialcolor(FL_OBJECT *obj, long) {
  ((DisplayDashboard*) ImageFormGfx::objdash(obj))->set_nomaterialcolor_gui();
}

void DisplayDashboard::set_nomaterialcolor_gui() {
  if(active()) {
    set_nomaterialcolor(colorbrowser(nomaterialcolor));
    garcon()->log(menucommand() + "nomaterialcolor=" +
		  to_charstring(nomaterialcolor));
  }
}

void DisplayDashboard::set_nomaterialcolor_cmd(const CharString&) {
  ((DisplayDashboard*) get_dashboard())->set_nomaterialcolor();
}

void DisplayDashboard::set_nomaterialcolor() {
  set_nomaterialcolor(nomaterialcolor);
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

#include "enumio.C"
