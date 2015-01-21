// -*- C++ -*-
// $RCSfile: imagecanvas.C,v $
// $Revision: 1.6 $
// $Author: langer $
// $Date: 2001-01-12 19:17:31 $

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */

#include "adaptmesh.h"
#include "colorutils.h"
#include "dashboard.h"
#include "goof.h"
#include "image.h"
#include "imagecanvas.h"
#include "imageform.h"
#include "mouseclick.h"
#include "noderubberband.h"
#include "rubberband.h"
#include <math.h>
#include <X11/Xlib.h>

ImageCanvas::ImageCanvas(ImageFormGfx *iform, int x, int y, int w, int h,
			 int iw, int ih, double scalefactor,
			 void (*scb)(const MeshCoord&, const MeshCoord&, void*))
  : ScrollCanvas(iform->the_form, x, y, w, h, scb, iform),
    imagew(iw), imageh(ih),
    imageform(iform),
    rubberband(0),
    pixind_outofdate(true)
{
  // make pixmap the same size as the image
  resizepixmap(iw*scalefactor, ih*scalefactor);
  setcoords(0, iw, 0, ih);
}

// Copy an Image onto the canvas

void ImageCanvas::loadimage(Image *image) {
  imagew = image->query_width();
  imageh = image->query_height();

  ArrayIterator iter(*image);
  Cell_coordinate cell;
  if(imagew == pwidth() && imageh == pheight()) { // can copy point by point
     while(iter(cell)) {
      XSetForeground(gfxinfo.display(), gc(), (*image)[cell].pixel);
      XPoint xp = xpoint(cell.x, cell.y);
      XDrawPoint(gfxinfo.display(), pixmap(), gc(), xp.x, xp.y); 
    }
  }
  else {
    while(iter(cell)) {
      XSetForeground(gfxinfo.display(), gc(), (*image)[cell].pixel);
      XPoint ul = xpoint(cell.x, cell.y+1);
      XPoint lr = xpoint(cell.x+1, cell.y);
      XFillRectangle(gfxinfo.display(), pixmap(), gc(),
		     ul.x, ul.y, lr.x - ul.x, lr.y - ul.y);
    }
  }
}

void ImageCanvas::draw_pixels(const Vec<Cell_coordinate> &pixels,
			      const Color &color)
{
  XSetForeground(gfxinfo.display(), gc(), color.pixel);
  if(imagew == pwidth() && imageh == pheight()) {
    for(int i=0; i<pixels.capacity(); i++) {
      XPoint xp = xpoint(pixels[i].x, pixels[i].y);
      XDrawPoint(gfxinfo.display(), pixmap(), gc(), xp.x, xp.y);
    }
  }
  else {
    for(int i=0; i<pixels.capacity(); i++) {
      XPoint ul = xpoint(pixels[i].x, pixels[i].y+1);
      XPoint lr = xpoint(pixels[i].x+1, pixels[i].y);
      XFillRectangle(gfxinfo.display(), pixmap(), gc(),
		     ul.x, ul.y, lr.x - ul.x, lr.y - ul.y);
    }
  }
}

void ImageCanvas::draw_pixel(const Cell_coordinate &pixel,
			      const Color &color)
{
  XSetForeground(gfxinfo.display(), gc(), color.pixel);
  if(imagew == pwidth() && imageh == pheight()) {
    XPoint xp = xpoint(pixel.x, pixel.y);
    XDrawPoint(gfxinfo.display(), pixmap(), gc(), xp.x, xp.y);
  }
  else {
    XPoint ul = xpoint(pixel.x, pixel.y+1);
    XPoint lr = xpoint(pixel.x+1, pixel.y);
    XFillRectangle(gfxinfo.display(), pixmap(), gc(),
		   ul.x, ul.y, lr.x - ul.x, lr.y - ul.y);
  }
}

void ImageCanvas::redraw() { // called by ScrollCanvas when image needs redrawn
  imageform->draw();
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

// Mouse events are passed back to the ImageForm. The ImageForm only
// needs one mouse handler because it uses a MouseClick, although for
// generality the ScrollCanvas has three...

// min # of pixels the mouse has to move for a drag to be noticed
const int ImageCanvas::dragmove = 2;

void ImageCanvas::mouse(const MouseClick &click) {
  garcon()->log(imageform->menucommand() + "mouse click = " +
		to_charstring(click));
  
  if(holding) return;

  // Variables used during mouse motion don't have to be class
  // members, since there is only one mouse.
  static int panning = 0;	// currently panning?

  if(click.event == MOUSE_DN)
    return;			// don't do anything until mouse mv or up

  if(click.event == MOUSE_UP) {
    if(!downclick()) return;	// up but not down?

    if(rubberband) {		// stop rubberbanding
      delete rubberband;
      rubberband = 0;
    }
    if(panning) {		// stop panning
      panning = 0;
      return;
    }

    if(click.window != downclick().window) return;
    if(click.button != downclick().button) return;

    if(abs(click.xpoint.x - downclick().xpoint.x) > dragmove ||
       abs(click.xpoint.y - downclick().xpoint.y) > dragmove)
      imageform->current_dashboard()->draghandler(downclick(), click);
    else
      imageform->current_dashboard()->clickhandler(click);

    return;
  }

  if(click.event == MOUSE_MV) {
    if(click.button == 0) return; // ignore motion w/out button
    switch(dragmode) {
    case DM_RUBBERBAND:
      if(!rubberband) {
	switch(imageform->current_dashboard()->rubberbandtype) {
	case RB_RECTANGLE:
	  rubberband =
	    new RectangleRubberBand(downclick().xpoint, click.xpoint,
				    window(),
				    imageform->rubberband_color.pixel);
	  break;
	case RB_CIRCLE:
	  rubberband = 
	    new CircleRubberBand(downclick().xpoint, click.xpoint,
				 window(), imageform->rubberband_color.pixel);
	  break;
	case RB_ELLIPSE:
	  rubberband = new EllipseRubberBand(downclick().xpoint, click.xpoint,
					     window(),
					     imageform->rubberband_color.pixel);
	  break;
	case RB_NODE:
	  {
	    AdaptiveMesh *mesh = imageform->goof->mesh();
	    if(!mesh)
	      rubberband = 0;
	    else
	      rubberband =
		new NodeRubberBand(mesh, this,
				   downclick().coord, downclick().xpoint,
				   click.xpoint, window(),
				   imageform->rubberband_color.pixel);
	    break;
	  }
	default:
	  rubberband = 0;
	}
      }
      else
	rubberband->redraw(click.xpoint);
      break;
    case DM_PAN:
      if(click.button == 2) {	// middle mouse
	static int lastx, lasty;
	if(!panning) {
	  lastx = downclick().xpoint.x;
	  lasty = downclick().xpoint.y;
	  panning = 1;
	}
	int x = click.xpoint.x;
	int y = click.xpoint.y;
	scroll(lastx-x, lasty-y);
	lastx = x;
	lasty = y;
      }
      break;
    }
    imageform->current_dashboard()->movehandler(downclick(), click);
  }

  return;
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

// MouseClick::coord or ScrollCanvas::physicalcoords() are inaccurate
// due to round off errors, and aren't good for precise single pixel
// positioning in the original image. So keep arrays of which pixel in
// the pixmap corresponds to which pixel in the image. Construct these
// arrays with the same arithmetic used to draw the pixmap.

Cell_coordinate ImageCanvas::mouselocation(const MouseClick &click) {
  // make sure that arrays of pixel indices are up to date
  if(pixind_x.capacity() != pwidth() || pixind_outofdate) {
    pixind_x.resize(pwidth());
    if(pwidth() == imagew)
      for(int i=0; i<imagew; i++)
	pixind_x[i] = i;
    else
      for(int i=0; i<imagew; i++) { // loop over pixels in image
	// range of pixels in pixmap corresponding to this image pixel.
	int jmin = int(((double)i)*pwidth()/imagew);
	int jmax = int((i+1.0)*pwidth()/imagew);
	for(int j=jmin; j<jmax; j++)
	  pixind_x[j] = i;
      }
  }

  if(pixind_y.capacity() != pheight() || pixind_outofdate) {
    pixind_y.resize(pheight());
    if(pheight() == imageh)
      for(int i=0; i<imageh; i++)
	pixind_y[i] = imageh-i;
    else
      for(int i=0; i<imageh; i++) {
	int jmax = int(pheight() - ((double) i)*pheight()/imageh);
	int jmin = int(pheight() - (i+1.0)*pheight()/imageh);
	for(int j=jmin; j<jmax; j++)
	  pixind_y[j] = i;
      }
  }

  pixind_outofdate = false;

  // make sure click is in bounds, or array lookup won't work.
  Cell_coordinate where;
  if(click.cell.x < 0 || click.cell.x > pwidth())
    where.x = (short) click.coord.x;
  else
    where.x = pixind_x[click.cell.x];

  if(click.cell.y < 0 || click.cell.y > pheight())
    where.y = (short) click.coord.y;
  else
    where.y = pixind_y[click.cell.y];

  return where;
}

void ImageCanvas::resizepixmap(int w, int h) {
  pixind_outofdate = true;
  ScrollCanvas::resizepixmap(w, h);
}
