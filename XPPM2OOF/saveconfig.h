// -*- C++ -*-
// $RCSfile: saveconfig.h,v $
// $Revision: 1.4 $
// $Author: langer $
// $Date: 2001-04-11 18:02:53 $


/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */

#ifndef SAVECONFIG_H
#define SAVECONFIG_H

#include <iostream.h>

class Goof;
class Menu;

void saveconfig(ostream&, const Goof*);
void loadconfig(const CharString&);		// sets current_goof
void loadconfig(istream &);
int configversion();
Menu *configmenu();

#endif
