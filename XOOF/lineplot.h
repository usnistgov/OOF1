// -*- C++ -*-
// $RCSfile: lineplot.h,v $
// $Revision: 1.3 $
// $Author: langer $
// $Date: 2000-09-06 20:11:03 $


/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */

#ifndef LINEPLOT_H
#define LINEPLOT_H

#include "menuDef.h"
#include "meshcoord.h"
#include "charstring.h"

class LinePlot {
public:
  LinePlot()
    : npts(10), equalspace(1), start(0,0), finish(1,1),
      plotx(1), ploty(0), plots(0)
  {}
  LinePlot(const CharString &nm, const MeshCoord &st, const MeshCoord &fn,
	   int n, double tol, bool eq, bool px, bool py, bool ps)
    : name(nm), start(st), finish(fn), npts(n), tolerance(tol), equalspace(eq),
      plotx(px), ploty(py), plots(ps)
  {}
  CharString name;
  MeshCoord start;
  MeshCoord finish;
  int npts;
  double tolerance;		// used only if equalspace==0
  TrueFalse equalspace;
  TrueFalse plotx;
  TrueFalse ploty;
  TrueFalse plots;			// arclength
};

Menu *crosssectionmenu();

#endif
