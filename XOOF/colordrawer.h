// -*- C++ -*-
// $RCSfile: colordrawer.h,v $
// $Revision: 1.4 $
// $Author: langer $
// $Date: 2001-02-18 02:34:49 $


/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


// Abstract base class for drawers which color the elements according
// to an ElementFunction.  The concrete derived classes are defined by
// the template in stressdrawer.h.

#ifndef COLORDRAWER_H
#define COLORDRAWER_H

class ColorDrawer;

#include "attributes.h"
#include "colordashboard.h"
#include "colormap.h"
#include "elementfunctionbase.h"
#include "meshdrawer.h"
#include "plotdashboard.h"
#include "timestamp.h"

class Element;
class FormDrawer;
class Grid;

class ColorDrawer : public MeshDrawer {
protected:
  virtual const ElementFunctionBase &func() const = 0;  // gets value to plot
  Vec<float> values;
  void setvalues();
  void fullscale();
  double valmin, valmax;  // min and max values in grid
  double plotmin, plotmax;  // min and max values being plotted

  virtual void gridchanged();

  virtual void drawelements();
  virtual void drawelement(const Element*);
  virtual void drawextras();

  virtual void drawelements_ps(FILE*, const Rectangle&);
  virtual void drawextras_ps(FILE*, const Rectangle&) const;

  SimpleAttributes simpleattributes;
  ColorDashboard colordashboard;
  PlotDashboard plotdashboard;
private:

  void fill_colormap();

  DrawerColorMap *colormap;

  TimeStamp gridchanged_time;
  TimeStamp setvalues_time;

public:
  ColorDrawer(Grid*, FormDrawer*);
  virtual ~ColorDrawer();

  friend class FormDrawer;
  friend class ColorDashboard;
  friend class PlotDashboard;
};

#endif

