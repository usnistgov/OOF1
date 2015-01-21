// -*- C++ -*-
// $RCSfile: quit.C,v $
// $Revision: 1.5 $
// $Author: langer $
// $Date: 2000-10-30 19:23:28 $

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */

#include "forms.h"
#include "image_drawer2.h"
#include "goof.h"
#include "menuDef.h"
#include "menucolor.h"
#include "quit.h"
#include "saveconfig.h"
#include <fstream.h>

QuitForm::QuitForm()
  : Form("Quit?"),
    done(0),
    quitnow(0)
{
  fdui = create_form_quitform();
  fdui->vdata = this;
  the_form = fdui->quitform;
  fl_set_object_color(fdui->background, COL_BACKGROUND, 0);
  fl_set_object_color(fdui->text, COL_BACKGROUND, 0);
}

void QuitForm::CB_quitsave(FL_OBJECT *obj, long) {
  const char *filename = fl_show_fselector("Save session", 0, 0, 0);
  if(filename) {
    ofstream os(filename);
    if(!os) {
      garcon()->msout << ms_error << "Can't open file " << filename << "!"
		      << endl << ms_normal;
      return;
    }
    saveconfig(os, current_goof);
    //garcon()->printlog(os);
    QuitForm *qf = (QuitForm*)((FD_quitform*) obj->form->fdui)->vdata;
    qf->quitnow = 1;
    qf->done = 1;
  }
}

void QuitForm::CB_quitnow(FL_OBJECT *obj, long) {
  QuitForm *qf = (QuitForm*)((FD_quitform*)obj->form->fdui)->vdata;
  qf->quitnow = 1;
  qf->done = 1;
}

void QuitForm::CB_quitcancel(FL_OBJECT *obj, long) {
  QuitForm *qf = (QuitForm*)((FD_quitform*) obj->form->fdui)->vdata;
  qf->quitnow = 0;
  qf->done = 1;
}
  
void QuitForm::makevisible() {
  if(the_form->visible) return;
  fl_show_form(the_form, FL_PLACE_MOUSE, FL_TRANSIENT, name.charstar());
    Nforms()++;
}

int quitcmd() {
  if(!garcon()->logdirty()) return 1; // quit immediately, nothing to save

  // graphics interface
  if(garcon()->graphics()) {
    fl_deactivate_all_forms();
    QuitForm qf;
    qf.makevisible();
    do
      fl_check_only_forms();
    while(!qf.done);
    fl_activate_all_forms();
    return qf.quitnow;
  }

  // text interface
  CharString answer;
  garcon()->msout << ms_prompt
		  << "Save session before quitting? [yes no cancel] "
		  << ends << ms_normal;
  cin >> answer;
  if(cin) {
    if(answer == "no" || answer == "n") return 1;
    if(answer == "cancel" || answer == "c") return 0;
    if(answer == "yes" || answer == "y" || answer[0] == '\0') {
      CharString filename;
      garcon()->msout << ms_prompt << "Save into which file? "
		      << ends << ms_normal;
      cin >> filename;
      if(!cin) return 0;
      ofstream os(filename);
      if(!os) {
	garcon()->msout << ms_error << "Error opening file " << filename << "!"
			<< endl << ms_normal;
	return 0;
      }
      saveconfig(os, current_goof);
      //garcon()->printlog(os);
      return 1;
    }
  }
  return 0;  // don't quit if answer isn't comprehensible
}
