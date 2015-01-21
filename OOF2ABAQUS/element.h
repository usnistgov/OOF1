// -*- C++ -*-
// $RCSfile: element.h,v $
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


// element types
 
#ifndef ELEMENT_H
#define ELEMENT_H

class Element;

#include "truefalse.h"
#include "vec.h"
#include "menuDef.h"
#include <iostream.h>
#include <stdio.h>

class Grid;
class EulerAngle;
class Node;
class ElementGroup;
class BP_block;
class SymmMatrix;
class MeshCoord;

// base class for elements

class Element {
private:
  Element(const Element&); // prohibited!
  Element &operator=(const Element&); // prohibited
public:
  int index;			// unique among all elements
  Vec<Node*> corner;		// pointers to corners
  Vec<Node*> edgenode;

  TrueFalse planestrain;
 // moved here from isoelement.h and genelement.h because abaqus
 // requires it for all elements

public:	// would be protected if friend templates were allowed
  static float gray_dflt;
  static int index_dflt;
  static Vec<int> nodenumber_dflt;
  static Element *binaryread(FILE*, TrueFalse&);
  static unsigned char inputformatflag;
  static int cloneindex;	// element to copy stiffness from
protected:
  virtual bool same_type(const Element*) const;
  virtual void print(ostream&) const = 0;
public:
  // constructors
  Element() {}
  Element(Grid*);
  virtual ~Element();

  virtual void make_edgenodes(Grid *) = 0;
	
  // for identification purposes only
  virtual const CharString &tag() const = 0;
  virtual bool isempty() const { return false; }

  static void reset_defaults();	// resets default values for binaryread
	
  // for construction of initialization menus in ElementCmd
  static void AddArgs(CommandM *cmd) {
    AddArgument(cmd, "gray", gray_dflt);
  }
  // called before create_element during execution of ElementCmd
  virtual void OptionalArgHandler() {}

  virtual CharString parameters() const { return CharString();}

  float intrinsic_gray;

  friend ostream &operator<<(ostream&, const Element&);
  friend bool operator==(const Element&, const Element&);
};

#endif


