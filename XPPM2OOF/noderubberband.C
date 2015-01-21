// -*- C++ -*-
// $RCSfile: noderubberband.C,v $
// $Revision: 1.3 $
// $Author: langer $
// $Date: 2000-09-06 20:11:42 $

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */

// Draw a node being moved

#include "adaptmesh.h"
#include "amtriangle.h"
#include "colorutils.h"
#include "goof.h"
#include "imagecanvas.h"
#include "meshcoord.h"
#include "noderubberband.h"
#include "rubberband.h"

NodeRubberBand::NodeRubberBand(const AdaptiveMesh *mesh,
			       const ImageCanvas *canvas,
			       const MeshCoord &down,
			       const XPoint &p1, const XPoint &p2,
			       Window w, unsigned long pxl)
  : RubberBand(p1, p2, w, pxl)
{
  // Find node closest to starting point
  node = mesh->closest_node(down);

  // Find other nodes
  Vec<AMNode*> othernode;
  int size = node->triangle.capacity();
  if(node->top() || node->btm() || node->lft() || node->rgt())
    size++;
  othernode.setphysicalsize(size);
  int i;
  for(i=0; i<node->triangle.capacity(); i++)  // loop over triangles
    node->triangle[i]->list_other_nodes(node, othernode);
  
  // get XPoints for other nodes
  segments.resize(othernode.capacity());
  XPoint offset = canvas->get_scroll_pixels();
  for(i=0; i<othernode.capacity(); i++) {
    XPoint pt(canvas->mesh2xpoint(othernode[i]->coord()));
    segments[i].x1 = pt.x - offset.x;
    segments[i].y1 = pt.y - offset.y;
  }
  XSetLineAttributes(gfxinfo.display(), gc, 2, LineSolid, CapButt, JoinBevel);
  draw();
}

NodeRubberBand::~NodeRubberBand() {
  draw();
}

void NodeRubberBand::draw() const {
  for(int i=0; i<segments.capacity(); i++) {
    segments[i].x2 = current.x;
    segments[i].y2 = current.y;
  }
  XDrawSegments(gfxinfo.display(), window, gc,
		&segments[0], segments.capacity());
}
