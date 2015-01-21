
// -*- C++ -*-
// $RCSfile: ppm2oof.h,v $
// $Revision: 1.5 $
// $Author: langer $
// $Date: 2000-08-25 21:12:50 $


#ifndef PPM2OOF_H
#define PPM2OOF_H

#include "menuDef.h"

#include "beginheader.h"

Menu *ppm2oofmenu();
Menu *selectmethodmenu();
Menu *macromenu();
Menu *periodicmenu();

int goofinit(void);	// has current_goof been set yet?

EXTERN int restricted_colors DFLT(0);
EXTERN int textmode DFLT(0);

void fatalerror();

#include "endheader.h"

#endif
