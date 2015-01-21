// -*- C++ -*-
// $RCSfile: autodash.h,v $
// $Revision: 1.11 $
// $Author: langer $
// $Date: 2000-09-06 20:11:24 $

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */

#ifndef AUTODASH_H
#define AUTODASH_H

#include "color.h"
#include "dashboard.h"
#include "menuDef.h"

class AutomationDashboard : public Dashboard {
private:
  AutomationDashboard(ImageForm*, FL_OBJECT*);
  virtual ~AutomationDashboard();

  virtual int hide();

  bool get_params();
  bool check_params();
  static void params_changed(const CharString&);
  void update_gui() const;
  void update_dflts(const CharString &) const;
  Color mincolor, maxcolor;

  static void CB_go(FL_OBJECT*, long);
  void go_gui();
  void go();
  static CommandFn go_cmd();

  CharString graingroup;
  static CharString graingroup_dflt;
  
  CharString bdygroup;
  static CharString bdygroup_dflt;

  TrueFalse verbose;
  static TrueFalse verbose_dflt;
  
  TrueFalse redraw;
  static TrueFalse redraw_dflt;

  Color lowerColorBound;
  static Color lowerColorBound_dflt;

  Color upperColorBound;
  static Color upperColorBound_dflt;

  int localBurn;
  static int localBurn_dflt;

  int globalBurn;
  static int globalBurn_dflt;

  int minSize;
  static int minSize_dflt;

  double maxAspect;
  static double maxAspect_dflt;

  double minAspect;
  static double minAspect_dflt;

  double maxPerimeter;
  static double maxPerimeter_dflt;

  friend class ImageForm;
  friend class ImageFormGfx;
  friend class ImageFormTxt;
  friend FD_image_drawer *create_form_image_drawer();
};

#endif
