// -*- C++ -*-
// $RCSfile: cijkl.C,v $
// $Revision: 1.5 $
// $Author: langer $
// $Date: 2000-10-31 19:15:09 $

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */

#include <iostream.h>
#include "cijkl.h"
#include "fatalerror.h"
#include "mvmult.h"
#include "strainstress.h"
#include "symeig3.h"
#include "iomanip.h"

// ----------------------------------------------------------- //

double &Cijkl::operator()(const int i, const int j, const int k, const int l) {
  return c(voigt(i, j), voigt(k, l));
}

double Cijkl::operator()(const int i, const int j,
				const int k, const int l) const
{
  return c(voigt(i, j), voigt(k, l));
}

// ----------------------------------------------------------- //

Cijkl Cijkl::transform(const MV_ColMat_double &A) const {
  Cijkl B;
  /* should use partial sums to speed this up */
  for(int i=0; i<3; i++)
    for(int j=0; j<=i; j++)
      for(int k=0; k<3; k++)  /* k<=i ? */
 	for(int l=0; l<=k; l++) {
 	  double &b = B(i, j, k, l);
 	  b = 0.0;
 	  for(int ii=0; ii<3; ii++)
 	    for(int jj=0; jj<3; jj++)
 	      for(int kk=0; kk<3; kk++)
 		for(int ll=0; ll<3; ll++)
 		  b += A(i, ii)*A(j, jj)*A(k, kk)*A(l, ll)*
		    (*this)(ii, jj, kk, ll);
 	}
  return B;
}

// This version doesn't work because at the first iteration
// B1(ijkl) != B1(ijlk), so it can't be stored in a Cijkl. The symmetry
// should be taken advantage of...

// Cijkl Cijkl::transform(const MV_ColMat_double &A) const {
//   Cijkl B, B1;
//   int i, ii, j, jj, k, kk, l, ll;
  
//   for(i=0; i<3; i++)
//     for(j=0; j<3; j++)
//       for(k=0; k<3; k++)
// 	for(ll=0; ll<3; ll++) {
// 	  double &b = B1(i, j, k, ll);
// 	  for(l=0; l<3; l++)
// 	    b += A(l, ll)*(*this)(i, j, k, l);
// 	}

//   for(i=0; i<3; i++)
//     for(j=0; j<3; j++)
//       for(kk=0; kk<3; kk++)
// 	for(ll=0; ll<3; ll++) {
// 	  double &b = B(i, j, kk, ll);
// 	  for(k=0; k<3; k++)
// 	    b += A(k, kk)*B1(i, j, k, ll);
// 	}

//   for(i=0; i<3; i++)
//     for(jj=0; jj<3; jj++)
//       for(kk=0; kk<3; kk++)
// 	for(ll=0; ll<3; ll++) {
// 	  double &b = B1(i, jj, kk, ll);
// 	  for(j=0; j<3; j++)
// 	    b += A(j, jj)*B(i, j, kk, ll);
// 	}

//   for(ii=0; ii<3; ii++)
//     for(jj=0; jj<3; jj++)
//       for(kk=0; kk<3; kk++)
// 	for(ll=0; ll<3; ll++) {
// 	  double &b = B(ii, jj, kk, ll);
// 	  for(i=0; i<3; i++)
// 	    b += A(i, ii)*B1(i, jj, kk, ll);
// 	}
//   return B;
// }

// ----------------------------------------------------------- //

// extract the 3x6 submatrix for plane strain calculations

MV_ColMat_double Cijkl::planestrain() const {
  MV_ColMat_double D(3, 6);
    
  // First index is 00, 11, 01, second index is voigt
  int i,j;
  for(i=0; i<2; i++)
    for(j=0; j<6; j++) 
      D(i, j) = c(i, j);
  for(j=0; j<6; j++)
    D(2, j) = c(5, j);

  return D;
}

// ----------------------------------------------------------- //

// Extract the 3x3 submatrix for plane stress calculations.
// Used in GTElement::elastic_straintensor() and Cijkl::planestress()


class XiMatrix {  // auxilliary class for using eigensystem template
private:
  const Cijkl &c;
public:
  XiMatrix(const Cijkl &cc) : c(cc) {}
  double operator()(int i, int j) const { return c(2,i,2,j); }
};

MV_ColMat_double Cijkl::psi_matrix() const {
  // psi_{ipq} = (c_{3i3j})^-1 c_{3jpq}
  // first index on psi (i) is normal, with values 0,1,2 (x,y,z).
  // second index on psi (pq) is 0->00, 1->11, 2->01
  // indices on c are Voigt
  // to use normal indices on c, use (*this)(i,j,k,l)
  MV_ColMat_double psi(3, 3, 0.0);

  double denom =
    - c(2,4)*c(2,4)*c(3,3)
    + 2*c(2,3)*c(2,4)*c(3,4)
    - c(2,2)*c(3,4)*c(3,4)
    - c(2,3)*c(2,3)*c(4,4)
    + c(2,2)*c(3,3)*c(4,4);
  if(denom != 0.0) {		// xi isn't singular
    denom = 1./denom;

    // compute inverse of xi_{ij} = c_{3i3j}
    MV_ColMat_double xiinv(3, 3);
    xiinv(0, 0) = denom * (-c(2, 3)*c(2, 3) + c(2, 2)*c(3, 3));
    xiinv(0, 1) = denom * ( c(2, 3)*c(2, 4) - c(2, 2)*c(3, 4));
    xiinv(0, 2) = denom * (-c(2, 4)*c(3, 3) + c(2, 3)*c(3, 4));
    xiinv(1, 0) = xiinv(0, 1);
    xiinv(1, 1) = denom * (-c(2, 4)*c(2, 4) + c(2, 2)*c(4, 4));
    xiinv(1, 2) = denom * ( c(2, 4)*c(3, 4) - c(2, 3)*c(4, 4));
    xiinv(2, 0) = xiinv(0, 2);
    xiinv(2, 1) = xiinv(1, 2);
    xiinv(2, 2) = denom * (-c(3, 4)*c(3, 4) + c(3, 3)*c(4, 4));
    
    // psi_{lpq} = (xi^{-1})_{lk} c_{3kpq}
    
    for(int k=0; k<3; k++)
      for(int ell=0; ell<3; ell++) {
	double xx = xiinv(ell, k);
	int vk = voigt(2, k);
	psi(ell, 0) += xx * c(vk, 0); // xiinv(ell, k) * c(2, k, 0, 0)
	psi(ell, 1) += xx * c(vk, 1); // xiinv(ell, k) * c(2, k, 1, 1)
	psi(ell, 2) += xx * c(vk, 5); // xiinv(ell, k) * c(2, k, 0, 1)
      }
  }
  else {
    int i, j;
    // xi = c_{3i3j} is singular!
    // Instead of (c_{3i3j})^-1, use
    //      R^-1 lamda' R
    // where R is the matrix that diagonalizes xi, and lamda' is the
    // diagonal matrix of inverse eigenvalues of xi, with 1/0 replaced
    // with 0.  So
    //       psi_{ipq} = (R^-1)_{ik} (lambda')_{km} R_{mj} c_{3jpq}
    const XiMatrix xi(*this);

//       cerr << "xi" << endl;
//       for(int k=0; k<3; k++) {
//         for(int kk=k; kk<3; kk++)
//   	cerr << setw(13) << xi(k, kk) << " ";
//         cerr << ";";
//       }
//       cerr << endl;

    // find eigenvalues and eigenvectors of xi
    EigenVector eig[3];
    eigensystem(xi, eig[0], eig[1], eig[2]);
    // compute lambda'
    double lambda[3];
    for(i=0; i<3; i++) {
      lambda[i] = eig[i].eigenvalue;
      if(lambda[i] != 0.0)
	lambda[i] = 1.0/lambda[i];
    }
//      cerr << "---" << endl;
//      cerr << eig[0] << endl;
//      cerr << eig[1] << endl;
//      cerr << eig[2] << endl;

#define R(i,j) eig[i].eigenvector[j]	// matrix that diagonalizes xi

    // apply R to C_{3jpq}:  Bpq[i] = R_{ij} C_{3jpq}
    MV_Vector_double B00(3, 0.0);
    MV_Vector_double B11(3, 0.0);
    MV_Vector_double B01(3, 0.0);
    for(i=0; i<3; i++) {
      for(j=0; j<3; j++) {
	B00[i] += R(i,j)*(*this)(2, j, 0, 0);
	B11[i] += R(i,j)*(*this)(2, j, 1, 1);
	B01[i] += R(i,j)*(*this)(2, j, 0, 1);
      }
    }
    // apply lambda'
    for(i=0; i<3; i++) {
      B00[i] *= lambda[i];
      B11[i] *= lambda[i];
      B01[i] *= lambda[i];
    }
    // apply R^-1
    for(i=0; i<3; i++) {
      for(j=0; j<3; j++) {
	psi(i, 0) += R(j,i)*B00[j];
	psi(i, 1) += R(j,i)*B11[j];
	psi(i, 2) += R(j,i)*B01[j];
      }
    }
#ifdef DEBUGCODE
    // debug -- check that R xi R^-1 = diag(lambda)
    MV_ColMat_double test(3,3,0.0);
    MV_ColMat_double midway(3,3,0.0);
    MV_ColMat_double diag(3,3,0.0);
    for(i=0; i<3; i++)
      for(j=0; j<3; j++)
	test(i,j) = R(j,i);
    for(i=0; i<3; i++)
      for(j=0; j<3; j++)
	for(int k=0; k<3; k++)
	  midway(i,j) += xi(i,k)*test(k,j);
    for(i=0; i<3; i++)
      for(j=0; j<3; j++)
	for(int k=0; k<3; k++)
	  diag(i,j) += R(i,k)*midway(k,j);
    //    cerr << "diag?" << endl << diag << endl;
#endif
  }
  //  cerr << "psi" << endl << psi << endl;

  return psi;
#undef R
}

MV_ColMat_double Cijkl::planestress() const {
  // indices on D are 0->00, 1->11, 2->01
  const MV_ColMat_double psi(psi_matrix());
  MV_ColMat_double D(3, 3);

  D(0, 0) = c(0, 0);
  D(0, 1) = c(0, 1);
  D(0, 2) = c(0, 5);
  D(1, 0) = c(1, 0);
  D(1, 1) = c(1, 1);
  D(1, 2) = c(1, 5);
  D(2, 0) = c(5, 0);
  D(2, 1) = c(5, 1);
  D(2, 2) = c(5, 5);

  for(int pq=0; pq<3; pq++)
    for(int ell=0; ell<3; ell++) {
      D(0, pq) -= psi(ell, pq)*(*this)(0, 0, 2, ell);
      D(1, pq) -= psi(ell, pq)*(*this)(1, 1, 2, ell);
      D(2, pq) -= psi(ell, pq)*(*this)(0, 1, 2, ell);
    }

  return D;
}

// ----------------------------------------------------------- //

ostream &operator<<(ostream &os, const Cijkl &cijkl) {
  return os << cijkl.c;
}

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

// Tensor multiplication C_ijkl * M_kl

StrainStress operator*(const Cijkl &cijkl, const SymmMatrix &m) {
  if(m.size() != 3) {
    cerr << "Error in Cijkl*Mkl!" << endl;
    fatalerror();
  }
  //#define SLOW
#ifdef SLOW
  StrainStress product;
  for(int i=0; i<3; i++)
    for(int j=i; j<3; j++) {
      double &r = product(i,j);
      for(int k=0; k<3; k++)
	for(int l=0; l<3; l++)
	  r += cijkl(i,j,k,l)*m(k,l);
    }
  return product;
#else
  // this could be done more efficiently by assuming a particular
  // internal representation for the SymmMatrix class.
  MV_Vector_double mv(6);
  mv(0) = m(0,0);
  mv(1) = m(1,1);
  mv(2) = m(2,2);
  mv(3) = 2*m(1,2);
  mv(4) = 2*m(0,2);
  mv(5) = 2*m(0,1);
  MV_Vector_double rv(cijkl.c * mv);
  StrainStress product;
  product(0,0) = rv(0);
  product(1,1) = rv(1);
  product(2,2) = rv(2);
  product(1,2) = rv(3);
  product(0,2) = rv(4);
  product(0,1) = rv(5);
  return product;
#endif
}

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

#include "tntfmat.h"
#include "tntvec.h"
#include "tntlu.h"

StrainStress Cijkl::solve(const StrainStress &stress) const {

  // have to use Fortran_Matrix here, because LU relies on it.

  TNT::Fortran_Matrix<double> fullc(6,6);
  TNT::Vector<double> x(6);

  for(int i=0; i<6; i++) 
    for(int j=0; j<6; j++)
      fullc(i+1,j+1) = (*this)(i,j);

  x(1) = stress(0,0);
  x(2) = stress(1,1);
  x(3) = stress(2,2);
  x(4) = stress(1,2);
  x(5) = stress(0,2);
  x(6) = stress(0,1);

  TNT::Vector<int> ipiv(6);
  TNT::LU_factor(fullc, ipiv);
  TNT::LU_solve(fullc, ipiv, x);

  StrainStress eps;
  eps(0,0) = x(1);
  eps(1,1) = x(2);
  eps(2,2) = x(3);
  eps(1,2) = 0.5*x(4);
  eps(0,2) = 0.5*x(5);
  eps(0,1) = 0.5*x(6);

  return eps;
}
