// -*- C++ -*-
// $RCSfile: colorbrowser.C,v $
// $Revision: 1.3 $
// $Author: langer $
// $Date: 2000-09-06 20:10:48 $

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */

/* Form definition file generated with fdesign. */

#include "forms.h"
#include "stdlib.h"
#include "colorutils.h"

void create_form_colorbrowser(ColorBrowser *fdui)
{
  FL_OBJECT *obj;
  fdui->the_form = fl_bgn_form(FL_NO_BOX, 140, 330);
  fdui->background = obj = fl_add_box(FL_UP_BOX,0,0,140,330,"");
  fdui->slider_red = obj = fl_add_valslider(FL_VERT_FILL_SLIDER,20,20,30,170,"");
    fl_set_object_color(obj,FL_COL1,FL_RED);
    fl_set_object_lsize(obj,FL_DEFAULT_SIZE);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_SouthWest);
    fl_set_object_resize(obj, FL_RESIZE_Y);
    fl_set_object_callback(obj,ColorBrowser::CB_slider,(long) fdui);
     fl_set_slider_return(obj, FL_RETURN_CHANGED);
  fdui->slider_grn = obj = fl_add_valslider(FL_VERT_FILL_SLIDER,55,20,30,170,"");
    fl_set_object_color(obj,FL_COL1,FL_GREEN);
    fl_set_object_lsize(obj,FL_DEFAULT_SIZE);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_SouthWest);
    fl_set_object_resize(obj, FL_RESIZE_Y);
    fl_set_object_callback(obj,ColorBrowser::CB_slider,(long) fdui);
     fl_set_slider_return(obj, FL_RETURN_CHANGED);
  fdui->slider_blu = obj = fl_add_valslider(FL_VERT_FILL_SLIDER,90,20,30,170,"");
    fl_set_object_color(obj,FL_COL1,FL_BLUE);
    fl_set_object_lsize(obj,FL_DEFAULT_SIZE);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_SouthWest);
    fl_set_object_resize(obj, FL_RESIZE_Y);
    fl_set_object_callback(obj,ColorBrowser::CB_slider,(long) fdui);
     fl_set_slider_return(obj, FL_RETURN_CHANGED);
  fdui->box_new = obj = fl_add_box(FL_FRAME_BOX,20,195,100,40,"");
    fl_set_object_gravity(obj, FL_SouthWest, FL_SouthWest);
  fdui->box_old = obj = fl_add_box(FL_FRAME_BOX,20,240,100,40,"");
    fl_set_object_gravity(obj, FL_SouthWest, FL_SouthWest);
  fdui->button_ok = obj = fl_add_button(FL_NORMAL_BUTTON,20,290,45,30,"OK");
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);
    fl_set_object_gravity(obj, FL_SouthWest, FL_SouthWest);
    fl_set_object_callback(obj,ColorBrowser::CB_ok,(long) fdui);
  fdui->button_cancel = obj = fl_add_button(FL_NORMAL_BUTTON,70,290,45,30,"Nope");
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);
    fl_set_object_gravity(obj, FL_SouthWest, FL_SouthWest);
    fl_set_object_callback(obj,ColorBrowser::CB_cancel,(long) fdui);
  fl_end_form();

}
/*---------------------------------------*/

