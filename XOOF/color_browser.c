/* Form definition file generated with fdesign. */

#include "forms.h"
#include <stdlib.h>
#include "colorbrowser.h"

FD_colorbrowser *create_form_colorbrowser(void)
{
  FL_OBJECT *obj;
  FD_colorbrowser *fdui = (FD_colorbrowser *) fl_calloc(1, sizeof(*fdui));

  fdui->colorbrowser = fl_bgn_form(FL_NO_BOX, 140, 330);
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

  return fdui;
}
/*---------------------------------------*/

