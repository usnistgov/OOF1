// -*- C++ -*-
// $RCSfile: hashmesh.C,v $
// $Revision: 1.3 $
// $Author: langer $
// $Date: 2000-09-06 20:11:01 $

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


#ifndef HASHMESH_C
#define HASHMESH_C

#include "hashmesh.h"
#include "meshcoord.h"
#include "shape.h"

template <class TYPE>
void HashMesh<TYPE>::deletemesh() {
    if(mesh) {
	for(int i=0; i<nx; i++) {
	    for(int j=0; j<ny; j++) {
		HMLink *hml = mesh[i][j];
		while(hml) {
		    HMLink *next = hml->next;
		    delete hml;
		    hml = next;
		}
	    }
	    delete [] mesh[i];
	}
	delete [] mesh;
    }
}


template <class TYPE>
void HashMesh<TYPE>::setsize(int nxx, int nyy, const Rectangle &range) {
    deletemesh();
    
    nx = (nxx==0 ? 1 : nxx);
    ny = (nyy==0 ? 1 : nyy);
    xmin = range.xmin;
    ymin = range.ymin;
    stepx = (range.xmax - xmin)/nx;
    stepy = (range.ymax - ymin)/ny;

    mesh = new HMLink**[nx];
    for(int i=0; i<nx; i++) {
	mesh[i] = new HMLink*[ny];
	for(int j=0; j<ny; j++)
	    mesh[i][j] = 0;
    }
}

/* which bin would the object be in if it were there? */
template <class TYPE>
void HashMesh<TYPE>::whichbin(const MeshCoord &where,
						int &i, int &j) const {
    i = int((where.x - xmin)/stepx);
    if(i == nx) i--;
    j = int((where.y - ymin)/stepy);
    if(j == ny) j--;
}

template <class TYPE>
void HashMesh<TYPE>::add(TYPE *what) {
    int i, j;
    whichbin((what->*position)(grid->enhancement()), i, j);
    mesh[i][j] = new HMLink(what, mesh[i][j]);
}

template <class TYPE>
TYPE *HashMesh<TYPE>::find(const MeshCoord &coord) const {
  int i, j;
  TYPE *closest = 0;
  double dist;
  double closestdist = 1.e100;
  whichbin(coord, i, j);
  for(int ii=i-1; ii<=i+1; ii++) {
    if(ii < 0 || ii >= nx) continue;
    for(int jj=j-1; jj<=j+1; jj++) {
      if(jj < 0 || jj >= ny) continue;
      HMLink *hml = mesh[ii][jj];
      while(hml) {
	if(hml->which->hashfunc(coord, dist)) {
	  if(dist < closestdist) {
	    closestdist = dist;
	    closest = hml->which;
	  }
	}
	hml = hml->next;
      }
    }
  }
  return closest;
}

#endif
