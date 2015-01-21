// -*- C++ -*-
// $RCSfile: selectdrawer.C,v $
// $Revision: 1.3 $
// $Author: langer $
// $Date: 2000-09-06 20:11:14 $


/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


#include "colorutils.h"
#include "formdrawer.h"
#include "griditerator.h"
#include "node.h"
#include "oofcanvas.h"
#include "pixmap.h"
#include "selectdrawer.h"
#include <X11/cursorfont.h>


static Drawer *create_SelectDrawer(Grid *g, FormDrawer *fd) {
  return new SelectDrawer(g, fd);
}

DrawerRegistration SelectDrawer::registration("Mesh", create_SelectDrawer, 0.0);

// ---------------------------------------------------------- //

SelectDrawer::SelectDrawer(Grid *g, FormDrawer *fd)
  : MeshDrawer(g, fd),
    blushmap(Ngrays),
    nodecolor(0, 0, 255),
    selectattributes(this)
{
  Colormap colormap = formdrawer->canvas->get_colormap();
  attributes = &selectattributes;
  switch(gfxinfo.c_class()) {
  case TrueColor:
    nodecolor.RO_pixel();
    blushmap.fill_RO();
    break;
  case PseudoColor:
    nodecolor.RW_pixel(colormap, FREECOL + Ngrays + 1);
    blushmap.fill_RW(colormap, FREECOL + Ngrays + 2);
    break;
  default:
    cerr << "SelectDrawer: unsupported X visual" << endl;
  }

  add_dashboard(&coorddashboard);
  add_dashboard(&selectattributes);
  add_dashboard(&elementinfo);
  add_dashboard(&nodeinfo);
}

// ------------------------------------------------------------- //

void SelectDrawer::draw() {
  if(grid) {
    MeshDrawer::draw();
    if(selectattributes.hot_nodes) drawnodes();
  }
}

void SelectDrawer::drawelements() {
  Element *el;
  ElementIterator iter(selectattributes.empties?EI_ALL:EI_NONEMPTY, grid);
  while(el = iter()) {
    if(selectattributes.hot_elements && el->isselected())
      formdrawer->paintelement(el, blushmap.pixel(el->intrinsic_gray));
    else
      formdrawer->paintelement(el, graymap.pixel(el->intrinsic_gray));
  }
}

void SelectDrawer::drawelement(const Element *el) {
  if(selectattributes.elements &&
     (selectattributes.empties || !el->isempty())) {
    if(selectattributes.hot_elements && el->isselected())
      formdrawer->paintelement(el, blushmap.pixel(el->intrinsic_gray));
    else
      formdrawer->paintelement(el, graymap.pixel(el->intrinsic_gray));
  }
  if(selectattributes.edges)
    drawedges(el);

  // draw nodes, since element will have overdrawn them
  if(selectattributes.hot_nodes) {
    for(int i=0; i<el->corner.capacity(); i++)
      drawnode(el->corner[i]);
  }
}

void SelectDrawer::drawnodes() {
  if(selectattributes.hot_nodes && !hold())
    for(int i=0; i<grid->node.capacity(); i++) {
      Node *n = grid->node[i];
      if(n->isselected())
	formdrawer->paintnode(n, selectattributes.noderadius, nodecolor.pixel);
    }
}

void SelectDrawer::drawnode(const Node *n) {
  if(selectattributes.hot_nodes && !hold() && n->isselected())
      formdrawer->paintnode(n, selectattributes.noderadius, nodecolor.pixel);
}
