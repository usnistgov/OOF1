// -*- C++ -*-
// $RCSfile: cubic_aniso_crack_element.C,v $
// $Revision: 1.2 $
// $Author: langer $
// $Date: 2001-02-18 02:42:44 $


/* This software was produced by NIST, an agency of the U.S. government,
* and by statute is not subject to copyright in the United States.
* Recipients of this software assume all responsibilities associated
* with its operation, modification and maintenance. However, to
* facilitate maintenance we ask that before distributing modifed
* versions of this software, you first contact the authors at
* oof_manager@ctcms.nist.gov.
*/

// Code contributed by Yi Fang, University of Houston

// Element with Cubic symmetry,
// based on GTElement with energy release criterion
// element fracture on the crystal plane families {100}, {110}, {111}

// Fracture will happens on one crystal plane when stored strain
// energy is higher than the crystal plane fracture energy

// If strain energy is higher than several crystal plane's fracture
// energy fracture will happens on the plane on which the tensile
// stress is max


//note that the thermal coefficients are not knocked down

#include "createelement.h"
#include "inputflags.h"
#include "readbinary.h"
#include "symeig3.h"
#include "cubic_aniso_crack_element.h"
#include <math.h>

double plane_index_100[3][3] = {{1,0,0}, {0,1,0}, {0,0,1}};
double plane_index_110[6][3] = {{1,1,0}, {1,-1,0}, {0,1,1}, {0,1,-1}, 
{1,0,1}, {1,0,-1}};
double plane_index_111[4][3] = {{1,1,1}, {1,-1,1}, {-1,1,1}, {-1,-1,1}};
// the possible fracture crystal plane indexes
//double v[3];
//int i;

//int counter;

// static default data
CharString CubicAnisoCrackElement::nameplate("cubic_aniso_crack");

double CubicAnisoCrackElement::young_dflt(1.0);
double CubicAnisoCrackElement::poisson_dflt(0.0);
double CubicAnisoCrackElement::t_coeff_dflt(0.0);
double CubicAnisoCrackElement::anisotropy_dflt(1.0);
#ifdef THERMAL
double CubicAnisoCrackElement::kappa_dflt(1.0);
#endif // THERMAL
double CubicAnisoCrackElement::knockdown1_dflt(0.1);
double CubicAnisoCrackElement::knockdown2_dflt(0.2);
double CubicAnisoCrackElement::gamma_100_dflt(0.1);
double CubicAnisoCrackElement::gamma_110_dflt(0.1);
double CubicAnisoCrackElement::gamma_111_dflt(0.1);
int CubicAnisoCrackElement::Nnodes(3);

ElementTypeRegistration
CubicAnisoCrackElement::reg(nameplate, CubicAnisoCrackElement::binaryread);

CubicAnisoCrackElement::CubicAnisoCrackElement(Grid *g)
  : GTElement(g),
    young(young_dflt),
    poisson(poisson_dflt),
    t_coeff(t_coeff_dflt),
    anisotropy(anisotropy_dflt),
#ifdef THERMAL
    kappa(kappa_dflt),
#endif // THERMAL
    knockdown1(knockdown1_dflt),
    knockdown2(knockdown2_dflt),
    gamma_100(gamma_100_dflt),
    gamma_110(gamma_110_dflt),
    gamma_111(gamma_111_dflt)
{
  if( mutated) {
    oldgray = oldgray_dflt;
    oldorientation = oldorientation_dflt;
  }
  else
    cijkl = virgin_cijkl();
  if(!Kset) {
    setstiffness();
#ifdef THERMAL
    setstiffnessT();
#endif // THERMAL
    Kset = TF_TRUE;
  }
}

Cijkl CubicAnisoCrackElement::virgin_cijkl() const {
  Cijkl c;
  double tmp = young/((1.+poisson)*(1.-2*poisson));
  c(0, 0) = c(1, 1) = c(2, 2) = tmp*(1.-poisson);
  c(0, 1) = c(0, 2) = c(1, 2) = tmp*poisson;
  c(3, 3) = c(4, 4) = c(5, 5) = 0.5*anisotropy*young/(1.+poisson);
  return c;
}

void CubicAnisoCrackElement::binarywrite(FILE *file, char formatflag) const 
{
  if(mutated) formatflag |= MUTATED;
  GTElement::binarywrite(file, formatflag);

  float c;
  c = poisson;  writebinary(file, c);	   // poisson (float)
  c = young;  writebinary(file, c);	   // young (float)
  c = t_coeff;  writebinary(file, c);	   // alpha (float)
  c = anisotropy;  writebinary(file, c);   // anisotropy (float)
#ifdef THERMAL
  c = kappa; writebinary(file, c);	   // kappa (float)
#endif // THERMAL
  float gma_100 = gamma_100;
  float gma_110 = gamma_110;
  float gma_111 = gamma_111;
  float kd1 = knockdown1;
  float kd2 = knockdown2;
  writebinary(file, gma_100);	// gamma_100 (float)
  writebinary(file, gma_110);	// gamma_110 (float)
  writebinary(file, gma_111);	// gamma_111 (float)
  writebinary(file, kd1);	// knockdown 1 (float)
  writebinary(file, kd2);	// knockdown 2 (float)
  MutableElement::binarywrite(file);
}


// static member function that reads the goof file BEFORE creating an
// element.

Element *CubicAnisoCrackElement::binaryread(FILE *file, TrueFalse &ok) {

  // read base class information
  GTElement::binaryread(file, ok);
  if(!ok) return 0;

  // read data specific to this class and set the static variables that will
  // be used by the constructor.
  float c;
  if(!readbinary(file, c)) { ok = TF_FALSE; return 0; } // poisson (float)
  poisson_dflt = c;  // float to double
  if(!readbinary(file, c)) { ok = TF_FALSE; return 0; } // young (float)
  young_dflt = c;  // float to double
  if(!readbinary(file, c)) { ok = TF_FALSE; return 0; } // alpha (float)
  t_coeff_dflt = c;  // float to double
  if(!readbinary(file, c)) { ok = TF_FALSE; return 0; } // anisotropy (float)
  anisotropy_dflt = c;  // float to double
#ifdef THERMAL
  if(!readbinary(file, c)) { ok = TF_FALSE; return 0; } // kappa (float)
  kappa_dflt = c;  // float to double
#endif // THERMAL

  float  kd1, kd2, gma_100, gma_110, gma_111;

  if(!readbinary(file, gma_100)) { ok = TF_FALSE; return 0; } // maxstress (float)
  gamma_100_dflt = gma_100;
  if(!readbinary(file, gma_110)) { ok = TF_FALSE; return 0; } // maxstress (float)
  gamma_110_dflt = gma_110;
  if(!readbinary(file, gma_111)) { ok = TF_FALSE; return 0; } // maxstress (float)
  gamma_111_dflt = gma_111;
  if(!readbinary(file, kd1)) { ok = TF_FALSE; return 0; } // knockdown1 (float)
  knockdown1_dflt = kd1;
  if(!readbinary(file, kd2)) { ok = TF_FALSE; return 0; } // knockdown2 (float)
  knockdown2_dflt = kd2;

  if(!MutableElement::binaryread(file))
    return 0;

  // create the element
  return create_element((CubicAnisoCrackElement*) 0);
}


CharString CubicAnisoCrackElement::parameters() const {
  return GTElement::parameters()
    + " poisson=" + to_charstring(poisson)
    + " young=" + to_charstring(young)
    + " alpha=" + to_charstring(t_coeff)
#ifdef THERMAL
    + " kappa=" + to_charstring(kappa)
#endif // THERMAL
    + " anisotropy=" + to_charstring(anisotropy)
    + " gamma_100=" + to_charstring(gamma_100)
    + " gamma_110=" + to_charstring(gamma_110)
    + " gamma_111=" + to_charstring(gamma_111)
    + " knockdown1=" + to_charstring(knockdown1)
    + " knockdown2=" + to_charstring(knockdown2)
    + MutableElement::parameters();
}

Cijkl CubicAnisoCrackElement::setCijkl() const {
  return cijkl;
}

#ifdef THERMAL
Kij CubicAnisoCrackElement::setKij() const {
  Kij k;
  k(0, 0) = k(1, 1) = k(2, 2) = kappa;
  return k;
}
#endif // THERMAL

SymmMatrix CubicAnisoCrackElement::setAlpha() const {
  SymmMatrix a(3);
  a(0, 0) = a(1, 1) = a(2, 2) = t_coeff;
  return a;
}

int CubicAnisoCrackElement::mutate() {

  //the following tmp data set will stored the information of
  //temporary fracture surface (crystal plane index, tensile stress
  //and it's euler angle)
  MV_Vector_double tmp_crystal_plane(3, 0.0);  //initialize the tmp data set
  double tmp_max_stress = 0.0;
  EulerAngle tmp_EA_transform(SphereCoord(tmp_crystal_plane));

  // the total stored energy must exceed the surface energy times a
  // characteristic element length, for the element length use the
  // diameter of a circle with equivalent element area e >= 2 gamma
  // l/area = 8 gamma/(pi l) = 4 gamma/sqrt(pi*area)
  if(onceonly && mutated)
    return 0;
  stresstensor();
  EigenVector stresseig1, stresseig2, stresseig3;
  eigensystem(stress, stresseig1, stresseig2, stresseig3);
  // ensure that the max tensile stress is pos.
  if(stresseig1.eigenvalue <= 0)
    return 0;

  // first calculate the possible fracture surface in crystal plane
  // family {100}
   double release_rate_100 = 4*gamma_100/sqrt(M_PI*original_area());


   // family {100}: only plane (100), (010) and (001) will be
   // considered in calculation
   for (int counter = 0; counter <3; counter++) {
     // assign the euler angle for the normal of current crystal plane
     MV_Vector_double v(3);
     for (int i = 0; i<3; i++)
       v[i] = plane_index_100[counter][i];
     SphereCoord sph(v);
     EulerAngle euler(sph);

     // transform the elastic strain tensor so that the zz component is
     // in the direction of the normal of the particular crystal plane
     elastic_straintensor();
     StrainStress estrain(elastic_strain.transform(euler));

     // calculate the elastic stress tensor so that the zz component is
     // in the direction of the normal of the particular crystal plane
     StrainStress estress(stress.transform(euler));

     // use only the part of the energy that will be released when the
     // modulus is zeroed in the direction of the normal of this
     // crystal plane.
     double stored_e = 0.5*(1-knockdown1)*estress(2,2)*estrain(2,2);

     // if energy criteria is not satisfied
     // then check the next plane in {100} family
     if (stored_e < release_rate_100)
       continue;

     // if energy criteria is satisfied and tensile stress is higher
     // than the previous max then the current plane will be
     // considered the most possible fracture plane
     // the information of this plane will be stored in tmp data set
     if ((stored_e > release_rate_100) && (estress(2,2) > tmp_max_stress)) {
       tmp_crystal_plane = plane_index_100[counter];
       tmp_max_stress = estress(2,2);
       tmp_EA_transform = euler;
     }
   }


   // calculate the possible fracture surface in crystal plane family {110}
   double release_rate_110 = 4*gamma_110/sqrt(M_PI*original_area());


   // family {110}: plane (110), (1-10), (011), (01-1), (101) and
   // (10-1) will be considered in calculation
   for (int counter = 0; counter < 6; counter++) {
     EulerAngle euler(SphereCoord(plane_index_110[counter]));
     // assign the euler angle for the normal of current crystal plane
     
     // transform the elastic strain tensor so that the zz component is
     // in the direction of the normal of the particular crystal plane
     elastic_straintensor();
     StrainStress estrain(elastic_strain.transform(euler));

     // calculate the elastic stress tensor so that the zz component is
     // in the direction of the normal of the particular crystal plane
     StrainStress estress(stress.transform(euler));

     // use only the part of the energy that will be released when the
     // modulus is zeroed in the direction of the normal of this
     // crystal plane.
     double stored_e = 0.5*(1-knockdown1)*estress(2,2)*estrain(2,2);

     // if energy criteria is not satisfied then check the next plane
     // in {110} family
     if (stored_e < release_rate_110)
       continue;

     // if energy criteria is satisfied and tensile stress is higher
     // than the previous max then the current plane will be
     // considered the most possible fracture plane
     // the information of this plane will be stored in tmp data set
     if ((stored_e > release_rate_110) && (estress(2,2) > tmp_max_stress)) {
       tmp_crystal_plane = plane_index_110[counter];
       tmp_max_stress = estress(2,2);
       tmp_EA_transform = euler;
     }
   }

   // calculate the possible fracture surface in crystal plane family {111}
   double release_rate_111 = 4*gamma_110/sqrt(M_PI*original_area());


   // family {111}: plane (111), (1-11), (-110) and (-1-11) will be
   // considered in calculation
   for (int counter = 0; counter < 4; counter++) {
     EulerAngle euler(SphereCoord(plane_index_111[counter]));
   // assign the transform euler angle for the normal of current
   // crystal plane to zz direction

     // transform the elastic strain tensor so that the zz component is
     // in the direction of the normal of the particular crystal plane
     elastic_straintensor();
     StrainStress estrain(elastic_strain.transform(euler));

     // calculate the elastic stress tensor so that the zz component is
     // in the direction of the normal of the particular crystal plane
     StrainStress estress(stress.transform(euler));

     // use only the part of the energy that will be released when the
     // modulus is zeroed in the direction of the normal of this
     // crystal plane.
     double stored_e = 0.5*(1-knockdown1)*estress(2,2)*estrain(2,2);

     // if energy criteria is not satisfied
     // then check the next plane in {110} family
     if (stored_e < release_rate_111)
       continue;

     // if energy criteria is satisfied and tensile stress is higher
     // than the previous max then the current plane will be
     // considered the most possible fracture plane
     // the information of this plane will be stored in tmp data set
     if ((stored_e > release_rate_111) && (estress(2,2) > tmp_max_stress)) {
       tmp_crystal_plane = plane_index_111[counter];
       tmp_max_stress = estress(2,2);
       tmp_EA_transform = euler;
     }
   }

   // The final fracture surface will have its crystal plane index in
   // "tmp_crystal_plane",
   // the orientation of the surface is in "tmp_EA_transform"
   // the stress on the plane will be "tmp_max_stress"



   if(!mutated)
     oldorientation = orientation; // only save *original* orientation
   orientation = tmp_EA_transform;

   Cijkl aligned_c = Cijkl(cijkl.transform(orientation));
   aligned_c(0,2) *= knockdown2;
   aligned_c(0,3) *= knockdown2;
   aligned_c(0,4) *= knockdown2;
   aligned_c(1,2) *= knockdown2;
   aligned_c(1,3) *= knockdown2;
   aligned_c(1,4) *= knockdown2;
   aligned_c(2,2) *= knockdown1;
   aligned_c(2,3) *= knockdown2;
   aligned_c(2,4) *= knockdown2;
   aligned_c(2,5) *= knockdown2;
   aligned_c(3,3) *= knockdown2;
   aligned_c(3,4) *= knockdown2;
   aligned_c(3,5) *= knockdown2;
   aligned_c(4,4) *= knockdown2;
   aligned_c(4,5) *= knockdown2;

   cijkl = Cijkl(aligned_c.transform(-orientation));
   not_current();
   garcon()->msout << ms_info << "fracturing " << index
		   << " at stress " << tmp_max_stress
		   << " and orientation " << orientation << "on crystal plane"
		   << tmp_crystal_plane << endl << ms_normal;
   orientation = 0;
   if (intrinsic_gray > 0.0) oldgray = intrinsic_gray;
   intrinsic_gray = 0.0;
   mutated = 1;
   return 1;
}

void CubicAnisoCrackElement::revert() {
  if(mutated) {
    intrinsic_gray = oldgray;
    orientation = oldorientation;
    cijkl = virgin_cijkl();
#ifdef THERMAL
    setstiffnessT();
#endif // THERMAL
    setstiffness();
    mutated = 0;
    not_current();
  }
}

