// -*- C++ -*-
// $RCSfile: menuinterruptform.h,v $
// $Revision: 1.3 $
// $Author: langer $
// $Date: 2000-09-06 20:11:06 $


/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


#ifdef MENUGFX

class InterruptForm;

#ifndef MENUINTERRUPTFORM_H
#define MENUINTERRUPTFORM_H

#include "menuform.h"

class InterruptForm : public Form {
  friend void IFCB_stop(FL_OBJECT*, long);
  friend class FormInterruptHandler;
private:
  FL_OBJECT *IF_stop;
  InterruptForm();
  virtual ~InterruptForm() {}
  virtual void makevisible();
  void stopbutton();
  friend void create_form_interruptform(InterruptForm*);
};

#endif

#endif
