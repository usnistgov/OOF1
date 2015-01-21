// -*- C++ -*-
// $RCSfile: despeckle.C,v $
// $Revision: 1.8 $
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

#include "despeckle.h"
#include "goof.h"
#include "pixelgroups.h"
#include "ppm2oof.h"

static Cell_coordinate north(0, 1);
static Cell_coordinate east(1, 0);
static Cell_coordinate northeast(1, 1);
static Cell_coordinate northwest(-1, 1);

// count number of selected neighbors
static int Nnbrs(const Cell_coordinate &who, const Array<int> &sel) {
  int h = current_goof->query_height() - 1;
  int w = current_goof->query_width() - 1;
  
  int howmany = 0;	// number of neighbors selected
  if(who.x > 0) if(sel[who - east]) howmany++;
  if(who.x < w) if(sel[who + east]) howmany++;
  if(who.y > 0) if(sel[who - north]) howmany++;
  if(who.y < h) if(sel[who + north]) howmany++;
  if(who.x > 0 && who.y > 0) if(sel[who - northeast]) howmany++;
  if(who.x < w && who.y < h) if(sel[who + northeast]) howmany++;
  if(who.x < w && who.y > 0) if(sel[who - northwest]) howmany++;
  if(who.x > 0 && who.y < h) if(sel[who + northwest]) howmany++;
  
  return howmany;
}

static Cell_coordinate umklapp(Cell_coordinate who, int w, int h) {
  if(who.x < 0) who.x += w;
  if(who.x >= w) who.x -= w;
  if(who.y < 0) who.y += h;
  if(who.y >= h) who.y -= h;
  return who;
}

static int Nnbrs_pbc(const Cell_coordinate &who, const Array<int> &sel) {
  // this version uses periodic boundary conditions. Not very useful
  // in general, but is fun for "life".
  int h = current_goof->query_height() - 1;
  int w = current_goof->query_width() - 1;
  int howmany = 0;	// number of neighbors selected
  if(sel[umklapp(who - east, w, h)]) howmany++;
  if(sel[umklapp(who + east, w, h)]) howmany++;
  if(sel[umklapp(who - north, w, h)]) howmany++;
  if(sel[umklapp(who + north, w, h)]) howmany++;
  if(sel[umklapp(who - northeast, w, h)]) howmany++;
  if(sel[umklapp(who + northeast, w, h)]) howmany++;
  if(sel[umklapp(who - northwest, w, h)]) howmany++;
  if(sel[umklapp(who + northwest, w, h)]) howmany++;
  
  return howmany;
}

// is a pixel active and not selected?
static inline bool actunsel(const Cell_coordinate &who, const Array<int> &sel) {
  return current_goof->active(who) && !sel[who];
}

// add unselected neighbors to neighbor list
static void addnbrs_unsel(const Cell_coordinate &who, const Array<int> &sel,
			  PixelGroup &nbr)
{
  int h = current_goof->query_height() - 1;
  int w = current_goof->query_width() - 1;
  
  if(who.x > 0) if(actunsel(who - east, sel)) nbr.append(who-east);
  if(who.x < w) if(actunsel(who + east, sel)) nbr.append(who+east);
  if(who.y > 0) if(actunsel(who - north, sel)) nbr.append(who-north);
  if(who.y < h) if(actunsel(who + north, sel)) nbr.append(who+north);
  if(who.x > 0 && who.y > 0)
    if(actunsel(who - northeast, sel))
      nbr.append(who - northeast);
  if(who.x < w && who.y < h)
    if(actunsel(who + northeast, sel))
      nbr.append(who + northeast);
  if(who.x < w && who.y > 0)
    if(actunsel(who - northwest, sel))
      nbr.append(who - northwest);
  if(who.x > 0 && who.y < h)
    if(actunsel(who + northwest, sel))
      nbr.append(who + northwest);
}

// is a pixel active and selected?
static inline bool actsel(const Cell_coordinate &who, const Array<int> &sel) {
  return current_goof->active(who) && sel[who];
}

// add selected neighbors to neighbor list
static void addnbrs_sel(const Cell_coordinate &who, const Array<int> &sel,
			PixelGroup &nbr)
{
  int h = current_goof->query_height() - 1;
  int w = current_goof->query_width() - 1;
  
  if(who.x > 0) if(actsel(who - east, sel)) nbr.append(who-east);
  if(who.x < w) if(actsel(who + east, sel)) nbr.append(who+east);
  if(who.y > 0) if(actsel(who - north, sel)) nbr.append(who-north);
  if(who.y < h) if(actsel(who + north, sel)) nbr.append(who+north);
  if(who.x > 0 && who.y > 0)
    if(actsel(who - northeast, sel))
      nbr.append(who - northeast);
  if(who.x < w && who.y < h)
    if(actsel(who + northeast, sel))
      nbr.append(who + northeast);
  if(who.x < w && who.y > 0)
    if(actsel(who - northwest, sel))
      nbr.append(who - northwest);
  if(who.x > 0 && who.y < h)
    if(actsel(who + northwest, sel))
      nbr.append(who + northwest);
}

// ---------------------------------------------------------------- //

static void set_selected(const Array<int> &sel, PixelGroup *grp) {
  grp->clear();
  ArrayIterator iter(current_goof->material);
  Cell_coordinate pixel;
  while(iter(pixel)) {
    if(sel[pixel])
      grp->append(pixel);
  }
}

static void get_selected(const PixelGroup *grp, Array<int> &sel) {
  sel.clear(0);
  for(int i=0; i<grp->size(); i++)
    sel[(*grp)[i]] = 1;
}

// ---------------------------------------------------------------- //

// select all pixels with at least nbr_threshold selected neighbors

void Goof::despeckle(const int nbr_threshold) {
  init_selection(1);
  int more = doDespeckle(nbr_threshold);
  garcon()->msout << ms_info << "   selected " <<  more << " additional pixel"
		  << (more == 1? "" : "s") << endl << ms_normal;
}

// ---------------------------------------------------------------- //

// select all pixels with at least nbr_threshold selected neighbors
// without printing the number of additional pixels selected

int Goof::doDespeckle(const int nbr_threshold) {

  //  init_selection(1);
  int initial = selected_grp->size();
  
  // array of selected pixels
  Array<int> sel(height, width);
  get_selected(selected_grp, sel);
  
  // make array of neighbors
  PixelGroup neighbor;
  ArrayIterator iter(material);
  Cell_coordinate pixel;
  while(iter(pixel))
    if(sel[pixel])
      addnbrs_unsel(pixel, sel, neighbor);
  neighbor.weed();
  
  while(neighbor.size() > 0) {
    Cell_coordinate candidate(neighbor[neighbor.size()-1]);
    neighbor.trim(candidate);
    if(Nnbrs(candidate, sel) >= nbr_threshold) {
      sel[candidate] = 1;
      addnbrs_unsel(candidate, sel, neighbor);
    }
  }
  
  // update the selected list
  set_selected(sel, selected_grp);
  
  return selected_grp->size() - initial;
}

// ----------------------------------------------------------- //

// unselect all pixels with fewer than nbr_threshold selected neighbors

void Goof::elkcepsed(const int nbr_threshold) {
  init_selection(1);
  int initial = selected_grp->size();
  
  // array of selected pixels
  Array<int> sel(height, width);
  get_selected(selected_grp, sel);
  
  // Group of candidates for unselecting
  PixelGroup candidates = *selected_grp;
  candidates.weed();
  
  while(candidates.size() > 0) {
    Cell_coordinate pixel(candidates[candidates.size()-1]);
    candidates.trim(pixel);	// remove from list of candidates
    if(active(pixel) && Nnbrs(pixel, sel) < nbr_threshold) {
      sel[pixel] = 0;
      addnbrs_sel(pixel, sel, candidates); // add selected nbrs to list
    }
  }
  
  // update the selected list
  set_selected(sel, selected_grp);
  
  int fewer = initial - selected_grp->size();
  garcon()->msout << ms_info << "   unselected " <<  fewer << " pixel"
		  << (fewer == 1? "" : "s") << endl << ms_normal;
  
}

// ----------------------------------------------------------- //

static void lifecycle(const PixelGroup *original, PixelGroup *final, int pbc) {
  ArrayIterator iter(current_goof->material);
  Cell_coordinate pixel;
  Array<int> sel(current_goof->query_height(), current_goof->query_width());
  sel.clear(0);
  final->clear();
  for(int i=0; i<original->size(); i++)
    sel[(*original)[i]] = 1;
  while(iter(pixel)) {
    if(current_goof->active(pixel)) {
      int s = sel[pixel];
      int n = (pbc? Nnbrs_pbc(pixel, sel) : Nnbrs(pixel, sel));
      if((s && (n == 2 || n == 3)) || (!s && n == 3))
	final->append(pixel);
    }
    else if(sel[pixel])
      final->append(pixel);
  }
  current_goof->selected() = *final;
}

CommandFn LifeCmd::func() {
  if(!goofinit()) return;
  current_goof->init_selection(1);
  PixelGroup testtube1, testtube2;
  int npix = current_goof->query_height()*current_goof->query_width();
  testtube1.setphysicalsize(npix);
  testtube2.setphysicalsize(npix);
  testtube1 = current_goof->selected();
  PixelGroup *thisgeneration = &testtube1;
  PixelGroup *nextgeneration = &testtube2;
  
  Interrupter stop;
  
  for(int i=0; i<maxiter && !stop(); i++) {
    lifecycle(thisgeneration, nextgeneration, pbc);
    current_goof->redraw();
    if(thisgeneration == &testtube1) {
      thisgeneration = &testtube2;
      nextgeneration = &testtube1;
    }
    else {
      thisgeneration = &testtube1;
      nextgeneration = &testtube2;
    }
  }
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

CommandFn DespeckleCmd::func() {
  if(!goofinit()) return;
  if(nnbrs > 8 || nnbrs < 4) {
    garcon()->msout << ms_error
		    << "The only reasonable values are 4, 5, 6, 7, 8"
		    << endl << ms_normal;
    return;
  }
  current_goof->despeckle(nnbrs);
  current_goof->redraw();
}

CommandFn elkcepseDCmd::func() {
  if(!goofinit()) return;
  current_goof->elkcepsed(nnbrs);
  current_goof->redraw();
}
