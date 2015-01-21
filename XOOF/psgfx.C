// -*- C++ -*-
// $RCSfile: psgfx.C,v $
// $Revision: 1.1 $
// $Author: langer $
// $Date: 2001-02-18 02:54:50 $

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */

#include "attributes.h"
#include "colordrawer.h"
#include "formdrawer.h"
#include "griditerator.h"
#include "meshdrawer.h"
#include "node.h"
#include "oofcanvas.h"
#include "parameters.h"
#include "selectdrawer.h"
#include "shape.h"
#include "version.h"
#include <stdio.h>
#include <time.h>

// init_psgfx is mostly copied from the FOAM program, which was
// written in C, so some of its features are irrelevant here, and some
// are not C++-like.

static double pointsize;  // 1/72 inch on the page, in natural units

// Here, "natural" units are the units of the thing being drawn (ie,
// width of a pixel = 1 for ppm2oof).  They're what are called
// "physical" units in scrollcanvas.C, but that could be confused with
// the page dimensions here.

static void init_psgfx(FILE *file,
		       double width, // width in inches on the page
		       bool landscape, bool flipx, bool flipy,
		       const Rectangle &range) // in natural coord system
{
  time_t t = time(NULL);
  double scalefactor = 72*width/range.width();
  pointsize = 1/scalefactor;

//   double xmin = range.ll.x;
//   double xmax = range.ur.x;
//   double ymin = range.ll.y;
//   double ymax = range.ur.y;

  /* height of image in inches */
  double height = width*range.height()/range.width();

  /* coordinates of bounding box in points in original page coord system */
  int bbxmin, bbxmax, bbymin, bbymax;

  /* size of page in inches */
  double pagew = 8.5;
  double pageh = 11.0;

  fputs("%!PS-Adobe-3.0 EPSF-3.0\n", file);

  if(!landscape) {
    bbxmin = int(0.5*(pagew - width)*72);
    bbxmax = int(bbxmin + 72*width);
    bbymin = int(0.5*(pageh - height)*72);
    bbymax = int(bbymin + 72*height);
  }
  else {
    bbxmin = int(0.5*(pagew - height)*72);
    bbxmax = int(bbxmin + 72*height);
    bbymin = int(0.5*(pageh - width)*72);
    bbymax = int(bbymin + 72*width);
  }
  fprintf(file, "%%%%BoundingBox: %d %d %d %d\n",
	  bbxmin, bbymin, bbxmax, bbymax);
  fputs("%%Creator: oof " + oofversion() + "\n", file);
  fprintf(file, "%%%%CreationDate: %s", ctime(&t));
  fputs("%%EndComments\n", file);
  fputs("%%EndProlog\n", file);
  fputs("gsave\n", file);

  fputs("\n% set coordinate system\n", file);
  
  if(landscape) {
    fprintf(file, "90 rotate\n");
    fprintf(file, "0 -612 translate\n");  /* 612 = 8.5*72 */
  }

  /* move origin to proper corner of bounding box */
  if(flipx) {
    if(landscape)
      fprintf(file, "%e 0 translate\n", 36*(pageh+width));
    else
      fprintf(file, "%e 0 translate\n", 36*(pagew+width));
  }
  else {   /* !flipx */
    if(landscape)
      fprintf(file, "%e 0 translate\n", 36*(pageh-width));
    else
      fprintf(file, "%e 0 translate\n", 36*(pagew-width));
  }
  if(flipy) {
    if(landscape)
      fprintf(file, "0 %e translate\n", 36*(pagew+height));
    else
      fprintf(file, "0 %e translate\n", 36*(pageh+height));
  }
  else {  /* !flipy */
    if(landscape)
      fprintf(file, "0 %e translate\n", 36*(pagew-height));
    else
      fprintf(file, "0 %e translate\n", 36*(pageh-height));
  }
  fprintf(file, "%e %e scale\n",
	  (flipx?-1:1)*scalefactor,
	  (flipy?-1:1)*scalefactor);

  /* move origin to proper point w/in bounding box
   * (compensate for xmin != 0)
   */
  fprintf(file, "%e %e translate\n", -range.xmin, -range.ymin);
  
  /* from here on, all postscript graphics is done using physical coordinates */

  /* set clipping region */
  fputs("\n% Set clipping region\n", file);
  fprintf(file, "newpath\n");
  fprintf(file, "%e %e moveto\n", range.xmin, range.ymin);
  fprintf(file, "%e %e lineto\n", range.xmin, range.ymax);
  fprintf(file, "%e %e lineto\n", range.xmax, range.ymax);
  fprintf(file, "%e %e lineto closepath clip\n", range.xmax, range.ymin);
}

static void end_psgfx(FILE *file) {
  fputs("showpage grestore\n", file);
}

static void setcolor(FILE *file, const Color &color) {
  fprintf(file, "%5.3f %5.3f %5.3f setrgbcolor\n",
	  color.red(), color.green(), color.blue());
}

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//


#define WIDTH 6.0 /* width in inches of output image */

void FormDrawer::draw_ps(FILE *file) const {
  // find bounding box
  Rectangle bbox = canvas->visible_region();
  // if the image doesn't fill the visible region of the canvas,
  // reduce the size of the box
  Rectangle occupied = grid->query_extreme_pos();
  if(bbox.xmax > occupied.xmax) bbox.xmax = occupied.xmax;
  if(bbox.xmin < occupied.xmin) bbox.xmin = occupied.xmin;
  if(bbox.ymax > occupied.ymax) bbox.ymax = occupied.ymax;
  if(bbox.ymin < occupied.ymin) bbox.ymin = occupied.ymin;

  init_psgfx(file, WIDTH, false, false, false, bbox);
  fputs(
"/tricol { newpath setrgbcolor moveto lineto lineto closepath fill} def\n",
	file);
  fputs("/tri { newpath moveto lineto lineto closepath stroke} def\n", file);
  current_drawer->draw_ps(file, bbox);
  end_psgfx(file);
}

void MeshDrawer::draw_ps(FILE *file, const Rectangle &bbox) {
  if(!grid) return;
  if(attributes->elements) drawelements_ps(file, bbox);
  if(attributes->edges) drawedges_ps(file, bbox);
  drawextras_ps(file, bbox);
}

void FormDrawer::paintelement_ps(const Element *e, const Color &color,
				 FILE *file, const Rectangle &bbox) const
{
  if(!e->outside(bbox)) {
    for(int i=0; i<e->corner.capacity(); i++) {
      MeshCoord p = e->corner[i]->position(grid->enhancement());
      fprintf(file, "%e %e ", p.x, p.y);
    }
    fprintf(file, "%5.3f %5.3f %5.3f tricol\n",
	    color.red(), color.green(), color.blue());
  }
}

void FormDrawer::paintedges_ps(const Element *e,
			       FILE *file, const Rectangle &bbox) const
{
  if(!e->outside(bbox)) {
    for(int i=0; i<e->corner.capacity(); i++) {
      MeshCoord p = e->corner[i]->position(grid->enhancement());
      fprintf(file, "%e %e ", p.x, p.y);
    }
    fputs("tri\n", file);
  }
}

void FormDrawer::paintnode_ps(const Node *node, int radius,
			      FILE *file, const Rectangle &bbox) const
{
  MeshCoord p = node->position(grid->enhancement());
  if(bbox.contains(p)) {
    fprintf(file, "newpath %e %e %e 0 360 arc fill\n",
	    p.x, p.y, radius*pointsize);
  }
}

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

void MeshDrawer::drawelements_ps(FILE *file, const Rectangle &bbox) {
  Element *el;
  ElementIterator iter(showingempties()?EI_ALL:EI_NONEMPTY, grid);
  while(el = iter())
    formdrawer->paintelement_ps(el, graymap.color(el->intrinsic_gray),
				file, bbox);
}

void MeshDrawer::drawedges_ps(FILE *file, const Rectangle &bbox) const {
  fputs("gsave\n", file);
  setcolor(file, edgecolor);
  fprintf(file, "%e setlinewidth\n", attributes->edgewidth*pointsize);
  
  Element *el;
  ElementIterator iter(showingempties()? EI_ALL:EI_NONEMPTY, grid);
  while(el = iter())
    formdrawer->paintedges_ps(el, file, bbox);
  fputs("grestore\n", file);
}

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

void SelectDrawer::draw_ps(FILE *file, const Rectangle &bbox) {
  if(grid) {
    MeshDrawer::draw_ps(file, bbox);
    if(selectattributes.hot_nodes) drawnodes_ps(file, bbox);
  }
}

void SelectDrawer::drawelements_ps(FILE *file, const Rectangle &bbox) {
  Element *el;
  ElementIterator iter(selectattributes.empties?EI_ALL:EI_NONEMPTY, grid);
  while(el = iter()) {
    if(selectattributes.hot_elements && el->isselected())
      formdrawer->paintelement_ps(el, blushmap.color(el->intrinsic_gray),
				  file, bbox);
    else
      formdrawer->paintelement_ps(el, graymap.color(el->intrinsic_gray),
				  file, bbox);
  }
}

void SelectDrawer::drawnodes_ps(FILE *file, const Rectangle &bbox) const {
  if(selectattributes.hot_nodes) {
    fputs("gsave\n", file);
    setcolor(file, nodecolor);
    for(int i=0; i<grid->node.capacity(); i++) {
      Node *n = grid->node[i];
      if(n->isselected())
	formdrawer->paintnode_ps(n, selectattributes.noderadius, file, bbox);
    }
    fputs("grestore\n", file);
  }
}

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

void ColorDrawer::drawelements_ps(FILE *file, const Rectangle &bbox) {
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
	formdrawer->paintelement_ps(el,
				 colormap->color(1-(values[i]-plotmin)*scale),
				 file, bbox);
      i++;
    }
  }
  else {
    while(el = iter()) {
      if(showingempties() || !el->isempty())
	formdrawer->paintelement_ps(el,
				    colormap->color((values[i]-plotmin)*scale),
				    file, bbox);
      i++;
    }
  }
}

void ColorDrawer::drawextras_ps(FILE *file, const Rectangle &bbox) const {
  plotdashboard.draw_plotline_ps(file, bbox);
}


void PlotDashboard::draw_plotline_ps(FILE *file, const Rectangle&) const {
  if(linedrawn) {
    fputs("% cross section plot line\n", file);
    fputs("gsave\n", file);
    fprintf(file, "%e setlinewidth\n", pointsize);
    setcolor(file, dynamic_cast<MeshDrawer*>(drawer)->plotlinecolor);
    fprintf(file, "newpath %e %e moveto %e %e lineto stroke\n",
	    currentplot.start.x, currentplot.start.y,
	    currentplot.finish.x, currentplot.finish.y);
    fputs("grestore\n", file);
  }
}
