// -*- C++ -*-
// $RCSfile: system.C,v $
// $Revision: 1.2 $
// $Author: langer $
// $Date: 2000-09-06 20:11:49 $

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */

#include "menuDef.h"
#include "system.h"
#include "stdlib.h"
#include <unistd.h>

class ChangeDirCmd : public CommandM {
private:
  CharString dir;
public:
  ChangeDirCmd()
    : CommandM("changedir", "change directory"),
      dir("..")
  {
    AddArgument(this, "directory", dir);
  }
  virtual CommandFn func() {
    if(chdir(dir))
      garcon()->msout << ms_error << "Can't change directory to "
		      << dir << "!" << endl << ms_normal;
  }
  virtual CommandM *clone() const {
    ChangeDirCmd *cdc = new ChangeDirCmd;
    cdc->dir =  dir;
    return cdc;
  }
};

class ExecuteCmd : public CommandM {
private:
  CharString cmd;
public:
  ExecuteCmd()
    : CommandM("execute", "execute a shell command")
  {
    AddArgument(this, "command", cmd);
  }
  virtual CommandFn func() {
    if(cmd)
      system(cmd);
  }
  virtual CommandM *clone() const {
    ExecuteCmd *ec = new ExecuteCmd;
    ec->cmd = cmd;
    return ec;
  }
};


Menu *systemmenu() {
  static Menu *menu = 0;
  if(!menu) {
    menu = new Menu("system");
    menu->AddCommand(new ChangeDirCmd);
    menu->AddCommand(new ExecuteCmd);
  }
  return menu;
}
