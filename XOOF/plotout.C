// -*- C++ -*-
// $RCSfile: plotout.C,v $
// $Revision: 1.3 $
// $Author: langer $
// $Date: 2000-09-06 20:11:11 $


/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */



#include "nodegroups.h"
#include "plotout.h"
#include "parameters.h"
#include "output.h"
#include "menuDef.h"


static CharString plotscript;
static Menu *plotoutmenu;
static MenuStream *filestream = 0;
static int ownfilestream = 1;

static const int plotmenudepth = 2; /* number of levels in the "plot set"
                                     * menu preceding the things to be plotted.
				     */

// Add a function to the list of things being plotted
// All commands in the "plot set" submenu are really this one.

static CommandFn addtoplot() {
  int depth = garcon()->get_depth();
  for(int i=depth-plotmenudepth-1; i>=0; i--)
    plotscript = plotscript.spaceappend(garcon()->get_command(i));
  plotscript += ";";
}

static CommandFn clearplot() {
  plotscript = "";
}

class PlotFileCmd : public FileCmd {
public:
  PlotFileCmd()
    : FileCmd("file", "select an output file", "plotoutput")
    {}
  virtual CommandFn func() {
    if(filename == "-") {
      filestream = &garcon()->msout;
      ownfilestream = 0;
    }
    else {
      FILE *file = fopen(filename, "w");
      if(!file)
	garcon()->msout << ms_error << "Can't open file \"" << filename
			<< "\"" << endl << ms_normal;
      else {
	filestream = new FileMenuStream(file);
	ownfilestream = 1;
      }
    }
  }
  virtual FileCmd *subclone() const {
    return new PlotFileCmd;
  }
};

static CommandFn closeplot() {
  if(ownfilestream)
    delete filestream;
  filestream = 0;
}

static CommandFn labelplot() {
  if(filestream)
    *filestream << "## " << plotscript << endl;
}

static CommandFn showplot() {
  if(plotscript == "")
    garcon()->msout << "Not plotting anything." << endl;
  else
    garcon()->msout << "Now plotting: " << plotscript << endl;
}

void doplot() {
  if(!filestream) return;
  ScriptWaiter sw(plotscript, garcon()->msout);
  sw.inhibit_logging();
  sw.order(plotoutmenu);
  *filestream << endl;
}

static CommandFn do_plot() {
  doplot();
}

// --------------------------------------------------------------- //

// Commmands in the plotoutmenu

static CommandFn plotenergy() {
  if(!filestream) return;
  *filestream << grid->energy() << " ";
}

static CommandFn plotavgdisp() {
  NodeGroup *n;
  if(!filestream) return;
  if(n = groupname()) {
    MeshCoord d(grid->avgdisp(n));
    *filestream << d.x << " " << d.y << " ";
  }
}

static CommandFn plotforce() {
  NodeGroup *n;
  if(!filestream) return;
  if(n = groupname()) {
    MeshCoord f(grid->force(n));
    *filestream << f.x << " " << f.y << " ";
  }
}

// --------------------------------------------------------------- //

// initialization goes here

Menu *plotmenu() {
  static Menu *pm = 0;
  if(pm) return pm;
  pm = new Menu("plot");
  pm->AddCommand(new PlotFileCmd);
  Menu *setmenu = pm->AddSubMenu("set", "add items to plot list");
  pm->AddCommand("show", "show functions being plotted", showplot);
  pm->AddCommand("clear", "clear the list of things being plotted", clearplot);
  pm->AddCommand("out", "actually plot a line", do_plot);
  pm->AddCommand("label", "print the plot list to the plot file", labelplot);
  pm->AddCommand("close", "close the plot file", closeplot);
    
  /* the "plot set" submenus must match the plotoutmenu exactly,
   * except that the "set" commands are all "addtoplot". The
   * plotoutmenu commands are the things that do the actual plotting.
   */
  setmenu->AddCommand("energy", "plot energy", addtoplot);
  setmenu->AddSubMenu("displacement",
		      "plot avg displacement",
		      new GroupMenu<Node>("displacement",
					  "plot average displacement of group",
					  addtoplot));
  setmenu->AddSubMenu("force",
		      "plot avg force",
		      new GroupMenu<Node>("force",
					  "plot average force on group",
					  addtoplot));

  // plotoutmenu is used w/in the plotscript to do the actual plotting
  plotoutmenu = new Menu("plotcmds");
  plotoutmenu->AddSubMenu("displacement", "",
			  new GroupMenu<Node>("displacement", "", plotavgdisp));
  plotoutmenu->AddSubMenu("force", "",
			  new GroupMenu<Node>("force", "", plotforce));
  plotoutmenu->AddCommand("energy", "", plotenergy);
  
#ifdef DEBUG
  pm->AddSubMenu("debug", "output commands usually only accessed automatically",
		 plotoutmenu);
#endif
  
  return pm;
}


