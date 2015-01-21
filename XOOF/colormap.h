// -*- C++ -*-
// $RCSfile: colormap.h,v $
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


#ifndef COLORMAP_H
#define COLORMAP_H

#include "vec.h"
#include "charstring.h"
#include "color.h"
#include <X11/Xlib.h>

class DrawerColorMap {
private:
  virtual float red(double) const = 0;
  virtual float grn(double) const = 0;
  virtual float blu(double) const = 0;
  int size;
  Vec<Color> palette;
public:
  DrawerColorMap(int);
  virtual ~DrawerColorMap() {}
  Color color(double x) const;
  unsigned long pixel(double x) const;
  void fill_RO(); // fill with read-only colors
  void fill_RW(Colormap, unsigned long start);
     // fill with read-write colors, starting at the given pixel
  void change_RO(double x, const Color &); // change a RO color
  void change_RW(double x, const Color &, Colormap); // change a RW color
  void revert_RO(double x);  // revert a RO color to original value
  void revert_RW(double x, Colormap);
};

class ColorMapRegistration {
private:
  CharString the_name;
  DrawerColorMap *(*create)(int size);
  double order_; // used to order maps in menu
  static int sorted;
  static void sort();
public:
  ColorMapRegistration(const CharString &name,
		       DrawerColorMap *(*create)(int),
		       double order);
  const CharString &name() const { return the_name; }
  double order() const { return order_; }

  friend class ColorDashboard;
  friend class ColorDrawer;
};

extern Vec<ColorMapRegistration*> &colormapregistry();

// --------------------------------------------------- //

class GrayMap : public DrawerColorMap {
private:
  virtual float red(double x) const { return x; }
  virtual float grn(double x) const { return x; }
  virtual float blu(double x) const { return x; }
  static ColorMapRegistration registration;
public:
  GrayMap(int sz) : DrawerColorMap(sz) {}
};

class BlushMap : public DrawerColorMap {
private:
  virtual float red(double x) const { return 0.5*(1+x); }
  virtual float grn(double x) const { return 0.5*x; }
  virtual float blu(double x) const { return 0.5*x; }
public:
  BlushMap(int sz) : DrawerColorMap(sz) {}
};

#endif
