
// -*- C++ -*-
// $RCSfile: stressdrawer.C,v $
// $Revision: 1.7 $
// $Author: langer $
// $Date: 2000-10-31 19:15:14 $


/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


// ******** See copious comments in elementfunction.h. ********


#include "elementfunction.h"
#include "stressdrawer.h"
#include "symmmatrix.h"
#include <math.h>

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

class StressXX : public ElementFunction<StressXX> {
public:
  StressXX() {}
  static CharString id() { return "Stress XX"; }
  static double order() { return 1.0; }
  virtual double operator()(Element &el) const {
    return el.stresstensor()(0,0);
  }
};
template class ElementFunction<StressXX>;

class StressYY : public ElementFunction<StressYY> {
public:
  StressYY() {}
  static CharString id() { return "Stress YY"; }
  static double order() { return 1.1; }
  virtual double operator()(Element &el) const {
    return el.stresstensor()(1,1);
  }
};
template class ElementFunction<StressYY>;

class StressXY : public ElementFunction<StressXY> {
public:
  StressXY() {}
  static CharString id() { return "Stress XY"; }
  static double order() { return 1.3; }
  virtual double operator()(Element &el) const {
    return el.stresstensor()(0,1);
  }
};
template class ElementFunction<StressXY>;

class StressZZ : public ElementFunction<StressZZ> {
public:
  StressZZ() {}
  static CharString id() { return "Stress ZZ"; }
  static double order() { return 1.2; }
  virtual double operator()(Element &el) const {
    return el.stresstensor()(2,2);
  }
};
template class ElementFunction<StressZZ>;

class StressXZ : public ElementFunction<StressXZ> {
public:
  StressXZ() {}
  static CharString id() { return "Stress XZ"; }
  static double order() { return 1.4; }
  virtual double operator()(Element &el) const {
    return el.stresstensor()(0,2);
  }
};
template class ElementFunction<StressXZ>;

class StressYZ : public ElementFunction<StressYZ> {
public:
  StressYZ() {}
  static CharString id() { return "Stress YZ"; }
  static double order() { return 1.5; }
  virtual double operator()(Element &el) const {
    return el.stresstensor()(1,2);
  }
};
template class ElementFunction<StressYZ>;

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

class StressInv1 : public ElementFunction<StressInv1> {
public:
  StressInv1() {}
  static CharString id() { return "Stress Invariant 1"; }
  static double order() { return 2.1; }
  virtual double operator()(Element &el) const {
    return el.stresstensor().invariant1();
  }
};
template class ElementFunction<StressInv1>;

class StressInv2 : public ElementFunction<StressInv2> {
public:
  StressInv2() {}
  static CharString id() { return "Stress Invariant 2"; }
  static double order() { return 2.2; }
  virtual double operator()(Element &el) const {
    return el.stresstensor().invariant2();
  }
};
template class ElementFunction<StressInv2>;

class StressInv3 : public ElementFunction<StressInv3> {
public:
  StressInv3() {}
  static CharString id() { return "Stress Invariant 3"; }
  static double order() { return 2.3; }
  virtual double operator()(Element &el) const {
    return el.stresstensor().invariant3();
  }
};
template class ElementFunction<StressInv3>;

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

class StressPrincipal1 : public ElementFunction<StressPrincipal1> {
public:
  StressPrincipal1() {}
  static CharString id() { return "Stress Principal 1"; }
  static double order() { return 3.1; }
  virtual double operator()(Element &el) const {
    return el.stresstensor().principal1();
  }
};
template class ElementFunction<StressPrincipal1>;

class StressPrincipal2 : public ElementFunction<StressPrincipal2> {
public:
  StressPrincipal2() {}
  static CharString id() { return "Stress Principal 2"; }
  static double order() { return 3.2; }
  virtual double operator()(Element &el) const {
    return el.stresstensor().principal2();
  }
};
template class ElementFunction<StressPrincipal2>;

class StressPrincipal3 : public ElementFunction<StressPrincipal3> {
public:
  StressPrincipal3() {}
  static CharString id() { return "Stress Principal 3"; }
  static double order() { return 3.3; }
  virtual double operator()(Element &el) const {
    return el.stresstensor().principal3();
  }
};
template class ElementFunction<StressPrincipal3>;

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

class ShearStress : public ElementFunction<ShearStress> {
public:
  ShearStress() {}
  static CharString id() { return "Shear Stress"; }
  static double order() { return 3.4; }
  virtual double operator()(Element &el) const {
    return el.stresstensor().shear();
  }
};
template class ElementFunction<ShearStress>;

class ShearStress2D : public ElementFunction<ShearStress2D> {
public:
  ShearStress2D() {}
  static CharString id() { return "Shear Stress 2D"; }
  static double order() { return 3.5; }
  virtual double operator()(Element &el) const {
    return el.stresstensor().shear2d();
  }
};
template class ElementFunction<ShearStress2D>;

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

class StrainXX : public ElementFunction<StrainXX> {
public:
  StrainXX() {}
  static CharString id() { return "Strain XX"; }
  static double order() { return 4.1; }
  virtual double operator()(Element &el) const {
    return el.total_straintensor()(0,0);
  }
};
template class ElementFunction<StrainXX>;

class StrainXY : public ElementFunction<StrainXY> {
public:
  StrainXY() {}
  static CharString id() { return "Strain XY"; }
  static double order() { return 4.4; }
  virtual double operator()(Element &el) const {
    return el.total_straintensor()(0,1);
  }
};
template class ElementFunction<StrainXY>;

class StrainYY : public ElementFunction<StrainYY> {
public:
  StrainYY() {}
  static CharString id() { return "Strain YY"; }
  static double order() { return 4.2; }
  virtual double operator()(Element &el) const {
    return el.total_straintensor()(1,1);
  }
};
template class ElementFunction<StrainYY>;

class StrainZZ : public ElementFunction<StrainZZ> {
public:
  StrainZZ() {}
  static CharString id() { return "Strain ZZ"; }
  static double order() { return 4.3; }
  virtual double operator()(Element &el) const {
    return el.total_straintensor()(2,2);
  }
};
template class ElementFunction<StrainZZ>;

class StrainXZ : public ElementFunction<StrainXZ> {
public:
  StrainXZ() {}
  static CharString id() { return "Strain XZ"; }
  static double order() { return 4.5; }
  virtual double operator()(Element &el) const {
    return el.total_straintensor()(0,2);
  }
};
template class ElementFunction<StrainXZ>;

class StrainYZ : public ElementFunction<StrainYZ> {
public:
  StrainYZ() {}
  static CharString id() { return "Strain YZ"; }
  static double order() { return 4.6; }
  virtual double operator()(Element &el) const {
    return el.total_straintensor()(1,2);
  }
};
template class ElementFunction<StrainYZ>;

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//


class StrainInv1 : public ElementFunction<StrainInv1> {
public:
  StrainInv1() {}
  static CharString id() { return "Strain Invariant 1"; }
  static double order() { return 5.1; }
  virtual double operator()(Element &el) const {
    return el.total_straintensor().invariant1();
  }
};
template class ElementFunction<StrainInv1>;

class StrainInv2 : public ElementFunction<StrainInv2> {
public:
  StrainInv2() {}
  static CharString id() { return "Strain Invariant 2"; }
  static double order() { return 5.2; }
  virtual double operator()(Element &el) const {
    return el.total_straintensor().invariant2();
  }
};
template class ElementFunction<StrainInv2>;

class StrainInv3 : public ElementFunction<StrainInv3> {
public:
  StrainInv3() {}
  static CharString id() { return "Strain Invariant 3"; }
  static double order() { return 5.3; }
  virtual double operator()(Element &el) const {
    return el.total_straintensor().invariant3();
  }
};
template class ElementFunction<StrainInv3>;

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

class StrainPrincipal1 : public ElementFunction<StrainPrincipal1> {
public:
  StrainPrincipal1() {}
  static CharString id() { return "Strain Principal 1"; }
  static double order() { return 6.1; }
  virtual double operator()(Element &el) const {
    return el.total_straintensor().principal1();
  }
};
template class ElementFunction<StrainPrincipal1>;

class StrainPrincipal2 : public ElementFunction<StrainPrincipal2> {
public:
  StrainPrincipal2() {}
  static CharString id() { return "Strain Principal 2"; }
  static double order() { return 6.2; }
  virtual double operator()(Element &el) const {
    return el.total_straintensor().principal2();
  }
};
template class ElementFunction<StrainPrincipal2>;

class StrainPrincipal3 : public ElementFunction<StrainPrincipal3> {
public:
  StrainPrincipal3() {}
  static CharString id() { return "Strain Principal 3"; }
  static double order() { return 6.3; }
  virtual double operator()(Element &el) const {
    return el.total_straintensor().principal3();
  }
};
template class ElementFunction<StrainPrincipal3>;

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

class ShearStrain : public ElementFunction<ShearStrain> {
public:
  ShearStrain() {}
  static CharString id() { return "Shear Strain"; }
  static double order() { return 6.4; }
  virtual double operator()(Element &el) const {
    return el.total_straintensor().shear();
  }
};
template class ElementFunction<ShearStrain>;

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

class EnergyDensity : public ElementFunction<EnergyDensity> {
public:
  EnergyDensity() {}
  static CharString id() { return "Elastic Energy Density"; }
  static double order() { return 7.1; }
  virtual double operator()(Element &el) const {
    return el.elastic_energy();
  }
};
template class ElementFunction<EnergyDensity>;

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

#ifdef THERMAL
class Temperature : public ElementFunction<Temperature> {
public:
  Temperature() {}
  static CharString id() { return "Temperature"; }
  static double order() { return 8.0; }
  virtual double operator()(Element &el) const {
    return el.average_T();
  }
};
template class ElementFunction<Temperature>;

class DeltaTemperature : public ElementFunction<DeltaTemperature> {
public:
  DeltaTemperature() {}
  static CharString id() { return "Temperature Change"; }
  static double order() { return 8.1; }
  virtual double operator()(Element &el) const {
    return el.deltaT();
  }
};
template class ElementFunction<DeltaTemperature>;

class GradTemperatureX : public ElementFunction<GradTemperatureX> {
public:
  GradTemperatureX() {}
  static CharString id() { return "dT/dx"; }
  static double order() { return 8.2; }
  virtual double operator()(Element &el) const {
    return el.gradient_temperature_field()[0];
  }
};
template class ElementFunction<GradTemperatureX>;

class GradTemperatureY : public ElementFunction<GradTemperatureY> {
public:
  GradTemperatureY() {}
  static CharString id() { return "dT/dy"; }
  static double order() { return 8.3; }
  virtual double operator()(Element &el) const {
    return el.gradient_temperature_field()[1];
  }
};
template class ElementFunction<GradTemperatureY>;

class GradTemperatureZ : public ElementFunction<GradTemperatureZ> {
public:
  GradTemperatureZ() {}
  static CharString id() { return "dT/dz"; }
  static double order() { return 8.4; }
  virtual double operator()(Element &el) const {
    return el.gradient_temperature_field()[2];
  }
};
template class ElementFunction<GradTemperatureZ>;

class HeatFluxX : public ElementFunction<HeatFluxX> {
public:
  HeatFluxX() {}
  static CharString id() { return "Heat Flux X"; }
  static double order() { return 8.5; }
  virtual double operator()(Element &el) const {
    return el.heat_flux_field()[0];
  }
};
template class ElementFunction<HeatFluxX>;

class HeatFluxY : public ElementFunction<HeatFluxY> {
public:
  HeatFluxY() {}
  static CharString id() { return "Heat Flux Y"; }
  static double order() { return 8.6; }
  virtual double operator()(Element &el) const {
    return el.heat_flux_field()[1];
  }
};
template class ElementFunction<HeatFluxY>;

class HeatFluxZ : public ElementFunction<HeatFluxZ> {
public:
  HeatFluxZ() {}
  static CharString id() { return "Heat Flux Z"; }
  static double order() { return 8.7; }
  virtual double operator()(Element &el) const {
    return el.heat_flux_field()[2];
  }
};
template class ElementFunction<HeatFluxZ>;

class HeatFluxMag : public ElementFunction<HeatFluxMag> {
public:
  HeatFluxMag() {}
  static CharString id() { return "Heat Flux"; }
  static double order() { return 8.45; }
  virtual double operator()(Element &el) const {
    MV_Vector_double flux(el.heat_flux_field());
    return sqrt(flux[0]*flux[0] + flux[1]*flux[1] + flux[2]*flux[2]);
  }
};
#endif // THERMAL

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

class MutationStatus : public ElementFunction<MutationStatus> {
public:
  MutationStatus() {}
  static CharString id() { return "Mutation State"; }
  static double order() { return 10.0; }
  virtual double operator()(Element &el) const {
    return el.mutation_status();
  }
};
template class ElementFunction<MutationStatus>;
