// -*- C++ -*-
// $RCSfile: select_by_color.h,v $
// $Revision: 1.4 $
// $Author: langer $
// $Date: 2000-09-06 20:11:47 $

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */

#ifndef SELECT_BY_COLOR_H
#define SELECT_BY_COLOR_H

#include "menuDef.h"
class Cell_coordinate;
class Color;
class Image;
class ImageForm;

class DemogCmd : public CommandM {
private:
  Color picked_color;
  bool compare_color(const Color&) const;
  SelectDashboard *sd;
public:
  DemogCmd(SelectDashboard*);
  virtual CommandFn func();
  virtual CommandM *clone(void) const;
  void select(const Image&, const Cell_coordinate &, int);
};

class DemograyfyCmd : public CommandM {
private:
  int grayvalue;
  int dev_gray;
  TrueFalse replace;		// replace existing selection?
  const ImageForm *imageform;
public:
  DemograyfyCmd(const ImageForm*);
  virtual CommandFn func();
  virtual CommandM *clone(void) const;
};


class DemoRGBCmd : public CommandM {
private:
  int redvalue, greenvalue, bluevalue;
  int dev_r, dev_g, dev_b;
  TrueFalse replace;		// replace existing selection?
  const ImageForm *imageform;
public:
  DemoRGBCmd(const ImageForm*);
  virtual CommandFn func();
  virtual CommandM *clone(void) const;
};

void select_demography(const Image&, const Cell_coordinate&, int);

#endif

