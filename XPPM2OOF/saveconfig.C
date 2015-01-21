// -*- C++ -*-
// $RCSfile: saveconfig.C,v $
// $Revision: 1.9 $
// $Author: langer $
// $Date: 2001-05-08 21:16:18 $



/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */

#include "adaptmesh.h"
#include "goof.h"
#include "imageform.h"
#include "material.h"
#include "compressedfile.h"
#include "ppm2oof.h"
#include "saveconfig.h"
#include <iostream.h>

static const int output_cfgversion = 3;
static int input_cfgversion = 1;
static istream *configfile;

// When reading a config file, other routines may need to know the
// version number.

int configversion() {
  return input_cfgversion;
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

class GoofCmd : public CommandM {
private:
  int height, width;
public:
  GoofCmd() : CommandM("goof", "create a new goof"),
    width(0), height(0)
  {
    AddArgument(this, "height", height);
    AddArgument(this, "width", width);
  }
  virtual void func() {
#ifdef DEBUG
    cerr << "creating Goof" << endl << flush;
#endif // DEBUG
    current_goof = new Goof(height, width);
  }
  virtual CommandM *clone() const {
    GoofCmd *gc = new GoofCmd;
    gc->width = width;
    gc->height = height;
    return gc;
  }
};

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

class LoadImageCmd : public CommandM {
private:
  CharString name;
public: LoadImageCmd() : CommandM("image", "load image")
  {
    AddArgument(this, "name", name);
  }
  virtual CommandFn func() {
#ifdef DEBUG
    cerr << "reading image " << name << "..." << flush;
#endif // DEBUG
    Image picture;
    picture.read_image(*configfile);
    current_goof->addimage(picture, name);
#ifdef DEBUG
    cerr << " ok" << endl;
#endif // DEBUG
  }
  virtual CommandM *clone() const {
    LoadImageCmd *lic = new LoadImageCmd;
    lic->name = name;
    return lic;
  }
};

void Goof::savegallery(ostream &file) const {
  for(int i=0; i<gallery.capacity(); i++) {
    file <<  "image name=" << gallery[i]->name() << endl;
    file << *gallery[i] << endl;
  }
  file << endl;
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

static const int no_material = -1;

void Goof::savematerials(ostream &file) const {
  // Materials are saved in the order in which they were created, so
  // their index data is correct.  The config file just uses the
  // commands in the main material menu -- they will create the
  // Materials but won't assign them to pixels because there's no
  // current selection yet.  So it's important that the materials are
  // saved before the selections are!
  for(int i=0; i<Material::allmaterials.capacity(); i++)
    file << "ppm2oof material " << Material::allmaterials[i]->parameters\
	 << endl;
  file << "materials" << endl;
  ArrayIterator iter(material);
  Cell_coordinate cell;
  while(iter(cell)) {
    if(material[cell])
      file << material[cell]->index() << endl;
    else
      file << no_material << endl;
  }
}

static CommandFn loadmaterials() {
  Cell_coordinate cell;
  ArrayIterator iter(current_goof->material);
#ifdef DEBUG
  cerr << "Reading " << current_goof->material.query_height() << "x"
       << current_goof->material.query_width() << " array of materials..."
       << flush;
#endif // DEBUG
  while(iter(cell) && *configfile) {
    int mat;
    *configfile >> mat;
    if(mat != no_material)
      current_goof->assign_material(cell, Material::allmaterials[mat]);
  }
#ifdef DEBUG
  cerr << " ok" << endl;
#endif // DEBUG
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

void Goof::savefirewalls(ostream &file) const {
  file << "firewalls" << endl;
  Cell_coordinate cell;
  ArrayIterator iter(fireproof);
  while(iter(cell))
    file << fireproof[cell] << endl;
}

static CommandFn loadfirewalls() {
#ifdef DEBUG
  cerr << "reading firewalls..." << flush;
#endif // DEBUG
  Cell_coordinate cell;
  ArrayIterator iter(current_goof->fireproof);
  while(iter(cell) && *configfile)
    *configfile >> current_goof->fireproof[cell];
#ifdef DEBUG
  cerr << " ok" << endl;
#endif // DEBUG
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

void Goof::savegroups(ostream &file) const {
  for(int i=0; i<grouplist.capacity(); i++) {
    const PixelGroup &grp = *grouplist[i];
#ifdef DEBUG
    cerr << "saving group " << grp.query_name() << " with "
	 << grp.size() << " pixels" << endl;
#endif // DEBUG
    file << "group name=" << grp.query_name()
	 << " npixels=" << grp.size() << endl;
    for(int j=0; j<grp.size(); j++)
      file << grp[j] << endl;
  }
}

class LoadGroupCmd : public CommandM {
private:
  int npixels;
  CharString name;
public:
  LoadGroupCmd() : CommandM("group", "load a group"),
    npixels(0)
  {
    AddArgument(this, "name", name);
    AddArgument(this, "npixels", npixels);
  }
  virtual CommandFn func() {
#ifdef DEBUG
    cerr << "loading group " << name << " with "
	 << npixels << " pixels..." << flush;
#endif // DEBUG
    PixelGroup *grp = current_goof->add_group(name);
    grp->setphysicalsize(npixels);
    for(int i=0; i<npixels; i++) {
      Cell_coordinate cell;
      *configfile >> cell;
      grp->append(cell);
    }
#ifdef DEBUG
    cerr << " ok" << endl;
#endif // DEBUG
  }
  virtual CommandM *clone() const {
    LoadGroupCmd *lgc = new LoadGroupCmd;
    lgc->npixels = npixels;
    lgc->name = name;
    return lgc;
  }
};

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

void Goof::saveselections(ostream &file) const {
  // All Goofs are constructed with an initial empty selection. We
  // don't want to save it, so this loop starts with i=1.
  for(int i=1; i<selections.capacity(); i++) {
    const PixelGroup &sel = *selections[i];
    file << "selection npixels=" << sel.size() << endl;
    for(int j=0; j<sel.size(); j++)
      file << sel[j] << endl;
  }
  file << "currentselection=" << selected_index << endl;
}

class LoadSelectionsCmd : public CommandM {
private:
  int npixels;
public:
  LoadSelectionsCmd()
    : CommandM("selection", "load a set of selected pixels"),
      npixels(0)
  {
    AddArgument(this, "npixels", npixels);
  }
  virtual void func() {
    // make a list of pixels in the selection
    Vec<Cell_coordinate> pxl(npixels);
#ifdef DEBUG
    cerr << "Reading selection with " << npixels << " pixels..." << flush;
#endif // DEBUG
    for(int i=0; i<npixels && *configfile; i++) 
      *configfile >> pxl[i];

    // select all pixels at once
    // override may be set in menu commands that were read at the
    // start of the configuration file, so it has to be overridden
    // here.
    TrueFalse active_override_save = Goof::active_override;
    Goof::active_override = TF_FALSE;
    current_goof->init_selection(0);
    current_goof->select(pxl);
    Goof::active_override = active_override_save;
#ifdef DEBUG
    cerr << " ok" << endl;
#endif // DEBUG
  }
  virtual CommandM *clone() const {
    LoadSelectionsCmd *lsc = new LoadSelectionsCmd;
    lsc->npixels = npixels;
    return lsc;
  }
};

static int currentselection = 0;

static void set_currentselection(const CharString&) {
  current_goof->set_selection(currentselection);
}

void Goof::set_selection(int which) {
  if(which < selections.capacity()) {
    selected_index = which;
    selected_grp = selections[which];
  }
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

static void write_activearea(const ActiveArea &active, ostream &file) {
  ArrayIterator iter(active);
  Cell_coordinate cell;
  while(iter(cell))
    if(active[cell])		// write coords of active pixels
      file << cell << endl;
}

void Goof::saveactiveareas(ostream &file) const {
  for(int i=0; i<stored_active_areas.capacity(); i++) {
    const ActiveArea &active = *stored_active_areas[i];
    file << "activearea name=" << active.name
	 << " npixels=" << query_width()*query_height()-active.ninactive
	 << endl;
    write_activearea(active, file);
  }
  if(current_active_area.ninactive != 0) {
    file << "currentactivearea npixels="
	 << query_height()*query_width()-current_active_area.ninactive << endl;
    write_activearea(current_active_area, file);
  }
}


class LoadActiveAreaCmd : public CommandM {
private:
  CharString name;
  int npixels;
public:
  LoadActiveAreaCmd()
    : CommandM("activearea", "load a stored active area")
  {
    AddArgument(this, "name", name);
    AddArgument(this, "npixels", npixels);
  }
  virtual void func() {
#ifdef DEBUG
    cerr << "reading active area " << name << "..." << endl << flush;
#endif // DEBUG
    PixelGroup grp;		// store pixels in here temporarily
    grp.setphysicalsize(npixels);
    for(int i=0; i<npixels; i++) {
      Cell_coordinate cell;
      *configfile >> cell;
      grp.append(cell);
    }
    
    current_goof->remove_restrictions();
    current_goof->activate_group(grp, Goof::AM_INTERSECT);
    current_goof->store_active_area(name);
    current_goof->remove_restrictions();
#ifdef DEBUG
    cerr << " ok" << endl;
#endif // DEBUG
  }
  virtual CommandM *clone() const {
    LoadActiveAreaCmd *laac = new LoadActiveAreaCmd;
    laac->name = name;
    laac->npixels = npixels;
    return laac;
  }
};

class LoadCurrentActiveAreaCmd : public CommandM {
private:
  int npixels;
public:
  LoadCurrentActiveAreaCmd()
    : CommandM("currentactivearea", "load a stored active area")
  {
    AddArgument(this, "npixels", npixels);
  }
  virtual void func() {
#ifdef DEBUG
    cerr << "reading current active area..." << flush;
#endif // DEBUG
    PixelGroup grp;		// store pixels in here temporarily
    grp.setphysicalsize(npixels);
    for(int i=0; i<npixels; i++) {
      Cell_coordinate cell;
      *configfile >> cell;
      grp.append(cell);
    }

    current_goof->remove_restrictions();
    current_goof->activate_group(grp, Goof::AM_INTERSECT);
#ifdef DEBUG
    cerr << " ok" << endl;
#endif // DEBUG
  }
  virtual CommandM *clone() const {
    LoadCurrentActiveAreaCmd *lcaac = new LoadCurrentActiveAreaCmd;
    lcaac->npixels = npixels;
    return lcaac;
  }
};

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

void Goof::savemeshes(ostream &file) const {
  for(CircleStackIterator<AdaptiveMesh*> iter(meshstack); !iter.end(); ++iter)
    {
      file << "mesh" << endl;
      meshstack[iter]->save(file);
    }
}

void Goof::readmesh(istream &file) {
#ifdef DEBUG
  cerr << "Reading mesh..." << flush;
#endif // DEBUG
  add_mesh(new AdaptiveMesh);
  the_mesh->load(file);
//   the_mesh->inherit_pixel_groups(0);
//   the_mesh->inherit_pixel_materials(0); // Not forced! Some triangles
	// already have read their materials and groups (since they have
	// inhibit_inheritance set) and we don't want to override
	// them.
#ifdef DEBUG
  cerr << " ok" << endl;
#endif // DEBUG
}

CommandFn read_mesh() {
  current_goof->readmesh(*configfile);
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

static CommandFn read_imageform() {
#ifdef DEBUG
  cerr << "reading imageform..." << flush;
#endif // DEBUG
  ImageForm::read_imageform(*configfile);
#ifdef DEBUG
  cerr << " ok" << endl;
#endif // DEBUG
}

static CommandFn read_commands() {
  // Everything from here on in the config file will be simply loaded
  // into the command log, so this command had better be the last one
  // in the file.
#ifdef DEBUG
  cerr << "reading commands..." << flush;
#endif // DEBUG
  garcon()->readlog(*configfile);
#ifdef DEBUG
  cerr << " ok" << endl;
#endif // DEBUG
}

static CommandFn configfile_cmd() {
  // This command does nothing... A command with the same name in the
  // main ppm2oof menu transfers control to the config menu, though.
  // That lets the same routines load either a config file or a log
  // file.
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

Menu *configmenu() {
  static Menu *menu = 0;
  if(!menu) {
    menu = new Menu("load");
    // commands only used when parsing the config file
    AddVariable(menu, "cfgversion", "configuration file version number",
		input_cfgversion);
    menu->AddCommand(new GoofCmd);
    menu->AddCommand("imageform", "read imageform data", read_imageform);
    menu->AddCommand(new LoadImageCmd);
    menu->AddCommand("materials", "load materials", loadmaterials);
    menu->AddCommand("firewalls", "load firewalls", loadfirewalls);
    menu->AddCommand(new LoadGroupCmd);
    menu->AddCommand(new LoadSelectionsCmd);
    AddVariable(menu, "currentselection", "current selection",
		currentselection, set_currentselection);
    menu->AddCommand(new LoadActiveAreaCmd);
    menu->AddCommand(new LoadCurrentActiveAreaCmd);
    menu->AddCommand("mesh", "read a mesh", read_mesh);
    // all of the regular ppm2oof commands
    menu->AddSubMenu("ppm2oof", "all of the ppm2oof commands", ppm2oofmenu());
    menu->AddCommand("commands", "read command history", read_commands);
    menu->AddCommand("configfile", "a dummy command", configfile_cmd);
  }
  return menu;
}

void loadconfig(const CharString &filename) {
  CompressedFile file(filename);
  if(!file.ok()) return;
  loadconfig(file.stream());
}
  
void loadconfig(istream &file) {
  configfile = &file;

#ifdef IOSTREAM_HAS_EXCEPTIONS
  configfile->exceptions(ios::badbit);
#endif // IOSTREAM_HAS_EXCEPTIONS

  if(current_goof)
    delete current_goof;

  garcon()->graphics_off();
  garcon()->inhibit_logging();
#ifdef IOSTREAM_HAS_EXCEPTIONS
  try {
    garcon()->runfile(*configfile, configmenu(), 0);
  }
  catch(ios::failure)
#else
    garcon()->runfile(*configfile, configmenu(), 0);
    if(configfile->bad())
#endif // IOSTREAM_HAS_EXCEPTIONS
      {
	garcon()->msout << ms_error << "Error reading configuration file!"
			<< endl << ms_normal;
	if(current_goof)
	  delete current_goof;
	current_goof = 0;
      }
    //    cerr << "Final state of configuration file:" << endl
    //  	    << "  good = " << configfile->good() << endl
    //  	    << "   eof = " << configfile->eof() << endl
    //  	    << "  fail = " << configfile->fail() << endl
    //  	    << "   bad = " << configfile->bad() << endl;
    garcon()->disinhibit_logging();
    garcon()->graphics_on();
    if(current_goof)
      current_goof->redraw();
}

void saveconfig(ostream &file, const Goof *goof) {
  // The order in which things are saved depends on the order in which
  // they must be read.

  // Zeroth, label this file as a config file, instead of a log file.
  file << "configfile" << endl;
  file << "cfgversion=" << output_cfgversion << endl;

  //  First, save the minimal information necessary to make a goof.
  file << "goof height=" << current_goof->query_height()
       << " width=" << current_goof->query_width() << endl;

  // Save all menu variables.
  ppm2oofmenu()->print(file, "ppm2oof", TF_TRUE);

  ImageForm::save_imageforms(file);
  goof->savegallery(file); // Save the images.
  goof->savematerials(file);
  goof->savefirewalls(file);
  goof->savegroups(file);	// save pixel groups
  goof->saveselections(file);
  goof->saveactiveareas(file);
  goof->savemeshes(file);

  // Save the command history. This must be done last!
  file << "commands" << endl;
  garcon()->printlog(file);
}
