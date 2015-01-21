// -*- C++ -*-
// $RCSfile: menuinterrupt.h,v $
// $Revision: 1.1 $
// $Author: langer $
// $Date: 2000-12-22 14:11:10 $


/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


// Interrupt handlers for Waiters

// If you are going to use an Interrupter in graphics mode, you MUST
// call interrupt_init(int argc, char *argv[]) BEFORE creating a
// FormWaiter.

#ifndef MENUINT_H
#define MENUINT_H

class InterruptHandler;
class TextInterruptHandler;

#include "menuDef.h"
#include "menuform.h"

class InterruptHandler;

class Interrupter {
    private:
	InterruptHandler *the_handler;
    public:
	Interrupter();
	~Interrupter();
	int operator()();
	void reset();
};

//void interrupt_init(int, char *[]);
//void interrupt_end();

#endif
