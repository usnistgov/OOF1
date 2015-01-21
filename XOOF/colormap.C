// -*- C++ -*-
// $RCSfile: colormap.C,v $
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

#include "colormap.h"
#include "colorutils.h"
#include "menucolor.h"
#include <math.h>
#include "stdlib.h"
#include <X11/Xlib.h>

ColorMapRegistration::ColorMapRegistration(const CharString &nm,
                        DrawerColorMap *(*func)(int), double ord)
  : the_name(nm), create(func), order_(ord)
{
  colormapregistry().grow(1, this);
  sorted = 0;
}

Vec<ColorMapRegistration*> &colormapregistry() {
  static Vec<ColorMapRegistration*> list;
  return list;
}

static int maporder(const void *d1, const void *d2) {
  double o1 = (*(ColorMapRegistration**) d1)->order();
  double o2 = (*(ColorMapRegistration**) d2)->order();
  if(o1 < o2) return -1;
  if(o1 > o2) return 1;
  return 0;
}

int ColorMapRegistration::sorted = 0;

void ColorMapRegistration::sort() {
  if(sorted) return;
  qsort(&(colormapregistry()[0]), colormapregistry().capacity(),
        sizeof(ColorMapRegistration*), maporder);
  sorted = 1;
}

// -------------------------------------------------- //

DrawerColorMap::DrawerColorMap(int s)
  : size(s), palette(s)
{
}

Color DrawerColorMap::color(double x) const {
  if(x < 0.0) return palette[0];
  if(x >= 1.0) return palette[size-1];
  return palette[x*size];
}

unsigned long DrawerColorMap::pixel(double x) const {
  return color(x).pixel;
}

void DrawerColorMap::fill_RO() {
  double dx = 1.0/(size-1);
  for(int i=0; i<size; i++) {
    double x = i*dx;
    double r = red(x);
    double g = grn(x);
    double b = blu(x);
    palette[i] = Color(r, g, b);
    palette[i].RO_pixel();
  }
}

void DrawerColorMap::fill_RW(Colormap cmap, unsigned long start) {
  double dx = 1.0/(size-1);
  XColor *xc = new XColor[size];
  int i;
  for(i=0; i<size; i++) {
    double x = i*dx;
    float r = red(x);
    float g = grn(x);
    float b = blu(x);
    palette[i] = Color(r, g, b);
    xc[i] = palette[i].xcolor();
    xc[i].flags = DoRed | DoBlue | DoGreen;
    xc[i].pixel = start + i;
  }
  XStoreColors(gfxinfo.display(), cmap, xc, size);

  for(i=0; i<size; i++)
    palette[i].pixel = xc[i].pixel;

  delete [] xc;
}

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

void DrawerColorMap::change_RO(double x, const Color &newcolor) {
  int i = int(x*size);
  palette[i] = newcolor;
  palette[i].RO_pixel();
}

void DrawerColorMap::change_RW(double x, const Color &newcolor, Colormap cmap) {
  int i = int(x*size);
  unsigned long oldpixel = palette[i].pixel;
  palette[i] = newcolor;
  palette[i].RW_pixel(cmap, oldpixel);
}

void DrawerColorMap::revert_RO(double x) {
  // x is a number between 0 and 1. Change the bar containing x.
  int i = int(x*size);  // which bar to change
  // when computing rgb, have to use the same x that was used in fill_RO()
  x = i/(size - 1.0);
  double r = red(x);
  double g = grn(x);
  double b = blu(x);
  palette[i] = Color(r, g, b);
  palette[i].RO_pixel();
}

void DrawerColorMap::revert_RW(double x, Colormap cmap) {
  int i = int(x*size);
  x = i/(size - 1.0); // see comment in revert_RO()
  unsigned long oldpixel = palette[i].pixel;
  double r = red(x);
  double g = grn(x);
  double b = blu(x);
  palette[i] = Color(r, g, b);
  palette[i].RW_pixel(cmap, oldpixel);
}

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//


static DrawerColorMap *create_graymap(int size) {
  return new GrayMap(size);
}

ColorMapRegistration GrayMap::registration("Gray", create_graymap, 10.0);

// --------------------------------------------------- //

class BlueGreenRedMap : public DrawerColorMap {
private:
  virtual float red(double x) const {
    if(x < 0.5)
      return 0;
    return -3+x*(8-4*x);
  }
  virtual float grn(double x) const {
    return 4*x*(1-x);
  }
  virtual float blu(double x) const {
    if(x < 0.5)
      return 1-4*x*x;
    return 0;
  }
  static ColorMapRegistration registration;
public:
  BlueGreenRedMap(int sz) : DrawerColorMap(sz) {}
};

static DrawerColorMap *create_bgrmap(int size) {
  return new BlueGreenRedMap(size);
}

ColorMapRegistration BlueGreenRedMap::registration("BlueGreenRed",
						   create_bgrmap, 1.0);

// --------------------------------------------------- //

class TequilaSunrise : public DrawerColorMap {
private:
  virtual float red(double) const {
    return 1.0;
  }
  virtual float grn(double x) const {
    return 0.86667*sqrt(x);
  }
  virtual float blu(double) const {
    return 0.0;
  }
  static ColorMapRegistration registration;
public:
  TequilaSunrise(int sz) : DrawerColorMap(sz) {}
};

static DrawerColorMap *create_tsrmap(int size) {
  return new TequilaSunrise(size);
}

ColorMapRegistration TequilaSunrise::registration("TequilaSunrise",
						  create_tsrmap, 2.0);


// ------------------------------------------------- //

class ThermalMap : public DrawerColorMap {
private:
  virtual float red(double x) const {
    if(x < 0.33) return x/0.33;
    return 1.0;
  }
  virtual float grn(double x) const {
    if(x < 0.33) return 0;
    if(x < 0.67) return (x-0.33)/0.34;
    return 1.0;
  }
  virtual float blu(double x) const {
    if(x < 0.67) return 0;
    return (x-0.67)/0.33;
  }
  static ColorMapRegistration registration;
public:
  ThermalMap(int sz) : DrawerColorMap(sz) {}
};

static DrawerColorMap *create_thermalmap(int size) {
  return new ThermalMap(size);
}

ColorMapRegistration ThermalMap::registration("Thermal",
						  create_thermalmap, 0.5);

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

class BlueRedMap : public DrawerColorMap {
private:
  virtual float red(double x) const {
    if(x > 0.5) return sqrt(2.0*x-1.0);
    return 0.0;
  }
  virtual float grn(double) const {
    return 0.0;
  }
  virtual float blu(double x) const {
    if(x < 0.5) return sqrt(1-2*x);
    return 0.0;
  }
  static ColorMapRegistration registration;
public:
  BlueRedMap(int sz) : DrawerColorMap(sz) {}
};

static DrawerColorMap *create_blueredmap(int size) {
  return new BlueRedMap(size);
}

ColorMapRegistration BlueRedMap::registration("BlueRed",
						  create_blueredmap, 3.0);

class BlueRedMap2 : public DrawerColorMap {
private:
  virtual float red(double x) const {
    if(x > 0.5) return 2.0*x-1.0;
    return 0.0;
  }
  virtual float grn(double) const {
    return 0.0;
  }
  virtual float blu(double x) const {
    if(x < 0.5) return 1-2*x;
    return 0.0;
  }
  static ColorMapRegistration registration;
public:
  BlueRedMap2(int sz) : DrawerColorMap(sz) {}
};

static DrawerColorMap *create_blueredmap2(int size) {
  return new BlueRedMap2(size);
}

ColorMapRegistration BlueRedMap2::registration("BlueRed2",
						  create_blueredmap2, 3.0);
