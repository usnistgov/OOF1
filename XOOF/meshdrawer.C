// -*- C++ -*-
// $RCSfile: meshdrawer.C,v $
// $Revision: 1.3 $
// $Author: langer $
// $Date: 2000-09-06 20:11:07 $


/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


#include "attributes.h"
#include "colorutils.h"
#include "formdrawer.h"
#include "graphics.h"
#include "griditerator.h"
#include "menucolor.h"
#include "meshdrawer.h"
#include "oofcanvas.h"
#include "pixmap.h"
#include <math.h>

const int MeshDrawer::Ngrays = 64;//number of grays for element intrinsic colors

unsigned long const MeshDrawer::BLACKPIXEL = 0;
unsigned long const MeshDrawer::WHITEPIXEL = 1;
unsigned long const MeshDrawer::WALLPAPER_FG = 2;
unsigned long const MeshDrawer::WALLPAPER_BG = 3;
unsigned long const MeshDrawer::EDGECOLOR = 4;
unsigned long const MeshDrawer::RUBBERBANDCOLOR = 5;
unsigned long const MeshDrawer::PLOTLINECOLOR = 6;
unsigned long const MeshDrawer::FREECOL = 7;

MeshDrawer::MeshDrawer(Grid *g, FormDrawer *fd)
  : Drawer(g, fd),
    graymap(Ngrays),
    coorddashboard(this),
    elementinfo(this),
    nodeinfo(this),
    black_(0, 0, 0),
    white_(255, 255, 255),
    edgecolor(0, 0, 0),
    rubberbandcolor(255, 255, 255),
    plotlinecolor(0, 255, 0),	// ugly -- shouldn't be here
    wallpaper_fg_(wallpaper_foreground), // make copies of globals, since
    wallpaper_bg_(wallpaper_background)  // pixel value may be different
{
  // install colors in colormap
  Colormap colormap = formdrawer->canvas->get_colormap();
  switch(gfxinfo.c_class()) {
  case TrueColor:
    white_.RO_pixel();
    black_.RO_pixel();
    wallpaper_fg_.RO_pixel();
    wallpaper_bg_.RO_pixel();
    edgecolor.RO_pixel();
    rubberbandcolor.RO_pixel();
    plotlinecolor.RO_pixel();
    graymap.fill_RO();
    break;
  case PseudoColor:
    black_.RW_pixel(colormap, BLACKPIXEL);
    white_.RW_pixel(colormap, WHITEPIXEL);
    wallpaper_fg_.RW_pixel(colormap, WALLPAPER_FG);
    wallpaper_bg_.RW_pixel(colormap, WALLPAPER_BG);
    edgecolor.RW_pixel(colormap, EDGECOLOR);
    rubberbandcolor.RW_pixel(colormap, RUBBERBANDCOLOR);
    plotlinecolor.RW_pixel(colormap, PLOTLINECOLOR);
    graymap.fill_RW(colormap, FREECOL);
    break;
  default:
    cerr << "MeshDrawer: unsupported X visual" << endl;
  }
  wallpaper(use_wallpaper);
  formdrawer->canvas->clearpixmap();
}

MeshDrawer::~MeshDrawer() {
}

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

int MeshDrawer::showingempties() const {
  return attributes->showingempties();
}

void MeshDrawer::scrollcallback(const MeshCoord &ul, const MeshCoord &lr)
{
  coorddashboard.set_coord_text(ul, lr);
}

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

void MeshDrawer::draw() {
  if(hold()) return;
  if(!grid) return;
  formdrawer->canvas->clearpixmap();
  if(attributes->elements) drawelements();
  if(attributes->edges) drawedges();
  drawextras();
}

void MeshDrawer::drawelements() {
  Element *el;
  ElementIterator iter(showingempties()?EI_ALL:EI_NONEMPTY, grid);
  while(el = iter())
    formdrawer->paintelement(el, graymap.pixel(el->intrinsic_gray));
}


void MeshDrawer::drawedges() {
  Element *el;
  ElementIterator iter(showingempties()?EI_ALL:EI_NONEMPTY, grid);
  XSetLineAttributes(gfxinfo.display(), formdrawer->canvas->gc(),
		     attributes->edgewidth,
		     LineSolid, CapButt, JoinBevel);
  while(el = iter())
    formdrawer->paintedges(el, edgecolor.pixel);
}

void MeshDrawer::drawedges(const Element *el) {
  XSetLineAttributes(gfxinfo.display(), formdrawer->canvas->gc(),
		     attributes->edgewidth,
		     LineSolid, CapButt, JoinBevel);
  formdrawer->paintedges(el, edgecolor.pixel);
}

void MeshDrawer::drawelement(const Element *el) {
  if(attributes->elements && (showingempties() || !el->isempty()))
    formdrawer->paintelement(el, graymap.pixel(el->intrinsic_gray));
  if(attributes->edges)
    drawedges(el);
}

void MeshDrawer::drawnodes() {}

void MeshDrawer::drawnode(const Node *) {}




