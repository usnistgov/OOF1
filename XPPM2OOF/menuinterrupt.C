// -*- C++ -*-
// $RCSfile: menuinterrupt.C,v $
// $Revision: 1.4 $
// $Author: langer $
// $Date: 2000-09-06 20:11:40 $

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


#include "menuDef.h"
#include "menuinterrupt.h"
#ifdef MENUGFX
#include "forms.h"
#include "menuform.h"
#include "menuinterruptform.h"
#include "menucolor.h"
#endif // MENUGFX
#include <signal.h>

// SunOS 4.x but not Solaris2 or SGI
#if defined(__sun) && !defined(__SVR4)
#define VARARG_SIGNALS
#endif


// ---------  Interrupt handling -------------------- //

class InterruptHandler {
    public:
	InterruptHandler() {
	    caught_it = 0;
	}
	static int caught_it;
	virtual ~InterruptHandler() {}
	virtual int operator()() = 0;
	virtual void reset() = 0;
};

int InterruptHandler::caught_it = 0;


class TextInterruptHandler : public InterruptHandler {
    private:
#ifdef VARARG_SIGNALS
        void (*oldsig)(int, ...);
#else
	void (*oldsig)(int);
#endif
	static int count;
    public:
	TextInterruptHandler();
	~TextInterruptHandler();
	virtual int operator()() { return caught_it; }
	virtual void reset();
};

int TextInterruptHandler::count = 0;

#ifdef MENUGFX

class FormInterruptHandler : public InterruptHandler {
private:
  static int count;
  static InterruptForm *iform;
public:
  FormInterruptHandler();
  ~FormInterruptHandler();
  virtual int operator()();
  virtual void reset();
  friend class InterruptForm;
};

int FormInterruptHandler::count = 0;
InterruptForm *FormInterruptHandler::iform = 0;
#endif // MENUGFX

// -------------------------------------------------------- //

/* the Interrupter class just frees the user from having to even
 * know that there are two kinds of interrupters.
 */

Interrupter::Interrupter() {
#ifdef MENUGFX
    if(garcon()->graphics())
	the_handler = new FormInterruptHandler;
    else
#endif // MENUGFX
	the_handler = new TextInterruptHandler;
}

Interrupter::~Interrupter() {
    delete the_handler;
}

void Interrupter::reset() {
    the_handler->reset();
}

int Interrupter::operator()() {
    return (*the_handler)();
}

// ----------------------------------------------------- //

#ifndef VARARG_SIGNALS
static void catch_it(int) {
    InterruptHandler::caught_it = 1;
}
#else
static void catch_it(int, ...) {
    InterruptHandler::caught_it = 1;
}
#endif

TextInterruptHandler::TextInterruptHandler() {
    if(count++ == 0)
	oldsig = signal(SIGINT, catch_it);
}

TextInterruptHandler::~TextInterruptHandler() {
    if(--count == 0)
	(void) signal(SIGINT, oldsig);
}

void TextInterruptHandler::reset() {
    caught_it = 0;
    (void) signal(SIGINT, catch_it);
}

// ----------------------------------------------------- //

#ifdef MENUGFX
FormInterruptHandler::FormInterruptHandler() {
  if(iform == 0) {
    iform = new InterruptForm;
    fl_deactivate_all_forms();
    // All other forms are deactivated if an InterruptForm is visible
  }
  count++;
  iform->makevisible();
}

FormInterruptHandler::~FormInterruptHandler() {
  if(--count == 0) {
    delete iform;
    iform = 0;
    fl_activate_all_forms();
  }
}

int FormInterruptHandler::operator()() {
  if(XEventsQueued(fl_get_display(), QueuedAfterReading))
    fl_check_forms();
  return caught_it;
}

void FormInterruptHandler::reset() {
  caught_it = 0;
}

InterruptForm::InterruptForm()
    : Form("stop it!")
{
  create_form_interruptform(this);
  fl_set_object_color(IF_stop, COL_IF_STOP_UP, COL_IF_STOP_DOWN);
}

void InterruptForm::makevisible() {
  if(the_form->visible) return;
  fl_show_form(the_form, FL_PLACE_MOUSE, FL_TRANSIENT, name.charstar());
  Nforms()++;
}

void IFCB_stop(FL_OBJECT*, long ptr) {
  if(Form::exists(ptr))
    ((InterruptForm*) ptr)->stopbutton();
}

void InterruptForm::stopbutton() {
  InterruptHandler::caught_it = 1;
}

#endif // MENUGFX
