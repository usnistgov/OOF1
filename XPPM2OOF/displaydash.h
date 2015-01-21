// -*- C++ -*-
// $RCSfile: displaydash.h,v $
// $Revision: 1.4 $
// $Author: langer $
// $Date: 2000-09-06 20:11:29 $

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */

#ifndef DISPLAYDASH_H
#define DISPLAYDASH_H

#include "color.h"
#include "dashboard.h"
#include "enum.h"
#include "truefalse.h"

enum MaterialDisplayMethod {MDM_PIXEL, MDM_TRIANGLE};
enum ImageBackground {BG_IMAGE, BG_MATERIAL};

class DisplayDashboard : public Dashboard {
private:
  DisplayDashboard(ImageForm *imf, FL_OBJECT *grp);
  virtual ~DisplayDashboard();

  void buildmenu(Menu*, MenuAttribute);

  Enum<ImageBackground> background;
  static Enum<ImageBackground> background_dflt;
  static void CB_button_image(FL_OBJECT*, long);
  static void CB_button_material(FL_OBJECT*, long);
  static void background_cmd(const CharString&);
  void set_background();
  void show_image_gui();
  void show_image();
  void show_material_gui();
  void show_material();

  Enum<MaterialDisplayMethod> material_display_method;
  static Enum<MaterialDisplayMethod> material_display_dflt;
  static void CB_button_pixmaterial(FL_OBJECT*, long);
  static void CB_button_trimaterial(FL_OBJECT*, long);
  static void set_display_method_cmd(const CharString&);
  void set_display_method(Enum<MaterialDisplayMethod>);
  void set_display_method_gui(Enum<MaterialDisplayMethod>);
  void set_display_method_buttons();

  TrueFalse mesh_visible;
  static TrueFalse mesh_visible_dflt;
  static void CB_button_mesh(FL_OBJECT*, long);
  static void set_mesh_cmd(const CharString&);
  void set_mesh_gui();
  void set_mesh_visibility(const TrueFalse&);
  void set_mesh_visibility();
  

  int meshwidth;
  static int meshwidth_dflt;
  static void CB_input_meshwidth(FL_OBJECT*, long);
  static void meshwidth_cmd(const CharString&);
  void set_meshwidth_gui();
  void set_meshwidth(int);
  void set_meshwidth();

  Color meshcolor;
  static Color meshcolor_dflt;
  void set_meshcolor(const Color&);
  static void CB_button_meshcolor(FL_OBJECT*, long);
  static void set_meshcolor_cmd(const CharString&);
  void set_meshcolor_gui();
  void set_meshcolor();

  TrueFalse selected_pixels_visible;
  static TrueFalse selected_pixels_visible_dflt;
  static void CB_button_selpix(FL_OBJECT*, long);
  void set_selpixvisible_gui(FL_OBJECT*);
  static void set_selpixvis_cmd(const CharString&);
  void set_selpixvisible();

  Color selectedcolor;		// color of selected pixels
  static Color selectedcolor_dflt;
  void set_selectedcolor(const Color&);
  static void CB_button_pixcolor(FL_OBJECT*, long);
  static void set_selectedcolor_cmd(const CharString&);
  void set_selectedcolor_gui();
  void set_selectedcolor();

  TrueFalse firewalls_visible;
  static TrueFalse firewalls_visible_dflt;
  static void CB_button_firewalls(FL_OBJECT*, long);
  void set_firewalls_visible_gui(FL_OBJECT*);
  static void set_firewallsvis_cmd(const CharString&);
  void set_firewallsvisible();

  Color firewallcolor;
  static Color firewallcolor_dflt;
  void set_firewallcolor(const Color&);
  static void CB_button_firewallcolor(FL_OBJECT*, long);
  void set_firewallcolor_gui();
  static void set_firewallcolor_cmd(const CharString&);
  void set_firewallcolor();

  TrueFalse selected_mesh_visible;
  static TrueFalse selected_mesh_visible_dflt;
  static void CB_button_selectedmesh(FL_OBJECT*, long);
  void set_selectedmeshvis_gui(FL_OBJECT*);
  static void set_selectedmeshvis_cmd(const CharString&);
  void set_selectedmeshvis();

  Color selectedmeshcolor;
  Color selectedmeshfadecolor;
  static Color selectedmeshcolor_dflt;
  static void CB_button_selectedmeshcolor(FL_OBJECT*, long);
  void set_selectedmeshcolor_gui();
  static void set_selectedmeshcolor_cmd(const CharString&);
  void set_selectedmeshcolor(const Color &);
  void set_selectedmeshcolor();

  Color nomaterialcolor;
  static Color nomaterialcolor_dflt;
  static void CB_button_nomaterialcolor(FL_OBJECT*, long);
  void set_nomaterialcolor_gui();
  static void set_nomaterialcolor_cmd(const CharString&);
  void set_nomaterialcolor(const Color &);
  void set_nomaterialcolor();

  void activate_mesh_controls();
  void deactivate_mesh_controls();

  friend class ImageForm;
  friend class ImageFormGfx;
  friend class ImageFormTxt;
  friend FD_image_drawer *create_form_image_drawer();
};

#endif

