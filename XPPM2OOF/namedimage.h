// -*- C++ -*-
// $RCSfile: namedimage.h,v $
// $Revision: 1.3 $
// $Author: langer $
// $Date: 2000-09-06 20:11:42 $


/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */

#ifndef NAMEDIMAGE_H
#define NAMEDIMAGE_H

#include "image.h"
#include "charstring.h"

class NamedImage : public Image {
public:
  NamedImage(const Image &im, const CharString &nm) : Image(im), name_(nm) {}
  NamedImage(const CharString &nm) : name_(nm) {}
  static int comparename(const NamedImage* &a, const NamedImage* &b) {
    return (a->name() == b->name());
  }
  const CharString &name() const { return name_; }
private:
  CharString name_;
};

#endif
