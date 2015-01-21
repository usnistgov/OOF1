// -*- C++ -*-
// $RCSfile: memusage.C,v $
// $Revision: 1.4 $
// $Author: langer $
// $Date: 2001-10-25 18:58:22 $

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


#ifdef MEMUSAGE

#include "menuDef.h"
#include "memusage.h"
#include <iostream.h>
#ifdef HAVE_MALLOC_H
#include <malloc.h>
#endif

void print_memusage(const CharString &str) {
  static int lastused = 0;
  struct mallinfo memory = mallinfo();
  int used = memory.usmblks + memory.uordblks;
  garcon()->msout << ms_info << "MEMUSAGE------ " << str << ": " << used/1024.
		  << " Kbytes allocated" << ms_normal;
  if(lastused > 0)
    garcon()->msout << ms_info << " (" << used - lastused << " more bytes)"
		    << ms_normal;
  garcon()->msout << endl;
  lastused = used;
}

CommandFn pmemory() {
  print_memusage("");
}

#endif
