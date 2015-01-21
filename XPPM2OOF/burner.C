// -*- C++ -*-
// $RCSfile: burner.C,v $
// $Revision: 1.10 $
// $Author: langer $
// $Date: 2000-10-30 19:23:23 $



/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


#include "burner.h"
#include "goof.h"
#include "vec.h"

void Burner::ignite(const Image &img, const Cell_coordinate &where,
		    int shiftkey)
{
  nburned_ = 0;
  forest = &img;
  fireproof = &current_goof->fireproof;
  startingpoint = where;
  startingcolor = (*forest)[startingpoint];
  burned.resize(forest->query_size());
  burned.clear(false);
  
  burn(startingpoint);
  
  current_goof->init_selection(shiftkey);
  Vec<Cell_coordinate> burnt;
  store_burned(burnt);
  current_goof->select(burnt);
}

void Burner::igniteNoUndo(const Image &img, const Cell_coordinate &where, 
			  int shiftkey)
{
  //same as above ignite, but no undo information is saved

  nburned_ = 0;
  forest = &img;
  fireproof = &current_goof->fireproof;
  startingpoint = where;
  startingcolor = (*forest)[startingpoint];
  burned.resize(forest->query_size());
  burned.clear(false);
  
  burn(startingpoint);
  
  current_goof->selected().clear();
  Vec<Cell_coordinate> burnt;
  store_burned(burnt);
  current_goof->select(burnt);
}


// The recursive burning algorithm is simpler than the non-recursive
// one, and uses one fewer large array. But it uses more stack space,
// and can seg fault when it runs out of stack space on large
// burns. The non-recursive version uses more memory for small burns
// and is more complicated, but doesn't have stack problems and
// doesn't seem to run any slower. So the non-recursive version is
// preferred.

//#define RECURSIVE
#ifdef RECURSIVE

void Burner::burn(const Cell_coordinate &here) {
  if(!current_goof->active(here) || (fireproof && (*fireproof)[here]))
    // can't burn this pixel
    return;
  nburned_++;
  burned[here] = true;
  Color thiscolor((*forest)[here]);
  // this relies on the order in which nbrs are listed in the Nbr class:
  int nbrmax = (next_nearest? 8 : 4);
  for(int i=0; i<nbrmax; i++) {
    Cell_coordinate target = here + neighbor[i];
    if(forest->contains(target) && !burned[target] &&
       spread(thiscolor, (*forest)[target]))
      burn(target);
  }   
}

void Burner::burn_nbrs(Vec<Cell_coordinate>&, const Cell_coordinate&) {}
// only used in non-recursive method

#else  // not RECURSIVE

void Burner::burn_nbrs(Vec<Cell_coordinate> &activelist,
			   const Cell_coordinate &here)
{
  // Burn neighboring pixels and put them in the active list if
  // they're flammable
  int nbrmax = (next_nearest? 8 : 4);
  Color thiscolor((*forest)[here]);
  for(int i=0; i<nbrmax; i++) {
    Cell_coordinate target = here + neighbor[i];
    if(forest->contains(target) && !burned[target]
       && (!fireproof || !(*fireproof)[target])
       && current_goof->active(target)
       && spread(thiscolor, (*forest)[target])) {
      burned[target] = true;
      nburned_++;
      activelist.grow(1, target);
    }
  }
}

void Burner::burn(const Cell_coordinate &start) { // non-recursive
  // see if the first pixel is flammable
  if(!current_goof->active(start) || (fireproof && (*fireproof)[start]))
    return;

  // active sites are sites whose neighbors have to be checked
  Vec<Cell_coordinate> activesites;
  // preallocate maximum amount of space to avoid reallocation
  activesites.setphysicalsize(forest->query_height()*forest->query_width());

  // burn the first pixel
  burned[start] = true;
  nburned_++;
  activesites.grow(1, start);

  while(activesites.capacity() > 0) {
    // Remove last site in the active list, burn its neighbors, and add
    // them to the list.
    int n = activesites.capacity()-1;
    const Cell_coordinate here = activesites[n];
    activesites.resize(n);	// remove last active pixel from list
    burn_nbrs(activesites, here);
  }
}

#endif // RECURSIVE

void Burner::store_burned(Vec<Cell_coordinate> &list) const {
  list.resize(0);
  list.setphysicalsize(nburned_);
  ArrayIterator iter(burned);
  Cell_coordinate pxl;
  while(iter(pxl))
    if(burned[pxl])
      list.grow(1, pxl);
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

Burner::Nbr Burner::neighbor;

Burner::Nbr::Nbr() {
  // don't change the order here without changing Burner::burn(), or
  // the nearest neighbor/next nearest neighbor distinction will be
  // screwed up.
  nbr[0] = Cell_coordinate( 0, -1);
  nbr[1] = Cell_coordinate(-1,  0);
  nbr[2] = Cell_coordinate( 1,  0);
  nbr[3] = Cell_coordinate( 0,  1);
  nbr[4] = Cell_coordinate(-1, -1);
  nbr[5] = Cell_coordinate( 1, -1);
  nbr[6] = Cell_coordinate(-1,  1);
  nbr[7] = Cell_coordinate( 1,  1);
}
