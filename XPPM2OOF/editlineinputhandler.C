// -*- C++ -*-
// $RCSfile: editlineinputhandler.C,v $
// $Revision: 1.5 $
// $Author: langer $
// $Date: 2000-10-30 19:23:24 $

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */

#include <stdio.h>
#include "menustream.h"
#include "charstring.h"

extern "C" {
    extern char *readline(const char *);
    extern void add_history(const char *);
}

char *editlineinputhandler(const CharString &prompt, MenuStream&, FILE*)
{
    char *inputline = readline(prompt);
    if(inputline) {
	add_history(inputline);
	return inputline;
    }
    else
	return 0;
}
