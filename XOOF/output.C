// -*- C++ -*-
// $RCSfile: output.C,v $
// $Revision: 1.22 $
// $Author: langer $
// $Date: 2005-07-30 20:16:08 $

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


// Output routines

#include "binaryread.h"
#include "grid.h"
#include "griditerator.h"
#include "eigenvalues.h"
#include "elementgroups.h"
#include "enum.h"
#include "eulerangle.h"
#include "lineplot.h"
#include "menuDef.h"
#include "node.h"
#include "nodegroups.h"
#include "output.h"
#include "parameters.h"
#include "statistics.h"
#include "symeig3.h"
#include "symmmatrix.h"
#include <stdio.h>
#include <iostream.h>
#include <iomanip.h>
#include <fstream.h>


// save the ascii version of the grid. can be read back in.

class SaveAscii : public FileCmd {
public:
  SaveAscii() : FileCmd("ascii", "write ascii grid", "ascii.goof") {}
  virtual CommandFn func() {
    if(gridinit()) {
      ofstream file(filename);
      if(file) {
	grid->asciioutput(file);
	file.close();
      }
    }
  }
  virtual FileCmd *subclone() const {
    return new SaveAscii;
  }
};

class SaveBinary : public FileCmd {
private:
  TrueFalse longform;
public:
  SaveBinary() : FileCmd("binary", "write binary grid", "binary.goof"), 
    longform(TF_FALSE)
    {
      AddArgument(this, "save_stiffness", longform);
    }
  virtual CommandFn func() {
    if(gridinit()) {
      FILE *file = fopen(filename, "w");
      if(file) {
	binarywrite(file, longform);
	fclose(file);
      }
    }
  }
  virtual FileCmd *subclone() const {
    SaveBinary *sb =  new SaveBinary;
    sb->longform = longform;
    return sb;
  }
};

class SaveOOF2 : public FileCmd {
private:
  int height, width;
public:
  SaveOOF2() : FileCmd("oof2", "write oof2 data file", "grid.py"),
	       height(100), width(100)
  {
    AddArgument(this, "width_in_pixels", width);
    AddArgument(this, "height_in_pixels", height);
  }
  virtual CommandFn func() {
    if(gridinit()) {
      ofstream os(filename);
      if(os) {
	grid->oof2oof2(os, filename, width, height);
	os.close();
      }
    }
  }
  virtual FileCmd *subclone() const {
    SaveOOF2 *so2 = new SaveOOF2;
    so2->height = height;
    so2->width = width;
    return so2;
  }
};

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

// Control what output is printed to a file as well as to the screen

class TeeCmd : public CommandM {
private:
  TrueFalse data;		// print data?
  TrueFalse errs;		// print errors?
  TrueFalse info;		// print miscellaneous info?
  TrueFalse cmds;		// print commands and prompts?
  TrueFalse append;		// append to file or overwrite?
public:
  TeeCmd()
    : CommandM("open", "send output to a file as well as to the screen"),
      data(TF_TRUE),
      errs(TF_FALSE),
      info(TF_FALSE),
      cmds(TF_TRUE),
      append(TF_FALSE)
  {
    AddArgument(this, "filename", teefilename);
    AddArgument(this, "data", data);
    AddArgument(this, "commands", cmds);
    AddArgument(this, "info", info);
    AddArgument(this, "errors", errs);
    AddArgument(this, "append", append);
  }
  virtual CommandFn func() {
    if(!garcon()->tee(teefilename, (append?"a":"w"))) {
      garcon()->msout << ms_error << "Can't open file \"" << teefilename
		      << "\"!" << endl << ms_normal;
      return;
    }
    garcon()->tee_normal(data);
    garcon()->tee_errors(errs);
    garcon()->tee_inform(info);
    garcon()->tee_prompt(cmds);
    tee_on = TF_TRUE;
  }
  virtual CommandM *clone() const {
    TeeCmd *tc = new TeeCmd;
    tc->data = data;
    tc->errs = errs;
    tc->info = info;
    tc->cmds = cmds;
    tc->append = append;
    return tc;
  }
  static FileName teefilename;
  static TrueFalse tee_on;
};

FileName TeeCmd::teefilename("oof.out");
TrueFalse TeeCmd::tee_on;

static CommandFn teestop() {
  garcon()->teestop();
  TeeCmd::teefilename = CharString("");
  TeeCmd::tee_on = TF_FALSE;
}

static CommandFn show_teefilename() {
  if(TeeCmd::tee_on)
    garcon()->msout << ms_info << TeeCmd::teefilename << ms_normal << endl;
  else
    garcon()->msout << ms_info << "There is no current tee file."
		    << ms_normal << endl;
}

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

class CommentCmd : public CommandM {
private:
  CharString comment;
public:
  CommentCmd() : CommandM("comment", "print a comment")
  {
    AddArgument(this, "string", comment);
  }
  virtual CommandFn func() {
    garcon()->msout << comment << endl;
  }
  virtual CommandM *clone() const {
    CommentCmd *cc = new CommentCmd;
    cc->comment = comment;
    return cc;
  }
};

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

#ifdef DEBUG
// print the matrices

static CommandFn printKii() {
  ofstream file("Kii");
  if(file) {
    grid->printKii(file); 
    file.close();
  }
}

static CommandFn printKib() {
  ofstream file("Kib");
  if(file) {
    grid->printKib(file); 
    file.close();
  }
}

static CommandFn printKbb() {
  ofstream file("Kbb");
  if(file) {
    grid->printKbb(file); 
    file.close();
  }
}

static CommandFn printKlocal() {
  ofstream file("Klocal");
  if(file) {
    grid->printKlocal(file);
    file.close();
  }
}
#endif // DEBUG

// --------------------------------------------------------------- //

// show node positions and types

class ShowNodeCommand : public CommandM {
private:
  int index;
public:
  ShowNodeCommand() : CommandM("node", "show node") {
    index = 0;
    AddArgument(this, "node_number", index);
  }
  virtual CommandFn func() {
    if(gridinit()) {
      int nnodes = grid->Nnodes();
      if(index >= nnodes || index < 0) {
	garcon()->msout << ms_error
			<< "Node number must be nonnegative and less than "
			<< nnodes << endl << ms_normal;
      }
      else
	garcon()->msout << *grid->getnode(index) << endl;
    }
  }
  virtual CommandM *clone() const {
    ShowNodeCommand *snc = new ShowNodeCommand;
    snc->index = index;
    return snc;
  }
};

// -------------------------------------------------------------- //

// total force on a group of nodes

static CommandFn groupforce() {
  NodeGroup *n;
  if(gridinit() && (n = groupname())) {
    MeshCoord f = grid->force(n);
    garcon()->msout << f.x << " " << f.y << endl;
  }
}

static double xforce(Element *elem, const MeshCoord &normal) {
  const StrainStress &stress = elem->stresstensor();
  return stress(0,0)*normal.x + stress(0,1)*normal.y;
}

static double yforce(Element *elem, const MeshCoord &normal) {
  const StrainStress &stress = elem->stresstensor();
  return stress(1,0)*normal.x + stress(1,1)*normal.y;
}

static CommandFn grouptraction() {
  NodeGroup *n;
  if(gridinit() && (n = groupname())) {
    double fx = grid->flux(*n, xforce);
    double fy = grid->flux(*n, yforce);
    garcon()->msout << fx << " " << fy << endl;
  }
}

// avg displacement of a group of nodes

static CommandFn groupdisplacement() {
  NodeGroup *n;
  if(gridinit() && (n = groupname())) {
    MeshCoord d = grid->avgdisp(n);
    garcon()->msout << d.x << " " << d.y << endl;
  }
}

static CommandFn groupposition() {
  NodeGroup *n;
  if(gridinit() && (n = groupname())) {
    MeshCoord p = grid->avgposition(n);
    garcon()->msout << p.x << " " << p.y << endl;
  }
}

#ifdef THERMAL
// total heat rate of a group of nodes

static CommandFn groupheatrate() {
  NodeGroup *n;
  if(gridinit() && (n = groupname())) {
    double hr = grid->heatrate(n);
    garcon()->msout << hr << endl;
  }
}

static CommandFn grouptemperature() {
  NodeGroup *n;
  if(gridinit() && (n = groupname())) {
    double temp = grid->avgtemp(n);
    garcon()->msout << temp << endl;
  }
}

static double heatflux(Element *elem, const MeshCoord &normal) {
  MeshCoord in_plane(elem->heat_flux_field()[0],
		     elem->heat_flux_field()[1]);
  return dot(in_plane, normal);
}

static CommandFn groupheatflux() {
  NodeGroup *n;
  if(gridinit() && (n = groupname())) {
    double hf = grid->flux(*n, heatflux);
    garcon()->msout << hf << endl;
  }
}
#endif // THERMAL

static CommandFn energy() {
  if(gridinit())
    garcon()->msout << grid->energy() << endl;
}

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

// total area of a group of elements

static CommandFn group_current_area() {
  ElementGroup *eg = Egroupname();
  if(gridinit() && eg) {
    GroupIterator<Element> iter(*eg);
    double area = 0;
    Element *e;
    while(e = iter())
      area += e->current_area();
    garcon()->msout << area << endl;
  }
}

static CommandFn group_original_area() {
  ElementGroup *eg = Egroupname();
  if(gridinit() && eg) {
    GroupIterator<Element> iter(*eg);
    double area = 0;
    Element *e;
    while(e = iter())
      area += e->original_area();
    garcon()->msout << area << endl;
  }
}
//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

// average stress & strain on a group of elements

static void avg_stress(Iterator<Element> &iter) {
  Element *e;
  StrainStress stress;
  double area = 0;
  while(e = iter()) {
    double a = e->original_area();
    stress += a * e->stresstensor();
    area += a;
  }
  if(area == 0.0)
    garcon()->msout << ms_error << "area == 0??" << endl << ms_normal;
  else
    garcon()->msout << stress/area << endl;
}

static CommandFn groupstress() {
  ElementGroup *eg = Egroupname();
  if(gridinit() && eg) {
    GroupIterator<Element> iter(*eg);
    avg_stress(iter);
  }
}

static CommandFn allstress() {
  if(gridinit()) {
    ElementIterator iter(EI_ALL, grid);
    avg_stress(iter);
  }
}

static CommandFn selectedstress() {
  if(gridinit()) {
    ElementIterator iter(EI_SELECTED, grid);
    avg_stress(iter);
  }
}

// stress statistics

enum StatType {CARTESIAN, INVARIANT, EXTREMES, EIGENVALUES};
static Enum<StatType> stress_stat_type(CARTESIAN);
static Enum<StatType> strain_stat_type(CARTESIAN);


static void printstats(StressStrainStat s, const Enum<StatType> &stat_type) {
  if(s.samplesize() == 0) {
    garcon()->msout
      << ms_error
      << "I beg your pardon, there are no elements in the group"
      << endl << ms_normal;
    return;
  }
  switch(stat_type) {
  case CARTESIAN:
    {
      // This is ugly, but lets us use the SymmMatrix operator<<.
      SymmMatrix avg(3);
      SymmMatrix dev(3);
      avg(0,0) = s.xx.average();
      avg(0,1) = s.xy.average();
      avg(0,2) = s.xz.average();
      avg(1,1) = s.yy.average();
      avg(1,2) = s.yz.average();
      avg(2,2) = s.zz.average();
      dev(0,0) = s.xx.deviation();
      dev(0,1) = s.xy.deviation();
      dev(0,2) = s.xz.deviation();
      dev(1,1) = s.yy.deviation();
      dev(1,2) = s.yz.deviation();
      dev(2,2) = s.zz.deviation();
      garcon()->msout << ms_info << "average:" << endl << ms_normal
		      << avg << endl;
      garcon()->msout << ms_info << "deviation:" << endl << ms_normal
		      << dev << endl;
    }
    break;
  case EIGENVALUES:
    garcon()->msout << ms_info
		    << "** (average, deviation) of:  emax emid emin shear"
		    << endl << ms_normal;
    garcon()->msout << "("
		    << s.maxeigen.average() << ", "
		    << s.maxeigen.deviation() << ")  ("
		    << s.mideigen.average() << ", "
		    << s.mideigen.deviation() << ")  ("
		    << s.mineigen.average() << ", "
		    << s.mineigen.deviation() << ")  ("
		    << s.shear.average() << ", "
		    << s.shear.deviation() << ")" << endl;
    break;
  case INVARIANT:
    garcon()->msout << ms_info <<
      "** (average, deviation) of 1st, 2nd, and 3rd invariants"
		    << endl << ms_normal << "("
		    << s.inv1.average() << ", " << s.inv1.deviation()
		    << ")  ("
		    << s.inv2.average() << ", " << s.inv2.deviation()
		    << ")  ("
		    << s.inv3.average() << ", " << s.inv3.deviation()
		    << ")" << endl;
    break;
  case EXTREMES:
    {
      SymmMatrix max(3);
      SymmMatrix min(3);
      max(0,0) = s.xx.maximum();
      max(0,1) = s.xy.maximum();
      max(0,2) = s.xz.maximum();
      max(1,1) = s.yy.maximum();
      max(1,2) = s.yz.maximum();
      max(2,2) = s.zz.maximum();
      min(0,0) = s.xx.minimum();
      min(0,1) = s.xy.minimum();
      min(0,2) = s.xz.minimum();
      min(1,1) = s.yy.minimum();
      min(1,2) = s.yz.minimum();
      min(2,2) = s.zz.minimum();
      garcon()->msout << ms_info << "max stress:" << endl << ms_normal
		      << max << endl;
      garcon()->msout << ms_info << "min stress:" << endl << ms_normal
		      << min << endl;
      garcon()->msout << ms_info
		      << "** (max, min) of: emax emid emin shear"
		      << endl << ms_normal;
      garcon()->msout << "(" << s.maxeigen.maximum() << ", "
		      << s.maxeigen.minimum() << ")  ("
		      << s.mideigen.maximum() << ", "
		      << s.mideigen.minimum() << ")  ("
		      << s.mineigen.maximum() << ", "
		      << s.mineigen.minimum() << ")  ("
		      << s.shear.maximum() << ", "
		      << s.shear.minimum() << ")" << endl;
    }
    break;
  default:
    garcon()->msout << ms_error
		    << "No current type of statistics selected!"
		    << endl << ms_normal;
  }
}

static CommandFn all_stress_stats() {
  if(gridinit()) {
    ElementIterator iter(EI_ALL, grid);
    printstats(grid->stress_statistics(iter), stress_stat_type);
  }
}

static CommandFn selected_stress_stats() {
  if(gridinit()) {
    ElementIterator iter(EI_SELECTED, grid);
    printstats(grid->stress_statistics(iter), stress_stat_type);
  }
}

static CommandFn group_stress_stats() {
  ElementGroup *eg = Egroupname();
  if(gridinit() && eg) {
    GroupIterator<Element> iter(*eg);
    printstats(grid->stress_statistics(iter), stress_stat_type);
  }
}


template <>
VarObject* Var<Enum<StatType> >::create_varobject(Variable *var, Form *form,
						  VarObjectType t,
						  int x, int y, int w, int h)
{
  return new EnumVarObject<StatType>(var, form, t, x, y, w, h);
}

static Menu *groupstressmenu() {
  static Menu *gsm = 0;
  if(gsm) return gsm;
  gsm = new GroupMenu<Element>("group stress",
			       "compute average stress on element group",
			       groupstress);
  gsm->AddCommand("all", "compute average stress on all elements",
		  allstress);
  gsm->AddCommand("selected", "compute average stress on selected elements", 
		  selectedstress);

  Menu *statmenu = gsm->AddSubMenu("statistics",
				   "compute stress statistics",
		      new GroupMenu<Element>("stress statistics",
				     "compute stress statistics of group",
					     group_stress_stats));

  AddVariable(statmenu, "type", "set type of statistics to print", stress_stat_type);

  statmenu->AddCommand("all", "compute stress statistics on all elements",
		       all_stress_stats);
  statmenu->AddCommand("selected",
		       "compute stress statistics on selected elements",
		       selected_stress_stats);

  return gsm;
}

// -------------------------------------------------------------- //

static void avg_strain(Iterator<Element> &iter) {
  Element *e;
  StrainStress strain;
  double area = 0;
  while(e = iter()) {
    double a = e->original_area();
    strain += a * e->total_straintensor();
    area += a;
  }
  if(area == 0.0)
    garcon()->msout << ms_error << "area == 0??" << endl << ms_normal;
  else
    garcon()->msout << strain/area << endl;
}

static CommandFn groupstrain() {
  ElementGroup *eg = Egroupname();
  if(gridinit() && eg) {
    GroupIterator<Element> iter(*eg);
    avg_strain(iter);
  }
}

static CommandFn allstrain() {
  if(gridinit()) {
    ElementIterator iter(EI_ALL, grid);
    avg_strain(iter);
  }
}

static CommandFn selectedstrain() {
  if(gridinit()) {
    ElementIterator iter(EI_SELECTED, grid);
    avg_strain(iter);
  }
}

static CommandFn all_strain_stats() {
  if(gridinit()) {
    ElementIterator iter(EI_ALL, grid);
    printstats(grid->strain_statistics(iter), strain_stat_type);
  }
}

static CommandFn selected_strain_stats() {
  if(gridinit()) {
    ElementIterator iter(EI_SELECTED, grid);
    printstats(grid->strain_statistics(iter), strain_stat_type);
  }
}

static CommandFn group_strain_stats() {
  ElementGroup *eg = Egroupname();
  if(gridinit() && eg) {
    GroupIterator<Element> iter(*eg);
    printstats(grid->strain_statistics(iter), strain_stat_type);
  }
}

static Menu *groupstrainmenu() {
  static Menu *gsm = 0;
  if(gsm) return gsm;
  gsm = new GroupMenu<Element>("group strain",
		    "compute average strain on element group", groupstrain);
  gsm->AddCommand("all", "compute average strain on all elements",
		  allstrain);
  gsm->AddCommand("selected", "compute average strain on selected elements", 
		  selectedstrain);
  Menu *statmenu = gsm->AddSubMenu("statistics",
				   "compute strain statistics",
		      new GroupMenu<Element>("strain statistics",
				     "compute strain statistics of group",
					     group_strain_stats));
  AddVariable(statmenu, "type", "set type of statistics to print", strain_stat_type);
  statmenu->AddCommand("all", "compute strain statistics on all elements",
		       all_strain_stats);
  statmenu->AddCommand("selected", 
		       "compute strain statistics on selected elements",
		       selected_strain_stats);
  return gsm;
}

// -------------------------------------------------------------- //

template <class OUTTYPE>
class ElementOutputCmd : public FileCmd {
protected:
  OUTTYPE (*elfun)(Element*);
  OUTTYPE (Element::*elmember)();
  OUTTYPE (Element::*elcmember)() const;
public:
  ElementOutputCmd(const CharString &n, const CharString &hs,
		   const CharString &f, OUTTYPE(*funk)(Element*))
    : FileCmd(n, hs, f),
      elfun(funk), elmember(0), elcmember(0)
  {}
  ElementOutputCmd(const CharString &n, const CharString &hs,
 		   const CharString &f, OUTTYPE (Element::*funk)())
    : FileCmd(n, hs, f),
      elfun(0), elmember(funk), elcmember(0)
  {}
  ElementOutputCmd(const CharString &n, const CharString &hs,
 		   const CharString &f, OUTTYPE (Element::*funk)() const)
    : FileCmd(n, hs, f),
      elfun(0), elmember(0), elcmember(funk)
  {}
  virtual CommandFn func() {
    if(gridinit()) {
      ofstream file(filename, ios::out|ios::trunc);
      // (I don't think ios::out should be needed here, but g++ doesn't
      // work without it.)
      if(file) {
	Element *el;
	ElementIterator iter(EI_SORTED, grid);
	while(el = iter()) {
	  file << setw(4) << el->index << " ";
	  if(elfun)
	    file << elfun(el);
	  else if(elmember)
	    file << (el->*elmember)();
	  else if(elcmember)
	    file << (el->*elcmember)();
	  file << endl;
	}
      }
      file.close();
    }
  }
  virtual FileCmd *subclone() const {
    if(elfun)
      return new ElementOutputCmd(name, helpstring, filename, elfun);
    else if(elmember)
      return new ElementOutputCmd(name, helpstring, filename, elmember);
    else
      return new ElementOutputCmd(name, helpstring, filename, elcmember);
  }
};

template <class OUTTYPE>
class ElementGroupOutputCmd : public ElementOutputCmd<OUTTYPE> {
public:
  ElementGroupOutputCmd(const CharString &n, const CharString &hs,
		   const CharString &f, OUTTYPE(*funk)(Element*))
    : ElementOutputCmd<OUTTYPE>(n, hs, f, funk)
  {}
  ElementGroupOutputCmd(const CharString &n, const CharString &hs,
 		   const CharString &f, OUTTYPE (Element::*funk)())
    : ElementOutputCmd<OUTTYPE>(n, hs, f, funk)
  {}
  ElementGroupOutputCmd(const CharString &n, const CharString &hs,
 		   const CharString &f, OUTTYPE (Element::*funk)() const)
    : ElementOutputCmd<OUTTYPE>(n, hs, f, funk)
  {}
  virtual CommandFn func() {
    if(gridinit()) {
      ElementGroup *eg = Egroupname();
      ofstream file(this->filename, ios::out|ios::trunc);
      // (I don't think ios::out should be needed here, but g++ doesn't
      // work without it.)
      if(file && eg) {
	Element *el;
	GroupIterator<Element> iter(*eg);
	while(el = iter()) {
	  file << setw(4) << el->index << " ";
	  if(this->elfun)
	    file << this->elfun(el);
	  else if(this->elmember)
	    file << (el->*this->elmember)();
	  else if(this->elcmember)
	    file << (el->*this->elcmember)();
	  file << endl;
	}
      }
      file.close();
    }
  }
  virtual FileCmd *subclone() const {
    if(this->elfun)
      return new ElementGroupOutputCmd(this->name,
                                       this->helpstring,
                                       this->filename,
                                       this->elfun);
    else if(this->elmember)
      return new ElementGroupOutputCmd(this->name,
                                       this->helpstring,
                                       this->filename,
                                       this->elmember);
    else
      return new ElementGroupOutputCmd(this->name,
                                       this->helpstring,
                                       this->filename,
                                       this->elcmember);
  }
};

// This is a menu in which all groups automatically appear.  Executing
// a function (whose name is a group name) invokes the given function,
// funk, on each element in the group.  OUTTYPE is the type of the
// value returned by funk.  The function executed is an
// ElementGroupOutputCmd.

template <class OUTTYPE>
class GroupOutputMenu : public GAMenu<Element> {
private:
  OUTTYPE (*elfun)(Element*);
  OUTTYPE (Element::*elmember)();
  OUTTYPE (Element::*elcmember)() const;
  const CharString file;
  const CharString help;
public:
  GroupOutputMenu(const CharString &prmpt, const CharString &hs,
		  const CharString &fname, OUTTYPE (*funk)(Element*))
    : GAMenu<Element>(prmpt, hs),
    file(fname),
    help(hs),
    elfun(funk), elmember(0), elcmember(0)
  {}
  GroupOutputMenu(const CharString &prmpt, const CharString &hs,
		  const CharString &fname, OUTTYPE (Element::*funk)())
    : GAMenu<Element>(prmpt, hs),
    file(fname),
    help(hs),
    elfun(0), elmember(funk), elcmember(0)
  {}
  GroupOutputMenu(const CharString &prmpt, const CharString &hs,
		  const CharString &fname, OUTTYPE (Element::*funk)() const)
    : GAMenu<Element>(prmpt, hs),
    file(fname),
    help(hs),
    elfun(0), elmember(0), elcmember(funk)
  {}
  virtual void addcmd(const CharString &nm) {
    if(elfun)
      AddCommand(new ElementGroupOutputCmd<OUTTYPE>(nm,
						    help + " in group " + nm,
						    file+"_"+nm, elfun));
    else if(elmember)
      AddCommand(new ElementGroupOutputCmd<OUTTYPE>(nm,
						    help + " in group " + nm,
						    file+"_"+nm, elmember));
    else if(elcmember)
      AddCommand(new ElementGroupOutputCmd<OUTTYPE>(nm,
						    help + " in group " + nm,
						    file+"_"+nm, elcmember));
  }
};

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

static EigenValues stress_evals(Element *el) {
  EigenValues evs;
  eigenvalues(el->stresstensor(), evs);
  return evs;
}

static EigenValues strain_evals(Element *el){
  EigenValues evs;
  eigenvalues(el->total_straintensor(), evs);
  return evs;
}


//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

static Menu *individualgroupmenu();

static Menu *individualmenu() {
  static Menu *menu = 0;
  if(menu) return menu;
  menu = new Menu("individual output");
  menu->AddSubMenu("group", "print element by element output for a group",
		   individualgroupmenu());

  menu->AddCommand(new ElementOutputCmd<double>
		   ("current_area",
		    "print current area of each element",
		    "current_area",
		    &Element::current_area));

  menu->AddCommand(new ElementOutputCmd<double>
		   ("original_area",
		    "print original area of each element",
		    "original_area",
		    &Element::original_area)); 
  
  menu->AddCommand(new ElementOutputCmd<EulerAngle>
		   ("orientation",
		    "print orientation of each element",
		    "orientation", &Element::query_orientation));
  
  menu->AddCommand(new ElementOutputCmd<double>
		   ("energy_density",
		    "print energy density of each element",
		    "energy_density", &Element::elastic_energy));
#ifdef THERMAL
  menu->AddCommand(new ElementOutputCmd<double>
		   ("temperature",
		    "print average temperature of each element",
		    "temperature", &Element::average_T));
  menu->AddCommand(new ElementOutputCmd<const MV_Vector_double&>
		   ("grad_temperature",
		    "print temperature gradient of each element",
		    "grad_temperature", &Element::gradient_temperature_field));
  menu->AddCommand(new ElementOutputCmd<const MV_Vector_double&>
		   ("heat_flux",
		    "print heat flux of each element",
		    "heat_flux", &Element::heat_flux_field));
#endif // THERMAL

  Menu *stressmenu = menu->AddSubMenu("stress", "print stress in each element");
  stressmenu->AddCommand(new ElementOutputCmd<const StrainStress&>
			 ("tensor",
			  "print full stress tensor for each element",
			  "stress_tensor", &Element::stresstensor));
  stressmenu->AddCommand(new ElementOutputCmd<EigenValues>
			 ("eigenvals",
			  "print eigenvalues of stress in each element",
			  "stress_eigenvals",
			  &stress_evals));
  
  Menu *strainmenu = menu->AddSubMenu("strain", "print strain in each element");
  strainmenu->AddCommand(new ElementOutputCmd<StrainStress>
		   ("tensor",
		    "print total strain tensor for each element",
		    "strain_tensor", &Element::total_straintensor));
  strainmenu->AddCommand(new ElementOutputCmd<EigenValues>
			 ("eigenvals",
			  "print eigenvalues of strain in each element",
			  "strain_eigenvals",
			  &strain_evals));
  return menu;
}

static Menu *individualgroupmenu() {
  static Menu *menu = 0;
  if(menu) return menu;
  menu = new Menu("group output");
  menu->AddSubMenu("current_area", "print current area of a group of elements",
		   new GroupOutputMenu<double>("current_area", 
				       "print current area of each element",
				       "current_area",
				       &Element::current_area));
  menu->AddSubMenu("original_area",
		   "print original area of a group of elements",
		   new GroupOutputMenu<double>("original_area",
				       "print original area of each element",
				       "original_area",
				       &Element::original_area));

  menu->AddSubMenu("orientation", 
		   "print orientation of a group of elements",
		   new GroupOutputMenu<EulerAngle>("orientation",
					   "print orientation of each element",
					   "orientation",
					   &Element::query_orientation));

  menu->AddSubMenu("energy_density",
		   "print energy density of a group of elements",
		   new GroupOutputMenu<double>("energy_density",
				       "print energy density of each element",
				       "energy_density",
				       &Element::elastic_energy));
#ifdef THERMAL
  menu->AddSubMenu("temperature",
		   "print average temperature of a group of elements",
		   new GroupOutputMenu<double>("temperature",
				       "print temperature of each element",
				       "temperature",
				       &Element::average_T));
  menu->AddSubMenu("grad_temperature",
	   "print temperature gradient of a group of elements",
	   new GroupOutputMenu<const MV_Vector_double&>("grad_temperature",
			       "print temperature gradient of each element",
			       "grad_temperature",
			       &Element::gradient_temperature_field));
 menu->AddSubMenu("heat_flux",
	   "print heat flux of a group of elements",
	   new GroupOutputMenu<const MV_Vector_double&>("heat_flux",
			       "print heat flux of each element",
			       "heat_flux",
			       &Element::heat_flux_field));
#endif // THERMAL
  Menu *stressmenu = menu->AddSubMenu("stress",
				    "print stress in each element in a group");
  stressmenu->AddSubMenu("tensor", 
			 "print stress tensor for a group of elements",
			 new GroupOutputMenu<const StrainStress&>("tensor",
			     "print full stress tensor for each element",
			     "stress_tensor", &Element::stresstensor));
  stressmenu->AddSubMenu("eigenvals", 
			 "print stress eigenvalues for a group of elements",
			 new GroupOutputMenu<EigenValues>("eigenvals",
			     "print stress eigenvalues for each element",
				     "stress_eigenvals",
				     &stress_evals));

  Menu *strainmenu = menu->AddSubMenu("strain",
				    "print strain in each element in a group");
  strainmenu->AddSubMenu("tensor", 
			 "print strain tensor for a group of elements",
			 new GroupOutputMenu<StrainStress>("tensor",
				  "print full strain tensor for each element",
				  "strain_tensor",
				  &Element::total_straintensor));
  strainmenu->AddSubMenu("eigenvals", 
			 "print strain eigenvalues for a group of elements",
			 new GroupOutputMenu<EigenValues>("eigenvals",
			     "print strain eigenvalues for each element",
				     "strain_eigenvals",
				     &strain_evals));
  return menu;
}

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

Menu *outmenu() {
  // These are used in groupstressmenu and groupstrainmenu
  Enum<StatType>::name(CARTESIAN, "cartesian");
  Enum<StatType>::name(INVARIANT, "invariants");
  Enum<StatType>::name(EXTREMES, "extremes");
  Enum<StatType>::name(EIGENVALUES, "eigenvalues");

  Menu *omenu = new Menu("output");

  Menu *tmenu = omenu->AddSubMenu("tee",
				  "send output to a file AND to the screen");
  tmenu->AddCommand(new TeeCmd);
  tmenu->AddCommand("close", "stop sending output to a file", teestop);
  tmenu->AddCommand("show", "print current tee filename", show_teefilename);

  Menu *gridmenu = omenu->AddSubMenu("grid", "print grid");
  gridmenu->AddCommand(new SaveAscii);
  gridmenu->AddCommand(new SaveBinary);
  gridmenu->AddCommand(new SaveOOF2);

  omenu->AddSubMenu("individual", "print element by element output",
		    individualmenu());

#ifdef DEBUG
  Menu *matmenu = gridmenu->AddSubMenu("matrix", "print matrices");
  matmenu->AddCommand("Kii", "Kii", printKii);
  matmenu->AddCommand("Kib", "Kib", printKib);
  matmenu->AddCommand("Kbb", "Kbb", printKbb);
  matmenu->AddCommand("Klocal", "Klocal", printKlocal);
#endif // DEBUG

  omenu->AddCommand(new ShowNodeCommand);

  omenu->AddSubMenu("current_area", "current total area of a group of elements",
		    new GroupMenu<Element>("current_area",
					   "print current total area of group",
					   group_current_area));
  omenu->AddSubMenu("original_area",
		    "original total area of a group of elements",
		    new GroupMenu<Element>("original_area",
					   "print original total area of group",
					   group_original_area));
  omenu->AddSubMenu("force", "total force on a group of nodes",
		    new GroupMenu<Node>("force",
					"print total force on group",
					groupforce));
  omenu->AddSubMenu("traction", "integrated flux over a boundary",
		    new GroupMenu<Node>("traction",
					"print integrated flux over group",
					grouptraction));

  omenu->AddSubMenu("displacement", "average displacement of a group of nodes",
		    new GroupMenu<Node>("displacement",
					"print average displacement of group",
					groupdisplacement));
  omenu->AddSubMenu("position", "average position of a group of nodes",
		    new GroupMenu<Node>("position",
					"print average position of group",
					groupposition));
  omenu->AddSubMenu("stress", "average stress on a group of elements", 
		    groupstressmenu());
  omenu->AddSubMenu("strain", "average strain on a group of elements", 
		    groupstrainmenu());
  omenu->AddSubMenu("cross_section",
		    "plot values along a line through the sample",
		    crosssectionmenu());
#ifdef THERMAL
  omenu->AddSubMenu("temperature", "average temperature of a group of nodes",
		    new GroupMenu<Node>("temperature",
					"print average temperature of group",
					grouptemperature));
  omenu->AddSubMenu("heatrate", "total heat rate of a group of nodes",
		    new GroupMenu<Node>("heatrate",
					"print total heatrate of group",
					groupheatrate));
  omenu->AddSubMenu("heatflux", "integrated heat flux through an edge",
		    new GroupMenu<Node>("heatflux",
					"print integrated heatflux through",
					groupheatflux));
#endif // THERMAL
  omenu->AddCommand("energy", "find internal elastic energy", energy);
  omenu->AddCommand(new CommentCmd);
  return omenu;
}

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

#include "enumio.C"
