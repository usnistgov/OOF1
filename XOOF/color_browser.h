#ifndef FD_colorbrowser_h_
#define FD_colorbrowser_h_
/* Header file generated with fdesign. */

/**** Callback routines ****/



/**** Forms and Objects ****/

typedef struct {
	FL_FORM *colorbrowser;
	FL_OBJECT *background;
	FL_OBJECT *slider_red;
	FL_OBJECT *slider_grn;
	FL_OBJECT *slider_blu;
	FL_OBJECT *box_new;
	FL_OBJECT *box_old;
	FL_OBJECT *button_ok;
	FL_OBJECT *button_cancel;
	void *vdata;
	long ldata;
} FD_colorbrowser;

extern FD_colorbrowser * create_form_colorbrowser(void);

#endif /* FD_colorbrowser_h_ */
