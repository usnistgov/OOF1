// -*- C++ -*-
// $RCSfile: colordrawer.C,v $
// $Revision: 1.3 $
// $Author: langer $
// $Date: 2000-09-06 20:10:49 $

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */

#include "colordrawer.h"
#include "colorutils.h"
#include "elementfunction.h"
#include "formdrawer.h"
#include "griditerator.h"
#include "oofcanvas.h"


ColorDrawer::ColorDrawer(Grid *g, FormDrawer *fd)
  : MeshDrawer(g, fd),
    valmax(1.0),
    valmin(0.0),
    values(0, BlockSize(64)),
    simpleattributes(this),
    colordashboard(this),
    plotdashboard(this)
{
  colordashboard.displaylimits(); // display range of values and of plot

  colormap =
    colormapregistry()[colordashboard.colormapnumber]
                                 ->create(colordashboard.colormapsize);
  fill_colormap();
  colordashboard.paint_colorbar();

  attributes = &simpleattributes;
  add_dashboard(&colordashboard);
  add_dashboard(&simpleattributes);
  add_dashboard(&coorddashboard);
  add_dashboard(&elementinfo);
  add_dashboard(&nodeinfo);
  add_dashboard(&plotdashboard);
}

ColorDrawer::~ColorDrawer() {
  delete colormap;
}

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

void ColorDrawer::setvalues() {
  if(gridchanged_time >= setvalues_time) {
    if(grid) {
      ++setvalues_time;

      // have to loop over all elements, including empties, so data is in order
      ElementIterator iter(EI_ALL, grid);
      
      Element *el;
      valmin = 1.e100;
      valmax = -1.e100;
      values.resize(0);
      garcon()->graphics_off();
      while(el = iter()) {
	double v = func()(*el);
	values.grow(1, v);
	if(!el->isempty()) {  // don't include empty elements in range
	  if(v > valmax) valmax = v;
	  if(v < valmin) valmin = v;
	}
      }
      garcon()->graphics_on();
    }
    else {
      valmin = 0.0;
      valmax = 1.0;
    }

    if(colordashboard.fullscale_flag)
      fullscale();
    else
      colordashboard.displaylimits();
  }
}

void ColorDrawer::gridchanged() {
  ++gridchanged_time;
}

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

void ColorDrawer::fullscale() {
  plotmin = valmin;
  plotmax = valmax;
  colordashboard.displaylimits();
  //formdrawer->draw();
}

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

void ColorDrawer::fill_colormap() {
  Colormap cmap = formdrawer->canvas->get_colormap();
  switch(gfxinfo.c_class()) {
  case TrueColor:
    colormap->fill_RO();
    break;
  case PseudoColor:
    colormap->fill_RW(cmap, FREECOL);
    break;
  default:
    cerr << "ColorDrawer: unsupported X visual" << endl;
  }
}


//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

void ColorDrawer::drawelements() {

  setvalues();

  Element *el;
  int i = 0;
  double scale = 0.0;
  if(plotmax != plotmin)
    scale = 1./(plotmax - plotmin);
  ElementIterator iter(EI_ALL, grid);
  if(colordashboard.flipcolors) {
    while(el = iter()) {
      if(showingempties() || !el->isempty())
	formdrawer->paintelement(el,
				 colormap->pixel(1-(values[i]-plotmin)*scale));
      i++;
    }
  }
  else {
    while(el = iter()) {
      if(showingempties() || !el->isempty())
	formdrawer->paintelement(el,
				 colormap->pixel((values[i]-plotmin)*scale));
      i++;
    }
  }
}

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

void ColorDrawer::drawelement(const Element *el) {

  setvalues();

  // This is really ugly. We have to find the index of this element in
  // the list of values. It's probably not a problem, though, since
  // this routine is unlikely to be called.
  ElementIterator iter(EI_ALL, grid);
  Element *eel;
  int i = 0;
  while(eel = iter()) {
    if(el == eel) {
      if(showingempties() || !el->isempty()) {
	double scale = 0;
	if(plotmax != plotmin)
	  scale = 1./(plotmax - plotmin);
	if(colordashboard.flipcolors)
	  formdrawer->paintelement(el,
				   colormap->pixel(1-(values[i]-plotmin)*scale));
	else
	  formdrawer->paintelement(el,
				   colormap->pixel((values[i]-plotmin)*scale));
      }
      return;
    }
    i++;
  }
}

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

void ColorDrawer::drawextras() {
  plotdashboard.draw_plotline();
}
