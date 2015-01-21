// -*- C++ -*-
// $RCSfile: word.h,v $
// $Revision: 1.1 $
// $Author: langer $
// $Date: 2000-12-22 14:11:10 $


/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


#ifndef WORD_H
#define WORD_H

class Word;
class WordList;

#include "charstring.h"
#include "vec.h"
#include <iostream.h>

class Word : public CharString {
public:
  Word() {}
  Word(const CharString &s) : CharString(s) {}
  Word(const char *s) : CharString(s) {}
  Word &operator=(const char c) {
    CharString::operator=(c);
    return *this;
  }   	
  friend istream &operator>>(istream&, Word&);
};


class WordList {
private:
  Vec<CharString*> list;
  void freeall();
public:
  WordList() {}
  WordList(const CharString &);
  WordList(const WordList&);
  ~WordList();
  const WordList &operator=(const WordList&);

  const WordList &operator+=(const CharString&); // append
  friend WordList operator+(const WordList&, const CharString&); // append
  void strip(int n=1); // remove n words from end
  void resize(int n) { list.resize(n); }
  void clear();        // remove all words

  int size() const { return list.capacity(); }

  CharString operator[](const int i) const { return *list[i]; }

  // concatenation of all words (space delimited)
  CharString concat(int n, int m) const; // skip n at beginning, m at end
  CharString concat() const { return concat(0, 0); }

  friend ostream &operator<<(ostream &os, const WordList&);
};


#endif
