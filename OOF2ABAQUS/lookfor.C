// -*- C++ -*-
// $RCSfile: lookfor.C,v $
// $Revision: 1.1 $
// $Author: langer $
// $Date: 2000-12-22 15:15:25 $

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


#include "lookfor.h"
#include "ctype.h"
#include <string.h>

int lookfor(char target, istream &is) {
  int c;
  do {
    c = is.get();
    if(c == '\0' || c == EOF) {
      is.clear(ios::badbit|is.rdstate());
      return 0;
    }
  } while(is && isspace(c));

  if(c != target) {
    is.putback(c);
    is.clear(ios::badbit|is.rdstate());
    return 0;
  }

  return 1;
}

int lookfor(const char *target, istream &is) {
  int len = strlen(target);
  for(int i=0; i<len; i++)
    if(!lookfor(target[i], is)) return 0;
  return 1;
}
