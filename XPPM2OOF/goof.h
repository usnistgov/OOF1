// -*- C++ -*-
// $RCSfile: goof.h,v $
// $Revision: 1.24 $
// $Author: langer $
// $Date: 2004-11-02 22:36:05 $

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */

#ifndef GOOF_H
#define GOOF_H

class Goof;

class AdaptiveMesh;
class AMTriangle;
class CharString;
class iShape;
class ImageForm;
class Material;

#include "array.h"
#include "circlestack.h"
#include "image.h"
#include "linklist.h"
#include "namedimage.h"
#include "truefalse.h"
#include "pixelgroups.h"
#include "ishape.h"
#include "shape.h"
#include "timestamp.h"
#include "vec.h"
#include <stdio.h>
#include <iostream.h>

#include "beginheader.h"

class ActiveArea : public Array<bool> {
private:
  ActiveArea() {}
  ActiveArea(int h, int w) : Array<bool>(h, w) {}
  CharString name;		// only used for stored active areas
  int ninactive;		// number of pixels not in the active area
  virtual void clear(const bool &x);
public:
  static double fade;		// amount to fade display of inactive area
  friend class Goof;
};


class Goof {
private:
  void setup(int, int);
  int height, width;
  PixelGroup *selected_grp;
  int selected_index;
  Vec<PixelGroup*> selections;
  AdaptiveMesh *the_mesh;
  CircleStack<AdaptiveMesh*> meshstack;
  static void overwritemeshstack(AdaptiveMesh* &);
  Vec<ImageForm*> imageforms;
  Goof(const Goof &oldgoof); // shouldn't be used

  Vec<NamedImage*> gallery;
  int gallerycount;		// used to assign unique names to images
  void cleargallery();
  void update_gallery_listings() const;

  Vec<ActiveArea*> stored_active_areas;
  ActiveArea current_active_area;
  Vec<Cell_coordinate> catalog;	// representive pixels for each pixel category

  // oof2 output functions
  void find_material_groups(Vec<PixelGroup*>&);
  bool check_plane_strain(const Vec<PixelGroup*>&, bool&) const;
#ifdef THERMAL
  bool check_Tplanarity(const Vec<PixelGroup*>&, bool&) const;
#endif
  void print_materials(ostream&, const Vec<PixelGroup*>&) const;
  void print_images(ostream&, const CharString&) const;
  void print_pixelgroups(ostream&, const CharString&) const;
  void print_categories(ostream&, const CharString&, const Vec<PixelGroup*>&);

public:
  Goof(const CharString &ppmfilename);
  Goof(int h, int w);
  virtual ~Goof();

  void autoSelect(const Image &, const CharString&, const CharString&,
		  const Color&,
		  const Color&, const int, const int, const int, const double,
		  const double, const double, bool, bool);

  Image materialimage;
  Array<Material*> material;
  Array<bool> fireproof;

  void addimage(const Image &im, const CharString &name);
  void displayimage(int);
  void removeimage(const CharString &name);
  void removeimage(int);
  int gallerysize() const { return gallery.capacity(); }
  int gallerycounter() const { return gallerycount; }
  Image *get_image(int i) { return gallery[i]; }

  void close_all_forms();     // close all ImageForms
  void transfer_forms(Goof*); // hand ImageForms over to another goof
  void apply_all_forms(void (ImageForm::*f)()) const; // apply f to all forms
	
  int query_height() const { return height; }
  int query_width() const { return width; }
  ImageRect entirety() const {
    return ImageRect(Cell_coordinate(0, 0), Cell_coordinate(width, height));
  }
  Rectangle enclosing_rectangle() const {
    return Rectangle(MeshCoord(0, 0), MeshCoord(width, height));
  }
	
  Vec<PixelGroup*> grouplist;
  Array<LinkList<PixelGroup*> > pixelgrouplist;  // list of groups for each pxl
  PixelGroup *add_group(const CharString &name); // make a new group
  PixelGroup *get_group(const CharString &name); // get an existing group
  void remove_group(const CharString &name);
  void weed_all_groups();
  TimeStamp groups_changed;
	
  void init_selection(int);  // arg=1 if old selection should be copied
	
  void select(const Cell_coordinate &which);
  void select(const Vec<Cell_coordinate> &list);
  void select(const PixelGroup &grp);
  void select(const Shape&, int);
  void unselect(const Cell_coordinate &which);
  void select_complement();
  void select_intersection(const PixelGroup&, int complement=0);
  void select_all_groups();
  void unselect_all();
  void undo_selection();
  void redo_selection();
  void selection_info() const;

  void set_imageform_undoredo_buttons() const;

  int is_selected(const Cell_coordinate &which) const;
	
  PixelGroup &selected() { return *selected_grp; }
  const PixelGroup &selected() const { return *selected_grp; }
  void set_selection(int);	// select by index in selections list.

  void set_firewall();
  void clear_firewall();
	
  static TrueFalse active_override;
  bool active(const MeshCoord&) const;
  bool active(const Cell_coordinate &pxl) const {
    return
      active_override ||
      (current_active_area.ninactive == 0) ||
      current_active_area[pxl];
  }
  enum ActivationMode {AM_UNION, AM_INTERSECT};
  void activate_group(PixelGroup&, ActivationMode);
  void activate_selection(ActivationMode);
  void remove_restrictions();
  void select_active_area();
  void unselect_inactive_area();
  void store_active_area(const CharString&);
  bool recall_active_area(const CharString&);
  
  void reset_materials();
  void reset_materials_selected(); // reset materials of selected pixels
  void assign_material(const Cell_coordinate&, Material*);
  TimeStamp materials_changed;

  AdaptiveMesh *&mesh() { return the_mesh; }
  void new_mesh(int, int);
  void add_mesh(AdaptiveMesh*);
  void destroy_mesh();
  void triangle_destroyed(const AMTriangle *tri) const;
  void dup_mesh();		// copy mesh on mesh stack
  int prev_mesh();		// go to previous mesh on mesh stack
  int next_mesh();		// go to next mesh on mesh stack
  void oldest_mesh();		// go to oldest mesh on stack
  void newest_mesh();		// go to newest mesh on stack
  static int meshstacksize;
  static void set_meshstacksize(const CharString&);

  // for fiddling with the adaptive mesh
  int Ncategories;		// number of types of pixels
  Array<int> pixelcategory;	// type of each pixel
  TimeStamp pixels_categorized;	// when types were computed
  void categorize_pixels();	// compute types of each pixel
  const Cell_coordinate &categorical_pixel(int category) const {
    return catalog[category];
  }
  bool write_oof2(ostream&, const CharString&, Vec<PixelGroup*> *pgrps=0);

  static TrueFalse periodic_x_dflt;
  static TrueFalse periodic_y_dflt;
  TrueFalse periodic_x;
  TrueFalse periodic_y;
  static void set_periodicity(const CharString&);
  void setperiodicity();

  void redraw();
  void mark_selected_pixels(Image&, const Color&) const;
  void mark_firewalls(Image&, const Color&) const;
  void mark_nomaterial(Image&, const Color&) const;

  void despeckle(const int);
  int doDespeckle(const int);	// returns number of extra pixels selected
  void elkcepsed(const int);

  int expandselection(double);
  int expandgroup(PixelGroup&, double);
  int shrinkselection(double);
  int shrinkgroup(PixelGroup&, double);

  bool prepare_output() const;
  int write_binary(void) const;
  int write_binary(FILE *fp) const;

  // routines used by saveconfig()
  void savegallery(ostream&) const;
  void savematerials(ostream&) const;
  void savefirewalls(ostream&) const;
  void savegroups(ostream&) const;
  void savemeshes(ostream&) const;
  void saveselections(ostream&) const;
  void saveactiveareas(ostream&) const;

  void readmesh(istream&);
  
  friend class ArrayIterator;  // loops over pixels
  friend class ImageForm;
  friend class ImageFormGfx;
  friend class ImageFormTxt;
};

EXTERN Goof *current_goof DFLT(0);

void redraw_picture(const CharString &);
void redraw_material(const CharString &);
void redraw_colorchange(const CharString &);

#include "endheader.h"
#endif































