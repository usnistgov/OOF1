// -*- C++ -*-
// $RCSfile: selectdrawer.h,v $
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


#ifndef SELECTDRAWER_H
#define SELECT_DRAWER_H

#include "attributes.h"
#include "meshdrawer.h"

class SelectDrawer : public MeshDrawer {
protected:
  static DrawerRegistration registration;

  BlushMap blushmap;

  virtual void draw();
  virtual void drawelements();
  virtual void drawelement(const Element*);
  virtual void drawnodes();
  virtual void drawnode(const Node*);

  virtual void draw_ps(FILE*, const Rectangle&);
  virtual void drawelements_ps(FILE*, const Rectangle&);
  virtual void drawnodes_ps(FILE*, const Rectangle&) const;

private:
  static const unsigned long NODECOLOR;
  Color nodecolor;
  SelectAttributes selectattributes;
public:
  SelectDrawer(Grid*, FormDrawer*);
  virtual ~SelectDrawer() {}

  friend class FormDrawer;
};

#endif
