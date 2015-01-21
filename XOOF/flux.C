// -*- C++ -*-
// $RCSfile: flux.C,v $
// $Revision: 1.2 $
// $Author: langer $
// $Date: 2000-12-15 22:20:21 $

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */

#include "grid.h"
#include "nodegroups.h"
#include "sparselink.h"
#include <math.h>

// Compute fluxes through edges, assuming that the given NodeGroup
// demarcates the entire edge.  Results are undefined for other
// NodeGroup geometries.

static int nodenumber(const Element *el, const Node *node) {
  for(int i=0; i<el->corner.capacity(); i++)
    if(el->corner[i] == node)
      return i;
  return -6;			// shouldn't happen
}

double Grid::flux(const NodeGroup &nodegroup,
		  double (*fun)(Element*, const MeshCoord&))
{
  update_node_elists();

  // first find which nodes in the group are connected
  SparseLinkMatrix<Element*> connection;
  Element *elem;
  for(int i=0; i<nodegroup.size()-1; i++) {
    for(int j=i+1; j<nodegroup.size(); j++) {
      elem = nodegroup[i]->neighboring(nodegroup[j]);
      if(elem)
	connection(i,j) = elem;
    }
  }

  // loop over edges
  SparseLinkIterator<Element*> iter(connection);
  int i, j;
  double fluxtotal = 0;
  while(iter(i, j, elem)) {
    // make sure that i comes before j when going counterclockwise around elem.
    int ni = nodenumber(elem, nodegroup[i]);// 0, 1, or 2
    int nj = nodenumber(elem, nodegroup[j]); // 0, 1, or 2
    if((ni-nj+3)%3 == 1) {
      int tmp = i;
      i = j;
      j = tmp;
    }

    // compute the normal to the edge
    MeshCoord side = nodegroup[j]->original() - nodegroup[i]->original();
    double length = sqrt(dot(side, side));
    if(length > 0.0) {
      MeshCoord normal(side.y, -side.x); // points out from element
      normal /= length;

      fluxtotal += (*fun)(elem, normal)*length;
    }
  }

  return fluxtotal;
}
