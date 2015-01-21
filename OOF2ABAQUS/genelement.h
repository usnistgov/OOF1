// -*- C++ -*-
// $RCSfile: genelement.h,v $
// $Revision: 1.3 $
// $Author: langer $
// $Date: 2000-12-26 14:36:48 $


/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


// general triangular element

#ifndef GTELEMENT_H
#define GTELEMENT_H

class GTElement;

#include "cijkl.h"
#include "enum.h"
#include "eulerangle.h"
#include "trielement.h"
#include "node.h"
#include "typeregistry.h"
#include "vec.h"
#include <stdio.h>


class GTElement : public TriElement {
protected:
  EulerAngle orientation;
  static TrueFalse planestrain_dflt;
  static EulerAngle orientation_dflt;
  static Element *binaryread(FILE*, TrueFalse&);
  
public:
  GTElement() {}
  GTElement(Grid*);
  virtual ~GTElement() {}
  
  virtual const CharString &tag() const = 0;
  
  static void AddArgs(CommandM *cmd) {
    TriElement::AddArgs(cmd);
    AddArgument(cmd, "planestrain", planestrain_dflt);
    AddArgument(cmd, "orientation", orientation_dflt);

  }
  virtual void OptionalArgHandler() {
    TriElement::OptionalArgHandler();
  }
  
  virtual CharString parameters() const;

  virtual EulerAngle query_orientation() const {return orientation;}
  virtual bool same_type(const Element*) const;
};

#endif







