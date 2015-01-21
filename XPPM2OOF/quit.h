// -*- C++ -*-
// $RCSfile: quit.h,v $
// $Revision: 1.3 $
// $Author: langer $
// $Date: 2000-09-06 20:11:44 $

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */

#ifndef QUIT_H
#define QUIT_H

class QuitForm;

#include "forms.h"
#include "menuform.h"

int quitcmd();

class QuitForm : public Form {
private:
  FD_quitform *fdui;
  int done, quitnow;

  static void CB_quitsave(FL_OBJECT*, long);
  static void CB_quitnow(FL_OBJECT*, long);
  static void CB_quitcancel(FL_OBJECT*, long);
  virtual void makevisible();
  QuitForm();
  ~QuitForm() {}

  friend int quitcmd();
  friend FD_quitform *create_form_quitform();
};

#endif
