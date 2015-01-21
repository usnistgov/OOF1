// -*- C++ -*-
// $RCSfile: fdcode.C,v $
// $Revision: 1.5 $
// $Author: langer $
// $Date: 2000-09-06 20:11:31 $

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
#include "menuform.h"
#include "menuinterruptform.h"

void create_form_generic_menu(MenuForm *fdui, const char *prompt,
			      const char *helpstr)
{
  FL_OBJECT *obj;

  fdui->the_form = fl_bgn_form(FL_NO_BOX, 610, 440);
  fdui->background = obj = fl_add_box(FL_UP_BOX,0,0,610,440,"");
    fl_set_object_color(obj,FL_LEFT_BCOL,FL_LEFT_BCOL);
    fl_set_object_lsize(obj,20);
    fl_set_object_lstyle(obj,FL_TIMES_STYLE);
  fdui->inputregion = obj = fl_add_box(FL_FLAT_BOX,20,350,565,75,"");
    fl_set_object_color(obj,FL_INACTIVE,FL_COL1);
    fl_set_object_gravity(obj, FL_SouthWest, FL_SouthEast);
    fl_set_object_resize(obj, FL_RESIZE_X);
  fdui->MF_menu_name = obj = fl_add_text(FL_NORMAL_TEXT,20,10,280,40,prompt);
    fl_set_object_boxtype(obj,FL_NO_BOX);
    fl_set_object_color(obj,FL_TOP_BCOL,FL_COL1);
    fl_set_object_lcol(obj,FL_RED);
    fl_set_object_lsize(obj,FL_LARGE_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMESITALIC_STYLE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthEast);
    fl_set_object_resize(obj, FL_RESIZE_X);
  fdui->MF_return_to_home_menu = obj = fl_add_button(FL_NORMAL_BUTTON,235,15,50,40,"Home");
    fl_set_object_lsize(obj,11);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthEast, FL_NorthEast);
    fl_set_object_callback(obj,MFCB_go_home,(long) fdui);
  fdui->MF_back_up = obj = fl_add_button(FL_NORMAL_BUTTON,290,15,50,40,"Back");
    fl_set_object_lsize(obj,11);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthEast, FL_NorthEast);
    fl_set_object_callback(obj,MFCB_go_back,(long) fdui);
  fdui->MF_close_me = obj = fl_add_button(FL_NORMAL_BUTTON,345,15,50,40,"Close");
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthEast, FL_NorthEast);
    fl_set_object_callback(obj,MFCB_close_form,(long) fdui);
  fdui->MF_frozen = obj = fl_add_lightbutton(FL_PUSH_BUTTON,410,15,80,40,"Freeze");
    fl_set_object_color(obj,39,FL_YELLOW);
    fl_set_object_lsize(obj,11);
    fl_set_object_lstyle(obj,FL_TIMESBOLDITALIC_STYLE);
    fl_set_object_gravity(obj, FL_NorthEast, FL_NorthEast);
    fl_set_object_callback(obj,MFCB_freeze,(long) fdui);
  fdui->MF_help_message = obj = fl_add_text(FL_NORMAL_TEXT,20,60,570,30,helpstr);
    fl_set_object_boxtype(obj,FL_NO_BOX);
    fl_set_object_color(obj,66,66);
    fl_set_object_lsize(obj,11);
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthEast);
    fl_set_object_resize(obj, FL_RESIZE_X);
  fdui->MF_varhelp = obj = fl_add_text(FL_NORMAL_TEXT,30,395,550,24,"");
    fl_set_object_boxtype(obj,FL_NO_BOX);
    fl_set_object_color(obj,FL_COL1,FL_COL1);
    fl_set_object_lsize(obj,11);
    fl_set_object_lalign(obj,FL_ALIGN_RIGHT|FL_ALIGN_INSIDE);
    fl_set_object_gravity(obj, FL_SouthWest, FL_SouthEast);
    fl_set_object_resize(obj, FL_RESIZE_X);
  fdui->MF_submenu_list = obj = fl_add_browser(FL_SELECT_BROWSER,20,120,180,225,"Menus");
    fl_set_object_lcolor(obj,FL_BLUE);
    fl_set_object_lsize(obj,16);
    fl_set_object_lalign(obj,FL_ALIGN_TOP);
    fl_set_object_gravity(obj, FL_North, FL_South);
    fl_set_object_callback(obj,MFCB_submenuhandler,(long) fdui);
  fdui->MF_command_list = obj = fl_add_browser(FL_SELECT_BROWSER,212,120,180,225,"Functions");
    fl_set_object_lcolor(obj,FL_BLUE);
    fl_set_object_lsize(obj,16);
    fl_set_object_lalign(obj,FL_ALIGN_TOP);
    fl_set_object_gravity(obj, FL_North, FL_South);
    fl_set_object_callback(obj,MFCB_commandhandler,(long) fdui);
  fdui->MF_variable_list = obj = fl_add_browser(FL_HOLD_BROWSER,405,120,180,225,"Variables");
    fl_set_object_lcolor(obj,FL_BLUE);
    fl_set_object_lsize(obj,16);
    fl_set_object_lalign(obj,FL_ALIGN_TOP);
    fl_set_object_gravity(obj, FL_North, FL_South);
    fl_set_object_callback(obj,MFCB_variablehandler,(long) fdui);
  fdui->MF_record = obj = fl_add_lightbutton(FL_PUSH_BUTTON,495,15,85,40,"Record");
    fl_set_object_color(obj,39,FL_RED);
    fl_set_object_lsize(obj,11);
    fl_set_object_lstyle(obj,FL_TIMESBOLDITALIC_STYLE);
    fl_set_object_gravity(obj, FL_NorthEast, FL_NorthEast);
    fl_set_object_callback(obj,MFCB_record,(long) fdui);
  fdui->dummyinput = obj = fl_add_box(FL_NO_BOX,30,360,550,30,"");
  fl_end_form();

}
/*---------------------------------------*/

void create_form_commandMform(CommandMForm *fdui, const char *name)
{
  FL_OBJECT *obj;

  fdui->the_form = fl_bgn_form(FL_NO_BOX, 270, 190);
  fdui->background = obj = fl_add_box(FL_UP_BOX,0,0,270,190,"");
  fdui->CF_homebutton = obj = fl_add_button(FL_HIDDEN_BUTTON,30,30,40,20,"");
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
    fl_set_object_callback(obj,CFCB_homebutton,(long) fdui);
  fdui->CF_backbox = obj = fl_add_box(FL_DOWN_BOX,10,10,250,130,"");
    fl_set_object_color(obj,FL_RIGHT_BCOL,FL_COL1);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthEast);
    fl_set_object_resize(obj, FL_RESIZE_X);
  fdui->CF_dismiss = obj = fl_add_button(FL_NORMAL_BUTTON,20,65,110,30,"Dismiss");
    fl_set_object_lsize(obj,11);
    fl_set_object_gravity(obj, FL_NorthWest, FL_North);
    fl_set_object_resize(obj, FL_RESIZE_X);
    fl_set_object_callback(obj,CFCB_dismiss,(long) fdui);
  fdui->CF_helpbutton = obj = fl_add_button(FL_NORMAL_BUTTON,20,100,60,30,"?");
    fl_set_object_lcolor(obj,FL_RED);
    fl_set_object_lsize(obj,16);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_North);
    fl_set_object_resize(obj, FL_RESIZE_X);
    fl_set_object_callback(obj,CFCB_helpbutton,(long) fdui);
  fdui->CF_copy = obj = fl_add_button(FL_NORMAL_BUTTON,190,100,60,30,"Copy");
    fl_set_object_lcolor(obj,FL_DARKTOMATO);
    fl_set_object_lsize(obj,11);
    fl_set_object_gravity(obj, FL_North, FL_NorthEast);
    fl_set_object_resize(obj, FL_RESIZE_X);
    fl_set_object_callback(obj,CMCB_copy,(long) fdui);
  fdui->CF_freeze = obj = fl_add_lightbutton(FL_PUSH_BUTTON,135,65,114,30,"Freeze");
    fl_set_object_color(obj,39,FL_YELLOW);
    fl_set_object_lcolor(obj,FL_BLUE);
    fl_set_object_lsize(obj,11);
    fl_set_object_gravity(obj, FL_North, FL_NorthEast);
    fl_set_object_resize(obj, FL_RESIZE_X);
    fl_set_object_callback(obj,CFCB_freeze,(long) fdui);
  fdui->CF_default = obj = fl_add_button(FL_NORMAL_BUTTON,85,100,100,30,"Set Default");
    fl_set_object_lcolor(obj,FL_MAGENTA);
    fl_set_object_lsize(obj,11);
    fl_set_object_gravity(obj, FL_North, FL_North);
    fl_set_object_resize(obj, FL_RESIZE_X);
    fl_set_object_callback(obj,CMCB_default,(long) fdui);
  fdui->dummyarg = obj = fl_add_box(FL_NO_BOX,10,150,250,30,"");
  fdui->CF_CommandButton = obj = fl_add_button(FL_RETURN_BUTTON,20,20,230,40,name);
    fl_set_object_lcolor(obj,FL_RED);
    fl_set_object_lsize(obj,FL_MEDIUM_SIZE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthEast);
    fl_set_object_resize(obj, FL_RESIZE_X);
    fl_set_object_callback(obj,CFCB_CommandButton,(long) fdui);
  fl_end_form();
}
/*---------------------------------------*/

void create_form_command0form(CommandZeroForm *fdui, const char *name)
{
  FL_OBJECT *obj;
 
  fdui->the_form = fl_bgn_form(FL_NO_BOX, 270, 120);
  obj = fl_add_box(FL_UP_BOX,0,0,270,120,"");
  fdui->background = obj = fl_add_box(FL_UP_BOX,0,0,270,120,"");
  fdui->CF_homebutton = obj = fl_add_button(FL_HIDDEN_BUTTON,40,30,30,20,"");
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
    fl_set_object_callback(obj,CFCB_homebutton,(long) fdui);
  fdui->CF_backbox = obj = fl_add_box(FL_DOWN_BOX,10,10,250,100,"");
    fl_set_object_color(obj,FL_RIGHT_BCOL,FL_COL1);
  fdui->CF_CommandButton = obj = fl_add_button(FL_RETURN_BUTTON,20,20,230,40,
					       name);
    fl_set_object_lcol(obj,FL_RED);
    fl_set_object_lcolor(obj,FL_RED);
    fl_set_object_lsize(obj,FL_MEDIUM_SIZE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_callback(obj,CFCB_CommandButton,(long) fdui);
  fdui->CF_dismiss = obj = fl_add_button(FL_NORMAL_BUTTON,20,70,90,30,"Dismiss");
    fl_set_object_lsize(obj,11);
    fl_set_object_callback(obj,CFCB_dismiss,(long) fdui);
  fdui->CF_helpbutton = obj = fl_add_button(FL_NORMAL_BUTTON,120,70,40,30,"?");
    fl_set_object_lcolor(obj,FL_RED);
    fl_set_object_lsize(obj,16);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);
    fl_set_object_callback(obj,CFCB_helpbutton,(long) fdui);
  fdui->CF_freeze = obj = fl_add_lightbutton(FL_PUSH_BUTTON,170,70,80,30,"Freeze");
    fl_set_object_color(obj,39,FL_YELLOW);
    fl_set_object_lcolor(obj,FL_BLUE);
    fl_set_object_lsize(obj,11);
    fl_set_object_callback(obj,CFCB_freeze,(long) fdui);
  fl_end_form();
}
/*---------------------------------------*/

void create_form_messageform(MessageForm *fdui)
{
  FL_OBJECT *obj;

  fdui->the_form = fl_bgn_form(FL_NO_BOX, 700, 280);
  fdui->background = obj = fl_add_box(FL_UP_BOX,0,0,700,280,"");
  fdui->MG_text = obj = fl_add_browser(FL_NORMAL_BROWSER,10,10,680,220,"");
    fl_set_object_color(obj,FL_TOP_BCOL,FL_YELLOW);
    fl_set_object_gravity(obj, FL_NorthWest, FL_SouthEast);
    fl_set_object_callback(obj,MGCB_text,(long) fdui);
  fdui->MG_close = obj = fl_add_button(FL_NORMAL_BUTTON,10,240,62,32,"Close");
    fl_set_object_color(obj,FL_TOP_BCOL,FL_COL1);
    fl_set_object_lsize(obj,11);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);
    fl_set_object_gravity(obj, FL_SouthWest, FL_SouthWest);
    fl_set_object_callback(obj,MGCB_close,(long) fdui);
  fdui->MG_clear = obj = fl_add_button(FL_NORMAL_BUTTON,80,240,62,32,"Clear");
    fl_set_object_color(obj,FL_TOP_BCOL,FL_COL1);
    fl_set_object_lsize(obj,11);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);
    fl_set_object_gravity(obj, FL_SouthWest, FL_SouthWest);
    fl_set_object_callback(obj,MGCB_clear,(long) fdui);
  fdui->MG_save = obj = fl_add_button(FL_NORMAL_BUTTON,150,240,62,32,"Save");
    fl_set_object_color(obj,FL_TOP_BCOL,FL_COL1);
    fl_set_object_lsize(obj,11);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);
    fl_set_object_gravity(obj, FL_SouthWest, FL_SouthWest);
    fl_set_object_callback(obj,MGCB_save,(long) fdui);
  fdui->MG_font = obj = fl_add_menu(FL_PUSH_MENU,450,240,90,30,"Font");
    fl_set_object_boxtype(obj,FL_SHADOW_BOX);
    fl_set_object_color(obj,FL_LEFT_BCOL,FL_RIGHT_BCOL);
    fl_set_object_gravity(obj, FL_SouthEast, FL_SouthEast);
    fl_set_object_callback(obj,MGCB_font,(long) fdui);
  fdui->MG_size = obj = fl_add_counter(FL_SIMPLE_COUNTER,580,240,100,30,"Size");
    fl_set_object_boxtype(obj,FL_SHADOW_BOX);
    fl_set_object_color(obj,FL_LEFT_BCOL,FL_BOTTOM_BCOL);
    fl_set_object_lalign(obj,FL_ALIGN_LEFT);
    fl_set_object_gravity(obj, FL_SouthEast, FL_SouthEast);
    fl_set_object_callback(obj,MGCB_size,(long) fdui);
  fdui->MG_home = obj = fl_add_button(FL_HIDDEN_BUTTON,10,10,30,30,"");
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
    fl_set_object_callback(obj,MGCB_home,(long) fdui);
  fl_end_form();
}
/*---------------------------------------*/

void create_form_stringform(StringForm *fdui)
{
  FL_OBJECT *obj;

  fdui->the_form = fl_bgn_form(FL_NO_BOX, 500, 130);
  fdui->background = obj = fl_add_box(FL_UP_BOX,0,0,500,130,"");
  fdui->SF_prompt = obj = fl_add_text(FL_NORMAL_TEXT,10,10,410,50,"Text");
    fl_set_object_color(obj,FL_SLATEBLUE,FL_COL1);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthEast);
    fl_set_object_resize(obj, FL_RESIZE_X);
  fdui->SF_input = obj = fl_add_input(FL_MULTILINE_INPUT,10,70,480,50,"");
    fl_set_object_color(obj,FL_MCOL,FL_MCOL);
    fl_set_object_lsize(obj,11);
  fdui->SF_okbutton = obj = fl_add_button(FL_RETURN_BUTTON,430,10,60,50,"");
    fl_set_object_gravity(obj, FL_NorthEast, FL_NorthEast);
    fl_set_object_callback(obj,SFCB_okbutton,(long) fdui);
  fl_end_form();
}
/*---------------------------------------*/

void create_form_interruptform(InterruptForm *fdui)
{
  FL_OBJECT *obj;

  fdui->the_form = fl_bgn_form(FL_NO_BOX, 160, 160);
  fdui->background = obj = fl_add_box(FL_UP_BOX,0,0,160,160,"");
  fdui->IF_stop = obj = fl_add_button(FL_NORMAL_BUTTON,10,10,140,140,"STOP");
    fl_set_object_color(obj,FL_RED,FL_YELLOW);
    fl_set_object_lsize(obj,FL_HUGE_SIZE);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);
    fl_set_object_callback(obj,IFCB_stop,(long) fdui);
  fl_end_form();
}
/*---------------------------------------*/

void create_form_questionform(QuestionForm *fdui)
{
  FL_OBJECT *obj;
  fdui->the_form = fl_bgn_form(FL_NO_BOX, 500, 130);
  fdui->background = obj = fl_add_box(FL_UP_BOX,0,0,500,130,"");
  fdui->question_text = obj = fl_add_text(FL_NORMAL_TEXT,10,20,480,60,"Text");
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);
  fdui->yes_button = obj = fl_add_button(FL_NORMAL_BUTTON,10,90,70,30,"Yes");
    fl_set_button_shortcut(obj,"^M",1);
    fl_set_object_lcolor(obj,FL_BLUE);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);
    fl_set_object_gravity(obj, FL_SouthWest, FL_SouthWest);
    fl_set_object_callback(obj,QFCB_yesbutton,(long) fdui);
  fdui->no_button = obj = fl_add_button(FL_NORMAL_BUTTON,90,90,70,30,"No");
    fl_set_object_lcolor(obj,FL_RED);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);
    fl_set_object_gravity(obj, FL_SouthWest, FL_SouthWest);
    fl_set_object_callback(obj,QFCB_nobutton,(long) fdui);
  fl_end_form();
}
/*---------------------------------------*/

void create_form_pauseform(PauseForm *fdui)
{
  FL_OBJECT *obj;
  fdui->the_form = fl_bgn_form(FL_NO_BOX, 150, 80);
  obj = fl_add_box(FL_UP_BOX,0,0,150,80,"");
  fdui->button_continue = obj = fl_add_button(FL_RETURN_BUTTON,10,10,130,60,"CONTINUE");
    fl_set_object_color(obj,FL_SPRINGGREEN,FL_COL1);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);
  fl_end_form();
}
/*---------------------------------------*/
