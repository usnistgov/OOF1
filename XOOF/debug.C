// -*- C++ -*-
// $RCSfile: debug.C,v $
// $Revision: 1.7 $
// $Author: langer $
// $Date: 2000-11-15 19:23:37 $

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


// debugging routines
// these are all in class Debug, which is a friend of everybody
// these are only included when DEBUG is defined during compilation

#ifdef DEBUG

#include "colorutils.h"
#include "debug.h"
#include "eulerangle.h"
#include "filename.h"
#include "grid.h"
#include "griditerator.h"
#include "main.h"
#include "menuDef.h"
#include "mvmult.h"
#include "parameters.h"
#include "comprow_double.h"
#include "sparselink.h"
#include "stiffness.h"
#include "symeig3.h"
#include "symmmatrix.h"
#include "timer.h"
#include <fstream.h>
#include <iostream.h>
#include "stdlib.h"

class Debug {
public:
  static CommandFn checksym();
  static CommandFn testKii();
  static CommandFn testKib();
  static CommandFn testKbb();
  static void printrow(int rowno);
  //	static CommandFn printelems();
};

// --------------------------------------------------------------- //

void breakpoint() {		// just for setting debugging breaks
  static int x = 0; 
  x++;
}

// --------------------------------------------------------------- //

// check that the stiffness matrix is symmetric

void checksymmetry(CompRow_Mat_double &K, char *name) {
  int rows = K.dim(0);
  int cols = K.dim(1);
  if(rows != cols) {
    cerr << name << " is " << rows << " x " << cols << "!" << endl;
  }
  for(int i=0; i<rows; i++)
    for(int j=0; j<cols; j++) {
      if(K(i, j) != K(j, i)) {
	cerr<<name<<"(" << i << "," << j << ") = " << K(i, j) << endl;
	cerr<<name<<"(" << j << "," << i << ") = " << K(j, i) << endl;
      }
    }
  cerr << name << " is symmetric." << endl;
}

CommandFn Debug::checksym() {
  //    checksymmetry(*(grid->Kii), "Kii"); 
  //    checksymmetry(*(grid->Kbb), "Kbb");
}

// --------------------------------------------------------------- //

/* Templates should be in a separate file if they're used. This one isn't.

// check that indices are stored properly

template <class TYPE> void checkindices(TYPE &list, char *name) {
    cout << "Checking indices in " << name << endl;
    for(int i=0; i<list.capacity(); i++) {
	if(i != list[i]->index) {
	    cout << name << "[" << i << "]->index = " << list[i]->index << endl;
	}
    }
}
*/

// --------------------------------------------------------------- //

static void testK(int n, int m, CompRow_Mat_double &Kglob, Stiffness &Kloc) {
  cout << "i j: local global" << endl;
  for(int i=0; i<m; i++) {
    MV_Vector_double x(m, 0.0);
    x[i] = 1.0;
    MV_Vector_double glob(Kglob * x);
    MV_Vector_double local(Kloc * x);
    for(int j=0; j<n; j++)
      if(local[j] != glob[j])
	cout << j << " " << i << ": "
	     << local[j] << " " << glob[j] << endl;
  }
}

CommandFn Debug::testKii() {
  if(gridinit()) {
    int n = grid->elastic_dof.free_dof.capacity();
    cout << "Kii is " << n << "x" << n << endl;
    grid->make_stiffness();
    Stiffness kii(grid, F_FREE, F_FREE);
    testK(n, n, grid->Kii, kii);
  }
}

CommandFn Debug::testKib() {
  if(gridinit()) {
    int n = grid->elastic_dof.free_dof.capacity();
    int m = grid->elastic_dof.fixed_dof.capacity();
    cout << "Kib is " << n << "x" << m << endl;
    grid->make_stiffness();
    Stiffness kib(grid, F_FREE, F_FIXED);
    testK(n, m, grid->Kib, kib);
  }
}

CommandFn Debug::testKbb() {
  if(gridinit()) {
    int m = grid->elastic_dof.fixed_dof.capacity();
    cout << "Kbb is " << m << "x" << m << endl;
    grid->make_stiffness();
    Stiffness kbb(grid, F_FIXED, F_FIXED);
    testK(m, m, grid->Kbb, kbb);
  }
}

void DEBUGprintelems() {
  if(gridinit()) {
    ElementIterator iter(EI_ALL, grid);
    Element *e;
    while(e = iter())
      garcon()->msout << *e << endl;
  }
}

class ConnectCmd: public CommandM {
private:
  int nodenumber;
public:
  ConnectCmd()
    : CommandM("connectivity", "is a node connected?"), nodenumber(0)
  {
    AddArgument(this, "node", nodenumber);
  }
  virtual CommandFn func() {
    if(gridinit()) {
      garcon()->msout << "node " << nodenumber << " is" << 
	(grid->connected(nodenumber) ? " " : " not") << " connected" <<
	endl;
    }
  }
  virtual CommandM *clone() const {
    ConnectCmd *cc = new ConnectCmd;
    cc->nodenumber = nodenumber;
    return cc;
  }
};

// --------------------------------------------------------------- //

// test speed of grid iterator

class SpeedCmd : public CommandM {
private:
  int n;
public:
  SpeedCmd() : CommandM("iterator_speed", "test speed of grid iterator"), 
    n(100)
  {
    AddArgument(this, "n", n);
  }
  virtual void func() {
    if(gridinit()) {
      Timer t1;
      int i;
      for(i=0; i<n; i++) {
	ElementIterator iter(EI_ALL, grid);
	Element *el;
	while(el = iter())
	  el->select();
      }
      garcon()->msout << "iterator: " << t1 << endl;
      Timer t2;
      for(i=0; i<n; i++) {
	int j;
	for(j=0; j<grid->interiorE.capacity(); j++)
	  grid->interiorE[j]->select();
	for(j=0; j<grid->boundaryE.capacity(); j++)
	  grid->boundaryE[j]->select();
      }
      garcon()->msout << "no iterator: " << t2 << endl;
    }
  }
  CommandM *clone() const {
    SpeedCmd *sc = new SpeedCmd;
    sc->n = n;
    return sc;
  }
};

// --------------------------------------------------------------- //

class EulerTestCmd : public CommandM {
private:
  EulerAngle orientation;
  SymmMatrix M;
  MV_Vector_double V;
public:
  EulerTestCmd()
    : CommandM("eulertest", "test euler angle rotation"), M(3), V(3, 0.0)
  {
    AddArgument(this, "orientation", orientation);
    AddArgument(this, "M00", M(0, 0));
    AddArgument(this, "M01", M(0, 1));
    AddArgument(this, "M02", M(0, 2));
    AddArgument(this, "M11", M(1, 1));
    AddArgument(this, "M12", M(1, 2));
    AddArgument(this, "M22", M(2, 2));
    AddArgument(this, "V0", V(0));
    AddArgument(this, "V1", V(1));
    AddArgument(this, "V2", V(2));
  }
  CommandFn func() {
    garcon()->msout << "R" << endl << orientation.rotation();
    garcon()->msout << "M" << endl << M << endl;
    garcon()->msout << "M transformed" << endl
		    << M.transform(orientation.rotation()) << endl;
    garcon()->msout << "V transformed" << endl << orientation.rotation()*V
		    << endl;
  }
  virtual CommandM *clone() const {
    EulerTestCmd *etc = new EulerTestCmd;
    etc->orientation = orientation;
    etc->M = M;
    etc->V = V;
    return etc;
  }
};

//==\\==//==\\==//==\\==//==\\==//==\\==//==\\==//==\\==//==\\==//==\\==//

#include "strainstress.h"

class EigenTest : public CommandM {
private:
   StrainStress A;
public:
  EigenTest()
    : CommandM("eigentest", "test 3x3 eigenvalue routine")
  {
    AddArgument(this, "a00", A(0,0));
    AddArgument(this, "a01", A(0,1));
    AddArgument(this, "a02", A(0,2));
    AddArgument(this, "a11", A(1,1));
    AddArgument(this, "a12", A(1,2));
    AddArgument(this, "a22", A(2,2));
  }
  CommandFn func() {
    garcon()->msout << "--- A ---" << endl << A << endl;

    garcon()->msout << "--- eigenvectors and eigenvalues ---" << endl;
    EigenVector eig[3];
    eigensystem(A, eig[0], eig[1], eig[2]);
    garcon()->msout << eig[0] << endl << eig[1] << endl << eig[2] << endl;

//     garcon()->msout << "--- eigenvalues alone ---" << endl;
//     EigenValues evs;
//     eigenvalues(A, evs);
//     garcon()->msout << evs << endl;

    for(int i=0; i<3; i++) {
      garcon()->msout << "--- residual " << i << "---" << endl;
      MV_Vector_double vec(&eig[i].eigenvector[0], 3);
      garcon()->msout << A*vec - eig[i].eigenvalue*vec << endl;
    }
    
  }
  CommandM *clone() const {
    EigenTest *et = new EigenTest;
    et->A = A;
    return et;
  }
};

//==\\==//==\\==//==\\==//==\\==//==\\==//==\\==//==\\==//==\\==//==\\==//

static CommandFn vardump() {
  get_rootmenu()->print(stderr);
}

//==\\==//==\\==//==\\==//==\\==//==\\==//==\\==//==\\==//==\\==//==\\==//

static TrueFalse X_debug = 0;

static void setXdebug(const CharString&) {
  if(!text_mode())
    XSynchronize(gfxinfo.display(), (X_debug? True : False));
}

static CommandFn colortest() {
  XColor xc;
  xc.pixel = 1;
  xc.flags = DoRed | DoGreen | DoBlue;
  xc.red = xc.green = xc.blue = 0;
  Colormap cmap = gfxinfo.colormap();
  XStoreColor(gfxinfo.display(), cmap, &xc);
}

//==\\==//==\\==//==\\==//==\\==//==\\==//==\\==//==\\==//==\\==//==\\==//

static CommandFn freegrid() {
  if(grid) {
    delete grid;
    grid = 0;
  }
}

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

static CommandFn testfilestream() {
  FILE *file = fopen("filestream.test", "w");
  MenuStream *filestream = new FileMenuStream(file);
  *filestream << "hello, world" << endl;
  delete filestream;
}

//==\\==//==\\==//==\\==//==\\==//==\\==//==\\==//==\\==//==\\==//==\\==//

class NodeGroupDumpCmd : public CommandM {
private:
  FileName filename;
public:
  NodeGroupDumpCmd()
    : filename("nodegroupdump"),
      CommandM("nodegroupdump", "dump nodegroup information")
  {
    AddArgument(this, "filename", filename);
  }
  virtual void func() {
    if(gridinit()) {
      ofstream os(filename);
      grid->nodegroupdump(os);      
    }
  }
  virtual CommandM *clone() const {
    NodeGroupDumpCmd *ngdc = new NodeGroupDumpCmd;
    ngdc->filename = filename;
    return ngdc;
  }
};

class DoFDumpCmd : public CommandM {
private:
  FileName filename;
public:
  DoFDumpCmd()
    : filename("dofdump"),
      CommandM("dofdump", "dump dof information")
  {
    AddArgument(this, "filename", filename);
  }
  virtual void func() {
    if(gridinit()) {
      ofstream os(filename);
      grid->dofdump(os);      
    }
  }
  virtual CommandM *clone() const {
    DoFDumpCmd *dfdc = new DoFDumpCmd;
    dfdc->filename = filename;
    return dfdc;
  }
};

class NodeDumpCmd : public CommandM {
private:
  FileName filename;
public:
  NodeDumpCmd()
    : filename("nodedump"),
      CommandM("nodedump", "dump node information")
  {
    AddArgument(this, "filename", filename);
  }
  virtual void func() {
    if(gridinit()) {
      ofstream os(filename);
      grid->nodedump(os);      
    }
  }
  virtual CommandM *clone() const {
    NodeDumpCmd *dfdc = new NodeDumpCmd;
    dfdc->filename = filename;
    return dfdc;
  }
};

static CommandFn shufflelists() {
  if(gridinit()) {
    grid->shufflelists_elastic();
#ifdef THERMAL
    grid->shufflelists_thermal();
#endif // THERMAL
  }
}

//==\\==//==\\==//==\\==//==\\==//==\\==//==\\==//==\\==//==\\==//==\\==//

Menu *debugmenu() {
  Menu *dmenu = new Menu("debug");
  AddVariable(dmenu, "Xdebug", "make synchronous X calls?", X_debug, setXdebug);
  dmenu->AddCommand("symmetry", "check stiffness matrix symmetry",
		    Debug::checksym);
  dmenu->AddCommand("Kii", "check global vs local Kii multiplication",
		    Debug::testKii);
  dmenu->AddCommand("Kib", "check global vs local Kib multiplication",
		    Debug::testKib);
  dmenu->AddCommand("Kbb", "check global vs local Kbb multiplication",
		    Debug::testKbb);
  //    dmenu->AddCommand("elements", "print all elements", Debug::printelems);
  dmenu->AddCommand(new ConnectCmd);
  dmenu->AddCommand(new SpeedCmd);
  dmenu->AddCommand(new EulerTestCmd);
  dmenu->AddCommand(new EigenTest);
  dmenu->AddCommand("filestream", "test FileMenuStream class", testfilestream);
  dmenu->AddCommand("vardump", "print all variables to stderr", vardump);
  dmenu->AddCommand("colortest", "test color map", colortest);
  dmenu->AddCommand("freegrid", "delete the current grid", freegrid);
  dmenu->AddCommand(new NodeGroupDumpCmd);
  dmenu->AddCommand(new DoFDumpCmd);
  dmenu->AddCommand(new NodeDumpCmd);
  dmenu->AddCommand("shuffle_lists", "run Grid::shufflelists()", shufflelists);
  return dmenu;
}

#endif
