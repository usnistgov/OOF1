// -*- C++ -*-
// $RCSfile: eulerangle.C,v $
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


/* 3D Euler angle rotation
 *     alpha = polar declination
 *     beta = azimuthal rotation
 *     gamma = axial twist
 * For input, operator>> accepts two formats:
 *    [alpha, beta, gamma]
 * and
 *    beta
 * Any of the numbers can be replaced by the letter 'r', and an
 * angle will be generated with the appropriate random distribution.
 */
 
#include "eulerangle.h"
#include "random.h"
#include "ctype.h"
#include <math.h>
#ifdef HAVE_IEEEFP_H
#include <ieeefp.h>   // for finite(), which is sometimes not in math.h
#endif

static const double DEGREES = 180/M_PI;
static const double RADIANS = M_PI/180;

EulerAngle::EulerAngle(const EulerAngle &ea) {
  alpha_ = ea.alpha_;
  beta_ = ea.beta_;
  gamma_ = ea.gamma_;
}

EulerAngle &EulerAngle::operator=(const EulerAngle &ea) {
  if(this != &ea) {
    alpha_ = ea.alpha_;
    beta_ = ea.beta_;
    gamma_ = ea.gamma_;
  }
  return *this;
}

ostream &operator<<(ostream &os, const EulerAngle &ea) {
    if(ea.alpha_ == 0.0 && ea.gamma_ == 0.0)
	os << DEGREES*ea.beta_;
    else
	os << "[" << DEGREES*ea.alpha_ << ", " << DEGREES*ea.beta_ << ", "
	    << DEGREES*ea.gamma_ << "]";
    return os;
}

static void skipspace(istream &is) {
  int ch;
  while(ch = is.get()) {
    if(!isspace(ch)) {
      is.putback(ch);
      return;
    }
  }
}
  
static int israndom(istream &is) {
  skipspace(is);
  int ch = is.get();
  if(ch == 'r') return 1;
  is.putback(ch);
  return 0;
}


istream &operator>>(istream &is, EulerAngle &ea) {
  int ch;
  if((ch = is.get()) != '[') {
    if(isdigit(ch) || ch == '.' || ch == '-') { /* single number */
      is.putback(ch);
      is >> ea.beta_;
      if(!is) return is;
      ea.beta_ *= RADIANS;
      ea.alpha_ = ea.gamma_ = 0.0;
      return is;
    }
    else if(ch == 'r') {	// construct a random angle
      ea.alpha_ = acos(2*rndm() - 1.0);
      ea.beta_ = 2*M_PI*rndm();
      ea.gamma_ = 2*M_PI*rndm();
      return is;
    }
    // not a single number, "r", or start of a triple in square brackets
    is.clear(ios::badbit | is.rdstate());
    return is;
  }
  
  if(israndom(is))
    ea.alpha_ = acos(2*rndm() - 1.0);
  else {
    is >> ea.alpha_;
    if(!is) return is;
    ea.alpha_ *= RADIANS;
  }

  skipspace(is);    
  if((ch = is.get()) != ',') {
    is.clear(ios::badbit | is.rdstate());
    return is;
  }

  if(israndom(is))
    ea.beta_ = 2*M_PI*rndm();
  else {
    is >> ea.beta_;
    if(!is) return is;
    ea.beta_ *= RADIANS;
  }

  skipspace(is);
  if((ch = is.get()) != ',') {
    is.clear(ios::badbit | is.rdstate());
    return is;
  }

  if(israndom(is)) 
    ea.gamma_ = 2*M_PI*rndm();
  else {
    is >> ea.gamma_;
    if(!is) return is;
    ea.gamma_ *= RADIANS;
  }

  skipspace(is);
  if((ch = is.get()) != ']') {
    is.clear(ios::badbit | is.rdstate());
    return is;
  }
  return is;
}

int operator==(const EulerAngle &om1, const EulerAngle &om2) {
  if(om1.alpha_ != om2.alpha_) return 0;
  if(om1.beta_ != om2.beta_) return 0;
  if(om1.gamma_ != om2.gamma_) return 0;
  return 1;
}

