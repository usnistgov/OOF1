/* Form definition file generated with fdesign. */

#include "forms.h"
#include <stdlib.h>
#include "form_drawer.h"

FD_form_drawer *create_form_form_drawer(void)
{
  FL_OBJECT *obj;
  FD_form_drawer *fdui = (FD_form_drawer *) fl_calloc(1, sizeof(*fdui));

  fdui->form_drawer = fl_bgn_form(FL_NO_BOX, 760, 530);
  fdui->background = obj = fl_add_box(FL_UP_BOX,0,0,760,530,"");
  fdui->homebutton = obj = fl_add_button(FL_HIDDEN_BUTTON,25,15,35,30,"");
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
    fl_set_object_callback(obj,FDCB_home,(long) fdui);
  fdui->type_selector = obj = fl_add_choice(FL_NORMAL_CHOICE,14,10,220,40,"");
    fl_set_object_boxtype(obj,FL_DOWN_BOX);
    fl_set_object_lsize(obj,FL_MEDIUM_SIZE);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
    fl_set_object_callback(obj,FDCB_set_current_drawer,(long) fdui);
  fdui->hold_button = obj = fl_add_lightbutton(FL_PUSH_BUTTON,134,60,100,30,"Hold");
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
    fl_set_object_callback(obj,FDCB_set_hold_image,(long) fdui);
  fdui->current_dashboard_menu = obj = fl_add_choice(FL_NORMAL_CHOICE,15,60,111,30,"");
    fl_set_object_boxtype(obj,FL_DOWN_BOX);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
    fl_set_object_callback(obj,FDCB_set_current_dashboard,(long) fdui);
  fdui->closebutton = obj = fl_add_button(FL_NORMAL_BUTTON,9,495,108,27,"Close");
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);
    fl_set_object_gravity(obj, FL_SouthWest, FL_SouthWest);
    fl_set_object_callback(obj,FDCB_close,(long) fdui);
  fdui->savebutton = obj = fl_add_button(FL_NORMAL_BUTTON,126,495,108,27,"Save");
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);
    fl_set_object_gravity(obj, FL_SouthWest, FL_SouthWest);
    fl_set_object_callback(obj,FDCB_save,(long) fdui);
  fdui->dashboard_area = obj = fl_add_box(FL_FLAT_BOX,10,100,222,390,"");
    fl_set_object_color(obj,FL_SLATEBLUE,FL_COL1);
    fl_set_object_gravity(obj, FL_NorthWest, FL_SouthWest);
    fl_set_object_resize(obj, FL_RESIZE_Y);
  fdui->image_region = obj = fl_add_box(FL_FLAT_BOX,240,10,510,510,"");
    fl_set_object_color(obj,FL_TOP_BCOL,FL_COL1);
    fl_set_object_gravity(obj, FL_NorthWest, FL_SouthEast);
  fl_end_form();

  fdui->form_drawer->fdui = fdui;

  return fdui;
}
/*---------------------------------------*/

FD_select_attributes *create_form_select_attributes(void)
{
  FL_OBJECT *obj;
  FD_select_attributes *fdui = (FD_select_attributes *) fl_calloc(1, sizeof(*fdui));

  fdui->select_attributes = fl_bgn_form(FL_NO_BOX, 750, 520);
  obj = fl_add_box(FL_UP_BOX,0,0,750,520,"");
  fdui->dontincludethisobject = obj = fl_add_box(FL_FLAT_BOX,12,96,222,390,"");
    fl_set_object_color(obj,FL_SLATEBLUE,FL_COL1);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);

  fdui->select_attibutes = fl_bgn_group();
  fdui->selecttext = obj = fl_add_frame(FL_BORDER_FRAME,30,330,180,140,"    Click to Select...");
    fl_set_object_color(obj,FL_BLACK,FL_BLACK);
    fl_set_object_lalign(obj,FL_ALIGN_TOP_LEFT);
    fl_set_object_lstyle(obj,FL_BOLDITALIC_STYLE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
  fdui->edges_button = obj = fl_add_checkbutton(FL_PUSH_BUTTON,40,118,30,30,"Edges");
    fl_set_object_color(obj,FL_COL1,FL_GREEN);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
    fl_set_object_callback(obj,FDCB_edges,(long) fdui);
  fdui->empties_button = obj = fl_add_checkbutton(FL_PUSH_BUTTON,40,184,30,30,"Empty Elements");
    fl_set_object_color(obj,FL_COL1,FL_GREEN);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
    fl_set_object_callback(obj,FDCB_empties,(long) fdui);
  fdui->hot_elements_button = obj = fl_add_checkbutton(FL_PUSH_BUTTON,40,217,30,30,"Hot Elements");
    fl_set_object_color(obj,FL_COL1,FL_GREEN);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
    fl_set_object_callback(obj,FDCB_hot_elements,(long) fdui);
  fdui->hot_nodes_button = obj = fl_add_checkbutton(FL_PUSH_BUTTON,40,251,30,30,"Hot Nodes");
    fl_set_object_color(obj,FL_COL1,FL_GREEN);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
    fl_set_object_callback(obj,FDCB_hot_nodes,(long) fdui);
  fdui->elements_button = obj = fl_add_checkbutton(FL_PUSH_BUTTON,40,150,30,30,"Elements");
    fl_set_object_color(obj,FL_COL1,FL_GREEN);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
    fl_set_object_callback(obj,FDCB_elements,(long) fdui);
  fdui->edgewidth_input = obj = fl_add_input(FL_INT_INPUT,170,118,40,30,"width=");
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
    fl_set_object_callback(obj,FDCB_edgewidth,(long) fdui);
  fdui->select_elements_button = obj = fl_add_roundbutton(FL_RADIO_BUTTON,36,355,30,30,"Elements");
    fl_set_object_color(obj,FL_BOTTOM_BCOL,FL_YELLOW);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
    fl_set_object_callback(obj,FDCB_set_selection,(long) fdui);
  fdui->select_nodes_button = obj = fl_add_roundbutton(FL_RADIO_BUTTON,36,387,30,30,"Nodes");
    fl_set_object_color(obj,FL_BOTTOM_BCOL,FL_YELLOW);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
    fl_set_object_callback(obj,FDCB_set_selection,(long) fdui);
  fdui->noderadius_input = obj = fl_add_input(FL_INT_INPUT,160,387,40,30,"radius=");
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
    fl_set_object_callback(obj,FDCB_noderadius,(long) fdui);
  fdui->select_nobody_button = obj = fl_add_roundbutton(FL_RADIO_BUTTON,36,420,30,30,"Neither");
    fl_set_object_color(obj,FL_BOTTOM_BCOL,FL_YELLOW);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
    fl_set_object_callback(obj,FDCB_set_selection,(long) fdui);
  fl_end_group();

  fl_end_form();

  fdui->select_attributes->fdui = fdui;

  return fdui;
}
/*---------------------------------------*/

FD_coordinates *create_form_coordinates(void)
{
  FL_OBJECT *obj;
  FD_coordinates *fdui = (FD_coordinates *) fl_calloc(1, sizeof(*fdui));

  fdui->coordinates = fl_bgn_form(FL_NO_BOX, 750, 520);
  obj = fl_add_box(FL_UP_BOX,0,0,750,520,"");
  fdui->dontincludethisobject = obj = fl_add_box(FL_FLAT_BOX,12,96,222,390,"");
    fl_set_object_color(obj,FL_SLATEBLUE,FL_TOMATO);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);

  fdui->coordinates = fl_bgn_group();
  fdui->zoomfactor_input = obj = fl_add_input(FL_FLOAT_INPUT,120,215,100,30,"Zoom Factor = ");
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
    fl_set_object_callback(obj,FDCB_zoomfactor,(long) fdui);
  fdui->center_button = obj = fl_add_button(FL_NORMAL_BUTTON,25,260,125,30,"Show Entire Image");
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
    fl_set_object_callback(obj,FDCB_center,(long) fdui);
  fdui->redraw_button = obj = fl_add_button(FL_NORMAL_BUTTON,160,260,60,30,"Redraw");
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
    fl_set_object_callback(obj,FDCB_redraw,(long) fdui);
  fdui->xmin_text = obj = fl_add_text(FL_NORMAL_TEXT,20,330,204,27,"xmin");
    fl_set_object_color(obj,FL_SLATEBLUE,FL_MCOL);
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
  fdui->xmax_text = obj = fl_add_text(FL_NORMAL_TEXT,20,357,204,27,"xmax");
    fl_set_object_color(obj,FL_SLATEBLUE,FL_MCOL);
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
  fdui->ymin_text = obj = fl_add_text(FL_NORMAL_TEXT,20,390,204,27,"ymin");
    fl_set_object_color(obj,FL_SLATEBLUE,FL_MCOL);
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
  fdui->ymax_text = obj = fl_add_text(FL_NORMAL_TEXT,20,417,204,27,"ymax");
    fl_set_object_color(obj,FL_SLATEBLUE,FL_MCOL);
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
  fdui->cliptext = obj = fl_add_text(FL_NORMAL_TEXT,20,302,189,26,"Visible Region");
    fl_set_object_color(obj,FL_SLATEBLUE,FL_MCOL);
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_BOLDITALIC_STYLE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
  fdui->zoomtext2 = obj = fl_add_text(FL_NORMAL_TEXT,19,161,210,20,"Middle mouse drags image");
    fl_set_object_color(obj,FL_SLATEBLUE,FL_MCOL);
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_BOLDITALIC_STYLE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
  fdui->zoomtext3 = obj = fl_add_text(FL_NORMAL_TEXT,19,183,210,20,"Right  mouse zooms in");
    fl_set_object_color(obj,FL_SLATEBLUE,FL_MCOL);
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_BOLDITALIC_STYLE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
  fdui->zoomtext1 = obj = fl_add_text(FL_NORMAL_TEXT,20,140,210,20,"Left mouse zooms out");
    fl_set_object_color(obj,FL_SLATEBLUE,FL_MCOL);
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_BOLDITALIC_STYLE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
  fdui->zoomtext0 = obj = fl_add_text(FL_NORMAL_TEXT,20,110,210,20,"Click on the image...");
    fl_set_object_color(obj,FL_SLATEBLUE,FL_MCOL);
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_BOLDITALIC_STYLE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
  fl_end_group();

  fl_end_form();

  fdui->coordinates->fdui = fdui;

  return fdui;
}
/*---------------------------------------*/

FD_color_control *create_form_color_control(void)
{
  FL_OBJECT *obj;
  FD_color_control *fdui = (FD_color_control *) fl_calloc(1, sizeof(*fdui));

  fdui->color_control = fl_bgn_form(FL_NO_BOX, 750, 520);
  obj = fl_add_box(FL_UP_BOX,0,0,750,520,"");
  fdui->dontincludethisobject = obj = fl_add_box(FL_FLAT_BOX,12,96,222,390,"");
    fl_set_object_color(obj,FL_SLATEBLUE,FL_COL1);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);

  fdui->color_control = fl_bgn_group();
  fdui->max_input = obj = fl_add_input(FL_FLOAT_INPUT,90,120,130,30,"");
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
    fl_set_object_callback(obj,FDCB_color_max,(long) fdui);
  fdui->min_input = obj = fl_add_input(FL_FLOAT_INPUT,90,430,130,30,"");
    fl_set_object_gravity(obj, FL_SouthWest, FL_SouthWest);
    fl_set_object_callback(obj,FDCB_color_min,(long) fdui);
  fdui->color_map_menu = obj = fl_add_choice(FL_NORMAL_CHOICE,90,210,120,30,"Color Map");
    fl_set_object_boxtype(obj,FL_DOWN_BOX);
    fl_set_object_lalign(obj,FL_ALIGN_TOP);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
    fl_set_object_callback(obj,FDCB_color_map_menu,(long) fdui);
  fdui->valmin_text = obj = fl_add_text(FL_NORMAL_TEXT,90,350,140,30,"min=");
    fl_set_object_color(obj,FL_SLATEBLUE,FL_MCOL);
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
  fdui->valmax_text = obj = fl_add_text(FL_NORMAL_TEXT,90,320,140,30,"max=");
    fl_set_object_color(obj,FL_SLATEBLUE,FL_MCOL);
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
  fdui->color_fullscale = obj = fl_add_checkbutton(FL_PUSH_BUTTON,90,380,30,30,"Full Scale");
    fl_set_object_color(obj,FL_COL1,FL_GREEN);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
    fl_set_object_callback(obj,FDCB_color_fullscale,(long) fdui);
  fdui->colormapsize_input = obj = fl_add_input(FL_NORMAL_INPUT,120,250,40,30,"size");
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
    fl_set_object_callback(obj,FDCB_colormapsize,(long) fdui);
  fdui->color_flip_button = obj = fl_add_lightbutton(FL_PUSH_BUTTON,165,250,60,30,"Flip");
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
    fl_set_object_callback(obj,FDCB_color_flip,(long) fdui);
  fdui->colorbar = obj = fl_add_canvas(FL_NORMAL_CANVAS,30,120,50,340,"");
    fl_set_object_gravity(obj, FL_NorthWest, FL_SouthWest);
    fl_set_object_resize(obj, FL_RESIZE_Y);
  fl_end_group();

  fl_end_form();

  fdui->color_control->fdui = fdui;

  return fdui;
}
/*---------------------------------------*/

FD_simple_attributes *create_form_simple_attributes(void)
{
  FL_OBJECT *obj;
  FD_simple_attributes *fdui = (FD_simple_attributes *) fl_calloc(1, sizeof(*fdui));

  fdui->simple_attributes = fl_bgn_form(FL_NO_BOX, 750, 520);
  obj = fl_add_box(FL_UP_BOX,0,0,750,520,"");
  fdui->dontincludethisobject = obj = fl_add_box(FL_FLAT_BOX,12,96,222,390,"");
    fl_set_object_color(obj,FL_SLATEBLUE,FL_COL1);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);

  fdui->simple_attributes = fl_bgn_group();
  fdui->edges_button = obj = fl_add_checkbutton(FL_PUSH_BUTTON,40,158,30,30,"Edges");
    fl_set_object_color(obj,FL_COL1,FL_GREEN);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
    fl_set_object_callback(obj,FDCB_edges,(long) fdui);
  fdui->empties_button = obj = fl_add_checkbutton(FL_PUSH_BUTTON,40,224,30,30,"Empty Elements");
    fl_set_object_color(obj,FL_COL1,FL_GREEN);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
    fl_set_object_callback(obj,FDCB_empties,(long) fdui);
  fdui->elements_button = obj = fl_add_checkbutton(FL_PUSH_BUTTON,40,190,30,30,"Elements");
    fl_set_object_color(obj,FL_COL1,FL_GREEN);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
    fl_set_object_callback(obj,FDCB_elements,(long) fdui);
  fdui->edgewidth_input = obj = fl_add_input(FL_INT_INPUT,170,158,40,30,"width=");
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
    fl_set_object_callback(obj,FDCB_edgewidth,(long) fdui);
  fl_end_group();

  fl_end_form();

  fdui->simple_attributes->fdui = fdui;

  return fdui;
}
/*---------------------------------------*/

FD_elementinfo *create_form_elementinfo(void)
{
  FL_OBJECT *obj;
  FD_elementinfo *fdui = (FD_elementinfo *) fl_calloc(1, sizeof(*fdui));

  fdui->elementinfo = fl_bgn_form(FL_NO_BOX, 755, 525);
  obj = fl_add_box(FL_UP_BOX,0,0,755,525,"");
  fdui->dontincludethisobject = obj = fl_add_box(FL_FLAT_BOX,10,95,222,390,"");
    fl_set_object_color(obj,FL_SLATEBLUE,FL_COL1);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);

  fdui->elementinfo = fl_bgn_group();
  fdui->infoframe1 = obj = fl_add_frame(FL_BORDER_FRAME,30,235,170,120,"");
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
  fdui->info_mouseposition = obj = fl_add_checkbutton(FL_PUSH_BUTTON,30,140,30,30,"mouse");
    fl_set_object_color(obj,FL_COL1,FL_GREEN);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
    fl_set_object_callback(obj,FDCB_info_mouseposition,(long) fdui);
  fdui->info_elementindex = obj = fl_add_checkbutton(FL_PUSH_BUTTON,120,170,30,30,"index");
    fl_set_object_color(obj,FL_COL1,FL_GREEN);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
    fl_set_object_callback(obj,FDCB_info_elementindex,(long) fdui);
  fdui->info_nodeindices = obj = fl_add_checkbutton(FL_PUSH_BUTTON,30,200,30,30,"nodes");
    fl_set_object_color(obj,FL_COL1,FL_GREEN);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
    fl_set_object_callback(obj,FDCB_info_nodeindices,(long) fdui);
  fdui->info_intrinsicgray = obj = fl_add_checkbutton(FL_PUSH_BUTTON,120,140,30,30,"gray");
    fl_set_object_color(obj,FL_COL1,FL_GREEN);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
    fl_set_object_callback(obj,FDCB_info_intrinsicgray,(long) fdui);
  fdui->info_elementparams = obj = fl_add_checkbutton(FL_PUSH_BUTTON,120,200,30,30,"params");
    fl_set_object_color(obj,FL_COL1,FL_GREEN);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
    fl_set_object_callback(obj,FDCB_info_elementparams,(long) fdui);
  fdui->info_stress = obj = fl_add_checkbutton(FL_PUSH_BUTTON,30,235,30,30,"stress");
    fl_set_object_color(obj,FL_COL1,FL_GREEN);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
    fl_set_object_callback(obj,FDCB_info_stress,(long) fdui);
  fdui->info_strain = obj = fl_add_checkbutton(FL_PUSH_BUTTON,120,235,30,30,"strain");
    fl_set_object_color(obj,FL_COL1,FL_GREEN);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
    fl_set_object_callback(obj,FDCB_info_strain,(long) fdui);
  fdui->info_components = obj = fl_add_checkbutton(FL_PUSH_BUTTON,50,265,20,20,"components");
    fl_set_object_color(obj,FL_COL1,FL_RED);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
    fl_set_object_callback(obj,FDCB_info_components,(long) fdui);
  fdui->info_invariants = obj = fl_add_checkbutton(FL_PUSH_BUTTON,50,285,20,20,"invariants");
    fl_set_object_color(obj,FL_COL1,FL_RED);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
    fl_set_object_callback(obj,FDCB_info_invariants,(long) fdui);
  fdui->info_eigenvalues = obj = fl_add_checkbutton(FL_PUSH_BUTTON,50,305,20,20,"eigenvalues");
    fl_set_object_color(obj,FL_COL1,FL_RED);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
    fl_set_object_callback(obj,FDCB_info_eigenvalues,(long) fdui);
  fdui->elementinfotext = obj = fl_add_text(FL_NORMAL_TEXT,22,110,198,30,"Click on an element for info");
    fl_set_object_color(obj,FL_SLATEBLUE,FL_MCOL);
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_BOLDITALIC_STYLE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
  fdui->info_energydensity = obj = fl_add_checkbutton(FL_PUSH_BUTTON,30,325,30,30,"energy density");
    fl_set_object_color(obj,FL_COL1,FL_GREEN);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
    fl_set_object_callback(obj,FDCB_info_energydensity,(long) fdui);
  fdui->info_elementtype = obj = fl_add_checkbutton(FL_PUSH_BUTTON,30,170,30,30,"type");
    fl_set_object_color(obj,FL_COL1,FL_GREEN);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
    fl_set_object_callback(obj,FDCB_info_elementtype,(long) fdui);
  fl_end_group();

  obj = fl_add_frame(FL_BORDER_FRAME,30,375,170,90,"");
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
  fdui->info_heatflux = obj = fl_add_checkbutton(FL_PUSH_BUTTON,30,435,30,30,"heat flux");
    fl_set_object_color(obj,FL_COL1,FL_GREEN);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
    fl_set_object_callback(obj,FDCB_info_heatflux,(long) fdui);
  fdui->info_tempgradient = obj = fl_add_checkbutton(FL_PUSH_BUTTON,30,405,30,30,"T gradient");
    fl_set_object_color(obj,FL_COL1,FL_GREEN);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
    fl_set_object_callback(obj,FDCB_info_tempgradient,(long) fdui);
  fdui->info_temperature = obj = fl_add_checkbutton(FL_PUSH_BUTTON,30,375,30,30,"temperature");
    fl_set_object_color(obj,FL_COL1,FL_GREEN);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
    fl_set_object_callback(obj,FDCB_info_temperature,(long) fdui);
  fl_end_form();

  fdui->elementinfo->fdui = fdui;

  return fdui;
}
/*---------------------------------------*/

FD_nodeinfo *create_form_nodeinfo(void)
{
  FL_OBJECT *obj;
  FD_nodeinfo *fdui = (FD_nodeinfo *) fl_calloc(1, sizeof(*fdui));

  fdui->nodeinfo = fl_bgn_form(FL_NO_BOX, 755, 525);
  obj = fl_add_box(FL_UP_BOX,0,0,755,525,"");
  fdui->dontincludethisobject = obj = fl_add_box(FL_FLAT_BOX,12,96,222,390,"");
    fl_set_object_color(obj,FL_SLATEBLUE,FL_COL1);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);

  fdui->nodeinfo = fl_bgn_group();
  fdui->nodeinfotext = obj = fl_add_text(FL_NORMAL_TEXT,30,130,190,30,"Click on a node for info");
    fl_set_object_color(obj,FL_SLATEBLUE,FL_MCOL);
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_BOLDITALIC_STYLE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
  fdui->info_mouseposition = obj = fl_add_checkbutton(FL_PUSH_BUTTON,30,170,30,30,"mouse position");
    fl_set_object_color(obj,FL_COL1,FL_GREEN);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
    fl_set_object_callback(obj,FDCB_info_mouseposition,(long) fdui);
  fdui->info_nodetype = obj = fl_add_checkbutton(FL_PUSH_BUTTON,30,210,30,30,"node type");
    fl_set_object_color(obj,FL_COL1,FL_GREEN);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
    fl_set_object_callback(obj,FDCB_info_nodetype,(long) fdui);
  fdui->info_nodeindex = obj = fl_add_checkbutton(FL_PUSH_BUTTON,30,240,30,30,"node index");
    fl_set_object_color(obj,FL_COL1,FL_GREEN);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
    fl_set_object_callback(obj,FDCB_info_nodeindex,(long) fdui);
  fdui->info_nodeposition = obj = fl_add_checkbutton(FL_PUSH_BUTTON,30,290,30,30,"current position");
    fl_set_object_color(obj,FL_COL1,FL_GREEN);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
    fl_set_object_callback(obj,FDCB_info_nodeposition,(long) fdui);
  fdui->info_nodedisplacement = obj = fl_add_checkbutton(FL_PUSH_BUTTON,30,320,30,30,"displacement");
    fl_set_object_color(obj,FL_COL1,FL_GREEN);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
    fl_set_object_callback(obj,FDCB_info_nodedisplacement,(long) fdui);
  fdui->info_nodeoriginalpos = obj = fl_add_checkbutton(FL_PUSH_BUTTON,30,350,30,30,"original position");
    fl_set_object_color(obj,FL_COL1,FL_GREEN);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
    fl_set_object_callback(obj,FDCB_info_nodeoriginalpos,(long) fdui);
  fdui->info_nodeforce = obj = fl_add_checkbutton(FL_PUSH_BUTTON,30,400,30,30,"force");
    fl_set_object_color(obj,FL_COL1,FL_GREEN);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
    fl_set_object_callback(obj,FDCB_info_nodeforce,(long) fdui);
  fl_end_group();

  fdui->info_nodetemperature = obj = fl_add_checkbutton(FL_PUSH_BUTTON,30,435,30,30,"temperature");
    fl_set_object_color(obj,FL_COL1,FL_GREEN);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
    fl_set_object_callback(obj,FDCB_info_nodetemperature,(long) fdui);
  fl_end_form();

  fdui->nodeinfo->fdui = fdui;

  return fdui;
}
/*---------------------------------------*/

FD_quitform *create_form_quitform(void)
{
  FL_OBJECT *obj;
  FD_quitform *fdui = (FD_quitform *) fl_calloc(1, sizeof(*fdui));

  fdui->quitform = fl_bgn_form(FL_NO_BOX, 310, 130);
  fdui->background = obj = fl_add_box(FL_UP_BOX,0,0,310,130,"");
  fdui->text = obj = fl_add_text(FL_NORMAL_TEXT,50,20,220,40,"Save session log before quitting?");
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
  fdui->button_save = obj = fl_add_button(FL_RETURN_BUTTON,10,70,90,30,"Save...");
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
  fdui->button_cancel = obj = fl_add_button(FL_NORMAL_BUTTON,210,70,90,30,"Cancel");
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
  fdui->button_quitnow = obj = fl_add_button(FL_NORMAL_BUTTON,110,70,90,30,"Don't Save");
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
  fl_end_form();

  fdui->quitform->fdui = fdui;

  return fdui;
}
/*---------------------------------------*/

FD_plot *create_form_plot(void)
{
  FL_OBJECT *obj;
  FD_plot *fdui = (FD_plot *) fl_calloc(1, sizeof(*fdui));

  fdui->plot = fl_bgn_form(FL_NO_BOX, 750, 520);
  obj = fl_add_box(FL_UP_BOX,0,0,750,520,"");
  fdui->dontincludethisobject = obj = fl_add_box(FL_FLAT_BOX,12,96,222,390,"");
    fl_set_object_color(obj,FL_SLATEBLUE,FL_COL1);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
  obj = fl_add_text(FL_NORMAL_TEXT,19,146,40,30,"From:");
    fl_set_object_color(obj,FL_SLATEBLUE,FL_MCOL);
    fl_set_object_lalign(obj,FL_ALIGN_RIGHT|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
  obj = fl_add_text(FL_NORMAL_TEXT,19,176,40,30,"To:");
    fl_set_object_color(obj,FL_SLATEBLUE,FL_MCOL);
    fl_set_object_lalign(obj,FL_ALIGN_RIGHT|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
  fdui->input_xmin = obj = fl_add_input(FL_FLOAT_INPUT,80,145,60,30,"X");
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
    fl_set_object_callback(obj,FDCB_plot_input_xmin,(long) fdui);
  fdui->input_ymin = obj = fl_add_input(FL_FLOAT_INPUT,165,145,60,30,"Y");
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
    fl_set_object_callback(obj,FDCB_plot_input_ymin,(long) fdui);
  fdui->input_xmax = obj = fl_add_input(FL_FLOAT_INPUT,80,175,60,30,"X");
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
    fl_set_object_callback(obj,FDCB_plot_input_xmax,(long) fdui);
  fdui->input_ymax = obj = fl_add_input(FL_FLOAT_INPUT,165,175,60,30,"Y");
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
    fl_set_object_callback(obj,FDCB_plot_input_ymax,(long) fdui);
  fdui->input_npts = obj = fl_add_input(FL_INT_INPUT,165,210,60,30,"# pts");
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
    fl_set_object_callback(obj,FDCB_plot_input_npts,(long) fdui);
  fdui->button_saveplot = obj = fl_add_button(FL_NORMAL_BUTTON,135,305,90,30,"Save Plot...");
    fl_set_object_lcolor(obj,FL_BLUE);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
    fl_set_object_callback(obj,FDCB_button_saveplot,(long) fdui);
  fdui->browser_plotselect = obj = fl_add_browser(FL_SELECT_BROWSER,30,365,185,80,"Recall Settings");
    fl_set_object_lalign(obj,FL_ALIGN_TOP);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_SouthWest);
    fl_set_object_resize(obj, FL_RESIZE_Y);
    fl_set_object_callback(obj,FDCB_browser_plotselect,(long) fdui);
  fdui->button_save_settings = obj = fl_add_button(FL_NORMAL_BUTTON,60,450,125,30,"Store Settings...");
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);
    fl_set_object_gravity(obj, FL_SouthWest, FL_SouthWest);
    fl_set_object_callback(obj,FDCB_button_save_settings,(long) fdui);
  obj = fl_add_text(FL_NORMAL_TEXT,19,101,210,40,"Draw a line with the mouse,\nor enter the endpoints below.");
    fl_set_object_color(obj,FL_SLATEBLUE,FL_MCOL);
    fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_BOLDITALIC_STYLE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
  obj = fl_add_frame(FL_ENGRAVED_FRAME,21,343,204,140,"");
    fl_set_object_gravity(obj, FL_NorthWest, FL_SouthEast);
    fl_set_object_resize(obj, FL_RESIZE_Y);
  fdui->button_plotx = obj = fl_add_checkbutton(FL_PUSH_BUTTON,40,275,30,30,"X");
    fl_set_object_color(obj,FL_COL1,FL_GREEN);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
    fl_set_object_callback(obj,FDCB_button_plotx,(long) fdui);
  fdui->button_ploty = obj = fl_add_checkbutton(FL_PUSH_BUTTON,87,275,30,30,"Y");
    fl_set_object_color(obj,FL_COL1,FL_GREEN);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
    fl_set_object_callback(obj,FDCB_button_ploty,(long) fdui);
  fdui->button_plotarc = obj = fl_add_checkbutton(FL_PUSH_BUTTON,135,275,30,30,"Arclength");
    fl_set_object_color(obj,FL_COL1,FL_GREEN);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
    fl_set_object_callback(obj,FDCB_button_plotarc,(long) fdui);
  fdui->button_clearplot = obj = fl_add_button(FL_NORMAL_BUTTON,25,305,70,30,"Clear");
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
    fl_set_object_callback(obj,FDCB_button_clearplot,(long) fdui);
  fdui->input_tolerance = obj = fl_add_input(FL_FLOAT_INPUT,165,240,60,30,"Tolerance");
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
    fl_set_object_callback(obj,FDCB_plot_input_tol,(long) fdui);
  fdui->button_even_spacing = obj = fl_add_checkbutton(FL_PUSH_BUTTON,75,215,30,30,"Equally\nspaced?");
    fl_set_object_lalign(obj,FL_ALIGN_LEFT);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
    fl_set_object_callback(obj,FDCB_button_spacing,(long) fdui);
  fl_end_form();

  fdui->plot->fdui = fdui;

  return fdui;
}
/*---------------------------------------*/

