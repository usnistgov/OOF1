// -*- C++ -*-
// $RCSfile: robomouse.h,v $
// $Revision: 1.3 $
// $Author: langer $
// $Date: 2000-09-06 20:11:46 $

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */

#ifndef ROBOMOUSE_H
#define ROBOMOUSE_H

#include "filename.h"
#include "menuDef.h"
#include "mouseclick.h"

class ImageForm;
class Dashboard;

ImageForm *get_imageform(); // decipher menu command to get ptr to ImageForm
Dashboard *get_dashboard(); // ditto for current dashboard

class RoboMouseCmd : public CommandM {
private:
  MouseClick click;
public:
  RoboMouseCmd();
  CommandFn func();
  virtual CommandM *clone() const;
};

class ZoomButtonCmd : public CommandM {
private:
  int whichbutton;
public:
  ZoomButtonCmd();
  virtual void func();
  virtual CommandM* clone() const;
};

class ScrollCommand : public CommandM {
private:
  int dx, dy;
public:
  ScrollCommand();
  virtual void func();
  virtual CommandM *clone() const;
};

class ResizeCmd : public CommandM {
private:
  int w, h;
public:
  ResizeCmd();
  virtual void func();
  virtual CommandM* clone() const;
};


class SaveCmd : public CommandM {
private:
  FileName filename;
public:
  SaveCmd();
  virtual void func();
  virtual CommandM *clone() const;
};

#endif
