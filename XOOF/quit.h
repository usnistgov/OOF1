// -*- C++ -*-
// $RCSfile: quit.h,v $
// $Revision: 1.3 $
// $Author: langer $
// $Date: 2000-09-06 20:11:12 $


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
  FL_OBJECT *button_save;
  FL_OBJECT *button_quitnow;
  FL_OBJECT *button_cancel;
  FL_OBJECT *text;

  virtual void makevisible();
  QuitForm();
  ~QuitForm() {}

  friend int quitcmd();
  friend void create_form_quitform(QuitForm*);
};

#endif
