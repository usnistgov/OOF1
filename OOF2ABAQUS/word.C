// -*- C++ -*-
// $RCSfile: word.C,v $
// $Revision: 1.2 $
// $Author: langer $
// $Date: 2005-02-15 20:26:02 $


/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


#include "specialchars.h"
#include "word.h"
#include <iostream.h>
#include "ctype.h"
#ifdef DONT_HAVE_STRSTREAM_H
#include <strstream>
using namespace std;
#else
#include <strstream.h>
#endif

// class that has functionality of TextWaiter::word() in its operator>>.

istream &operator>>(istream &is, Word &w) {
  if(!is) {
    w = CharString("");
    return is;
  }
  
  int c;
  do {			    // skip leading spaces
    c = is.get();
    if(c == '\0' || c == '\n' || c == EOF) {
      is.clear(ios::badbit|is.rdstate());
      w = CharString("");
      return is;
    }
  } while(is && isspace(c));

  if(c == QUOTE) {		    // read quoted word
    while(is && (c=is.get())!=QUOTE && c!='\0' && c!='\n' && c!=EOF)
      w += c;
    if(!is) return is;
    if(c != QUOTE) {		// didn't find closing quote!
      is.clear(ios::badbit|is.rdstate());
      return is;
    }
  }
  else if(c == HELPCHAR) {	// copy one or two '?'s
    w = char(c);
    c = is.peek();		// is there another?
    if(!is) return is;
    if(c == HELPCHAR) {
      c = is.get();	// get second '?'
      w += c;
    }
  }
  else if(is_special(c)) {	// copy one special character
    w = char(c);
  }
  else {			// copy one whitespace delimited word
    w = char(c);	// explicit cast required: CharString(int) exists
    while(is && !isspace(c=is.get())
	  && !is_special(c) && c != '\0' && c != EOF)
      w += c;

    // Sun's CC, but not SGI's, sets ios::failbit when reading to the
    // end of the string. This makes it seem as if reading the word
    // failed.  So unset the failbit.
    if(is.eof())
      is.clear(is.rdstate() & ~ios::failbit);
    if(is_special(c)) is.putback(c);
  }
  return is;
}

// ------------------------------------------------------ //

WordList::WordList(const CharString &s) {
  CharString str(s);	// work with a copy
  char *next = &str[0];   // kludge to allow modification of copy
  do {
    char *ptr = next;
    skipspace(&ptr);
    char *word = ptr;
    if(*ptr == QUOTE) {
      ptr++;  // skip quote
      while(*ptr  && *ptr != QUOTE) ptr++;
      if(*ptr == QUOTE) ptr++;	// skip closing quote
      // else unterminated quote
    }
    else {
      while(*ptr && !isspace(*ptr))
	ptr++;
    }
    if(*ptr) *ptr++ = '\0';
    next = ptr;
    list.grow(1, new CharString(word));
  } while(*next);
}

WordList::WordList(const WordList &other) {
  freeall();
  list.resize(other.list.capacity());
  for(int i=0; i<list.capacity(); i++)
    list[i] = new CharString(other[i]);
}

const WordList &WordList::operator=(const WordList &other) {
  if(&other != this) {
    freeall();
    list.resize(other.list.capacity());
    for(int i=0; i<list.capacity(); i++)
      list[i] = new CharString(other[i]);
  }
  return *this;
}

void WordList::freeall() {
  for(int i=0; i<list.capacity(); i++)
    delete list[i];
}

WordList::~WordList() {
  freeall();
}

const WordList &WordList::operator+=(const CharString &str) {
  list.grow(1, new CharString(str));
  return *this;
}

WordList operator+(const WordList &wl, const CharString &str) {
  WordList newlist(wl);
  newlist += str;
  return newlist;
}

CharString WordList::concat(int skipstart, int skipend) const {
  if(list.capacity() <= skipstart + skipend)
    return CharString("");
  int last = list.capacity() - skipend;
  CharString str(*list[skipstart]);
  for(int i=skipstart + 1; i<last; i++) {
    str += " ";
    str += *list[i];
  }
  return str;
}

void WordList::strip(int n) {
  if(list.capacity() < n) n = list.capacity();
  for(int i=list.capacity()-n; i<list.capacity(); i++)
    delete list[i];
  list.grow(-n);
}

ostream &operator<<(ostream &os, const WordList &wl) {
  os << wl.concat();
  return os;
}
