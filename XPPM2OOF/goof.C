// -*- C++ -*-
// $RCSfile: goof.C,v $
// $Revision: 1.29 $
// $Author: langer $
// $Date: 2004-10-26 02:17:17 $

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */

#include "adaptmesh.h"
#include "compressedfile.h"
#include "gfx.h"
#include "goof.h"
#include "image.h"
#include "imageform.h"
#include "material.h"
#include "menuDef.h"
#include "namedimage.h"
#include "menuDef.h"
#include "meshcmds.h"
#include "pixelgroups.h"
#include "restrict.h"
#include "truefalse.h"
#include <fstream.h>

int Goof::meshstacksize = 20;
TrueFalse Goof::active_override(0);

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

Goof::Goof(const Goof&)
  : meshstack(20)
{
  cerr << "Error! The goof copy constructor was called!" << endl;
}

Goof::Goof(const CharString &filename)
  : gallerycount(0),
    the_mesh(0),
    selected_index(0),
    selected_grp(0),
    meshstack(meshstacksize, Goof::overwritemeshstack)
{
  CompressedFile cfppm(filename);
  istream *ppmfile = &cfppm.stream();
  if(!ppmfile || !*ppmfile) {
    garcon()->msout << ms_error << "Can't open file \"" << filename << "\"!"
		    << endl << ms_normal;
    return;
  }
  Image picture;
  if(!picture.read_image(*ppmfile)) {
    garcon()->msout << ms_error << "Can't read image file!"
		    << endl << ms_normal;
    return;
  }
  
  addimage(picture, filename);	// makes a copy of the image
  //  materialimage.copy_image(picture);
  setup(picture.query_height(), picture.query_width());
}

Goof::Goof(int h, int w)
  : gallerycount(0),
    the_mesh(0),
    selected_index(0),
    selected_grp(0),
    meshstack(meshstacksize, Goof::overwritemeshstack)
{
  setup(h, w);
}

void Goof::setup(int h, int w) {
  height = h;
  width = w;
  material = Array<Material*>(h, w);
  material.clear(0);	// set all to null pointers
  fireproof = Array<bool>(h, w);
  fireproof.clear(0);
  current_active_area = ActiveArea(h, w);
  current_active_area.clear(1);
  pixelgrouplist = Array<LinkList<PixelGroup*> >(h, w);
  pixelcategory = Array<int>(h, w);
  selected_grp = new PixelGroup;
  selections.grow(1, selected_grp);
  materialimage = Image(w, h);
  periodic_x = periodic_x_dflt;
  periodic_y = periodic_y_dflt;
  catalog = Vec<Cell_coordinate>(0, BlockSize(10));
}

Goof::~Goof() {
  int i;

  close_all_forms();

  // when a group is deleted, it deletes all entries pointing to it in
  // the pixelgrouplists. So to speed things up, squash those lists
  // first.
  ArrayIterator iter(pixelgrouplist);
  Cell_coordinate cell;
  while(iter(cell))
    pixelgrouplist[cell].clear();
  // delete groups
  for(i=grouplist.capacity()-1; i>=0; i--)
    delete grouplist[i];
  
  for(i=0; i<selections.capacity(); i++)
    delete selections[i];

  meshstack.clear();

  cleargallery();
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

void Goof::addimage(const Image &im, const CharString &name) {
  // Does this image have the same name as one already in the gallery?
  int replaced = 0;
  for(int i=0; i<gallery.capacity(); i++) {
    if(gallery[i]->name() == name) { // replace image with same name
      delete gallery[i];
      gallery[i] = new NamedImage(im, name);
      replaced = 1;
      break;
    }
  }
  if(!replaced) {		// old image not replaced
    ++gallerycount;		// used in constructing default image names
    gallery.grow(1, new NamedImage(im, name));
  }
  update_gallery_listings();
}

void Goof::removeimage(const CharString &name) {
  //  NamedImage dummy(name);
  // gallery.remove(&dummy), NamedImage::comparename);// why doesn' this work?
  for(int i=0; i<gallery.capacity(); i++)
    if(gallery[i]->name() == name) {
      delete gallery[i];
      gallery.remove(gallery[i]);
      break;
    }
  update_gallery_listings();
}

void Goof::displayimage(int which) {
  for(int i=0; i<imageforms.capacity(); i++) {
    if(imageforms[i])
	imageforms[i]->set_picture(*gallery[which]);
  }
}

void Goof::removeimage(int which) {
  delete gallery[which];
  gallery.remove(gallery[which]);
  update_gallery_listings();
}

void Goof::cleargallery() {
  for(int i=0; i<gallery.capacity(); i++)
    delete gallery[i];
  gallery.resize(0);
  update_gallery_listings();
}

void Goof::update_gallery_listings() const {
  for(int i=0; i<imageforms.capacity(); i++)
    imageforms[i]->update_gallery_listing(gallery);
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

void Goof::close_all_forms() {
  for(int i=imageforms.capacity()-1; i>=0; i--)
    delete imageforms[i];
  imageforms.resize(0);
}

void Goof::transfer_forms(Goof *othergoof) {
  for(int i=imageforms.capacity()-1; i>=0; i--)
    imageforms[i]->change_goof(othergoof);
}

void Goof::apply_all_forms(void (ImageForm::*f)()) const {
  for(int i=0; i<imageforms.capacity(); i++)
    (imageforms[i]->*f)();
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

void Goof::new_mesh(int nx, int ny) {
  add_mesh(new AdaptiveMesh(this, nx, ny));
}

void Goof::add_mesh(AdaptiveMesh *mesh) {
  meshstack.push(mesh);
  the_mesh = meshstack.current();
  if(meshstack.capacity() == 1)
    for(int i=0; i<imageforms.capacity(); i++)
      imageforms[i]->mesh_created();
}

void Goof::destroy_mesh() {
  meshstack.clear();
  for(int i=0; i<imageforms.capacity(); i++)
    imageforms[i]->mesh_destroyed();
  the_mesh = 0;
}

void Goof::triangle_destroyed(const AMTriangle *tri) const {
  for(int i=0; i<imageforms.capacity(); i++)
    imageforms[i]->triangle_destroyed(tri);
}

void Goof::dup_mesh() {
  // don't copy if the stack is set to hold only one mesh
  if(the_mesh && meshstack.get_rollover() > 1) {
    meshstack.push(the_mesh->copy());
    the_mesh = meshstack.current();
  }
}

int Goof::prev_mesh() {
  if(meshstack.prev()) {
    the_mesh = meshstack.current();
    return 1;
  }
  return 0;
}

int Goof::next_mesh() {
  if(meshstack.next()) {
    the_mesh = meshstack.current();
    return 1;
  }
  return 0;
}

void Goof::oldest_mesh() {
  meshstack.first();
  the_mesh = meshstack.current();
}

void Goof::newest_mesh() {
  meshstack.last();
  the_mesh = meshstack.current();
}

void Goof::overwritemeshstack(AdaptiveMesh *&m) {
  delete m;
}

void Goof::set_meshstacksize(const CharString&) {
  if(current_goof) {
    if(meshstacksize <= 0) {
      meshstacksize = 1;
      meshstackmenu()->update_forms();
    }
    current_goof->meshstack.set_rollover(meshstacksize);
    if(current_goof->the_mesh) {
      if(current_goof->meshstack.current() != current_goof->the_mesh) {
	current_goof->the_mesh = current_goof->meshstack.current();
	current_goof->redraw();
      }
    }
  }
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

void Goof::assign_material(const Cell_coordinate &which, Material *mat) {
  material[which] = mat;
  if(mat)
    materialimage[which] = mat->query_gray();
  materialimage.changed = 1;
  if(the_mesh)
    the_mesh->materials_need_recomputing();
  ++materials_changed;
}

void Goof::reset_materials() {
  material.clear(0);
  if(the_mesh)
    the_mesh->materials_need_recomputing();
  redraw();
  ++materials_changed;
}

void Goof::reset_materials_selected() {
  for(int i=0; i<selected().size(); i++) {
    Cell_coordinate which(selected()[i]);
    material[selected()[i]] = 0;
  }
  redraw();
  ++materials_changed;
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

void Goof::redraw() {
  for(int i=0; i<imageforms.capacity(); i++)
    imageforms[i]->draw();
}

void Goof::mark_selected_pixels(Image &image, const Color &color) const {
  selected_grp->weed();
  for(int i=0; i<selected_grp->size(); i++) {
    Color &c = image[(*selected_grp)[i]];
    c = c.blend(color, selected_opacity);
    //  image[(*selected_grp)[i]] = color;
  }
}

void Goof::mark_firewalls(Image &image, const Color &color) const {
  ArrayIterator iter(fireproof);
  Cell_coordinate cell;
  while(iter(cell))
    if(fireproof[cell])
      image[cell] = color;
}

void Goof::mark_nomaterial(Image &image, const Color &color) const {
  ArrayIterator iter(material);
  Cell_coordinate cell;
  while(iter(cell))
    if(!material[cell])
      image[cell] = color;
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

PixelGroup *Goof::add_group(const CharString &name) {
  // check for existing group w/ this name
  for(int i=0; i<grouplist.capacity(); i++)
    if(grouplist[i]->query_name() == name) {
      garcon()->msout << ms_error << "There is already a group named \"" << name
		      << "\"!" << endl << ms_normal;
      return 0;
    }
  PixelGroup *newgroup = new PixelGroup(name);
  grouplist.grow(1, newgroup);
  return newgroup;
}

PixelGroup *Goof::get_group(const CharString &name) {
  for(int i=0; i<grouplist.capacity(); i++)
    if(name == grouplist[i]->query_name())
      return grouplist[i];
  return 0;
}

void Goof::remove_group(const CharString &name) {
  for(int i=0; i<grouplist.capacity(); i++) {
    if(grouplist[i]->query_name() == name) {
      PixelGroup *deadgroup = grouplist[i];

      // remove this group from each pixel's list of groups
      ArrayIterator iter(pixelgrouplist);
      Cell_coordinate cell;
      while(iter(cell))
	pixelgrouplist[cell].Remove(deadgroup);

      // remove this group from the goof's list of groups
      grouplist.remove(deadgroup);
      delete deadgroup;
      if(mesh())
	mesh()->groups_need_recomputing();
      return;
    }
  }
}

void Goof::weed_all_groups() {
  for(int i=0; i<grouplist.capacity(); i++)
    grouplist[i]->weed();
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

void Goof::init_selection(int copy) {
  PixelGroup *lastselection = selected_grp;
  selected_grp = new PixelGroup;
  selected_index++;
  if(selections.capacity() == selected_index)	// just adding to list
    selections.grow(1, selected_grp);
  else {  // have to delete undone groups
    for(int i=selected_index; i<selections.capacity(); i++)
      delete selections[i];
    selections.resize(selected_index+1);
    selections[selected_index] = selected_grp;
  }
  
  if(copy)
    *selected_grp = *lastselection;	// copy all selected pixels
  else {			// clear the active region, copy others
    if(active_override || (current_active_area.ninactive == 0))
      selected_grp->clear();
    else {
      *selected_grp = *lastselection;
      // remove pixels in the active region
      Vec<Cell_coordinate> deletions;
      int sz = selected_grp->size();
      deletions.setphysicalsize(sz);
      for(int i=0; i<sz; i++)
	if(active((*selected_grp)[i]))
	  deletions.grow(1, (*selected_grp)[i]);
      selected_grp->remove(deletions);
    }
  }
  selected_grp->weed();

  set_imageform_undoredo_buttons();
}

void Goof::select(const Cell_coordinate &which) {
  if(active(which))
    selected_grp->append(which);
}

void Goof::select(const Vec<Cell_coordinate> &list) {
  // list should contain only active pixels!
  selected_grp->append(list);
}

void Goof::select(const PixelGroup &grp) {
  // grp should contain only active pixels!
  selected_grp->append(grp);
}

void Goof::unselect(const Cell_coordinate &which) {
  selected_grp->remove(which);
}

void Goof::select(const Shape &shape, int toggle) {
  if(toggle) {
    // toggle selected status of each pixel in the rectangle
    // see comments in select_complement()
    // make an array with selected pixels marked with a 1
    Array<int> sel(height, width);
    sel.clear(0);
    selected_grp->weed();
    for(int i=0; i<selected_grp->size(); i++)
      sel[(*selected_grp)[i]] = 1;
    // toggle array elements for each pixel in rect
    int nsel = selected_grp->size();
    ShapeIterator iter(shape && enclosing_rectangle());
    Cell_coordinate pxl;
    while(iter(pxl)) {
      if(active(pxl)) {
	if(sel.contains(pxl)) {
	  if(sel[pxl]) {
	    sel[pxl] = 0;
	    --nsel;
	  }
	  else {
	    sel[pxl] = 1;
	    ++nsel;
	  }
	}
      }
    }
    // make a list of array elements still selected
    Vec<Cell_coordinate> sellist(nsel);
    ArrayIterator iter2(sel);
    int j = 0;
    while(iter2(pxl))
      if(sel[pxl])
	sellist[j++] = pxl;
    // select all pixels in the list
    init_selection(0);
    select(sellist);
  }
  else {			// don't toggle selections, just select
    // loop over bounding box
    ShapeIterator iter(shape && enclosing_rectangle());
    Vec<Cell_coordinate> pixlist; // temporary list of selected pixels
    pixlist.setphysicalsize(iter.upperbound());	// efficiency!
    Cell_coordinate pxl;
    while(iter(pxl))
      if(materialimage.contains(pxl) && active(pxl))
	pixlist.grow(1, pxl);
    selected_grp->append(pixlist); // add all pixels at once
  }
}
 
void Goof::unselect_all() {
  init_selection(0);
}

void Goof::undo_selection() {
  if(selected_index > 0)
    selected_grp = selections[--selected_index];
  else
    garcon()->msout << ms_error << "No selections to undo!"
		    << endl << ms_normal;
  set_imageform_undoredo_buttons();
}

void Goof::redo_selection() {
  if(selected_index+1 < selections.capacity()) {
    selected_index++;
    selected_grp = selections[selected_index];
  }
  else
    garcon()->msout << ms_error << "No selections to redo!"
		    << endl << ms_normal;
  set_imageform_undoredo_buttons();
}

void Goof::select_complement() {
  /* The groups aren't indexed in a way that allows fast access to
   * the complement, so use an Array to store the complement
   */
  Array<bool> sel(height, width);
  sel.clear(0);

  selected_grp->weed();

  for(int i=0; i<selected_grp->size(); i++)
    sel[(*selected_grp)[i]] = 1;
 
  /* Adding pixels the the group one by one is slow, so create a list
   * of pixels in the complement
   */
  Vec<Cell_coordinate> sellist;
  sellist.setphysicalsize(height*width);
  ArrayIterator iter(sel);
  Cell_coordinate pxl;
  while(iter(pxl))
    if((!sel[pxl] && active(pxl)) || (sel[pxl] && !active(pxl)))
      sellist.grow(1, pxl);

  /* Now select all pixels in the list */
  init_selection(0);
  select(sellist);
}

// select the intersection of the set of currently selected pixels and
// the pixels in the given list. If complement=1, then select the
// intersection of the currently selected pixels and the complement of
// the pixels in the list.

void Goof::select_intersection(const PixelGroup &grp, int complement)
{
  int i;
  // make an array with a 1 for each selected pixel
  Array<int> sel(height, width);
  sel.clear(0);
  selected_grp->weed();
  for(i=0; i<selected_grp->size(); i++)
    sel[(*selected_grp)[i]] = 1;

  // make an array with a 1 for each pixel in list (0 if using complement)
  Array<int> listarray(height, width);
  listarray.clear(complement? 1 : 0);
  for(i=0; i<grp.size(); i++)
    listarray[grp[i]] ^= 1;

  // make a list of pixels in both arrays
  Vec<Cell_coordinate> sellist;
  sellist.setphysicalsize(height*width);
  ArrayIterator iter(sel);
  Cell_coordinate pxl;
  while(iter(pxl))
    if(active(pxl) && sel[pxl] && listarray[pxl])
      sellist.grow(1, pxl);

  init_selection(0);
  select(sellist);
}

void Goof::select_all_groups() {

  // SHOULD THIS ONLY SELECT PIXELS IN THE ACTIVE REGION?

  for(int i=0; i<grouplist.capacity(); i++)
    select(*grouplist[i]);
}

int Goof::is_selected(const Cell_coordinate &which) const {
  for(int i=0; i<selected_grp->size(); i++)
    if((*selected_grp)[i] == which) return 1;
  return 0;
}

void Goof::set_imageform_undoredo_buttons() const {
  int undoable = selected_index > 0;
  int redoable = selected_index + 1 < selections.capacity();
  for(int i=0; i<imageforms.capacity(); i++)
    imageforms[i]->set_undoredo_buttons(undoable, redoable);
}

void Goof::selection_info() const {
  if(selected().size() > 0)
    garcon()->msout <<  "npixels = " << selected().size()
		    << " aspect ratio = " << selected().aspectRatio()
		    << " perimeter ratio = " << selected().perimeter()
		    << endl;
  else
    garcon()->msout << ms_error << "No pixels selected" << endl << ms_normal;
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

void Goof::set_firewall() {
  for(int i=0; i<selected_grp->size(); i++) {
    fireproof[(*selected_grp)[i]] = 1;
  }
  redraw();
}

void Goof::clear_firewall() {
  fireproof.clear(0);
  redraw();
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

void ActiveArea::clear(const bool &x) {
  if(x)
    ninactive = 0;
  else
    ninactive = height*width;
  Array<bool>::clear(x);
}

bool Goof::active(const MeshCoord &pt) const {
  if(active_override || current_active_area.ninactive == 0) return 1;
  int x = int(floor(pt.x));
  if(x == width) --x;
  int y = int(floor(pt.y));
  if(y == height) --y;
  return current_active_area[Cell_coordinate(x,y)];
}

void Goof::activate_group(PixelGroup &grp, ActivationMode mode) {
  Array<bool> agrp(height, width); // grp represented as an array
  agrp.clear(0);
  grp.weed();
  for(int i=0; i<grp.size(); i++)
    agrp[grp[i]] = 1;

  ArrayIterator iter(current_active_area);
  Cell_coordinate pxl;

  if(mode == AM_INTERSECT) {
    while(iter(pxl)) {
      bool oldval = current_active_area[pxl];
      bool newval = agrp[pxl];
      if(oldval && newval)
	current_active_area[pxl] = 1;
      else {
	current_active_area[pxl] = 0;
	if(oldval)
	  ++current_active_area.ninactive;
      }
    }
  }
  else if(mode == AM_UNION) {
    while(iter(pxl)) {
      bool oldval = current_active_area[pxl];
      bool newval = agrp[pxl];
      if(oldval || newval) {
	current_active_area[pxl] = 1;
	if(!oldval)
	  --current_active_area.ninactive;
      }
      else {
	current_active_area[pxl] = 0;
	if(oldval)
	  ++current_active_area.ninactive;
      }
    }
  }
}

void Goof::activate_selection(ActivationMode mode) {
  activate_group(*selected_grp, mode);
}

void Goof::remove_restrictions() {
  current_active_area.clear(1);
}

void Goof::select_active_area() {
  Vec<Cell_coordinate> reslist;
  reslist.setphysicalsize(height*width);
  ArrayIterator iter(current_active_area);
  Cell_coordinate pxl;
  while(iter(pxl))
    if(current_active_area[pxl])
      reslist.grow(1, pxl);
  init_selection(0);
  select(reslist);
}

void Goof::unselect_inactive_area() {
  Vec<Cell_coordinate> reslist;
  reslist.setphysicalsize(height*width);
  for(int i=0; i<selected_grp->size(); i++)
    if(!current_active_area[(*selected_grp)[i]])
      reslist.grow(1, (*selected_grp)[i]);
  init_selection(1);
  selected_grp->remove(reslist);
}

void Goof::store_active_area(const CharString &name) {
  // if there's already an active area with this name, overwrite it
  bool foundit = 0;
  for(int i=0; i<stored_active_areas.capacity() && !foundit; i++)
    if(stored_active_areas[i]->name == name) {
      foundit = 1;
      delete stored_active_areas[i];
      stored_active_areas[i] = new ActiveArea(current_active_area);
      stored_active_areas[i]->name = name;
    }
  if(!foundit) {
    int n = stored_active_areas.grow(1, new ActiveArea(current_active_area));
    stored_active_areas[n]->name = name;
    activerecallmenu()->AddCommand(name, "recall active area '" + name + "'",
				   activerecall);
  }
}

bool Goof::recall_active_area(const CharString &name) {
  for(int i=0; i<stored_active_areas.capacity(); i++) {
    if(stored_active_areas[i]->name == name) {
      current_active_area = *stored_active_areas[i];
      return 1;
    }
  }
  return 0;
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

TrueFalse Goof::periodic_x_dflt = TF_FALSE; // these appear in the menu
TrueFalse Goof::periodic_y_dflt = TF_FALSE;

void Goof::set_periodicity(const CharString&) { // static, called by menu
  if(current_goof)
    current_goof->setperiodicity();
}

void Goof::setperiodicity() {	// nonstatic
  if(mesh()) {
    // mesh exists already, so it's possible to turn periodicity off,
    // but not to turn it on.
    if((!periodic_x && periodic_x_dflt) || (!periodic_y && periodic_y_dflt)) {
      // attempting to turn on periodicity
      periodic_x_dflt = periodic_x;// turn it back off
      periodic_y_dflt = periodic_y;
      periodicmenu()->update_forms(); // reflect changes in menu 
      garcon()->msout << ms_error
		      << "You cannot make the system periodic" << endl
		      << "after the mesh has been created!" << endl
		      << ms_normal;
    }
    else {			// mesh exists, turning off periodicity
      // turn off periodicity for all meshes in the mesh stack
      if(periodic_x && !periodic_x_dflt) { // turn off x
	for(CircleStackIterator<AdaptiveMesh*> iter(meshstack); !iter.end();
	    ++iter)
	    meshstack[iter]->remove_periodic_x();
      }
      if(periodic_y && !periodic_y_dflt) { // turn off y
	for(CircleStackIterator<AdaptiveMesh*> iter(meshstack); !iter.end();
	    ++iter)
	    meshstack[iter]->remove_periodic_y();
      }
      periodic_x = periodic_x_dflt;
      periodic_y = periodic_y_dflt;
    }
  }
  else {			// mesh doesn't exist yet
    periodic_x = periodic_x_dflt;
    periodic_y = periodic_y_dflt;
  }
}

// -----------------------

bool Goof::prepare_output() const {
  // check to see if default material is set, if necessary:
  if(!defaultmaterial) {
    Cell_coordinate pixel;
    ArrayIterator iter(material);
    while(iter(pixel)) {
      if(!material[pixel])
	return false;
    }
  }
  return true;
}
