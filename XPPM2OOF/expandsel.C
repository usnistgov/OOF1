// -*- C++ -*-
// $RCSfile: expandsel.C,v $
// $Revision: 1.4 $
// $Author: langer $
// $Date: 2000-10-30 19:23:24 $


/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */

#include "expandsel.h"
#include "goof.h"
#include "ppm2oof.h"

// Expand a group by howfar pixels in all directions

enum Marker {UNMARKED, MARKED, UNMARKABLE};

int Goof::expandgroup(PixelGroup &grp, double howfar) {
  // Make an array in which to mark added pixels
  Array<Marker> newcells(height, width);
  newcells.clear(UNMARKED);
  for(int k=0; k<grp.size(); k++)	// pixels currently in group
    newcells[grp[k]] = UNMARKABLE;

  int count = 0;
  double d2 = howfar*howfar;
  for(int i=0; i<grp.size(); i++) {
    const Cell_coordinate &here(grp[i]);
    int xmin = int(here.x - howfar);
    int xmax = int(here.x + howfar);
    int ymin = int(here.y - howfar);
    int ymax = int(here.y + howfar);
    if(xmin < 0) xmin = 0;
    if(xmax >= width) xmax = width - 1;
    if(ymin < 0) ymin = 0;
    if(ymax >= height) ymax = height - 1;

    // Mark all pixels within howfar of a group member

//   g++ doesn't like this!
//      ArrayIterator iter(ImageRect(Cell_coordinate(xmin, ymin),
//  				 Cell_coordinate(xmax, ymax)));
    ImageRect r(Cell_coordinate(xmin, ymin), Cell_coordinate(xmax, ymax));
    ArrayIterator iter(r);
    
    Cell_coordinate test;
    while(iter(test)) {
      if(newcells[test] == UNMARKED) {
	int dx = test.x - here.x;
	int dy = test.y - here.y;
	if((dx != 0 || dy != 0) && (dx*dx + dy*dy <= d2)) {
	  ++count;
	  newcells[test] = MARKED;
	}
      }
    }
  }

  // Make a list of all active marked pixels
  Vec<Cell_coordinate> list(count);
  ArrayIterator iter2(newcells);
  Cell_coordinate pxl;
  int m=0;
  while(iter2(pxl))
    if(current_goof->active(pxl) && newcells[pxl] == MARKED)
      list[m++] = pxl;

  // Put marked pixels in the group
  grp.append(list);
  grp.weed();
  return count;
}

int Goof::expandselection(double howfar) {
  init_selection(1);
  return expandgroup(*selected_grp, howfar);
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

// remove all pixels that aren't completely surrounded by other pixels
// in the group.

int Goof::shrinkgroup(PixelGroup &grp, double howfar) {
  PixelGroup oldgrp(grp);	// make a copy of the group
  oldgrp.weed();
  grp.clear();
  Array<int> cells(height, width);
  cells.clear(0);
  for(int i=0; i<oldgrp.size(); i++)
    cells[oldgrp[i]] = 1;

  double d2 = howfar*howfar;

  for(int j=0; j<oldgrp.size(); j++) {
    const Cell_coordinate &here(oldgrp[j]);
    if(active(here)) {
      int xmin = int(here.x - howfar);
      int xmax = int(here.x + howfar);
      int ymin = int(here.y - howfar);
      int ymax = int(here.y + howfar);
      if(xmin < 0) xmin = 0;
      if(xmax >= width) xmax = width - 1;
      if(ymin < 0) ymin = 0;
      if(ymax >= height) ymax = height - 1;

      //   g++ doesn't like this!
      //    ArrayIterator iter(ImageRect(Cell_coordinate(xmin, ymin),
      //				 Cell_coordinate(xmax, ymax)));
      ImageRect r(Cell_coordinate(xmin, ymin), Cell_coordinate(xmax, ymax));
      ArrayIterator iter(r);
      Cell_coordinate test;
      int surrounded = 1;
      while(surrounded && iter(test)) {
	int dx = test.x - here.x;
	int dy = test.y - here.y;
	if(!cells[test] && (dx*dx + dy*dy <= d2))
	  surrounded = 0;
      }
      if(surrounded)
	grp.append(here);
    }
    else			// inactive cells stay selected
      grp.append(here);
  }
  return oldgrp.size() - grp.size();
}

int Goof::shrinkselection(double howfar) {
  init_selection(1);
  return shrinkgroup(*selected_grp, howfar);
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

CommandFn ExpandSelectionCmd::func() {
  if(goofinit()) {
    int n = current_goof->expandselection(range);
    garcon()->msout << ms_info << n << " additional pixels selected."
		    << endl << ms_normal;
    current_goof->redraw();
  }
}

CommandM *ExpandSelectionCmd::clone() const {
  ExpandSelectionCmd *esc = new ExpandSelectionCmd;
  esc->range = range;
  return esc;
}

CommandFn ShrinkSelectionCmd::func() {
  if(goofinit()) {
    int n = current_goof->shrinkselection(range);
    garcon()->msout << ms_info << n << " pixels deselected."
		    << endl << ms_normal;
    current_goof->redraw();
  }
}

CommandM *ShrinkSelectionCmd::clone() const {
  ShrinkSelectionCmd *esc = new ShrinkSelectionCmd;
  esc->range = range;
  return esc;
}
