// -*- C++ -*-
// $RCSfile: charstring.C,v $
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

#include "charstring.h"
#include <stdio.h>
#include <string.h>
#include "ctype.h"

CharString &CharString::operator=(const CharString &s) {
    if(&s != this) {
	if(str) delete [] str;
	str = new char[strlen(s.str) + 1];
	strcpy(str, s.str);
    }
    return *this;
}

CharString &CharString::operator=(const char *s) {
    if(str) delete [] str;
    str = new char[strlen(s) + 1];
    strcpy(str, s);
    return *this;
}

CharString &CharString::operator=(const char c) {
    if(str) delete [] str;
    str = new char[2];
    str[0] = c;
    str[1] = '\0';
    return *this;
}

// length

size_t strlen(const CharString &s) {
    return strlen(s.str);
}

// concatenation

CharString &CharString::operator+=(const CharString &s) {
    char *newstr = new char[strlen(str) + strlen(s.str) + 1];
    strcat(strcpy(newstr, str), s.str);
    delete [] str;
    str = newstr;
    return *this;
}

CharString &CharString::operator+=(const char *s) {
    char *newstr = new char[strlen(str) + strlen(s) + 1];
    strcat(strcpy(newstr, str), s);
    delete [] str;
    str = newstr;
    return *this;
}

CharString &CharString::operator+=(const char c) {
    int len=strlen(str);
    char *newstr = new char[len + 2];
    strcpy(newstr, str);
    newstr[len] = c;
    newstr[len+1] = '\0';
    delete [] str;
    str = newstr;
    return *this;
}

CharString operator+(const CharString &s1, const CharString &s2) {
    CharString newcs;
    newcs.str = new char[strlen(s1.str) + strlen(s2.str) + 1];
    strcat(strcpy(newcs.str, s1.str), s2.str);
    return newcs;
}

CharString operator+(const CharString &s1, const char *s2) {
    CharString newcs;
    newcs.str = new char[strlen(s1.str) + strlen(s2) + 1];
    strcat(strcpy(newcs.str, s1.str), s2);
    return newcs;
}

CharString operator+(const char* s1, const CharString &s2) {
    CharString newcs;
    newcs.str = new char[strlen(s1) + strlen(s2.str) + 1];
    strcat(strcpy(newcs.str, s1), s2.str);
    return newcs;
}

CharString operator+(const CharString &s1, const char c) {
    CharString newcs;
    int len = strlen(s1.str);
    newcs.str = new char[len + 2];
    strcpy(newcs.str, s1.str);
    newcs.str[len] = c;
    newcs.str[len+1] = '\0';
    return newcs;
}

CharString operator+(const char c, const CharString &s1) {
    CharString newcs;
    int len = strlen(s1.str);
    newcs.str = new char[len + 2];
    newcs.str[0] = c;
    strcpy(&newcs.str[1], s1.str);
    return newcs;
}

CharString CharString::spaceappend(const CharString &s) const {
    if(strlen(str) == 0)
	return s;
    else
	return *this + " " + s;
}

CharString CharString::despace(char subst) const {
  CharString copy(*this);
  int l = strlen(copy.str);
  for(int i=0; i<l; i++)
    if(copy.str[i] == ' ') copy.str[i] = subst;
  return copy;
}

CharString CharString::head(int n) const {
  if(n > strlen(str))
    return *this;
  CharString anterior(n+1);
  strncpy(anterior.str, str, n);
  anterior.str[n] = '\0';
  return anterior;
}

CharString CharString::tail(int n) const {
  int len = strlen(str);
  if(n > len)
    return *this;
  CharString posterior(n+1);
  int start = len - n;
  for(int i=0; i<n; i++)
    posterior[i] = str[start + i];
  posterior[n] = '\0';
  return posterior;
}

ostream &operator<<(ostream &os, const CharString &cs) {
  return os << cs.str;
}

istream &operator>>(istream &is, CharString &cs) {
  CharString buf;
  char c;
  int quoting = 0;
  int starting = 1;
  while(is.get(c)) {
    if(quoting) {
      if(c == '"')
	break;
      else
	buf += c;
    }
    else if(starting) {
      if(c == '\n') break;     // empty input
      if(isspace(c)) continue;
      if(c == '"')
	quoting = 1;
      else
	buf += c;
      starting = 0;
    }
    else {
      if(c == '\n') break;
      buf += c;
    }
  }
  cs = buf;
  return is;
}

// -------------------------------------------------------------- //

void skipspace(const char ** ptr) {
    while(isspace(**ptr)) (*ptr)++;
}

void skipspace(char **ptr) {
  while(isspace(**ptr)) (*ptr)++;
}

const char *skipspace(const CharString &str) {
    const char *c = str;
    return skipspace(c);
}

const char *skipspace(const char *ptr) {
    while(isspace(*ptr)) *ptr++;
    return ptr;
}

char *skipspace(char *ptr) {
    while(isspace(*ptr)) *ptr++;
    return ptr;
}

