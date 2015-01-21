// -*- C++ -*-
// $RCSfile: main.C,v $
// $Revision: 1.16 $
// $Author: langer $
// $Date: 2001-06-28 15:38:56 $

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


#define MAIN
#include "bc.h"
#include "colorutils.h"
#include "distort.h"
#include "editlineinputhandler.h"
#include "effective.h"
#include "elementgroups.h"
#include "fatalerror.h"
#include "graphics.h"
#include "grid.h"
#include "initfile.h"
#include "main.h"
#include "memusage.h"
#include "menuDef.h"
#include "mouseclick.h"
#include "nodegroups.h"
#include "normtype.h"
#include "output.h"
#include "parameters.h"
#include "planarity.h"
#include "plotout.h"
#include "preconditioner.h"
#include "quit.h"
#include "random.h"
#include "replace.h"
#include "select.h"
#include "substring.h"
#include "twotwo.h"
#include "version.h"
#include <fstream.h>
#include <iostream.h>
#include <signal.h>
#include <stdio.h>
#include "stdlib.h"
#include <string.h>
#include <unistd.h>

static int textmode = 0;
int text_mode() { return textmode; }

static Menu *macromenu();
static void loadfile(const char*, Menu*), runrcfile(Menu*);

// signal handler for fatal errors

void fatalerror() {
  static const char *dumpfile = "oof_fatal.log";
  cerr << "!!! Saving log file in " << dumpfile << endl;
  ofstream os(dumpfile);
  garcon()->printlog(os);
  abort();
}

extern "C" {

static void exitfn() {
#ifdef DEBUG
  cerr << "Exiting..." << endl;
#endif
//   if(grid) delete grid;  // moved into quitcmd in quit.C
//   grid = 0;
  if(!text_mode()) {
    free_cursors();
    fl_finish(); // shuts down X connection
  }
}

static void fatal_error_handler(int sig, void*) {
  cerr << "@$#^&!! ";
  if(sig == SIGBUS)
    cerr << "Bus error";
  else if(sig == SIGSEGV)
    cerr << "Segmentation violation";
  fatalerror();
}

} // extern "C"

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

// command line options appear here, in the cmdopt[] array, and in
// main(). They must be consistent in all three places.

static void usage() {
    cerr << "Usage: oof [options]" << endl;
    cerr << "Options:   -text            use text interface" << endl;
    cerr << "           -start \\!<command>  run commands" << endl;
    cerr << "           -file <filename> process commands in filename" << endl;
    cerr << "           -grid <filename> read grid from filename" << endl;
    cerr << "           -singleclickfns  execute functions immediatedly with one click" << endl;
    cerr << "           -quit            quit" << endl;
    cerr << "           -log <logfile>   write a log file" << endl;
    cerr << "           -help            print this" << endl;
}

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

// XForms stuff to get command line arguments and/or X resources

// This associates command line args with resources.
// The X resource stuff will never see the -text option.

FL_CMD_OPT cmdopt[] = {
  {"-file", ".cmdfile", XrmoptionSepArg, 0},
  {"-start", ".start", XrmoptionSepArg, 0},
  {"-grid", ".gooffile", XrmoptionSepArg, 0},
  {"-quit", ".quit", XrmoptionNoArg, "1"},
  {"-log", ".logfile", XrmoptionSepArg, 0},
  {"-singleclickfns", ".singleclickfns", XrmoptionNoArg, "1"},
  {"-help", ".help", XrmoptionNoArg, "1"}
};
 // "1" here is the value to which a NoArg resource is set when it is
 // found on the command line or the resources database. It is not the
 // default value of the variable, which is set below.

static CharString filestoload(1000);
static CharString gooffile(100);
static CharString logfile(100);
static CharString startupstring(1000);
static int quitflag = 0;
static int printhelp = 0;
static int singleclickfns = 0;	// single click to execute fns immediately?

FL_resource res[] = {
  {"start", "Start", FL_STRING, &startupstring[0], "", 1000},
  {"cmdfile", "CmdFile", FL_STRING, &filestoload[0], "", 1000},
  {"gooffile", "GoofFile", FL_STRING, &gooffile[0], "", 100},
  {"logfile", "LogFile", FL_STRING, &logfile[0], "", 100},
  {"singleclickfns", "SingleClickFns", FL_BOOL, &singleclickfns, "0"},
  {"quit", "Quit", FL_BOOL, &quitflag, "0"},
  {"help", "Help", FL_BOOL, &printhelp, "0"}
};

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

int main(int argc, char *argv[]) {
  cerr << endl << "\
//-\\\\-//-\\\\-//-\\\\-//-\\\\-//-\\\\-//-\\\\-//-\\\\-//-\\\\-//-\\\\-//-\\\\-//-\\\\-//\n\
//                                                                //\n\
//   OOF --    Object Oriented Finite element analysis            //\n\
//                      for Materials Science                     //\n\
//                                                                //\n\
//                      Brought to you by the                     //\n\
//   Center for Theoretical and Computational Materials Science   //\n\
//            and the Information Technology Laboratory           //\n\
//      at the National Institute of Standards and Technology     //\n\
//                            and the                             //\n\
//         Department of Materials Science and Engineering        //\n\
//              Massachusetts Institute of Technology             //\n\
//               Stephen A. Langer   W. Craig Carter              //\n\
//                Edwin R. Fuller    R. Edwin Garcia              //\n\
//                                                                //\n\
//                 http://www.ctcms.nist.gov/oof/                 //\n\
//                                                                //\n\
//-\\\\-//-\\\\-//-\\\\-//-\\\\-//-\\\\-//-\\\\-//-\\\\-//-\\\\-//-\\\\-//-\\\\-//-\\\\-//"
	       << endl;
#ifndef THERMAL
  cerr << "OOF Version " << oofversion() << endl;
#else
  cerr << "THERMAL OOF Version " << oofversion() << endl;
#endif

  (void) atexit(exitfn);
  rndmseed(seed);
  MouseClick::init();
  init_planarity();
  preconditioner_init();
  init_normtype();

  extern char *optarg;
  int i;
  Waiter *w;

  // look for -text argument
  for(i=1; i<argc; i++) {
    if(!strcmp(argv[i], "-text")) {
      textmode = 1;
      break;
    }
  }
    
  if(textmode) {    // don't use X to parse args!
    for(i=1; i<argc; i++) {
      if(!strcmp(argv[i], "-text"))
	continue;
      if(!strcmp(argv[i], "-file")) {
	filestoload = argv[++i];
	continue;
      }
      if(!strcmp(argv[i], "-start")) {
	startupstring = argv[++i];
	continue;
      }
      if(!strcmp(argv[i], "-log")) {
	logfile = argv[++i];
	continue;
      }
      if(!strcmp(argv[i], "-grid")) {
	gooffile = argv[++i];
	continue;
      }
      if(!strcmp(argv[i], "-quit")) {
	quitflag = 1;
	continue;
      }
      if(!strcmp(argv[i], "-help")) {
	usage();
	exit(0);
      }
      fprintf(stderr, "Unknown option %s\n", argv[i]);
      usage();
      exit(1);
    }
    w = new TextWaiter(editlineinputhandler);
  }
  else { // not textmode, use X to parse args and start graphics

    // kludge -- xforms gets confused if program name has two dots in
    // it, and the webinstall script appends the dotty version number.
    argv[0] = "oof";

    w = new FormWaiter(&argc, argv, cmdopt, 7);
    fl_get_app_resources(res, 7);

    fl_add_signal_callback(SIGSEGV, fatal_error_handler, 0);
    fl_add_signal_callback(SIGBUS, fatal_error_handler, 0);
    
    if(argc > 1) {
      cerr << "Unknown option: " << argv[1] << endl;
      usage();
      exit(1);
    }
    if(printhelp) {
      usage();
      exit(0);
    }
    gfxinfo.setup();
    init_cursors();
  
    if(singleclickfns)
      MenuForm::clickmode = MenuForm::SINGLE_NOW;
    else
      MenuForm::clickmode = MenuForm::DOUBLE_NOW;
  }
  
  runrcfile(get_rootmenu()); // read .oofrc

  if(gooffile != "")       // load gooffile specified with -grid
    initfile(gooffile);

  // Load files and run commands specified with -file and -start.  The
  // two arguments are actually equivalent, and -start is the better
  // name.  -file is kept only for backwards compatibility.
  if(startupstring == "")
    startupstring = filestoload;
  else if(filestoload != "")
    startupstring += "," + filestoload;
  if(startupstring != "") {
    SubString sstr(startupstring);
    for(int i=0; i<sstr.size(); i++) {
      const CharString &str = sstr[i];
      if(str[0] == '!') {	// "files" beginning with ! are commands
	ScriptWaiter sw(&str[1], garcon()->msout);
	sw.order(get_rootmenu());
      }
      else
	loadfile(str, get_rootmenu());
    }
//      char *startptr = &filestoload[0];
//      char *endptr;
//      while(endptr = strchr(startptr, ',')) { // found a comma
//        *endptr = '\0';
//        loadfile(startptr, get_rootmenu());
//        startptr = endptr + 1;
//      }
//      loadfile(startptr, get_rootmenu()); // no comma follows last file 
  }

  if(quitflag){  // -quit option
    if(grid) delete grid;
    grid = 0;
    exit(0);
  }
    
  if(logfile != "")   // -log option
    w->startlog(logfile);

  w->quitcommand(quitcmd); // execute quitcmd when quit button is pressed

  w->order(get_rootmenu());
    
  return 0;
}

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

// read command from a file

static void loadfile(const char *filename, Menu *menu) {
    FILE *file = fopen(filename, "r");
    if(!file)
	cerr << "Can't open file " << filename << "!" << endl;
    else {
	/* this sends output to the screen, which may be the wrong thing
	 * to do if in gui mode. But this function might be called by the
	 * command line args, so the FormWaiter might not exist yet.
	 */
	if(!garcon()) {
	    TextWaiter w;
	    w.runfile(file, menu, 1);
	}
	else
	    garcon()->runfile(file, menu, 1);
    }
}

#define RCFILE ".oofrc"
static FILE *getrcfile() {
    FILE *rcfile;
    if(rcfile = fopen(RCFILE, "r"))
	return rcfile;
    if(rcfile = fopen(CharString(getenv("HOME"))+"/"+RCFILE, "r"))
	return rcfile;
    return NULL;
}

static void runrcfile(Menu *menu) {
    FILE *rcfile;
    if(rcfile = getrcfile()) {
	if(!garcon()) {
	    TextWaiter w;
	    w.runfile(rcfile, menu, 0);
	}
	else
	    garcon()->runfile(rcfile, menu, 0);
	fclose(rcfile);
    }
}

class LoadCommand : public CommandM {
private:
  FileName filename;
public:
  LoadCommand()
    : CommandM("commandfile", "load a command file"), 
      filename("inputfile")
    {
      AddArgument(this, "filename", filename);
    }
  virtual CommandFn func() {
    FILE *file = fopen(filename, "r");
    if(!file)
      garcon()->msout << ms_error << "Can't open file " << filename << "!"
		      << endl << ms_normal;
    else {
      garcon()->runfile(file, get_rootmenu(), 1);
      fclose(file);
    }
  }
  virtual CommandM *clone() const {
    LoadCommand *lc = new LoadCommand;
    lc->filename = filename;
    return lc;
  }
};

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

static CommandFn quit() {
  if(quitcmd())
    exit(0);
}

int gridinit() {
  if(!grid) {
    garcon()->msout << ms_error << "Grid is not initialized!"
		    << endl << ms_normal;
    return 0;
  }
  return 1;
}

#ifdef THERMAL
static CommandFn equilibrate_all() {
  if(gridinit()) {
    grid->equilibrate_all(use_globalK);
    grid->redraw();
  }
}

static CommandFn equilibrate_thermal() {
  if(gridinit()) {
    grid->thermal_equilibrate();
    grid->redraw();
  }
}
#endif // THERMAL

static CommandFn equilibrate_elastic() {
  if(gridinit()) {
    grid->equilibrate(use_globalK);
    grid->redraw();
  }
}

static CommandFn complete_equilibrate() {
  if(gridinit()) {
    grid->complete_equilibrate(use_globalK); // redraws as it goes
  }
}

static CommandFn mutate() {
  if(gridinit()) {
    int mutants = grid->mutate();
    garcon()->msout << ms_info << "Mutated " << mutants << " element"
		    << (mutants==1?".":"s.") << endl << ms_normal;
    grid->redraw();
  }
}

static CommandFn gridreset() {
  if(gridinit()) {
    grid->reset();
    grid->redraw();
  }
}

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

class StartLogCmd : public CommandM {
private:
  FileName filename;
public:
  StartLogCmd()
    : CommandM("start", "start a log file"),
      filename("cmdfile.oof") {
    AddArgument(this, "file", filename);
  }
  virtual CommandFn func() {
    garcon()->startlog(filename);
  }
  virtual CommandM *clone() const {
    StartLogCmd *lc = new StartLogCmd;
    lc->filename = filename;
    return lc;
  }
};

static CommandFn stoplog() {
  garcon()->stoplog();
}

class SaveLogCmd : public CommandM {
private:
  FileName filename;
public:
  SaveLogCmd()
    : CommandM("save", "save log file for entire session up to now",
               MOPT_NOLOG),
      filename("logfile")
    {
      AddArgument(this, "file", filename);
    }
  virtual CommandFn func() {
    ofstream os(filename);
    if(!os) {
      garcon()->msout << ms_error << "Can't open file " << filename << "!"
		      << endl << ms_normal;
      return;
    }
    garcon()->printlog(os);
  }
  virtual CommandM *clone() const {
    SaveLogCmd *slc = new SaveLogCmd;
    slc->filename = filename;
    return slc;
  }
};

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

#ifdef DEBUG
CommandFn checkpoint() {
  static int x = 0;
  x++;
}
#endif

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

// menus constructed elsewhere:
extern Menu *initmenu(), *outmenu(), *debugmenu();

Menu *get_rootmenu() {
  static Menu *root = 0;
  if(root) return root;
#ifdef THERMAL
  root = new Menu("OOF " + oofversion() + "T");
#else  // !THERMAL
  root = new Menu("OOF " + oofversion());
#endif // THERMAL
  // global parameters
#ifndef THERMAL
  AddVariable(root, "deltaT", "temperature change", delta_T);
#endif // !THERMAL
  AddVariable(root, "fictiveT", "fictive temperature for stress free strain",
	      fictiveT);
#ifndef THERMAL
  AddVariable(root, "maxiter", "max iterations per step", maxiter);
  AddVariable(root, "tolerance", "iterative method tolerance", tol);
  AddVariable(root, "globalK", "use global stiffness?", use_globalK);
  AddVariable(root, "preconditioner", "set sparse matrix preconditioner",
	      pctype);
#endif // !THERMAL
  AddVariable(root, "verbose", "verbosity of screen output", verbose);
  AddVariable(root, "seed", "random seed for anarchic diagonals", seed,
	      setseed);
  root->AddCommand(new LoadCommand);
#ifndef THERMAL
  root->AddCommand("equilibrate", "compute displacements", equilibrate_elastic);
  root->AddCommand("comp_equil", "equilibrate until nothing mutates",
		       complete_equilibrate);
#endif // !THERMAL
  root->AddCommand("mutate", "update mutable elements", mutate);
  root->AddCommand("reset",
		       "clear all displacements and applied forces", gridreset);
  root->AddCommand(new LoopCommand("cruise",
			"increment and equilibrate repeatedly",
			root, 
			"distort stealth_increment; equilibrate; plot out", 
			"plot label; plot out",
			"plot close")); 
  root->AddCommand(new LoopCommand("loop", "repeat a command",
				       root, "", "", "", 0));
  root->AddCommand("pause", "pause until told to go on", Waiter::pause);
  root->AddCommand("quit", "stop whatever you're doing RIGHT NOW!", quit,
		   MOPT_NOLOG);
#ifdef DEBUG
  root->AddCommand("checkpoint",
		       "dummy routine for setting debug checkpoints",
		       checkpoint);
#endif // DEBUG
  root->AddSubMenu("initialize", "set initial grid", initmenu());

#ifdef THERMAL
  Menu *equilimenu = root->AddSubMenu("equilibrate", "equilibrate");
  AddVariable(equilimenu, "maxiter", "max iterations per step", maxiter);
  AddVariable(equilimenu, "tolerance", "iterative method tolerance", tol);
  AddVariable(equilimenu, "preconditioner",
	      "choose preconditioner for iterative matrix method", pctype);
  AddVariable(equilimenu, "norm", "norm for computing residual", normtype);
  equilimenu->AddCommand("all", "compute displacements and temperatures",
			 equilibrate_all);
  equilimenu->AddCommand("thermal", "compute temperatures",
			 equilibrate_thermal);
  equilimenu->AddCommand("elastic", "compute temperatures",
			 equilibrate_elastic);
  equilimenu->AddCommand("complete", "equilibrate until nothing mutates",
		       complete_equilibrate);
#endif // THERMAL

  root->AddSubMenu("modify", "modify existing grid", modifymenu());

  Menu *grpmenu = root->AddSubMenu("groups",
				       "manage node and element groups");
  grpmenu->AddSubMenu("elements", "manage element groups", elemgroupmenu());
  grpmenu->AddSubMenu("nodes", "manage node groups", nodegroupmenu());
    
  root->AddSubMenu("select",
		       "select nodes and elements", mainselectmenu());
    
  root->AddSubMenu("bc", "assign boundary conditions to groups of nodes",
		       bcmenu());
  root->AddSubMenu("distort", "set and apply distortions and forces",
		       distmenu());
  //root->AddSubMenu("loop", "increment distortion and plot", loopmenu());
  root->AddSubMenu("output", "print stuff", outmenu());
  root->AddSubMenu("effective_properties", "compute effective properties",
		   effectivemenu());
  if(!textmode)
    root->AddSubMenu("graphics", "Graphics Drawers", graphicsmenu());
  root->AddSubMenu("plot", "plot stuff to a file", plotmenu());
#ifdef DEBUG
  root->AddSubMenu("debug", "run debugging routines", debugmenu());
#endif
  root->AddSubMenu("macros", "load and save macros", macromenu());
  
  Menu *logmenu = root->AddSubMenu("log", "keep a session log");
  logmenu->AddCommand(new StartLogCmd);
  logmenu->AddCommand("stop", "stop logging commands", stoplog);
  logmenu->AddCommand(new SaveLogCmd);
#ifdef MEMUSAGE
  root->AddCommand("memusage", "print memory allocated", pmemory);
#endif
  return root;
}

static Menu *macromenu() {
  static Menu *menu = 0;
  if(menu) return menu;
  menu = new Menu("macro management");
  menu->AddCommand(new SaveMacroCmd("save"));
  menu->AddCommand(new LoadMacroCmd("load"));
  return menu;
}


//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

#include "enumio.C"
