// -*- C++ -*-
// $RCSfile: noderubberband.h,v $
// $Revision: 1.3 $
// $Author: langer $
// $Date: 2000-09-06 20:11:42 $


/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */

#ifndef NODERUBBERBAND_H
#define NODERUBBERBAND_H

#include "rubberband.h"

class AMNode;
class AdaptiveMesh;
class MeshCoord;

class NodeRubberBand : public RubberBand {
private:
  AMNode* node;
  Vec<XSegment> segments;
public:
  NodeRubberBand(const AdaptiveMesh*, const ImageCanvas*,
		 const MeshCoord&,
		 const XPoint &p1, const XPoint &p2, Window w,
		 unsigned long pxl);
  virtual ~NodeRubberBand();
  virtual void draw() const;
};

#endif
