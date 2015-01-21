// -*- C++ -*-
// $RCSfile: imagemod.C,v $
// $Revision: 1.6 $
// $Author: langer $
// $Date: 2000-10-30 19:23:26 $


/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */

// Image modification tools

#include "color.h"
#include "image.h"
#include "menuDef.h"
#include "stencil.h"
#include <math.h>

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

void Image::makegray() {
  ArrayIterator iter(*this);
  Cell_coordinate cell;
  while(iter(cell))
    (*this)[cell] = (*this)[cell].gray();
  changed = 1;
  grayscale = 1;
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

void Image::maxcontrast() {

  // find range of colors in the image
  int min = 255;
  int max = 0;
  ArrayIterator iter(*this);
  Cell_coordinate cell;
  if(grayscale) {
    while(iter(cell)) {
      int gray = (*this)[cell].query_red();
      if(gray < min) min = gray;
      if(gray > max) max = gray;
    }
  }
  else {			// not grayscale
    while(iter(cell)) {
      Color &c = (*this)[cell];
      int r = c.query_red();
      int g = c.query_green();
      int b = c.query_blue();
      if(r < min) min = r;
      if(r > max) max = r;
      if(g < min) min = g;
      if(g > max) max = g;
      if(b < min) min = b;
      if(b > max) max = b;
    }
  }
  if(max == min) return;	// can't do anything w/out any contrast!

  // expand range to (0, 255)
  double scale = 255./(max - min);
  garcon()->msout << ms_info << "Range of color components is "
		  << min << " to " << max
		  << ". Scalefactor = " << scale << "."
		  << endl << ms_normal;
  iter.reset();
  while(iter(cell)) {
    Color &c = (*this)[cell];
    c = Color(int((c.query_red() - min)*scale),
	      int((c.query_green() - min)*scale),
	      int((c.query_blue() - min)*scale));
  }
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

static const Cell_coordinate xhat(1, 0);
static const Cell_coordinate yhat(0, 1);

// Use reflecting boundary conditions. First derivatives are zero,
// second derivatives are calculated as if the field is reflected
// through the boundary.

// One step of the diffusion process.
void diffuse(const Array<double> &f0, Array<double> &f1, double dt) {
  Cell_coordinate here;
  ArrayIterator iter(f0);
  int h = f0.query_height();
  int w = f0.query_width();
  while(iter(here)) {
    Cell_coordinate up(here + yhat);
    Cell_coordinate down(here - yhat);
    Cell_coordinate right(here + xhat);
    Cell_coordinate left(here - xhat);
    // reflecting boundaries
    if(up.y == h)
      up = down;
    else if(down.y < 0)
      down = up;
    if(left.x < 0)
      left = right;
    else if(right.x == w)
      right = left;

    f1[here] = (1-4*dt)*f0[here]
      + dt*(f0[up] + f0[down] + f0[left] + f0[right]);
  }
}

// One step of the non-linear diffusion process
// Tannenbaum's method for diffusing perpendicular to gradients:
// d\phi/dt = (\phi_{xx} \phi_y^2 - 2\phi_x \phi_y\phi_{xy}
//                + \phi_{yy} \phi_x^2)^{1/3}

static const double third = 1./3.;
static double alpha = 1;

static void nldiffuse(const Array<double> &f0, Array<double> &f1, double dt) {
  int h = f0.query_height();
  int w = f0.query_width();

  Cell_coordinate here;
  ArrayIterator iter(f0);
  while(iter(here)) {
    Cell_coordinate up(here + yhat);
    Cell_coordinate down(here - yhat);
    Cell_coordinate right(here + xhat);
    Cell_coordinate left(here - xhat);
    Cell_coordinate upleft(here - xhat + yhat);
    Cell_coordinate downleft(here - xhat - yhat);
    Cell_coordinate upright(here + xhat + yhat);
    Cell_coordinate downright(here + xhat - yhat);


    // reflecting boundaries
    if(up.y == h) {		// top row
      up = down;
      if(right.x == w)		// top right corner
	upright = downleft;
      else			// top, but not right
	upright = downright;
      if(left.x < 0)		// top left corner
	upleft = downright;
      else			// top, but not left
	upleft = downleft;
    }
    else if(down.y < 0) {	// bottom row
      down = up;
      if(right.x == w)		// bottom right corner
	downright = upleft;
      else			// bottom, but not right
	downright = upright;
      if(left.x < 0)		// bottom left corner
	downleft = upright;
      else			// bottom, but not left
	downleft = upleft;
    }
    if(right.x == w) {		// right side
      right = left;
      if(up.y != h && down.y >= 0) { // right side, but not a corner
	upright = upleft;
	downright = downleft;
      }
    }
    else if(left.x < 0)	{	// left side
      left = right;
      if(up.y != h && down.y >= 0) { // left side, but not a corner
	upleft = upright;
	downleft = downright;
      }
    }

    double fx = 0.5*(f0[right] - f0[left]);
    double fy = 0.5*(f0[up] - f0[down]);
    double fxx = f0[left] - 2*f0[here] + f0[right];
    double fyy = f0[up] - 2*f0[here] + f0[down];
    double fxy = 0.25*(f0[upright] + f0[downleft] - f0[upleft] - f0[downright]);
    
    double ff = (fxx*fy*fy - 2*fx*fy*fxy + fyy*fx*fx)/(1+alpha*(fx*fx + fy*fy));
    if(ff > 0)
      f1[here] = f0[here] + dt*pow(ff, third);
    else
      f1[here] = f0[here] - dt*pow(-ff, third);
  }
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

// Apply a stencil to the image

static const Stencil *current_stencil;

static void apply_stencil(const Array<double> &f0, Array<double> &f1, double) {
  int h = f0.query_height();
  int w = f0.query_width();

  double sul = (*current_stencil)(-1,  1);
  double su  = (*current_stencil)( 0,  1);
  double sur = (*current_stencil)( 1,  1);
  double sl  = (*current_stencil)(-1,  0);
  double sc  = (*current_stencil)( 0,  0);
  double sr  = (*current_stencil)( 1,  0);
  double sdl = (*current_stencil)(-1, -1);
  double sd  = (*current_stencil)( 0, -1);
  double sdr = (*current_stencil)( 1, -1);

  Cell_coordinate here;
  ArrayIterator iter(f0);
  while(iter(here)) {
    Cell_coordinate up(here + yhat);
    Cell_coordinate down(here - yhat);
    Cell_coordinate right(here + xhat);
    Cell_coordinate left(here - xhat);
    Cell_coordinate upleft(here - xhat + yhat);
    Cell_coordinate downleft(here - xhat - yhat);
    Cell_coordinate upright(here + xhat + yhat);
    Cell_coordinate downright(here + xhat - yhat);


    // reflecting boundaries
    if(up.y == h) {		// top row
      up = down;
      if(right.x == w)		// top right corner
	upright = downleft;
      else			// top, but not right
	upright = downright;
      if(left.x < 0)		// top left corner
	upleft = downright;
      else			// top, but not left
	upleft = downleft;
    }
    else if(down.y < 0) {	// bottom row
      down = up;
      if(right.x == w)		// bottom right corner
	downright = upleft;
      else			// bottom, but not right
	downright = upright;
      if(left.x < 0)		// bottom left corner
	downleft = upright;
      else			// bottom, but not left
	downleft = upleft;
    }
    if(right.x == w) {		// right side
      right = left;
      if(up.y != h && down.y >= 0) { // right side, but not a corner
	upright = upleft;
	downright = downleft;
      }
    }
    else if(left.x < 0)	{	// left side
      left = right;
      if(up.y != h && down.y >= 0) { // left side, but not a corner
	upleft = upright;
	downleft = downright;
      }
    }

    f1[here] = (f0[upleft]*sul + 
		f0[up]*su +
		f0[upright]*sur + 
		f0[left]*sl +
		f0[here]*sc +
		f0[right]*sr +
		f0[downleft]*sdl +
		f0[down]*sd +
		f0[downright]*sdr) / current_stencil->scale();
  }
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

// Copy one RGB component of an Image to an array of doubles.

static Array<double> image2double(const Image &image, float (Color::*f)() const)
{
  Cell_coordinate here;
  Array<double> workspace(image.query_height(), image.query_width());
  ArrayIterator iter(workspace);
  while(iter(here))
    workspace[here] = (image[here].*f)();
  return workspace;
}

static void double2image(const Array<double> &dbl, Image &image) {
  Cell_coordinate here;
  ArrayIterator iter(image);
  while(iter(here)) {
    double x = dbl[here];
    if(x < 0) x = 0;
    if(x > 1.0) x = 1.0;
    image[here] = Color(x, x, x);
  }
}

static void double2image(const Array<double> &red, const Array<double> &grn,\
		  const Array<double> &blu, Image &image)
{
  Cell_coordinate here;
  ArrayIterator iter(image);
  while(iter(here)) {
    double r = red[here];
    double g = grn[here];
    double b = blu[here];
    if(r < 0) r = 0;
    if(r > 1.0) r = 1.0;
    if(g < 0) g = 0;
    if(g > 1.0) g = 1.0;
    if(b < 0) b = 0;
    if(b > 1.0) b = 1.0;
    image[here] = Color(r, g, b);
  }
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

// Routine that applies a function to the rgb values in an image. Each
// color component is treated as a separate field, and doesn't
// interact with the other components. The final argument 
//  is the function to be applied to each component:
//   f(const Array<double> &comp, Array<double> &mod, double dt)
//       comp is an array filled with the r, g, or b values
//       mod is the modified array 
//       dt is the timestep

// do N iterations
static void dofunc(Array<double> &field, int Niter, double dt,
		   void (*func)(const Array<double>&, Array<double>&, double))
{
  Array<double> field2(field.query_height(), field.query_width());
  Array<double> *f0 = &field;
  Array<double> *f1 = &field2;
  for(int i=0; i<Niter; i++) {
    (*func)(*f0, *f1, dt);
    // switch fields for next step
    Array<double> *temp = f0;
    f0 = f1;
    f1 = temp;
  }
  if(f0 != &field)		// have to do one more copy
    field = *f0;
}

static void RGBapply(Image &image, int Niter, double dt,
		     void (*func)(const Array<double>&, Array<double>&, double))
{
  garcon()->graphics_off();
  if(image.isgray()) {
    Array<double> work(image2double(image, &Color::red));
    dofunc(work, Niter, dt, func);
    double2image(work, image);
  }
  else { // not gray!
    Array<double> red(image2double(image, &Color::red));
    dofunc(red, Niter, dt, func);
    Array<double> grn(image2double(image, &Color::green));
    dofunc(grn, Niter, dt, func);
    Array<double> blu(image2double(image, &Color::blue));
    dofunc(blu, Niter, dt, func);
    double2image(red, grn, blu, image);
  }
  garcon()->graphics_on();
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

void Image::smooth(double dt, int nsteps) {
  RGBapply(*this, nsteps, dt, diffuse);
}

void Image::nonlinear_smooth(double alfa, double dt, int nsteps) {
  alpha = alfa;
  RGBapply(*this, nsteps, dt, nldiffuse);
}

void Image::stencil(const Stencil &stencil, int nsteps) {
  current_stencil = &stencil;
  RGBapply(*this, nsteps, 0, apply_stencil);
}
