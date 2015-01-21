// -*- C++ -*-
// $RCSfile: enumio.C,v $
// $Revision: 1.3 $
// $Author: langer $
// $Date: 2000-09-06 20:10:57 $

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


#ifndef ENUMIO_C
#define ENUMIO_C

#include <iostream.h>
#include <assert.h>
#include "enum.h"
#include "word.h"
#include "menuDef.h"
#include "readbinary.h"

template <class ENUMTYPE>
ostream &operator<<(ostream &os, const Enum<ENUMTYPE> &e) {
  assert(e.x < e.names().capacity());
  os << *e.names()[e.x];
  return os;
}

template <class ENUMTYPE>
istream &operator>>(istream &is, Enum<ENUMTYPE> &e) {
  Word name;
  Vec<const CharString*> &namelist = e.names();
  int i;
  is >> name;
  // look for exact match
  for(i=0; i<namelist.capacity(); i++) {
    if(namelist[i] && name == *namelist[i]) {
      e.x = (ENUMTYPE) i;
      return is;
    }
  }
  // look for unique abbreviation
  int len = strlen(name);
  int nfound = 0;
  int which = 0;
  for(i=0; i<namelist.capacity(); i++) {
    if(namelist[i] && !strncmp(name, *namelist[i], len)) {
      nfound++;
      if(nfound > 1) break;
      which =  i;
    }
  }
  if(nfound == 1) {
    e.x = (ENUMTYPE) which;
    return is;
  }
  is.clear(ios::badbit|is.rdstate());
  garcon()->msout << ms_error
    << "Bad name (" << name << ") for enumerated type!" << endl;
  garcon()->msout << "Choices are:";
  for(i=0; i<namelist.capacity(); i++)
    if(namelist[i])
      garcon()->msout << " " << *namelist[i];
  garcon()->msout << endl << ms_normal;
  return is;
}

template <class ENUMTYPE>
void writebinary(FILE *file, const Enum<ENUMTYPE> &x, const int n) {
  // Write Enums as null terminated strings.  Writing them as ints
  // isn't necessarily portable.
  const Enum<ENUMTYPE> *p = &x;
  for(int i=0; i<n; i++) {
    CharString nm = to_charstring(p[i]);
    int len = strlen(nm) + 1;	// include terminating null character
    fwrite(nm.charstar(), sizeof(char), len, file);
  }
}

template <class ENUMTYPE>
int readbinary(FILE *file, Enum<ENUMTYPE> &x, const int n) {
  Enum<ENUMTYPE> *p = &x;
  for(int i=0; i<n; i++) {
    CharString nm(10);		// give it some initial space
    char c;

    // read characters one by one until a null character is read
    do {
      if(!fread(&c, sizeof(char), 1, file))
	return i;		// number of Enums read so far
      nm += c;
    } while(c != '\0');

    // convert the string to an Enum
    istrstream is(nm.charstar());
    is >> p[i];
    if(!is) return i;
  }
  return n;
}


#endif
