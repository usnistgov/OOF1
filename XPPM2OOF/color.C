// -*- C++ -*-
// $RCSfile: color.C,v $
// $Revision: 1.9 $
// $Author: langer $
// $Date: 2003-02-20 16:31:20 $


/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


#define _ABS_			// prevent SGI from defining abs(int)

#include "color.h"
#include "colorutils.h"
#include "lookfor.h"
#include <math.h>
#include <iostream.h>

Color::Color()
  : red_(0), green_(0), blue_(0)
{}

Color::Color(int r, int g, int b)
  : red_(r), green_(g), blue_(b)
{}

Color::Color(unsigned char r, unsigned char g, unsigned char b)
  : red_(r), green_(g), blue_(b)
{}

Color::Color(double r, double g, double b) {
  if(r >= 1.0)
    red_ = 255;
  else
    red_ = char(floor(256*r));
  if(g >= 1.0)
    green_ = 255;
  else
    green_ = char(floor(256*g));
  if(b >= 1.0)
    blue_ = 255;
  else
    blue_ = char(floor(256*b));
}

Color::Color(int g)
  : red_(g), green_(g), blue_(g)
{
  if(g>255){
    red_=255;
    green_=255;
    blue_=255;
  }
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

Color Color::redden(double scale) const {
  return Color((unsigned char)((1-scale)*red_ + scale), green_, blue_);
}

Color Color::blush(double scale) const {
  return Color(red()+scale*(1-red()),
	       (1-scale)*green(),
	       (1-scale)*blue());
}

Color Color::dim(double scale) const {
  int r = int(scale*query_red());
  int g = int(scale*query_green());
  int b = int(scale*query_blue());
  if(scale > 1.0) {
    if(r > 255) r = 255;
    if(g > 255) g = 255;
    if(b > 255) b = 255;
  }
  return Color(r, g, b);
}

Color Color::dull(double scale) const {
  return Color(0.5 + scale*(red()-0.5),
	       0.5 + scale*(green()-0.5),
	       0.5 + scale*(blue()-0.5));
}

Color Color::fade(double scale) const {
  return Color(red() + scale - red()*scale, // 1-(1-scale)*(1-red)
	       green() + scale - green()*scale ,
	       blue() + scale - blue()*scale);
}

Color Color::reverse() const {
  return Color(255-red_, 255-green_, 255-blue_);
}

Color Color::gray() const {
  unsigned char b = (unsigned char)brightness();
  return Color(b, b, b);
}

Color Color::blend(const Color &blnd, double opacity) const {
  // white is mapped to blnd, black is mapped to opacity*blnd, and
  // everything is linear in between
  return Color(opacity*blnd.red() + (1.-opacity)*red()*blnd.red(),
	       opacity*blnd.green() + (1.-opacity)*green()*blnd.green(),
	       opacity*blnd.blue() + (1.-opacity)*blue()*blnd.blue());
}

XColor Color::xcolor() const {
  XColor xc;
  xc.pixel = pixel;
  xc.red = red_ << 8;
  xc.green = green_ << 8;
  xc.blue = blue_ << 8;
  return xc;
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

// Get a pixel value from a colormap

static inline void shift(unsigned long &x, int sh) {
  if(sh < 0)
    x <<= -sh;
  else
    x >>= sh;
}

unsigned long Color::RO_pixel() {
  unsigned long r = red_ << 8;
  unsigned long g = green_ << 8;
  unsigned long b = blue_ << 8;
  shift(r, gfxinfo.red_shift());
  shift(g, gfxinfo.green_shift());
  shift(b, gfxinfo.blue_shift());
  r &= gfxinfo.red_mask();
  g &= gfxinfo.green_mask();
  b &= gfxinfo.blue_mask();
  pixel = r | g | b;
  return pixel;
}

unsigned long Color::RW_pixel(Colormap colormap, unsigned long index) {
  XColor xc(xcolor());
  xc.pixel = index;
  xc.flags = DoRed | DoGreen | DoBlue;
  XStoreColor(gfxinfo.display(), colormap, &xc);
  pixel = xc.pixel;
  return pixel;
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

ostream &operator<<(ostream &os, const Color &col) {
  if(col.red_ == col.blue_ && col.red_ == col.green_)
    os << int(col.red_);
  else
    os << "["
       << int(col.red_) << ", "
       << int(col.green_) << ", "
       << int(col.blue_)
       << "]";
  return os;
}

istream &operator>>(istream &is, Color &col) {
  int red, green, blue;
  if(!lookfor('[', is)) {
    is.clear();
    is >> red;
    if(is) {
      col.red_ = col.green_ = col.blue_ = red;
      return is;
    }
  }
  is >> red;
  if(!lookfor(',', is)) return is;
  is >> green;
  if(!lookfor(',', is)) return is;
  is >> blue;
  if(!lookfor(']', is)) return is;
  col.red_ = red;
  col.green_ = green;
  col.blue_ = blue;
  return is;
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

int operator==(const Color &flicka, const Color &misty) {
  return flicka.red_ == misty.red_
    && flicka.blue_ == misty.blue_
    && flicka.green_ == misty.green_;
}

int operator!=(const Color &flicka, const Color &misty) {
  return flicka.red_ != misty.red_
    || flicka.blue_ != misty.blue_
    || flicka.green_ != misty.green_;
}

int operator>(const Color &silver, const Color &pegasus) {
  if(silver.red_ > pegasus.red_) return 1;
  if(silver.red_ < pegasus.red_) return 0;
  if(silver.green_ > pegasus.green_) return 1;
  if(silver.green_ < pegasus.green_) return 0;
  if(silver.blue_ > pegasus.blue_) return 1;
  return 0;
}

int operator<(const Color &silver, const Color &pegasus) {
  if(silver.red_ < pegasus.red_) return 1;
  if(silver.red_ > pegasus.red_) return 0;
  if(silver.green_ < pegasus.green_) return 1;
  if(silver.green_ > pegasus.green_) return 0;
  if(silver.blue_ < pegasus.blue_) return 1;
  return 0;
}

int operator>=(const Color &silver, const Color &pegasus) {
  return silver > pegasus || silver == pegasus;
}

int operator<=(const Color &silver, const Color &pegasus) {
  return silver < pegasus || silver == pegasus;
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

inline int abs(int x) { return x > 0 ? x : -x; }

int L1_dist(const Color &trigger, const Color &MrEd) {
  return int((abs(trigger.query_red() - MrEd.query_red()) +
	  abs(trigger.query_green() - MrEd.query_green())+
	  abs(trigger.query_blue() - MrEd.query_blue()) )/3.0);
}

int L2_dist2(const Color &trigger, const Color &MrEd) {
  int dr = int(trigger.red_) - int(MrEd.red_);
  int dg = int(trigger.green_) - int(MrEd.green_);
  int db = int(trigger.blue_) - int(MrEd.blue_);
  return dr*dr + dg*dg + db*db;
}
