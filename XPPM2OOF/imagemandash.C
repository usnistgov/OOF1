// -*- C++ -*-
// $RCSfile: imagemandash.C,v $
// $Revision: 1.5 $
// $Author: langer $
// $Date: 2001-04-19 01:39:35 $


/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */

#include "goof.h"
#include "imageform.h"
#include "imagemandash.h"
#include "namedimage.h"
#include "ppm2oof.h"

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

ImageManDashboard::ImageManDashboard(ImageForm *iform, FL_OBJECT *grp)
  : Dashboard("Image_Gallery", iform, grp),
    store_cmd(this),
    retrieve_cmd(this),
    save_current_cmd(this),
    save_selected_cmd(this),
    delete_cmd(this)
{
  if(!textmode)
    fl_set_browser_dblclick_callback(fdui()->browser_imagelist,
				     ImageManDashboard::CB_button_retrieve, 0);

  buildmenu(menu, MOPT_NORMAL);
  buildmenu(imageformmenu(), MOPT_NOPRINT); // for backwards compatibility
}

void ImageManDashboard::buildmenu(Menu *carte, MenuAttribute attr) {
  carte->AddCommand(&store_cmd, attr);
  carte->AddCommand(&retrieve_cmd, attr);
  carte->AddCommand(&delete_cmd, attr);
  carte->AddCommand(&save_current_cmd, attr);
  carte->AddCommand(&save_selected_cmd, attr);
}

ImageManDashboard::~ImageManDashboard() {
  // Don't remove commands from the menu because they're defined as
  // members of ImageManDashboard, not created with new.
  delete menu;
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

// Store the current image in the gallery

void ImageManDashboard::store(const CharString &name) {
  //  goof()->addimage(imageform()->current_picture(), name);
  imageform()->store_current_picture(name);
  // select the newly stored image
  if(!textmode)
    fl_select_browser_line(fdui()->browser_imagelist, goof()->gallerysize());
}

void ImageManDashboard::CB_button_store(FL_OBJECT *obj, long) {
  ((ImageManDashboard*) ImageFormGfx::objdash(obj))->store_gui();
}

void ImageManDashboard::store_gui() {
  CharString name = garcon()->getstring("Enter name for stored image",
					dflt_name());
  garcon()->log(menucommand() + "store_image name=" + name);
  store(name);
}

ImageManDashboard::StoreCmd::StoreCmd(ImageManDashboard *dash)
  : CommandM("store_image", "store current image in the gallery"),
    imagename(dash->dflt_name()),
    imd(dash)
{
  AddArgument(this, "name", imagename);
}

CommandFn ImageManDashboard::StoreCmd::func() {
  imd->store(imagename);
}

CommandM *ImageManDashboard::StoreCmd::clone() const {
  StoreCmd *sc = new StoreCmd(imd);
  sc->imagename = imagename;
  return sc;
}

CharString ImageManDashboard::dflt_name() const {
  return "image" + to_charstring(goof()->gallerycounter());
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

// Delete the currently selected image from the gallery

void ImageManDashboard::delete_image(int which) {
  if(which < 0 || which >= goof()->gallerysize()) {
    garcon()->msout << ms_error << "Bad image number!" << endl
		    << ms_normal;
    return;
  }
  goof()->removeimage(which);
}

void ImageManDashboard::CB_button_delete(FL_OBJECT *obj, long) {
  ((ImageManDashboard*) ImageFormGfx::objdash(obj))->delete_gui();
}

void ImageManDashboard::delete_gui() {
  if(active()) {
    int which = fl_get_browser(fdui()->browser_imagelist) - 1;
    if(which >= 0) {
      garcon()->log(menucommand() + "delete number=" + to_charstring(which));
      delete_image(which);
    }
  }
}

ImageManDashboard::DeleteCmd::DeleteCmd(ImageManDashboard *dash)
  : CommandM("delete", "delete image from the gallery"),
    imd(dash)
{
  AddArgument(this, "number", which);
}

CommandFn ImageManDashboard::DeleteCmd::func() {
  imd->delete_image(which);
}

CommandM *ImageManDashboard::DeleteCmd::clone() const {
  DeleteCmd *dc = new DeleteCmd(imd);
  dc->which = which;
  return dc;
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

// retrieve an image from the gallery

void ImageManDashboard::retrieve(int which) {
  if(which < 0 || which >= goof()->gallerysize()) {
    garcon()->msout << ms_error << "Bad image number!" << endl
		    << ms_normal;
    return;
  }
  imageform()->set_picture(*goof()->get_image(which));
}

void ImageManDashboard::CB_browser_imagelist(FL_OBJECT*, long) {}

void ImageManDashboard::CB_button_retrieve(FL_OBJECT *obj, long) {
  ((ImageManDashboard*) ImageFormGfx::objdash(obj))->retrieve_gui();
}

void ImageManDashboard::retrieve_gui() {
  if(active()) {
    int which = fl_get_browser(fdui()->browser_imagelist) - 1;
    if(which >= 0) {
      garcon()->log(menucommand() + "retrieve number=" + to_charstring(which));
      retrieve(which);
    }
  }
}

ImageManDashboard::RetrieveCmd::RetrieveCmd(ImageManDashboard *dash)
  : CommandM("retrieve", "retrieve an image from the gallery"),
    imd(dash)
{
  AddArgument(this, "number", which);
}

CommandFn ImageManDashboard::RetrieveCmd::func() {
  imd->retrieve(which);
}

CommandM *ImageManDashboard::RetrieveCmd::clone() const {
  RetrieveCmd *rc = new RetrieveCmd(imd);
  rc->which = which;
  return rc;
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

// Save an image to a file. The "Save" button at the bottom of the
// ImageForm saves the image as it's currently displayed. The "Save"
// buttons on this dashboard save the raw image, without
// magnification, selected pixels, mesh, etc.

void ImageManDashboard::CB_button_save_current(FL_OBJECT *obj, long) {
  ((ImageManDashboard*) ImageFormGfx::objdash(obj))->save_current_gui();
}

void ImageManDashboard::CB_button_save_selected(FL_OBJECT *obj, long) {
  ((ImageManDashboard*) ImageFormGfx::objdash(obj))->save_selected_gui();
}

FileName ImageManDashboard::get_filename() const {
  static FileName lastname;
  FileName filename;
  if(lastname)
    filename = FileName(fl_show_fselector("Enter PPM file name", ".",
					  "*.p[bgp]m", lastname.charstar()));
  else
    filename = FileName(fl_show_fselector("Enter PPM file name", ".",
					  "*p[bgp]m", ""));
  if(filename != "")
    lastname = filename;
  return filename;
}

void ImageManDashboard::save_current_gui() {
  if(active()) {
    FileName filename(get_filename());
    if(filename != "") {
      garcon()->log(menucommand() + "save_current filename=" + filename);
      imageform()->current_picture().save(filename);
    }
  }
}

void ImageManDashboard::save_selected_gui() {
  if(active()) {
    FileName filename(get_filename());
    if(filename != "") {
      int which = fl_get_browser(fdui()->browser_imagelist) - 1;
      if(which >= 0) {
	garcon()->log(menucommand() +
		      "save_selected number=" + to_charstring(which) +
		      " filename=" + filename);
	goof()->get_image(which)->save(filename);
      }
    }
  }
}

ImageManDashboard::SaveCurrentCmd::SaveCurrentCmd(ImageManDashboard *dash)
  : CommandM("save_current", "save image to a file"),
    imd(dash)
{
  AddArgument(this, "filename", filename);
}

ImageManDashboard::SaveSelectedCmd::SaveSelectedCmd(ImageManDashboard *dash)
  : CommandM("save_selected", "save image to a file"),
    imd(dash)
{
  AddArgument(this, "filename", filename);
  AddArgument(this, "number", which);
}

CommandFn ImageManDashboard::SaveCurrentCmd::func() {
  imd->imageform()->current_picture().save(filename);
}

CommandFn ImageManDashboard::SaveSelectedCmd::func() {
  imd->goof()->get_image(which)->save(filename);
}

CommandM *ImageManDashboard::SaveCurrentCmd::clone() const {
  SaveCurrentCmd *sc = new SaveCurrentCmd(imd);
  sc->filename = filename;
  return sc;
}

CommandM *ImageManDashboard::SaveSelectedCmd::clone() const {
  SaveSelectedCmd *sc = new SaveSelectedCmd(imd);
  sc->filename = filename;
  sc->which = which;
  return sc;
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

void ImageManDashboard::update_gallery_listing(const Vec<NamedImage*> &gallery)
{
  if(textmode) return;
  fl_freeze_form(imageform()->xform());

  int selected = fl_get_browser(fdui()->browser_imagelist);
  int maxline = fl_get_browser_maxline(fdui()->browser_imagelist);

  fl_clear_browser(fdui()->browser_imagelist);
  for(int i=0; i<gallery.capacity(); i++)
    fl_add_browser_line(fdui()->browser_imagelist, gallery[i]->name());

  // select the same line in the browser, even though it probably
  // won't have the same contents that it did before.
  if(selected <= 0)
    fl_select_browser_line(fdui()->browser_imagelist, 1);
  else if(selected > maxline)
    fl_select_browser_line(fdui()->browser_imagelist, maxline);
  else
    fl_select_browser_line(fdui()->browser_imagelist, selected);

  fl_unfreeze_form(imageform()->xform());
}
