// -*- C++ -*-
// $RCSfile: emptyelement.h,v $
// $Revision: 1.2 $
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


// empty element

class EmptyElement;

#ifndef EMPTYELEMENT_H
#define EMPTYELEMENT_H

#include "trielement.h"
#include "typeregistry.h"

class EmptyElement : public TriElement {
private:
  static CharString nameplate;
  static ElementTypeRegistration reg;
  static Element *binaryread(FILE*, TrueFalse&);
public:
  // constructors
  EmptyElement(Grid*);
  EmptyElement() {}
  virtual ~EmptyElement() {}
  static void AddArgs(CommandM *cmd) {
    TriElement::AddArgs(cmd);
  }
  
  static int Nnodes;
  
  virtual const CharString &tag() const { return nameplate; }
  
  virtual bool isempty() const { return true; }
};

#endif
