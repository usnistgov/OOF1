/** Header file generated with fdesign on Thu Oct  5 22:16:47 2000.**/

#ifndef FD_form_drawer_h_
#define FD_form_drawer_h_

/** Callbacks, globals and object handlers **/
extern void FDCB_home(FL_OBJECT *, long);
extern void FDCB_set_current_drawer(FL_OBJECT *, long);
extern void FDCB_set_hold_image(FL_OBJECT *, long);
extern void FDCB_set_current_dashboard(FL_OBJECT *, long);
extern void FDCB_close(FL_OBJECT *, long);
extern void FDCB_save(FL_OBJECT *, long);

extern void FDCB_edges(FL_OBJECT *, long);
extern void FDCB_empties(FL_OBJECT *, long);
extern void FDCB_hot_elements(FL_OBJECT *, long);
extern void FDCB_hot_nodes(FL_OBJECT *, long);
extern void FDCB_elements(FL_OBJECT *, long);
extern void FDCB_edgewidth(FL_OBJECT *, long);
extern void FDCB_set_selection(FL_OBJECT *, long);
extern void FDCB_noderadius(FL_OBJECT *, long);

extern void FDCB_zoomfactor(FL_OBJECT *, long);
extern void FDCB_center(FL_OBJECT *, long);
extern void FDCB_redraw(FL_OBJECT *, long);

extern void FDCB_color_max(FL_OBJECT *, long);
extern void FDCB_color_min(FL_OBJECT *, long);
extern void FDCB_color_map_menu(FL_OBJECT *, long);
extern void FDCB_color_fullscale(FL_OBJECT *, long);
extern void FDCB_colormapsize(FL_OBJECT *, long);
extern void FDCB_color_flip(FL_OBJECT *, long);

extern void FDCB_edges(FL_OBJECT *, long);
extern void FDCB_empties(FL_OBJECT *, long);
extern void FDCB_elements(FL_OBJECT *, long);
extern void FDCB_edgewidth(FL_OBJECT *, long);

extern void FDCB_info_mouseposition(FL_OBJECT *, long);
extern void FDCB_info_elementindex(FL_OBJECT *, long);
extern void FDCB_info_nodeindices(FL_OBJECT *, long);
extern void FDCB_info_intrinsicgray(FL_OBJECT *, long);
extern void FDCB_info_elementparams(FL_OBJECT *, long);
extern void FDCB_info_stress(FL_OBJECT *, long);
extern void FDCB_info_strain(FL_OBJECT *, long);
extern void FDCB_info_components(FL_OBJECT *, long);
extern void FDCB_info_invariants(FL_OBJECT *, long);
extern void FDCB_info_eigenvalues(FL_OBJECT *, long);
extern void FDCB_info_energydensity(FL_OBJECT *, long);
extern void FDCB_info_elementtype(FL_OBJECT *, long);
extern void FDCB_info_heatflux(FL_OBJECT *, long);
extern void FDCB_info_tempgradient(FL_OBJECT *, long);
extern void FDCB_info_temperature(FL_OBJECT *, long);

extern void FDCB_info_mouseposition(FL_OBJECT *, long);
extern void FDCB_info_nodetype(FL_OBJECT *, long);
extern void FDCB_info_nodeindex(FL_OBJECT *, long);
extern void FDCB_info_nodeposition(FL_OBJECT *, long);
extern void FDCB_info_nodedisplacement(FL_OBJECT *, long);
extern void FDCB_info_nodeoriginalpos(FL_OBJECT *, long);
extern void FDCB_info_nodeforce(FL_OBJECT *, long);
extern void FDCB_info_nodetemperature(FL_OBJECT *, long);


extern void FDCB_plot_input_xmin(FL_OBJECT *, long);
extern void FDCB_plot_input_ymin(FL_OBJECT *, long);
extern void FDCB_plot_input_xmax(FL_OBJECT *, long);
extern void FDCB_plot_input_ymax(FL_OBJECT *, long);
extern void FDCB_plot_input_npts(FL_OBJECT *, long);
extern void FDCB_button_saveplot(FL_OBJECT *, long);
extern void FDCB_browser_plotselect(FL_OBJECT *, long);
extern void FDCB_button_save_settings(FL_OBJECT *, long);
extern void FDCB_button_plotx(FL_OBJECT *, long);
extern void FDCB_button_ploty(FL_OBJECT *, long);
extern void FDCB_button_plotarc(FL_OBJECT *, long);
extern void FDCB_button_clearplot(FL_OBJECT *, long);
extern void FDCB_plot_input_tol(FL_OBJECT *, long);
extern void FDCB_button_spacing(FL_OBJECT *, long);


/**** Forms and Objects ****/
typedef struct {
	FL_FORM *form_drawer;
	void *vdata;
	char *cdata;
	long  ldata;
	FL_OBJECT *background;
	FL_OBJECT *homebutton;
	FL_OBJECT *type_selector;
	FL_OBJECT *hold_button;
	FL_OBJECT *current_dashboard_menu;
	FL_OBJECT *closebutton;
	FL_OBJECT *savebutton;
	FL_OBJECT *dashboard_area;
	FL_OBJECT *image_region;
} FD_form_drawer;

extern FD_form_drawer * create_form_form_drawer(void);
typedef struct {
	FL_FORM *select_attributes;
	void *vdata;
	char *cdata;
	long  ldata;
	FL_OBJECT *dontincludethisobject;
	FL_OBJECT *select_attibutes;
	FL_OBJECT *selecttext;
	FL_OBJECT *edges_button;
	FL_OBJECT *empties_button;
	FL_OBJECT *hot_elements_button;
	FL_OBJECT *hot_nodes_button;
	FL_OBJECT *elements_button;
	FL_OBJECT *edgewidth_input;
	FL_OBJECT *select_elements_button;
	FL_OBJECT *select_nodes_button;
	FL_OBJECT *noderadius_input;
	FL_OBJECT *select_nobody_button;
} FD_select_attributes;

extern FD_select_attributes * create_form_select_attributes(void);
typedef struct {
	FL_FORM *coordinates;
	void *vdata;
	char *cdata;
	long  ldata;
	FL_OBJECT *dontincludethisobject;
	FL_OBJECT *coordinates;
	FL_OBJECT *zoomfactor_input;
	FL_OBJECT *center_button;
	FL_OBJECT *redraw_button;
	FL_OBJECT *xmin_text;
	FL_OBJECT *xmax_text;
	FL_OBJECT *ymin_text;
	FL_OBJECT *ymax_text;
	FL_OBJECT *cliptext;
	FL_OBJECT *zoomtext2;
	FL_OBJECT *zoomtext3;
	FL_OBJECT *zoomtext1;
	FL_OBJECT *zoomtext0;
} FD_coordinates;

extern FD_coordinates * create_form_coordinates(void);
typedef struct {
	FL_FORM *color_control;
	void *vdata;
	char *cdata;
	long  ldata;
	FL_OBJECT *dontincludethisobject;
	FL_OBJECT *color_control;
	FL_OBJECT *max_input;
	FL_OBJECT *min_input;
	FL_OBJECT *color_map_menu;
	FL_OBJECT *valmin_text;
	FL_OBJECT *valmax_text;
	FL_OBJECT *color_fullscale;
	FL_OBJECT *colormapsize_input;
	FL_OBJECT *color_flip_button;
	FL_OBJECT *colorbar;
} FD_color_control;

extern FD_color_control * create_form_color_control(void);
typedef struct {
	FL_FORM *simple_attributes;
	void *vdata;
	char *cdata;
	long  ldata;
	FL_OBJECT *dontincludethisobject;
	FL_OBJECT *simple_attributes;
	FL_OBJECT *edges_button;
	FL_OBJECT *empties_button;
	FL_OBJECT *elements_button;
	FL_OBJECT *edgewidth_input;
} FD_simple_attributes;

extern FD_simple_attributes * create_form_simple_attributes(void);
typedef struct {
	FL_FORM *elementinfo;
	void *vdata;
	char *cdata;
	long  ldata;
	FL_OBJECT *dontincludethisobject;
	FL_OBJECT *elementinfo;
	FL_OBJECT *infoframe1;
	FL_OBJECT *info_mouseposition;
	FL_OBJECT *info_elementindex;
	FL_OBJECT *info_nodeindices;
	FL_OBJECT *info_intrinsicgray;
	FL_OBJECT *info_elementparams;
	FL_OBJECT *info_stress;
	FL_OBJECT *info_strain;
	FL_OBJECT *info_components;
	FL_OBJECT *info_invariants;
	FL_OBJECT *info_eigenvalues;
	FL_OBJECT *elementinfotext;
	FL_OBJECT *info_energydensity;
	FL_OBJECT *info_elementtype;
	FL_OBJECT *info_heatflux;
	FL_OBJECT *info_tempgradient;
	FL_OBJECT *info_temperature;
} FD_elementinfo;

extern FD_elementinfo * create_form_elementinfo(void);
typedef struct {
	FL_FORM *nodeinfo;
	void *vdata;
	char *cdata;
	long  ldata;
	FL_OBJECT *dontincludethisobject;
	FL_OBJECT *nodeinfo;
	FL_OBJECT *nodeinfotext;
	FL_OBJECT *info_mouseposition;
	FL_OBJECT *info_nodetype;
	FL_OBJECT *info_nodeindex;
	FL_OBJECT *info_nodeposition;
	FL_OBJECT *info_nodedisplacement;
	FL_OBJECT *info_nodeoriginalpos;
	FL_OBJECT *info_nodeforce;
	FL_OBJECT *info_nodetemperature;
} FD_nodeinfo;

extern FD_nodeinfo * create_form_nodeinfo(void);
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
	FL_FORM *plot;
	void *vdata;
	char *cdata;
	long  ldata;
	FL_OBJECT *dontincludethisobject;
	FL_OBJECT *input_xmin;
	FL_OBJECT *input_ymin;
	FL_OBJECT *input_xmax;
	FL_OBJECT *input_ymax;
	FL_OBJECT *input_npts;
	FL_OBJECT *button_saveplot;
	FL_OBJECT *browser_plotselect;
	FL_OBJECT *button_save_settings;
	FL_OBJECT *button_plotx;
	FL_OBJECT *button_ploty;
	FL_OBJECT *button_plotarc;
	FL_OBJECT *button_clearplot;
	FL_OBJECT *input_tolerance;
	FL_OBJECT *button_even_spacing;
} FD_plot;

extern FD_plot * create_form_plot(void);

#endif /* FD_form_drawer_h_ */
