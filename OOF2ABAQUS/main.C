// -*- C++ -*-
// $RCSfile: main.C,v $
// $Revision: 1.10 $
// $Author: langer $
// $Date: 2005-09-06 18:06:22 $

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */

#define MAIN
#include "charstring.h"
#include "grid.h"
#include "initfile.h"
#include "main.h"
#include "menuDef.h"
#include "meshcoord.h"
#include "random.h"
#include <iostream.h>
#include <fstream.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>

static char *version = "OOF2ABAQUS version 1.0.1";

static void print_header(ostream &os, int, char**);

void usage() {
  cerr << "Usage:  oof2abaqus [-v][-e][-6][-o outputfile] inputfile" << endl;
  cerr << "     -e        Include empty elements" << endl;
  cerr << "     -6        Use six node triangles, instead of 3 node triangles"
       << endl;
  cerr << "     -o file   Send output to the file, instead of to stdout"
       << endl;
  cerr << "     -v        Print version number" << endl;
  exit(1);
}

void fatalerror() {
  cerr << "Fatal error!" << endl;
  exit(1);
}

int main(int argc, char *argv[]) {
  extern int optind;
  extern char *optarg;
  CharString outputfile;

  int c;
  while((c = getopt(argc, argv, "ve6o:")) != -1) {
    switch(c) {
    case 'o':			// output file name
      outputfile = optarg;
      break;
    case 'e':
      empties = true;
      break;
    case '6':
      sixnodes = true;
      break;
    case 'v':
      cerr << version << endl;
      exit(0);
      break;
    default:
    case '?':
      usage();
    }
  }

  if(optind != argc-1)
    usage();

  TextWaiter waiter;
  waiter.quiet();
  waiter.errors_off();
  initfile(argv[optind]);	// reads grid, using waiter.
  if(grid) {
    grid->find_material_groups();
    if(outputfile != "") {
      ofstream os(outputfile);
      print_header(os, argc, argv);
      grid->print_abaqus(os);
    }
    else {
      print_header(cout, argc, argv);
      grid->print_abaqus(cout);
    }
  }
}

#include <time.h>

static void print_header(ostream &os, int argc, char *argv[]) {
  time_t t = time((time_t*) 0);
  char *timestr = ctime(&t);
  *index(timestr, '\n') = '\0';
  os << "*Heading" << endl;
  os << "Abaqus file created by OOF2ABAQUS" << endl;
  os << timestr << endl;
  for(int i=0; i<argc; i++)
    os << argv[i] << " ";
  os << endl << endl;
  MeshCoord ll, ur;
  grid->get_dimensions(ll,ur);
  os << "Bounding box: lower left corner at  " << ll << endl;
  os << "             upper right corner at " << ur << endl;
}


