// -*- C++ -*-
// $RCSfile: arson.C,v $
// $Revision: 1.3 $
// $Author: langer $
// $Date: 2000-10-30 19:23:22 $


// Completely burn the whole system by picking a random pixel, burning
// from there, making the selected pixels into a group and making them
// fireproof, and repeating until all pixels are fireproof. The group
// names are constructed from a format string. A '#' in the string is
// replaced by successive integers.

#include "arson.h"
#include "burn.h"
#include "goof.h"
#include "mouseclick.h"
#include "pixelgroups.h"
#include "ppm2oof.h"
#include "random.h"


ArsonCmd::ArsonCmd()
  : CommandM("arson", "burn all pixels and put them in groups"),
    fmt("group#")
{
  AddArgument(this, "local_flmblty", burner.lcl);
  AddArgument(this, "global_flmblty", burner.glbl);
  AddArgument(this, "use_L2_norm", burner. use_L2norm );
  AddArgument(this, "burn_next_near", burner.next_nearest);
  AddArgument(this, "format", fmt);
}

CommandM *ArsonCmd::clone() const {
  ArsonCmd *ac = new ArsonCmd;
  ac->burner.lcl = burner.lcl;
  ac->burner.glbl = burner.glbl;
  ac->burner.use_L2norm = burner.use_L2norm;
  ac->burner.next_nearest = burner.next_nearest;
  ac->fmt = fmt;
  return ac;
}

void ArsonCmd::find_flammable() { // make list of flammable pixels
  flammable.clear();;
  Vec<Cell_coordinate> flist;
  flist.setphysicalsize(current_goof->query_width()*
			current_goof->query_height());
  ArrayIterator iter(current_goof->picture);
  Cell_coordinate pixel;
  while(iter(pixel))
    if(!current_goof->fireproof[pixel])
      flist.grow(1, pixel);
  flammable.append(flist);
}


void ArsonCmd::func() {
  if(!goofinit()) return;

  int ngroups = 0;
  int npixels = 0;

  // set default values for next arson attempt
  burner.lcl_dflt = burner.lcl;
  burner.glbl_dflt = burner.glbl;
  burner.L2_dflt = burner.use_L2norm;
  burner.nextnear_dflt = burner.next_nearest;

  // save firewalls
  Array<TrueFalse> save_firewalls = current_goof->fireproof;

  find_flammable();
  int groupnumber = 0;

  Interrupter stop;

  while(flammable.size() > 0 && !stop()) {  // burn it all to the ground
    current_goof->unselect_all();

    // pick a random starting point.
    Cell_coordinate start = flammable[rndm() * flammable.size()];

    // select a bunch of pixels
    burner.ignite(start, 0);
    fl_check_only_forms();
    current_goof->redraw();

    current_goof->set_firewall();    // make them fireproof

    // make a group and install the selected pixels
    CharString groupname(fmtname(groupnumber++));
    PixelGroup *group = current_goof->add_group(groupname);
    if(!group) {
      garcon()->msout << ms_info << "Overwriting group \"" << groupname
		      << "\"." << endl << ms_normal;
      current_goof->remove_group(groupname);
      group = current_goof->add_group(groupname);
    }
    group->append(current_goof->selected());

    ngroups++;
    npixels += group->size();

    find_flammable();
  }
  current_goof->fireproof = save_firewalls; // restore firewalls
  garcon()->msout << ms_info << "Created " << ngroups
		  << " new groups, containing " << npixels << " pixels."
		  << endl << ms_normal;
}


CharString ArsonCmd::fmtname(int n) const {
  CharString grpname;
  const char *ptr = &fmt[0];
  while(*ptr) {
    while(*ptr && *ptr != '#') grpname += *ptr++;  // obfuscation!
    if(*ptr == '#') {
      grpname += to_charstring(n);
      ptr++;
    }
  }
  return grpname;
}
