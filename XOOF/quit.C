// -*- C++ -*-
// $RCSfile: quit.C,v $
// $Revision: 1.4 $
// $Author: langer $
// $Date: 2001-06-04 19:13:03 $


/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


#include "create_form_drawer.h"
#include "grid.h"
#include "menuDef.h"
#include "menucolor.h"
#include "parameters.h"
#include "quit.h"

QuitForm::QuitForm()
  : Form("Quit?")
{
  create_form_quitform(this);
  fl_set_object_color(background, COL_BACKGROUND, 0);
  fl_set_object_color(text, COL_BACKGROUND, 0);
}

static int savelogfile() {
  const char *filename = fl_show_fselector("Save session log", 0, 0, 0);
  if(!filename) return 0;
  ofstream os(filename);
  if(!os) {
    garcon()->msout << ms_error << "Can't open file " << filename << "!"
		    << endl << ms_normal;
    return 0;
  }
  garcon()->printlog(os);
  return 1;
}
  
void QuitForm::makevisible() {
  if(the_form->visible) return;
  fl_show_form(the_form, FL_PLACE_MOUSE, FL_TRANSIENT, name.charstar());
    Nforms()++;
}

int quitcmd() {
  int quitnow = 0;
  if(garcon()->logdirty()) {
    if(garcon()->graphics()) {
      int done = 0;
      fl_deactivate_all_forms();
      QuitForm qf;
      qf.makevisible();
      while(!done) {
	FL_OBJECT *obj = fl_do_only_forms();
	if((obj == qf.button_save && savelogfile()) || obj == qf.button_quitnow)
	  {
	    quitnow = 1;
	    done = 1;
	  }
	else if(obj == qf.button_cancel) {
	  quitnow = 0;
	  done = 1;
	}
      }
      fl_activate_all_forms();
    }
    else {			// log  dirty, not using graphics
      CharString answer;
      garcon()->msout << ms_prompt
		      << "Save session log before quitting? [yes no cancel] "
		      << flush << ms_normal;
      cin >> answer;
      if(cin) {
	if(answer == "no" || answer == "n") return 1;
	if(answer == "cancel" || answer == "c") return 0;
	if(answer == "yes" || answer == "y" || answer[0] == '\0') {
	  CharString filename;
	  garcon()->msout << ms_prompt << "Save into which file? "
			  << flush << ms_normal;
	  cin >> filename;
	  if(!cin) return 0;
	  ofstream os(filename);
	  if(!os) {
	    garcon()->msout << ms_error << "Error opening file " << filename
			    << "!" << endl << ms_normal;
	    quitnow = 0;
	  }
	  else {
	    garcon()->printlog(os);
	    quitnow = 1;
	  }
	}
      }
      else			//couldn't read answer
	quitnow = 0;  // don't quit if answer isn't comprehensible
    }
  }
  else				// log not dirty
    quitnow = garcon()->question("Do you really want to quit?", 0);
  if(quitnow && grid) {
    delete grid;
    grid = 0;
  }
  return quitnow;
}

