// -*- C++ -*-
// $RCSfile: ppm2oof.C,v $
// $Revision: 1.39 $
// $Author: langer $
// $Date: 2001-10-25 21:15:10 $

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */

#define MAIN

#include "addfilter.h"
#include "amtriangle.h"
#include "pixelgroups.h"
#include "burn.h"
#include "charstring.h"
#include "colorutils.h"
#include "compressedfile.h"
#include "despeckle.h"
#include "editlineinputhandler.h"
#include "elector.h"
#include "expandsel.h"
#include "fatalerror.h"
#include "filename.h"
#include "gfx.h"
#include "goof.h"
#include "groups.h"
#include "imageform.h"
#include "material.h"
#include "menuDef.h"
#include "meshcmds.h"
#include "meshgroups.h"
#include "mouseclick.h"
#include "output.h"
#include "pixelgroups.h"
#include "planarity.h"
#include "ppm2oof.h"
#include "quit.h"
#include "random.h"
#include "restrict.h"
#include "robomouse.h"
#include "saveconfig.h"
#include "select_by_color.h"
#include "selectpixel.h"
#include "substring.h"
#include "system.h"
#include "version.h"
#include <signal.h>
#include <stdio.h>
#include "stdlib.h"
#include <unistd.h>
#ifdef HAVE_MALLOC_H
#include <malloc.h>
#endif // HAVE_MALLOC_H

Menu *ppm2oofmenu(void);
void loadfile(const CharString&);

static void exitfn() {
  if(!textmode) {
    free_cursors();
    fl_finish();
  }
#ifdef DEBUG
  cerr << "Exiting..." << endl;
#endif
}

// signal handler for fatal errors

void fatalerror() {
  static char *dumpfile = "ppm2oof_fatal.log";
  cerr << "!!! Saving log file in " << dumpfile << endl;
  ofstream os(dumpfile);
  garcon()->printlog(os);
#ifdef DEBUG
  abort();
#else
  exit(1);
#endif
}

static void fatal_error_handler(int sig, void*) {
  cerr << "@$#^&!! ";
  if(sig == SIGBUS)
    cerr << "Bus error";
  else if(sig == SIGSEGV)
    cerr << "Segmentation violation";
#ifdef __INSURE__
  _insight_set_option("unsuppress", "all");
  _Insure_cleanup();
#endif
  fatalerror();
}

static void usage(char *arg0) {
    cerr << "Usage: " << arg0 << " [options]" << endl;
    cerr << "   Options are:" << endl;
    cerr << "      -file ppmfile      read a ppmfile on startup"
	      << endl;
    cerr << "      -stdout            write goof to stdout" << endl;
    cerr << "      -text              use text interface" << endl;
    cerr << "      -log file          save commands in file" << endl;
    cerr << "      -start file        read startup commands or configuration from file"
	      << endl;
    cerr << "      -seed n            set random number seed" << endl;
    cerr << "      -quit              quit after reading startup file"
	      << endl;
    cerr << "      -help              print this" << endl;
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

// XForms stuff to get command line arguments and/or X resources

// This associates command line args with resources
static FL_CMD_OPT cmdopt[] = {
  {"-file", ".ppmfile", XrmoptionSepArg, 0},
  {"-stdout", ".useStdout", XrmoptionNoArg, "1"},
  {"-text", ".textmode", XrmoptionNoArg, "1"},
  {"-log", ".logFile", XrmoptionSepArg, 0},
  {"-start", ".startupFile", XrmoptionSepArg, 0},
  {"-seed", ".seed", XrmoptionSepArg, 0},
  {"-quit", ".quit", XrmoptionNoArg, "1"},
  {"-help", ".help", XrmoptionNoArg, "1"},
};

static CharString filetoload(100);
static CharString logfilename(100);
static CharString startupstring(100);
static int printhelp = 0;
static int quitflag = 0;

// This associates resources with variables
static FL_resource res[] = {
  {"ppmfile", "PpmFile", FL_STRING, &filetoload[0], "", 100},
  {"useStdout", "UseStdout", FL_BOOL, &use_stdout, "0"},
  {"logFile", "LogFile", FL_STRING, &logfilename[0], "", 100},
  {"startupFile", "StartUpFile", FL_STRING, &startupstring[0], "", 100},
  {"textmode", "TextMode", FL_BOOL, &textmode, "0"},
  {"seed", "Seed", FL_INT, &seed, "17"},
  {"quit", "Quit", FL_BOOL, &quitflag, "0"},
  {"help", "Help", FL_BOOL, &printhelp, "0"},
};

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//


int main(int argc, char *argv[]) {

  cerr << endl << "\
//-\\\\-//-\\\\-//-\\\\-//-\\\\-//-\\\\-//-\\\\-//-\\\\-//-\\\\-//-\\\\-//-\\\\-//-\\\\-//\n\
//                                                                //\n\
//   PPM2OOF --  Object Oriented Finite element analysis          //\n\
//                      for Materials Science                     //\n\
//                                                                //\n\
//                      Brought to you by the                     //\n\
//   Center for Theoretical and Computational Materials Science   //\n\
//            and the Information Technology Laboratory           //\n\
//      at the National Institute of Standards and Technology     //\n\
//                            and the                             //\n\
//         Department of Materials Science and Engineering        //\n\
//              Massachusetts Institute of Technology             //\n\
//               Stephen A. Langer    W. Craig Carter             //\n\
//                Daniel Vlacich      Edwin R. Fuller             //\n\
//                                                                //\n\
//                  http://www.ctcms.nist.gov/oof/                //\n\
//                                                                //\n\
//-\\\\-//-\\\\-//-\\\\-//-\\\\-//-\\\\-//-\\\\-//-\\\\-//-\\\\-//-\\\\-//-\\\\-//-\\\\-//"
       << endl;

#ifdef THERMAL
  cerr << "THERMAL PPM2OOF Version " << ppm2oofversion() << endl;
#else
  cerr << "PPM2OOF Version " << ppm2oofversion() << endl;
#endif

  (void) atexit(exitfn);
  
  MouseClick::init();
  AMTriangle::init();
  init_planarity();

  Menu *rootmenu = ppm2oofmenu();
  Waiter *w;

  // look for -text argument
  for(int i=1; i<argc; i++) {
    if(!strcmp(argv[i], "-text")) {
      textmode = 1;
      break;
    }
  }

  if(textmode) {		// don't use X to parse args!
    for(int i=1; i<argc; i++) {
      if(!strcmp(argv[i], "-text"))
	continue;
      if(!strcmp(argv[i], "-file")) {
	filetoload = argv[++i];
	continue;
      }
      if(!strcmp(argv[i], "-stdout")) {
	use_stdout = 1;
	continue;
      }
      if(!strcmp(argv[i], "-log")) {
	logfilename = argv[++i];
	continue;
      }
      if(!strcmp(argv[i], "-start")) {
	startupstring = argv[++i];
	continue;
      }
      if(!strcmp(argv[i], "-quit")) {
	quitflag = 1;
	continue;
      }
      if(!strcmp(argv[i], "-seed")) {
	seed = atoi(argv[++i]);
	continue;
      }
      if(!strcmp(argv[i], "-help")) {
	usage(argv[0]);
	exit(0);
      }
      cerr << "Unknown option: " << argv[i] << endl;
      usage(argv[0]);
      exit(1);
    }
    w = new TextWaiter(editlineinputhandler);
  }
  else {		// not textmode, use X to parse args and start graphics

    // kludge -- xforms gets confused if program name has two dots in
    // it, and the webinstall script appends the dotty version number.
    argv[0] = "ppm2oof";

    w = new FormWaiter(&argc, argv, cmdopt, 7);
    fl_get_app_resources(res, 8);

    if(argc > 1) {
      cerr << "Unknown option: " << argv[1] << endl;
      usage(argv[0]);
      exit(1);
    }
    if(printhelp) {
      usage(argv[0]);
      exit(0);
    }

    gfxinfo.setup();
    init_cursors();

    fl_add_signal_callback(SIGSEGV, fatal_error_handler, 0);
    fl_add_signal_callback(SIGBUS, fatal_error_handler, 0);
  }

  rndmseed(seed);

  if(filetoload != "")
    loadfile(filetoload);

  //  char *tmpfile = 0;
  if(logfilename != "") {	// start logging
//     // if logfile == startupfile, then the startup file has to be copied first
//     if(logfilename == startupstring) {
//       tmpfile = tempnam(0, "oof_");
//       system(CharString("cp ") + startupstring + " " +  tmpfile);
//       startupstring = tmpfile;
//       w->startlog(logfilename, TF_TRUE);
//     }
//     else 
    w->startlog(logfilename);
  }
  
  if(startupstring != "") {	// run startup file
    SubString sstr(startupstring); // break up startupstring into substrings
    for(int i=0; i<sstr.size(); i++) { // loop over substrings
      const CharString &str = sstr[i];
      if(str[0] == '!') {	// execute strings beginning with '!'
	ScriptWaiter sw(&str[1], garcon()->msout);
	sw.order(rootmenu);
      }
      else {			// strings not beginning with '!' are file names
	CompressedFile file(str);
	if(file.ok())
	  w->runfile(file.stream(), rootmenu, 1);
	else
	  garcon()->msout << ms_error << "Can't open file " << str << "!"
			  << endl << ms_normal;
      }
    }
    
//      if(tmpfile) {	// remove temporary file
//        unlink(tmpfile);
//        free(tmpfile);
//      }
  }

  if(quitflag) {		// -quit option
    //    if(current_goof)
    //      delete current_goof;
    return 0;
  }

  w->quitcommand(quitcmd);	// execute quitcmd when quit button is pressed

  w->order(rootmenu);
  return 0;  
}

// -------------------------------------------------------- //

CommandFn quit() {
  if(quitcmd()) {
#if 0
    // This doesn't work, and it's not really needed.  XForms thinks
    // that it should call input objects callback functions even if
    // the object has been deleted...  Something isn't being done in
    // the right order, but the problem can be solved by avoiding the
    // goof destructor.
    if(current_goof)
      delete current_goof;
#endif
    exit(0);
  }
}

// -------------------------------------------------------- //

// read a ppmfile

class ReadPPMCmd : public CommandM {
private:
  FileName filename;
public:
  ReadPPMCmd()
    : CommandM("ppmfile", "read a ppm file"), 
      filename("file.ppm")
  {
    AddArgument(this, "filename", filename);
  }
  virtual CommandFn func() {
    loadfile(filename);
  }
  virtual CommandM *clone() const {
    ReadPPMCmd *rpc = new ReadPPMCmd;
    rpc->filename = filename;
    return rpc;
  }
};

void loadfile(const CharString &filename) {
  Goof *old_goof = current_goof;
  Goof *new_goof = new Goof(filename); // make a goof
  if(new_goof->gallerysize() == 0) { // Failed to read the file!
    delete new_goof;
    return;
  }
  else if(!old_goof) {		// This is the first image
    current_goof = new_goof;
      open_window();
  }
  else if(new_goof->query_height() == old_goof->query_height() &&
	  new_goof->query_width() == old_goof->query_width()) {
    // New image is the same size as the old one. Add new image to
    // old Goof, and keep using the old Goof.
    current_goof = old_goof;
    current_goof->addimage(*new_goof->get_image(0), filename);
    // Display the new image in all the ImageForms.
    current_goof->displayimage(current_goof->gallerysize()-1);
    delete new_goof;
  }
  else {
    // The new image is a different size. Get rid of the old Goof and
    // display the new Goof in the old Goof's windows.
    current_goof = new_goof;
    old_goof->transfer_forms(current_goof);
    delete old_goof;
  }
}

// -------------------------------------------------------- //


#ifdef DEBUG
static void redraw_all(const CharString&) {
  if(current_goof) {
    current_goof->redraw();
  }
}

CommandFn randomcmd() {
  for(int i=0; i<10; i++)
    cout << rndm() << endl;
}
#endif

// -------------------------------------------------------- //

int goofinit() {    // check that current_goof has been initialized
    if(!current_goof) {
	garcon()->msout << ms_error << "No image!" << endl << ms_normal;
	return 0;
    }
    return 1;
}

// -------------------------------------------------------- //

class StartLogCmd : public CommandM {
private:
  FileName filename;
public:
  StartLogCmd()
    : CommandM("start", "start a log file", MOPT_NOLOG),
      filename("logfile.poof")
    {
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
      filename("logfile.poof")
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

class LoadCommand : public CommandM {
private:
  FileName filename;
public:
  LoadCommand() : CommandM("commandfile", "load a command file", MOPT_NOLOG), 
    filename("inputfile")
  {
    AddArgument(this, "filename", filename);
  }
  virtual CommandFn func() {
    CompressedFile file(filename);
    if(!file.ok())
      garcon()->msout << ms_error << "Can't open file " << filename << "!"
		      << endl << ms_normal;
    else {
      garcon()->runfile(file.stream(), ppm2oofmenu(), 1);
    }
  }
  virtual CommandM *clone() const {
    LoadCommand *lc = new LoadCommand;
    lc->filename = filename;
    return lc;
  }
};

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

class SaveConfigCmd : public CommandM {
private:
  FileName filename;
public:
  SaveConfigCmd() : CommandM("save", "save a configuration file", MOPT_NOLOG),
    filename("ppm2oof.cfg")
  {
    AddArgument(this, "filename", filename);
  }
  virtual CommandFn func() {
    ofstream file(filename);
    if(!file)
      garcon()->msout << ms_error << "Can't open file " << filename << "!"
		      << endl << ms_normal;
    else
      saveconfig(file, current_goof);
  }
  virtual CommandM *clone() const {
    SaveConfigCmd *scc = new SaveConfigCmd;
    scc->filename = filename;
    return scc;
  }
};

class LoadConfigCmd : public CommandM {
private:
  FileName filename;
public:
  LoadConfigCmd() : CommandM("load", "load a configuration file", MOPT_NOLOG),
    filename("ppm2oof.cfg")
  {
    AddArgument(this, "filename", filename);
  }
  virtual CommandFn func() {
    loadconfig(filename);
  }
  virtual CommandM *clone() const {
    LoadConfigCmd *lcc = new LoadConfigCmd;
    lcc->filename = filename;
    return lcc;
  }
};

// This command is executed when the rest of the file is to be
// interpreted as a configuration file, ie, when the command
// "configfile" appears at the top of a command file.

static CommandFn loadconfig_cmd() {
  loadconfig(*garcon()->inputstream());
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

Menu *modifyselectmenu() {
  static Menu *menu = 0;
  if(menu) return menu;
  menu = new Menu("modify selection");
  menu->AddCommand(new DespeckleCmd);
  menu->AddCommand(new elkcepseDCmd);
  menu->AddCommand(new ExpandSelectionCmd);
  menu->AddCommand(new ShrinkSelectionCmd);
  menu->AddCommand(new LifeCmd);
  return menu;
}

static CommandFn selectnone() {
  if(goofinit()) {
    current_goof->unselect_all();
    current_goof->redraw();
  }
}

static CommandFn selectcomplement() {
  if(goofinit()) {
    current_goof->select_complement();
    current_goof->redraw();
  }
}

static CommandFn undo() {
  if(goofinit()) {
    current_goof->undo_selection();
    current_goof->redraw();
  }
}

static CommandFn redo() {
  if(goofinit()) {
    current_goof->redo_selection();
    current_goof->redraw();
  }
}

static CommandFn selectinfo() {
  if(goofinit())
    current_goof->selection_info();
}

Menu *selectpixelmenu() {
  static Menu *menu = 0;
  if(menu) return menu;
  menu = new Menu("pixel selection tools");
  menu->AddCommand("none", "unselect all pixels", selectnone);
  menu->AddCommand("invert", "select all the other pixels", selectcomplement);
  menu->AddCommand("undo", "undo the previous selection", undo);
  menu->AddCommand("redo", "redo the previously undone selection", redo);
  menu->AddCommand("info", "get information on the current selection",
		   selectinfo);
  menu->AddSubMenu("group", "select all pixels in a group",
		   groupselectmenu());
  menu->AddSubMenu("group_too", "also select all pixels in another group",
		   groupselectmenu_too());
  menu->AddSubMenu("intersect_group",
   "select the intersection of a group with the previously selected pixels",
		   groupinterselectmenu());
  menu->AddSubMenu("unselect_group", "unselect the pixels in a group",
		   unselectgroupmenu());
  menu->AddSubMenu("modify", "modify current selection", modifyselectmenu());
  return menu;
}

// ---------------------------- //

Menu *macromenu() {
  static Menu *menu = 0;
  if(menu) return menu;
  menu = new Menu("macro management");
  menu->AddCommand(new SaveMacroCmd("save"));
  menu->AddCommand(new LoadMacroCmd("load"));
  return menu;
}

// -------------------------------------------------------- //

Menu *ppm2oofmenu() {
  static Menu *menu = 0;
  if(menu) return menu;
#ifdef THERMAL
  menu = new Menu("ppm2oof " + ppm2oofversion() + "T");
#else  // !THERMAL
  menu = new Menu("ppm2oof " + ppm2oofversion());
#endif // THERMAL

  AddVariable(menu, "width", "width in physical units", physical_width);
  AddVariable(menu, "seed", "random seed for mesh generation, etc", seed,
	      setseed);
  menu->AddCommand(new ReadPPMCmd);
  menu->AddCommand(new LoadCommand);

  menu->AddCommand("pause", "pause until told to go on", Waiter::pause);
  menu->AddCommand("quit", "take your toys and go home", quit, MOPT_NOLOG);

  menu->AddSubMenu("graphics", "manage graphics windows", graphicsmenu());
  menu->AddSubMenu("select", "ways to pick pixels", selectpixelmenu());
  menu->AddSubMenu("active_area", "define active area",
		   activatemenu());
  Menu *firewallmenu = menu->AddSubMenu("firewalls", "manage fire walls",
					"firewalls");
  firewallmenu->AddCommand("set", "make selected pixels inflammable",
			   set_firewall);
  firewallmenu->AddCommand("clear", "remove firewalls", clear_firewall);

  menu->AddSubMenu("pixelgroups", "manage pixel groups", groupmenu());
  menu->AddSubMenu("materials",
	     "assign material properties to selected pixels", materialmenu());

  Menu *outmenu = menu->AddSubMenu("simple_mesh", "write an oof file");
  outmenu->AddCommand(new OutputCmd);
  outmenu->AddCommand(new SubRegionOutputCmd);

  menu->AddSubMenu("adaptive_mesh", "create and refine a finite element mesh",
		   meshmenu());

  /*    
  menu->AddSubMenu("periodicity", "apply periodic boundary conditions",
		   periodicmenu());
  */
  menu->AddSubMenu("macros", "load and save macros", macromenu());
  
  Menu *logmenu = menu->AddSubMenu("log", "keep a session log");
  logmenu->AddCommand(new StartLogCmd);
  logmenu->AddCommand("stop", "stop logging commands", stoplog, MOPT_NOLOG);
  logmenu->AddCommand(new SaveLogCmd);

#ifdef DEBUG
  AddVariable(outmenu, "bluebox", "use recycled elements?", bluebox);
  AddVariable(menu, "Ncolors", "restrict # of colors in displayed image",
	      restricted_colors, redraw_all);

  //  menu->AddCommand(new ArsonCmd);
#endif

  Menu *configmenu = menu->AddSubMenu("configuration",
				      "load and save configuration files");
  configmenu->AddCommand(new SaveConfigCmd);
  configmenu->AddCommand(new LoadConfigCmd);

  // system menu isn't ready for prime time yet...
  //  menu->AddSubMenu("system", "execute system commands", systemmenu());

  // The following command only appears at the top of configuration files
  menu->AddCommand("configfile", "parse rest of the input as a config file",
		   loadconfig_cmd, MOPT_NOLOG|MOPT_SECRET);
  return menu;
}

Menu *periodicmenu() {
  static Menu *menu = 0;
  if(menu) return menu;
  menu = new Menu("periodicity");
  AddVariable(menu, "x", "make the x direction periodic",
	      Goof::periodic_x_dflt, &Goof::set_periodicity);
  AddVariable(menu, "y", "make the y direction periodic",
	      Goof::periodic_y_dflt, &Goof::set_periodicity);
  return menu;
}
