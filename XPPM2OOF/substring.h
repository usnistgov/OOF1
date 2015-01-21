// -*- C++ -*-
// $RCSfile: substring.h,v $
// $Revision: 1.1 $
// $Author: langer $
// $Date: 2000-11-02 14:40:16 $

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */

#ifndef SUBSTRING_H
#define SUBSTRING_H

class CharString;
#include "vec.h"

class SubString {
private:
  Vec<CharString*> substrings;
  char separator;
public:
  SubString(const CharString&, char sep=',');
  SubString(const SubString&);
  ~SubString();
  int size() const { return substrings.capacity(); }
  const CharString &operator[](int i) const { return *substrings[i]; }
};

#endif
