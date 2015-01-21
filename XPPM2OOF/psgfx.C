// -*- C++ -*-
// $RCSfile: psgfx.C,v $
// $Revision: 1.6 $
// $Author: langer $
// $Date: 2003-09-10 14:39:48 $

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */

#include "adaptmesh.h"
#include "amtriangle.h"
#include "displaydash.h"
#include "goof.h"
#include "imagecanvas.h"
#include "imageform.h"
#include "infodash.h"
#include "ishape.h"
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

  double xmin = range.ll.x;
  double xmax = range.ur.x;
  double ymin = range.ll.y;
  double ymax = range.ur.y;

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
  fputs("%%Creator: ppm2oof " + ppm2oofversion() + "\n", file);
  fprintf(file, "%%%%CreationDate: %s", ctime(&t));
  fputs("%%EndComments\n", file);
  fputs("%%EndProlog\n", file);
  fputs("gsave\n", file);

  fputs("\n/picstr 3 string def\n", file);
  fputs("/trailingimage\n", file);
  fputs("{\n\t{ currentfile picstr readhexstring pop }\n", file);
  fputs("\tfalse 3\n\tcolorimage\n} def\n", file);

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
  fprintf(file, "%e %e translate\n", -xmin, -ymin);
  
  /* from here on, all postscript graphics is done using physical coordinates */

  /* set clipping region */
  fputs("\n% Set clipping region\n", file);
  fprintf(file, "newpath\n");
  fprintf(file, "%e %e moveto\n", xmin, ymin);
  fprintf(file, "%e %e lineto\n", xmin, ymax);
  fprintf(file, "%e %e lineto\n", xmax, ymax);
  fprintf(file, "%e %e lineto closepath clip\n", xmax, ymin);
}

static void end_psgfx(FILE *file) {
  fputs("showpage grestore\n", file);
}

#define WIDTH 6.0 /* width in inches of output image */

void ImageFormGfx::draw_ps(FILE *file) const {
  // find bounding box
  Rectangle bbox(canvas->visible_region());
  // if image doesn't fill visible region of canvas, reduce the size
  // of the box
  if(bbox.ur.x > current_picture().query_width())
    bbox.ur.x = current_picture().query_width();
  if(bbox.ur.y > current_picture().query_height())
    bbox.ur.y = current_picture().query_height();
  if(bbox.ll.x < 0)
    bbox.ll.x = 0;
  if(bbox.ll.y < 0)
    bbox.ll.y = 0;

  init_psgfx(file, WIDTH, false, false, false, bbox);

  fputs(
"/tricol { newpath setrgbcolor moveto lineto lineto closepath fill} def\n",
	file);
  fputs("/tri { newpath moveto lineto lineto closepath stroke} def\n", file);
  fputs(
"/pxl { setrgbcolor newpath moveto lineto lineto lineto closepath fill} def\n",
	file);

  bool trimatdisp = displaydash->material_display_method == MDM_TRIANGLE &&
    displaydash->background == BG_MATERIAL;
  
  Image tempimage;
  if(!trimatdisp) {		// have to draw an image
    if(displaydash->background == BG_IMAGE) {
      tempimage.copy_image(current_picture());
    }
    else {
      tempimage.copy_image(goof->materialimage);
      goof->mark_nomaterial(tempimage, displaydash->nomaterialcolor);
    }
    // mark selections and other modifications
    if(displaydash->selected_pixels_visible)
      goof->mark_selected_pixels(tempimage, displaydash->selectedcolor);
    if(displaydash->firewalls_visible)
      goof->mark_firewalls(tempimage, displaydash->firewallcolor);

    // fade the pixels outside the active region
    ArrayIterator iter(tempimage);
    Cell_coordinate pxl;
    while(iter(pxl))
      if(!current_goof->active(pxl))
	tempimage[pxl] = tempimage[pxl].fade(ActiveArea::fade);
    
    fputs("%% pixels\n", file);
    localimage.draw_ps(file, bbox);
  }

  if(goof->mesh()) {
    if(trimatdisp) {		// draw filled triangles
      goof->mesh()->inherit_pixel_materials();
      goof->mesh()->setcolors(displaydash->nomaterialcolor);
      // If drawing material and selection, it's easiest to do them
      // both at once. So pretend that the material color is the
      // selected color for the selected triangles.
      if(displaydash->selected_mesh_visible)
	goof->mesh()->setselectedcolors(displaydash->selectedmeshcolor);
      fputs("%% material triangles\n", file);
      goof->mesh()->draw_material_ps(file, bbox);
    }
    else {
      // Draw selection, but not material -- not all triangles are filled
      if(displaydash->selected_mesh_visible)
	goof->mesh()->draw_selected_ps(file, displaydash->selectedmeshcolor,
				       displaydash->selectedmeshfadecolor,
				       bbox);
    }
  }

  current_dashboard()->draw_extras_ps(file, bbox);

  if(goof->mesh() && displaydash->mesh_visible) { // draw mesh last!
    fputs("%% mesh\n", file);
    goof->mesh()->draw_ps(file, displaydash->meshcolor, displaydash->meshwidth,
			  bbox);
  }

  end_psgfx(file);
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

static void ps_pixel(FILE *file, const Cell_coordinate &pxl, const Color &color)
{
  int i = pxl.x;
  int j = pxl.y;
  fprintf(file, "%d %d %d %d %d %d %d %d %5.3f %5.3f %5.3f pxl\n",
	  i, j,
	  i+1, j,
	  i+1, j+1,
	  i, j+1,
	  color.red(), color.green(), color.blue());
}

static void printhex(int val, FILE *file) {
  int hi = val/16;
  if(hi > 9)
    hi += 'a'-10;
  else
    hi += '0';
  int lo = val%16;
  if(lo > 9)
    lo += 'a'-10;
  else
    lo += '0';
  fprintf(file, "%c%c", hi, lo);
}

void Image::draw_ps(FILE *file, const Rectangle &bbox) const {
  fputs("% pixels from the image\n", file);
  fprintf(file, "gsave\n%d %d scale\n", query_width(), query_height());
  fprintf(file, "%d %d 8\n", query_width(), query_height());
  fprintf(file, "[%d 0 0 %d 0 %d]\n",
	  query_width(), -query_height(), query_height());
  fputs("trailingimage\n", file);
  
  ImageRect irect(bbox.containing_rectangle());
  int counter = 0;
  for(int j=irect.ymax-1; j>=irect.ymin; j--) {
    for(int i=irect.xmin; i<irect.xmax; i++) {
      Cell_coordinate  pxl(i,j);
      const Color &c((*this)[pxl]);
      printhex(c.query_red(), file);
      printhex(c.query_green(), file);
      printhex(c.query_blue(), file);
      if(++counter%8 == 0)
	fputs("\n", file);
    }
    if(counter%8 != 0)
      fputs("\n", file);
  }
  fputs("grestore\n", file);
	  


//   ImageRect irect(bbox.containing_rectangle());
//   for(int i=irect.xmin; i<irect.xmax; i++) {
//     for(int j=irect.ymin; j<irect.ymax; j++) {
//       Cell_coordinate pxl(i,j);
//       const Color &c((*this)[pxl]);
//       ps_pixel(file, pxl, c);
//     }
//   }
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

void AdaptiveMesh::draw_ps(FILE *file, const Color &color, int width,
			   const Rectangle &bbox) const
{
  fprintf(file, "%e setlinewidth\n", width*pointsize);
  fprintf(file, "%e %e %e setrgbcolor\n",
	  color.red(), color.green(), color.blue());
  for(AMIterator i(this, AMI_ALL); !i.end(); ++i) 
    (*this)[i]->draw_ps(file, bbox);
}

void AdaptiveMesh::draw_material_ps(FILE *file, const Rectangle &bbox) const {
  for(AMIterator i(this, AMI_ALL); !i.end(); ++i)
    (*this)[i]->fill_ps(file, bbox);
}

void AdaptiveMesh::draw_selected_ps(FILE *file,
				    const Color &activecolor,
				    const Color &inactivecolor,
				    const Rectangle &bbox) const
{
  for(AMIterator i(this, AMI_SELECTED); !i.end(); ++i) {
    AMTriangle *tri = (*this)[i];
    if(tri->active())
      tri->color = activecolor;
    else
      tri->color = inactivecolor;
    tri->fill_ps(file, bbox);
  }
}

void AMTriangle::draw_ps(FILE *file, const Rectangle &bbox) const {
  if(!outside(bbox)) {
    fprintf(file, "%e %e %e %e %e %e tri\n",
	    node[0]->coord().x, node[0]->coord().y,
	    node[1]->coord().x, node[1]->coord().y,
	    node[2]->coord().x, node[2]->coord().y);
  }
}

void AMTriangle::fill_ps(FILE *file, const Rectangle &bbox) const {
  if(!outside(bbox)) {
    fprintf(file, "%e %e %e %e %e %e %5.3f %5.3f %5.3f tricol\n",
	    node[0]->coord().x, node[0]->coord().y,
	    node[1]->coord().x, node[1]->coord().y,
	    node[2]->coord().x, node[2]->coord().y,
	    color.red(), color.green(), color.blue());
  }
}

void AMTriangle::outline_ps(FILE *file, const Color &color,
			    const Rectangle &bbox) const
{
  if(!outside(bbox)) {
    fprintf(file, "%% highlighted triangle\n");
    fprintf(file, "gsave %e setlinewidth\n", 4*pointsize);
    fprintf(file, "%5.3f %5.3f %5.3f setrgbcolor\n",
	    color.red(), color.green(), color.blue());
    draw_ps(file, bbox);
    fprintf(file, "grestore\n");
  }
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

void PixelInfoDashboard::draw_extras_ps(FILE *file, const Rectangle&) const
{
  if(textmode) return;
  if(imageform()->beenclicked && fl_get_button(fdui()->button_mark))
    ps_pixel(file, where, clicked_color);
}

void MeshInfoDashboard::draw_extras_ps(FILE *file, const Rectangle &bbox) const
{
  if(imageform()->beenclicked)
      if(clickedtriangle && highlight)
	clickedtriangle->outline_ps(file, highlight_color, bbox);
}
