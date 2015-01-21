// -*- C++ -*-
// $RCSfile: color.h,v $
// $Revision: 1.4 $
// $Author: langer $
// $Date: 2001-10-25 18:58:22 $


/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


#ifndef COLOR_H
#define COLOR_H

#include <iostream.h>
#include <X11/Xlib.h>

#define CHARTOFLOAT 0.0039215686274509803  // 1/255

class Color {
private:
  unsigned char red_, green_, blue_;
public:

  Color();
  Color(int r, int g, int b);
  Color(unsigned char r, unsigned char g, unsigned char b);
  Color(double r, double g, double b);
  Color(int gray);

  unsigned long pixel;   // X pixel value (index into X colormap)

  XColor xcolor() const;
  unsigned long RO_pixel();  // get pixel for read only Colormap
                             // Colormap info comes from gfxinfo.
  unsigned long RW_pixel(Colormap, unsigned long);
        // store color in read/write Colormap

  int query_red() const { return red_; }
  int query_blue() const { return blue_; }
  int query_green() const { return green_; }
  int brightness() const {
    return int((double(red_) + double(green_) + double(blue_))/3.0);
  }

  float red() const { return red_ * CHARTOFLOAT; }
  float green() const { return green_ * CHARTOFLOAT; }
  float blue() const { return blue_ * CHARTOFLOAT; }

  Color redden(double scale = 0.5) const;
  Color blush(double scale = 0.5) const;
  Color dim(double scale = 0.5) const;
  Color dull(double scale = 0.5) const;
  Color reverse() const;
  Color gray() const;

  friend int operator==(const Color&, const Color&);
  friend int operator!=(const Color&, const Color&);
  friend int operator>(const Color&, const Color&);
  friend int operator<(const Color&, const Color&);
  friend int operator>=(const Color&, const Color&);
  friend int operator<=(const Color&, const Color&);
  
  friend int L1_dist(const Color&, const Color&);
  friend int L2_dist2(const Color&, const Color&);
  friend ostream &operator<<(ostream&, const Color&);

  friend istream &operator>>(istream&, Color&);
};

// #ifdef sun
#include "varobjects.h"

template <>
VarObject *Var<Color>::create_varobject(Variable *var, Form *form,
					   VarObjectType type,
					   int x, int y, int w, int h);
// #endif // sun

#endif
