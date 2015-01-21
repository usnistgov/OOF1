// -*- C++ -*-
// $RCSfile: editlineinputhandler.h,v $
// $Revision: 1.1 $
// $Author: langer $
// $Date: 2000-12-22 14:11:09 $


/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


#ifndef EDITLINEINPUT_H
#define EDITLINEINPUT_H

#include <stdio.h>
#include "menustream.h"
#include "charstring.h"

char *editlineinputhandler(const CharString &prompt, MenuStream &pfile, FILE *ifile);

#endif
