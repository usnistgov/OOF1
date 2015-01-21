// -*- C++ -*-
// $RCSfile: trielement.h,v $
// $Revision: 1.3 $
// $Author: langer $
// $Date: 2000-12-26 14:36:49 $


/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


// Base class for triangular linear elements

#ifndef TRIELEMENT_H
#define TRIELEMENT_H

#include "element.h"
#include "symmmatrix.h"
#include "meshcoord.h"
#include "mvvd.h"

class TriElement : public Element {
public:
  TriElement() {}
  TriElement(Grid*);
  virtual ~TriElement() {}

  virtual void make_edgenodes(Grid*);

  virtual const CharString &tag() const = 0;
	
  static void AddArgs(CommandM *cmd) {
    Element::AddArgs(cmd);
  }

  virtual CharString parameters() const { return Element::parameters(); }
  virtual bool isempty() const { return false; }

protected:
  static Element *binaryread(FILE*, TrueFalse&);
  virtual bool same_type(const Element*) const;
  virtual void print(ostream&) const;

  // have to be able to read these, but don't have to do anything with
  // them.
  static SymmMatrix K_dflt;
  static MV_Vector_double thermal_dflt;
};

#endif // TRIELEMENT_H
