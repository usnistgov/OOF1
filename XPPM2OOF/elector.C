// -*- C++ -*-
// $RCSfile: elector.C,v $
// $Revision: 1.5 $
// $Author: langer $
// $Date: 2004-10-23 00:48:47 $


// Machinery to take care of homogeneity calculations.

// Elector classes are used to vote to find the most popular kind of
// pixel in a mesh triangle. There is only one of each type of
// Elector, so the constructors are private. These functions return
// the global Electors.

#include "adaptmesh.h"
#include "cell_coordinate.h"
#include "elector.h"
#include "goof.h"
#include "material.h"

void Elector::vote(const Cell_coordinate& pxl, double weight) {
  nvotes += weight;
  for(int j=0; j<catalog.capacity(); j++) {
    if(sameparty(pxl, catalog[j])) {
      abundance[j] += weight;
      return;
    }
  }
  // found a new category of pixel
  catalog.grow(1, pxl);
  abundance.grow(1, weight);
}

void Elector::tally() {
  // find out who won the election
  double max = 0;
  int winnerno = -1;
  for(int i=0; i<catalog.capacity(); i++) {
    if(abundance[i] > max) {
      max = abundance[i];
      winnerno = i;
    }
  }
  vote_fraction = max/nvotes;
  winner_ = catalog[winnerno];
  // get ready for next election
  catalog.resize(0);
  abundance.resize(0);
  nvotes = 0;
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

// Elector functions to decide if two pixels are the same:

// are two lists of groups the same?
static int comparegroups(LinkList<PixelGroup*> &g1, LinkList<PixelGroup*> &g2) {
  if(g1.size() != g2.size()) return 0;
  for(LinkListIterator<PixelGroup*> i1=g1.begin(), i2=g2.begin();
      !i1.end() && !i2.end();
      ++i1, ++i2) 
    //    if(g1[i1]->query_name() != g2[i2]->query_name())
    if(g1[i1] != g2[i2])
      return 0;
  return 1;
}

int GroupElector::sameparty(const Cell_coordinate &c1,
			    const Cell_coordinate &c2) const
{
  Material *mat1 = current_goof->material[c1];
  Material *mat2 = current_goof->material[c2];

  if(mattype || mattypeparam) {
    // if one material isn't defined, they aren't the same
    if((mat1 && !mat2) || (mat2 && !mat1)) return 0;
    
    if(mat1 && mat2) {
      // do the pixels have the same material type?
      if(mattype && mat1->tag() != mat2->tag()) return 0;
      // do the pixels have the same gray value?
      if(gray && mat1->query_gray() != mat2->query_gray()) return 0;
      // are the materials exactly the same?
      if(mattypeparam && !(*mat1 == *mat2)) return 0;
    }
  }

  if(groupmemb) {		// are the pixels in the same groups?
    if(!comparegroups(current_goof->pixelgrouplist[c1],
		      current_goof->pixelgrouplist[c2]))
       return 0;
  }
  return 1;
}

int MaterialElector::sameparty(const Cell_coordinate &c1,
			    const Cell_coordinate &c2) const
{
  Material *mat1 = current_goof->material[c1];
  Material *mat2 = current_goof->material[c2];

  // if one material isn't defined, they aren't the same
  if((mat1 && !mat2) || (mat2 && !mat1)) return 0;
  if(mat1 && mat2) {
    // do the pixels have the same gray value?
    if(gray && mat1->query_gray() != mat2->query_gray()) return 0;
    // if both are defined, but they're different, they're not the same
    if(!(*mat1 == *mat2)) return 0;
  }
  
  if(groupmemb) {		// are the pixels in the same groups?
    if(!comparegroups(current_goof->pixelgrouplist[c1],
		      current_goof->pixelgrouplist[c2]))
      return 0;
  }
  return 1;
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

int GroupElector::badrules() {
  return !(mattype || mattypeparam || groupmemb);
}
