// -*- C++ -*-
// $RCSfile: initfile.h,v $
// $Revision: 1.3 $
// $Author: langer $
// $Date: 2000-09-06 20:11:02 $



/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


class LoadGrid;

#ifndef INITFILE_H
#define INITFILE_H

#include "menuDef.h"
#include "filename.h"
#include <stdio.h>

void initfile(const CharString&);

// class that stores info about data files.
class InitInfo {
    private:
	InitInfo() {}	// don't actually create any instances
    public:
	static int inputversion;    // set when file is read
	static int outputversion;   // 
	static int elementcount;    // for early versions w/out element indices
	static void reset() {
	    elementcount = 0;
	}
};

// read in a file
class LoadGrid : public CommandM {
    private:
        FileName filename;
    public:
	LoadGrid() : CommandM("file", "load a grid from a file"),
	    filename("grid.goof")
	{
	    AddArgument(this, "filename", filename);
    	}
	virtual CommandFn func() { initfile(filename); }
	virtual CommandM *clone() const {
	    LoadGrid *lg = new LoadGrid;
	    lg->filename = filename;
	    return lg;
	}
};

#endif
