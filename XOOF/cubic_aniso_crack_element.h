// -*- C++ -*-
// $RCSfile: cubic_aniso_crack_element.h,v $
// $Revision: 1.1 $
// $Author: langer $
// $Date: 2001-02-01 15:39:59 $


/* This software was produced by NIST, an agency of the U.S. government,
* and by statute is not subject to copyright in the United States.
* Recipients of this software assume all responsibilities associated
* with its operation, modification and maintenance. However, to
* facilitate maintenance we ask that before distributing modifed
* versions of this software, you first contact the authors at
* oof_manager@ctcms.nist.gov.
*/


/* Cubic element with Griffith2 mutation */

// Element with Cubic symmetry,
// based on GTElement with energy release criterion
// element fracture on the crystal plane families {100}, {110}, {111}

// Fracture will happens on one crystal plane when stored strain
// energy is higher than the crystal plane fracture energy

// If strain energy is higher than several crystal plane's fracture energy
// fracture will happens on the plane on which the tensile stress is max


#ifndef CUBICANISOCRACK_H
#define CUBICANISOCRACK_H

class CubicAnisoCrackElement;

#include "genelement.h"
#include "mutableelement.h"

class CubicAnisoCrackElement : public GTElement, public MutableElement {
protected:
  double young, poisson, t_coeff, anisotropy;
  static double young_dflt, poisson_dflt, t_coeff_dflt, anisotropy_dflt;
#ifdef THERMAL
  double kappa;
  static double kappa_dflt;
#endif // THERMAL
  double  knockdown1, knockdown2, gamma_100, gamma_110, gamma_111;
  /*add fracture energy for different crystal plane */
  static double knockdown1_dflt, knockdown2_dflt, gamma_100_dflt, 
    gamma_110_dflt, gamma_111_dflt;
  static Element *binaryread(FILE*, TrueFalse&);
  static ElementTypeRegistration reg;
  static CharString nameplate;
  Cijkl virgin_cijkl() const;
public:
  CubicAnisoCrackElement() {}
  CubicAnisoCrackElement(Grid*);
  static int Nnodes;
  virtual const CharString &tag() const { return nameplate; }

  static void AddArgs(CommandM *cmd) {
    GTElement::AddArgs(cmd);
    AddArgument(cmd, "young", young_dflt);
    AddArgument(cmd, "poisson", poisson_dflt);
    AddArgument(cmd, "alpha", t_coeff_dflt);
#ifdef THERMAL
    AddArgument(cmd, "kappa", kappa_dflt);
#endif // THERMAL
    AddArgument(cmd, "anisotropy", anisotropy_dflt);
    AddArgument(cmd, "gamma_100", gamma_100_dflt);
    AddArgument(cmd, "gamma_110", gamma_110_dflt);
    AddArgument(cmd, "gamma_111", gamma_111_dflt);
    AddArgument(cmd, "knockdown1", knockdown1_dflt);
    AddArgument(cmd, "knockdown2", knockdown2_dflt);
    MutableElement::AddArgs(cmd);
  }

  virtual void binarywrite(FILE*, char) const;

  virtual CharString parameters() const;

  virtual Cijkl setCijkl() const;
  virtual SymmMatrix setAlpha() const;
#ifdef THERMAL
  virtual Kij setKij() const;
#endif // THERMAL

  virtual int mutate();
  virtual void revert();

  friend class Debug;
};

#endif

