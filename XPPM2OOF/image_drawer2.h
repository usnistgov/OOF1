/** Header file generated with fdesign on Sat Aug 26 21:45:42 2000.**/

#ifndef FD_image_drawer_h_
#define FD_image_drawer_h_

/** Callbacks, globals and object handlers **/




/**** Forms and Objects ****/
typedef struct {
	FL_FORM *image_drawer;
	void *vdata;
	char *cdata;
	long  ldata;
	FL_OBJECT *background;
	FL_OBJECT *dashboard_area;
	FL_OBJECT *closebutton;
	FL_OBJECT *savebutton;
	FL_OBJECT *coordinates;
	FL_OBJECT *input_zoomfactor;
	FL_OBJECT *button_showall;
	FL_OBJECT *button_redraw;
	FL_OBJECT *text_xmin;
	FL_OBJECT *text_xmax;
	FL_OBJECT *text_ymin;
	FL_OBJECT *text_ymax;
	FL_OBJECT *input_scalefactor;
	FL_OBJECT *current_dashboard_menu;
	FL_OBJECT *button_home;
	FL_OBJECT *display;
	FL_OBJECT *button_image;
	FL_OBJECT *button_material;
	FL_OBJECT *button_mesh;
	FL_OBJECT *input_meshwidth;
	FL_OBJECT *button_meshcolor;
	FL_OBJECT *button_selectedpixels;
	FL_OBJECT *button_selectedmesh;
	FL_OBJECT *button_selpixcolor;
	FL_OBJECT *button_selectedmeshcolor;
	FL_OBJECT *button_firewalls;
	FL_OBJECT *button_firewallcolor;
	FL_OBJECT *button_pixmaterial;
	FL_OBJECT *button_trimaterial;
	FL_OBJECT *button_nomaterialcolor;
	FL_OBJECT *imagemanager;
	FL_OBJECT *browser_imagelist;
	FL_OBJECT *button_store;
	FL_OBJECT *button_delete;
	FL_OBJECT *button_save_current;
	FL_OBJECT *button_save_selected;
	FL_OBJECT *meshinfo;
	FL_OBJECT *meshinfo_text_Ehomog;
	FL_OBJECT *meshinfo_text_Eshape;
	FL_OBJECT *meshinfo_E_homog;
	FL_OBJECT *meshinfo_depth;
	FL_OBJECT *meshinfo_area;
	FL_OBJECT *meshinfo_E_shape;
	FL_OBJECT *meshinfo_size;
	FL_OBJECT *meshinfo_mouse_xy;
	FL_OBJECT *meshinfo_text_E;
	FL_OBJECT *meshinfo_E;
	FL_OBJECT *meshinfo_material;
	FL_OBJECT *meshinfo_groups;
	FL_OBJECT *button_meshinfo;
	FL_OBJECT *button_movenode;
	FL_OBJECT *choice_meshinfo;
	FL_OBJECT *meshinfo_text_mouse;
	FL_OBJECT *button_highlight;
	FL_OBJECT *button_highlight_color;
	FL_OBJECT *button_meshinfo_selection;
	FL_OBJECT *button_meshinfo_selection_color;
	FL_OBJECT *button_select_triangle;
	FL_OBJECT *image_region;
	FL_OBJECT *pixelinfo;
	FL_OBJECT *info_mouse_xy;
	FL_OBJECT *info_pixel_xy;
	FL_OBJECT *info_color;
	FL_OBJECT *browser_material;
	FL_OBJECT *browser_groups;
	FL_OBJECT *button_mark;
	FL_OBJECT *info_image_size;
	FL_OBJECT *button_infopix_color;
	FL_OBJECT *imagemodification;
	FL_OBJECT *button_gray;
	FL_OBJECT *button_smooth;
	FL_OBJECT *button_nonlinear;
	FL_OBJECT *button_smooth_params;
	FL_OBJECT *button_nonlinear_params;
	FL_OBJECT *button_undomod;
	FL_OBJECT *button_redomod;
	FL_OBJECT *button_undoallmod;
	FL_OBJECT *button_stencil;
	FL_OBJECT *button_stencil_params;
	FL_OBJECT *button_maxcontrast;
	FL_OBJECT *button_equalize;
	FL_OBJECT *automation;
	FL_OBJECT *input_graingroup;
	FL_OBJECT *input_bdygroup;
	FL_OBJECT *input_minburn;
	FL_OBJECT *input_maxburn;
	FL_OBJECT *input_local_flam;
	FL_OBJECT *input_global_flam;
	FL_OBJECT *input_min_selection;
	FL_OBJECT *input_max_aspect;
	FL_OBJECT *input_max_perimeter;
	FL_OBJECT *input_min_aspect;
	FL_OBJECT *button_verbose;
	FL_OBJECT *button_auto_redraw;
	FL_OBJECT *button_go;
	FL_OBJECT *selectmethod;
	FL_OBJECT *button_burn;
	FL_OBJECT *button_rgbburn;
	FL_OBJECT *button_demography;
	FL_OBJECT *button_pixel;
	FL_OBJECT *button_repeat;
	FL_OBJECT *button_burn_params;
	FL_OBJECT *button_undo_selection;
	FL_OBJECT *button_redo_selection;
	FL_OBJECT *button_select_none;
	FL_OBJECT *button_select_complement;
	FL_OBJECT *input_selectclick;
	FL_OBJECT *button_repeat_shifted;
	FL_OBJECT *button_demograyfy;
	FL_OBJECT *box_pixelparams;
	FL_OBJECT *button_select_info;
} FD_image_drawer;

extern FD_image_drawer * create_form_image_drawer(void);
typedef struct {
	FL_FORM *quitform;
	void *vdata;
	char *cdata;
	long  ldata;
	FL_OBJECT *background;
	FL_OBJECT *text;
	FL_OBJECT *button_save;
	FL_OBJECT *button_cancel;
	FL_OBJECT *button_quitnow;
} FD_quitform;

extern FD_quitform * create_form_quitform(void);
typedef struct {
	FL_FORM *colorbrowser;
	void *vdata;
	char *cdata;
	long  ldata;
	FL_OBJECT *background;
	FL_OBJECT *slider_red;
	FL_OBJECT *slider_grn;
	FL_OBJECT *slider_blu;
	FL_OBJECT *box_new;
	FL_OBJECT *box_old;
	FL_OBJECT *button_ok;
	FL_OBJECT *button_cancel;
} FD_colorbrowser;

extern FD_colorbrowser * create_form_colorbrowser(void);

#endif /* FD_image_drawer_h_ */
