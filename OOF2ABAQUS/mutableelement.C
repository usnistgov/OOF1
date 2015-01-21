// -*- C++ -*-
// $RCSfile: mutableelement.C,v $
// $Revision: 1.3 $
// $Author: langer $
// $Date: 2000-12-22 21:24:32 $


/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


#include "element.h"
#include "inputflags.h"
#include "mutableelement.h"
#include "menuDef.h"
#include "readbinary.h"

TrueFalse MutableElement::mutated_dflt;
Cijkl MutableElement::cijkl_dflt;
float MutableElement::oldgray_dflt;
EulerAngle MutableElement::oldorientation_dflt;
TrueFalse MutableElement::onceonly_dflt;

void MutableElement::reset_defaults() {
  mutated_dflt = TF_FALSE;
  oldgray_dflt = 0;
  oldorientation_dflt = 0;
  onceonly_dflt = TF_FALSE;
}

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

void MutableElement::AddArgs(CommandM *cmd) {
  AddArgument(cmd, "only_once", onceonly_dflt);
  AddArgument(cmd, "cm11", cijkl_dflt(0,0), MOPT_SECRET);
  AddArgument(cmd, "cm12", cijkl_dflt(1,0), MOPT_SECRET);
  AddArgument(cmd, "cm13", cijkl_dflt(2,0), MOPT_SECRET);
  AddArgument(cmd, "cm14", cijkl_dflt(3,0), MOPT_SECRET);
  AddArgument(cmd, "cm15", cijkl_dflt(4,0), MOPT_SECRET);
  AddArgument(cmd, "cm16", cijkl_dflt(5,0), MOPT_SECRET);

  AddArgument(cmd, "cm22", cijkl_dflt(1,1), MOPT_SECRET);
  AddArgument(cmd, "cm23", cijkl_dflt(2,1), MOPT_SECRET);
  AddArgument(cmd, "cm24", cijkl_dflt(3,1), MOPT_SECRET);
  AddArgument(cmd, "cm25", cijkl_dflt(4,1), MOPT_SECRET);
  AddArgument(cmd, "cm26", cijkl_dflt(5,1), MOPT_SECRET);

  AddArgument(cmd, "cm33", cijkl_dflt(2,2), MOPT_SECRET);
  AddArgument(cmd, "cm34", cijkl_dflt(3,2), MOPT_SECRET);
  AddArgument(cmd, "cm35", cijkl_dflt(4,2), MOPT_SECRET);
  AddArgument(cmd, "cm36", cijkl_dflt(5,2), MOPT_SECRET);

  AddArgument(cmd, "cm44", cijkl_dflt(3,3), MOPT_SECRET);
  AddArgument(cmd, "cm45", cijkl_dflt(4,3), MOPT_SECRET);
  AddArgument(cmd, "cm46", cijkl_dflt(5,3), MOPT_SECRET);

  AddArgument(cmd, "cm55", cijkl_dflt(4,4), MOPT_SECRET);
  AddArgument(cmd, "cm56", cijkl_dflt(5,4), MOPT_SECRET);

  AddArgument(cmd, "cm66", cijkl_dflt(5,5), MOPT_SECRET);

  AddArgument(cmd, "oldgray", oldgray_dflt, MOPT_SECRET);
  AddArgument(cmd, "oldorientation", oldorientation_dflt, MOPT_SECRET);
  AddArgument(cmd, "mutated", mutated_dflt, MOPT_SECRET);
}

CharString MutableElement::parameters() const {
  CharString p(" only_once=" + to_charstring(onceonly));
  if(mutated)
    p +=
      " mutated=" + to_charstring(mutated);
//        " oldgray=" + to_charstring(oldgray) +
//        " oldorientation=" + to_charstring(oldorientation) +
//        " cm11=" + to_charstring(cijkl(0,0)) +
//        " cm12=" + to_charstring(cijkl(1,0)) +
//        " cm13=" + to_charstring(cijkl(2,0)) +
//        " cm14=" + to_charstring(cijkl(3,0)) +
//        " cm15=" + to_charstring(cijkl(4,0)) +
//        " cm16=" + to_charstring(cijkl(5,0)) +
      
//        " cm22=" + to_charstring(cijkl(1,1)) +
//        " cm23=" + to_charstring(cijkl(2,1)) +
//        " cm24=" + to_charstring(cijkl(3,1)) +
//        " cm25=" + to_charstring(cijkl(4,1)) +
//        " cm26=" + to_charstring(cijkl(5,1)) +
      
//        " cm33=" + to_charstring(cijkl(2,2)) +
//        " cm34=" + to_charstring(cijkl(3,2)) +
//        " cm35=" + to_charstring(cijkl(4,2)) +
//        " cm36=" + to_charstring(cijkl(5,2)) +
      
//        " cm44=" + to_charstring(cijkl(3,3)) +
//        " cm45=" + to_charstring(cijkl(4,3)) +
//        " cm46=" + to_charstring(cijkl(5,3)) +
      
//        " cm55=" + to_charstring(cijkl(4,4)) +
//        " cm56=" + to_charstring(cijkl(5,4)) +
      
//        " cm66=" + to_charstring(cijkl(5,5));
    return p;
}

int MutableElement::binaryread(FILE *file) {
  int f;
  if(!readbinary(file, f)) return 0;
  onceonly_dflt = f;
  mutated_dflt = Element::inputformatflag & MUTATED;
  if(mutated_dflt) {
    if(!readbinary(file, oldgray_dflt)) return 0;
    float a, b, c;
    if(!readbinary(file, a)) return 0;
    if(!readbinary(file, b)) return 0;
    if(!readbinary(file, c)) return 0;
    oldorientation_dflt = EulerAngle(a, b, c);
    return cijkl_dflt.binaryread(file);
  }
  return 1;
}

bool MutableElement::same_type(const Element *el) const {
  const MutableElement *other = dynamic_cast<const MutableElement*>(el);
  return other
    && mutated == other->mutated
    && onceonly == other->onceonly;
}
