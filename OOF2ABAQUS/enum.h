// -*- C++ -*-
// $RCSfile: enum.h,v $
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


/* Template for enumerated type that can be read and written by name.
 * 
 * Example:
 *   typedef enum {A, B, C} Letter;   // define a normal enumerated type
 *   Enum<Letter> x(A);    // define a printable variable of that type 
 *   // before using the variable, define the printable versions:
 *   Enum<Letter>::name(A, "hay");
 *   Enum<Letter>::name(B, "bee");
 *   Enum<Letter>::name(C, "see");
 *   cout << x << endl;             // This line prints "hay"
 *   cin >> x;     // this line reads "hay", "bee", or "see" and sets x
 *   switch(x) {   // x can be used wherever a Letter could be used.
 *     case A: ...
 *   }
 */

#ifndef ENUM_H
#define ENUM_H

template <class ENUMTYPE> class Enum;
#ifdef MENUGFX
template <class ENUMTYPE> class EnumVarObject;
#endif

#include <iostream.h>
#include "charstring.h"
#include "vec.h"

#ifdef MENUGFX
#include "menuDef.h"
#include "varobjects.h"
#endif

template <class ENUMTYPE>
ostream &operator<<(ostream &, const Enum<ENUMTYPE>&);
template <class ENUMTYPE>
istream &operator>>(istream &, Enum<ENUMTYPE>&);

template <class ENUMTYPE>
class Enum {
private:
  ENUMTYPE x;
public:
  Enum() : x(ENUMTYPE(0)) {}
  Enum(const ENUMTYPE &y) : x(y) {}
  virtual ~Enum() {}
  operator ENUMTYPE() const { return x; }
  static void name(const ENUMTYPE&, const CharString &nm);
  static Vec<const CharString*> &names();
#if defined(sun) && !defined(__GNUG__)
  friend ostream &operator<<(ostream &, const Enum<ENUMTYPE>&);
  friend istream &operator>>(istream &, Enum<ENUMTYPE>&);
#else
  friend ostream &operator<< <>(ostream &, const Enum<ENUMTYPE>&);
  friend istream &operator>> <>(istream &, Enum<ENUMTYPE>&);
#endif
  friend bool operator==(const Enum<ENUMTYPE> &p, const Enum<ENUMTYPE> &q) {
    return p.x == q.x;
  }
  friend bool operator!=(const Enum<ENUMTYPE> &p, const Enum<ENUMTYPE> &q) {
    return p.x != q.x;
  }
  friend bool operator==(const Enum<ENUMTYPE> &p, const ENUMTYPE &q) {
    return p.x == q;
  }
  friend bool operator!=(const Enum<ENUMTYPE> &p, const ENUMTYPE &q) {
    return p.x != q;
  }
};

template <class ENUMTYPE>
void writebinary(FILE*, const Enum<ENUMTYPE>&, const int n=1);

template <class ENUMTYPE>
int readbinary(FILE*, Enum<ENUMTYPE>&, const int n=1);

#ifdef MENUGFX

// classes for putting Enum's into menu forms:

// EVO is a base class for the templated EnumVarObject class.
// The callback function can't be a template since it has no
// arguments that can be used to determine the type. So it has
// to invoke a virtual function through the EVO base class.

class EVO : public VarObject {
public:
  EVO(Variable *vr, Form *frm, VarObjectType typ)
    : VarObject(vr, frm, typ)
      {
      }
  virtual ~EVO() {}
  virtual void set() = 0;
};

template <class ENUMTYPE>
class EnumVarObject : public EVO {
private:
  Vec<ENUMTYPE> value;  // values taken by enum, indexed by position in menu
  static void CB_choice(FL_OBJECT*, long);
public:
  EnumVarObject(Variable*, Form*, VarObjectType, int, int, int, int);
  FL_OBJECT *choice;
  virtual void set();
  virtual void show();
  virtual void hide();
  virtual CharString get();
};

#endif

#include "enum.C"

#endif
