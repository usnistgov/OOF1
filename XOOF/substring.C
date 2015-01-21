// -*- C++ -*-
// $RCSfile: substring.C,v $
// $Revision: 1.1 $
// $Author: langer $
// $Date: 2000-11-03 14:56:21 $

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */

#include "substring.h"
#include "charstring.h"
#include "vec.h"

static const char ESCAPE = '\\';
static const char DOUBLEQUOTE = '"';
static const char SINGLEQUOTE = '\'';

class ParserState {
protected:
  static char separator;
public:
  ParserState() {}
  virtual ~ParserState() {}
  virtual ParserState *store(Vec<CharString*>&, char) = 0;
  static void set_separator(char c) { separator = c; }
};

char ParserState::separator(';');

class NormalState : public ParserState {
public:
  NormalState() {}
  virtual ParserState *store(Vec<CharString*> &subs, char c);
};

class EscapeState : public ParserState {
public:
  EscapeState() {}
  virtual ParserState *store(Vec<CharString*> &subs, char c) {
    *subs[subs.capacity()-1] += c;
    delete this;
    return new NormalState;
  }
};

class SingleQuoteState : public ParserState {
public:
  SingleQuoteState() {}
  virtual ParserState *store(Vec<CharString*> &subs, char c) {
    if(c == SINGLEQUOTE) {
      delete this;
      return new NormalState;
    }
    *subs[subs.capacity()-1] += c;
    return this;
  }
};

class DoubleQuoteState : public ParserState {
public:
  DoubleQuoteState() {}
  virtual ParserState *store(Vec<CharString*> &subs, char c) {
    if(c == DOUBLEQUOTE) {
      delete this;
      return new NormalState;
    }
    *subs[subs.capacity()-1] += c;
    return this;
  }
};

ParserState *NormalState::store(Vec<CharString*> &subs, char c) {
  if(subs.capacity() == 0)
    subs.grow(1, new CharString);
  if(c == ESCAPE) {
    delete this;
    return new EscapeState;
  }
  if(c == SINGLEQUOTE) {
    delete this;
    return new SingleQuoteState;
  }
  if(c == DOUBLEQUOTE) {
    delete this;
    return new DoubleQuoteState;
  }
  if(c == separator)
    subs.grow(1, new CharString);
  else
    *subs[subs.capacity()-1] += c;
  return this;
}

SubString::SubString(const CharString &str, char sep)
  : separator(sep)
{
  const char *ptr = &str[0];
  ParserState::set_separator(sep);
  ParserState *ps = new NormalState;
  while(*ptr != '\0')
    ps = ps->store(substrings, *ptr++);
}

SubString::~SubString() {
  for(int i=0; i<substrings.capacity(); i++)
    delete substrings[i];
}

SubString::SubString(const SubString &other) {
  substrings.setphysicalsize(other.substrings.capacity());
  for(int i=0; i<substrings.capacity(); i++)
    substrings[i] = new CharString(*other.substrings[i]);
}
