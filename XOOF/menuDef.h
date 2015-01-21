// -*- C++ -*-
// $RCSfile: menuDef.h,v $
// $Revision: 1.10 $
// $Author: langer $
// $Date: 2003-10-30 21:33:05 $

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


#ifndef MENUDEF_H
#define MENUDEF_H

typedef void CommandFn;
class Command0;
class Command;
class CommandM;
class CommandS;
class Menu;
class MenuOption;
class SubMenu;
class TextWaiter;
class Variable;
class Waiter;
#ifdef MENUGFX
class FormWaiter;
#endif

class LoopCommand;

#include <string.h>
#include <stdio.h>
#include <iostream.h>
#include <fstream.h>
#include "charstring.h"
#include "menuform.h"
#include "menuinterrupt.h"
#include "menustream.h"
#include "tocharstring.h"
#include "truefalse.h"
#include "varobjects.h"
#include "vec.h"
#include "word.h"
#ifdef DONT_HAVE_STRSTREAM_H
#include <strstream>
using namespace std;
#else
#include <strstream.h>
#endif

//---------------------------------------------//

// Attributes of menu items. A separate class is used, instead of a
// simple enum, so that the result of operator| can be the same class
// (instead of an int, which is the result of enum|enum).

class MenuAttribute {
private:
  int flag;
public:
  MenuAttribute(int f) : flag(f) {}
  int operator()(const MenuAttribute &q) const { return flag & q.flag; }
  friend MenuAttribute operator|(const MenuAttribute &a, const MenuAttribute&b)
  {
    return MenuAttribute(a.flag|b.flag);
  }
};

extern const MenuAttribute MOPT_NORMAL;
extern const MenuAttribute MOPT_NOLOG;  // doesn't appear in log files
extern const MenuAttribute MOPT_SECRET; // doesn't appear in menu listings
extern const MenuAttribute MOPT_NOPRINT;// isn't printed via Menu::print

//----------------------------------------------//

class Menu {
  friend class TextWaiter;
  friend class CommandM;
  friend class Variable;

private:
  CharString prompt;
  Vec<MenuOption*> option;
  void execute(const CharString &word, TextWaiter &w);
  int help(const CharString&, TextWaiter&) const;
public:
  Menu(const CharString &prmpt);
  virtual ~Menu();
  //Menu(const Menu&);
  Menu* AddSubMenu(const CharString &nm, const CharString &help,
		   const CharString &prmpt = "",
		   const MenuAttribute& = MOPT_NORMAL);
  Menu* AddSubMenu(const CharString &nm, const CharString &help,
		   Menu* submenu, const MenuAttribute& = MOPT_NORMAL);
  void AddCommand(const CharString &nm, const CharString &help,
		  CommandFn (*func)(), const MenuAttribute& = MOPT_NORMAL);
  void AddCommand(Command*, const MenuAttribute& = MOPT_NORMAL);
  void RemoveOption(const CharString &);
  void RemoveOption(const MenuOption &);
  bool RenameOption(const CharString&, const CharString&);
  void update_forms();

  void highlight(const CharString &); // highlight an entry
  void highlight_only(const CharString &); // ... and unhighlight others
  void unhighlight(const CharString &);    // remove highlight
  void unhighlight(void);		    // ... from all entries

  void print(FILE *file, const TrueFalse &recursive=TF_TRUE) const;
  void print(FILE *file, CharString, const TrueFalse &recursive=TF_TRUE) const;
  void print(ostream &os, const TrueFalse &recursive=TF_TRUE) const;
  void print(ostream &os, CharString, const TrueFalse &recursive=TF_TRUE) const;

  void AddOption(MenuOption* newopt);
  // would be private, but is used by template functions
#ifdef MENUGFX
  friend class FormWaiter;
  friend class MenuForm;
  friend class CommandMForm;
private:
  Vec<MenuForm*> form;
#endif // MENUGFX
};

class MenuOption {		    // Item in a menu (virtual)
  friend class Menu;
  friend class TextWaiter;
  friend class SubMenu;
  friend class Variable;
  friend class CommandM;
  friend class Command0;
  friend int getcnum(const CharString&, const Vec<MenuOption*>&);
  friend int getcnum(const CharString&, const Vec<Variable*>&);
public:
  CharString name;
  MenuAttribute attribute;  // should be protected, but a template needs access
  void rename(const CharString &aka) { name = aka; }
private:
  virtual void execute(TextWaiter&) = 0;
protected:
  char tag;             // [*=/] --- indicates type of option in listings
  CharString helpstring;
  int active;           // is it actually being executed right now?
  int drawactive;	// is it lit up in the menus?
  int highlighted;      // is it marked? (by Menu::highlight, etc)
  Menu* menu;           // menu containing this option
  CharString browserline(void) const;
  virtual int help(TextWaiter&) const;
  MenuOption(const CharString &nm, const CharString &hs, char t,
	     const MenuAttribute &att);
  MenuOption()
    : active(0),
      highlighted(0),
      drawactive(0),
      attribute(MOPT_NORMAL)
  {}
  virtual ~MenuOption() {}
  virtual void print(FILE *file, CharString path, const TrueFalse &recursive)
       const = 0;
  virtual void print(ostream &os, CharString path, const TrueFalse &recursive)
       const = 0;
#ifdef MENUGFX
  friend class Form;
  friend class MenuForm;
  friend class CommandForm;
  friend class Command0Form;
  friend class CommandMForm;
private:
  virtual void AddToForm(MenuForm*) = 0;
#endif // MENUGFX
};


class Command : public MenuOption {
  friend class Waiter;
  friend class Menu;
protected:
#ifndef MENUGFX
  Command(const CharString &nm, const CharString &hs,
	  const MenuAttribute &att = MOPT_NORMAL)
    : MenuOption(nm, hs, '*', att)
  {}
#else // MENUGFX
  friend class MenuForm;
  friend class CommandForm;
  friend class CommandSForm;
  friend class CommandZeroForm;
  Command(const CharString &nm, const CharString &hs,
	  const MenuAttribute &att = MOPT_NORMAL)
    : MenuOption(nm, hs, '*', att),
      form(0)
  {}
  virtual void AddToForm(MenuForm*);
  virtual CommandForm *makecmdform(const WordList&) = 0;
  virtual void print(FILE*, CharString, const TrueFalse&) const {}
  virtual void print(ostream &, CharString, const TrueFalse&) const {}
  CommandForm *form;
#endif // MENUGFX
  /* execute is a virtual function that handles all kinds of
   * menu options for TextWaiters. pronto performs commands only, 
   * but is independent of the type of Waiter.
   */
  virtual void execute(TextWaiter&) = 0;
  virtual void pronto(Waiter &, const WordList&) = 0;
public:
  virtual ~Command();
};

class CommandM : public Command {   // command w/ multiple args in a Menu
  friend class Waiter;
  friend class TextWaiter;
  friend class ScriptWaiter;
  friend class Menu;
private:
  void koko(TextWaiter&, int);    // the lord high executioner
  CommandM *duplicate() const;	// make a copy of this command.
  Vec<CommandM*> *clonelist;
  int ismaster;
protected:
  virtual CommandFn func() = 0;
  virtual void execute(TextWaiter &);
  virtual void pronto(Waiter &w, const WordList&);
  virtual CommandM *clone() const = 0;// make a copy of derived parts

  CommandM(const CharString &nm, const CharString &hs,
	   const MenuAttribute &att = MOPT_NORMAL)
    : Command(nm, hs, att),
      arg(0),
      ismaster(1), 
      clonelist(new Vec<CommandM*>)
  {}
  CommandM(const CommandM &cmd);

public:
  virtual ~CommandM();
	
  // argstring() returns a string with the args of the cmd.
  CharString argstring() const;

  // this is public only because it's needed in the AddArgument templates
  Vec<Variable*> arg;

#ifdef MENUGFX
  friend class CommandMForm;
  friend class CommandForm;

  virtual CommandForm *makecmdform(const WordList&);
#endif // MENUGFX
};


class Command0 : public Command {	// command w/out args
  friend class Waiter;
  friend class Menu;
private:
  CommandFn (*func)();
  virtual void execute(TextWaiter &);
  virtual void pronto(Waiter &w, const WordList&);

  Command0(const CharString &nm, const CharString &hs, CommandFn (*fn)(),
	   const MenuAttribute &att)
    : Command(nm, hs, att), func(fn)
  {}
public:
  virtual ~Command0() {}
#ifdef MENUGFX
  friend class CommandForm;
  friend class Command0Form;
private:
  virtual CommandForm *makecmdform(const WordList&);
#endif // MENUGFX
};

class CommandS : public Command {   // command that executes a string
private:
  CharString script;
  Menu *menu;
  virtual void pronto(Waiter&, const WordList&);
  virtual void execute(TextWaiter&);
  virtual int help(TextWaiter&) const;
public:
  CommandS(const CharString &nm, CharString &scr, Menu *m)
    : script(scr), menu(m), Command(nm, "execute macro " + nm) {}
  virtual ~CommandS() {}
#ifdef MENUGFX
  friend class CommandSForm;
private:
  virtual CommandForm *makecmdform(const WordList&);
#endif // MENUGFX
};

class SubMenu : public MenuOption {
  friend class Menu;
  friend class Waiter;
private:
  Menu* menu;
  virtual void execute(TextWaiter& w);
  virtual int help(TextWaiter&) const;
#ifndef MENUGFX
  SubMenu(const CharString &nm, const CharString &hs,
	  const MenuAttribute &att = MOPT_NORMAL)
    : MenuOption(nm, hs, '/', att),
      menu(0)
  {}
#else	// MENUGFX
  friend class MenuForm;
private:
  SubMenu(const CharString &nm, const CharString &hs,
	  const MenuAttribute &att = MOPT_NORMAL)
    : MenuOption(nm, hs, '/', att),
      menu(0),
      form(0)
  {}
  virtual void AddToForm(MenuForm*);
  virtual void print(FILE *file, CharString path, const TrueFalse &recursive)
       const;
  virtual void print(ostream &os, CharString path, const TrueFalse &recursive)
       const;
  MenuForm *form;  // form representing the submenu
#endif // MENUGFX
};

// Interactively settable variables in a menu.

class Variable : public MenuOption {
public:
  void *address;	// would be private if templates could be friends
  virtual CharString tostring() const = 0;
protected:
  void (*hook)(const CharString&); // fn called when variable is set

  Variable(const CharString &nm, const CharString &hs, void *x, 
	   void (*f)(const CharString&)=0,
	   const MenuAttribute& = MOPT_NORMAL);
  Variable();
  virtual ~Variable();
	
  void *oldvalue;

  virtual void execute(TextWaiter&);
  CharString printstr() const;
  virtual int fromstring(const CharString&, Waiter&) const = 0;
  virtual void fromstring(TextWaiter&) const = 0;
  virtual void copyvalue(const Variable*) = 0;
  virtual void savevalue() = 0;
  virtual void restorevalue() = 0;
  void update_forms();
  virtual int help(TextWaiter&) const;
  virtual void print(FILE *file, CharString path,
		     const TrueFalse &recursive) const;
  virtual void print(ostream &os, CharString path,
		     const TrueFalse &recursive) const;
  friend class TextWaiter;
  friend class CommandM;
  friend class VariableRegistry;
#ifdef MENUGFX
  friend class Form;
  friend class MenuForm;
  friend class CommandMForm;
protected:
  virtual void AddToForm(MenuForm*);
  virtual VarObject* create_varobject(Variable*, Form*, VarObjectType,
				      int, int, int, int)=0;
  VarObject *varobj;
#endif // MENUGFX
};

// When a variable is added to a Menu with AddVariable, a new Variable
// is created. Each Variable is in only one menu, but each variable
// (small v) can be in more than one. Changes in one Variable should
// be reflected in the others. This is only a problem in the graphics
// interface, when both Variables may be on the screen at the same
// time.  The VariableRegistry keeps track of Variables corresponding
// to the same variable, and allows functions to be applied to all of
// them together.


class VariableRegistry {
private:
  VariableRegistry() {}
  ~VariableRegistry();
  Vec<Vec<Variable*>*> varlist;
  // add a new Variable to the registry
  void add_variable(Variable *);
public:
  // The constructor is private, but this function is used to allocate
  // a single instance of the class:
  static VariableRegistry &vr();

  // Call a function on each Variable corresponding to the given
  // Variable. Don't call it for the specified Variable, since the
  // calling function can do that easily if it wants to.
  void call_function(const Variable *, void (Variable::*)()) const;
  friend class Variable;
};

template <class TYPE>
class Var : public Variable {
public:
  Var(const CharString &nm, const CharString &hs, TYPE &x,
      void (*f)(const CharString&));
  Var(const CharString &nm, const CharString &hs, TYPE &x,
      void (*f)(const CharString&), const MenuAttribute&);
  Var(const CharString &nm, const CharString &hs, TYPE &x);
  Var(const CharString &nm, const CharString &hs, TYPE &x,
      const MenuAttribute&);
  Var(const CharString &nm, TYPE &x); // for arguments
  Var(const CharString &nm, TYPE &x, const MenuAttribute&); // for arguments
  Var() {};   // so that dummy instances can be made in kludge.C
  virtual ~Var();
  virtual CharString tostring() const;
protected:
  virtual int fromstring(const CharString &str, Waiter&) const;   	
  virtual void fromstring(TextWaiter&) const;   	
  virtual void copyvalue(const Variable*);
  virtual void savevalue();
  virtual void restorevalue();
  friend class TextWaiter;
  friend class CommandM;
#ifdef MENUGFX
protected:
  virtual VarObject *create_varobject(Variable*, Form*, VarObjectType,
				      int, int, int, int);
  friend class Form;
  friend class MenuForm;
  friend class CommandMForm;
#endif // MENUGFX
};

// declare specializations
template <> int Var<double>::fromstring(const CharString&, Waiter&) const;
template <> void Var<double>::fromstring(TextWaiter&) const;
template <> int Var<float>::fromstring(const CharString&, Waiter&) const;
template <> void Var<float>::fromstring(TextWaiter&) const;
template <> int Var<int>::fromstring(const CharString&, Waiter&) const;
template <> void Var<int>::fromstring(TextWaiter&) const;
template <> int Var<CharString>::fromstring(const CharString&, Waiter&) const;
template <> void Var<CharString>::fromstring(TextWaiter&) const;
template <> CharString Var<CharString>::tostring() const;
template <> VarObject*
   Var<TrueFalse>::create_varobject(Variable*, Form*,
                                    VarObjectType, int, int, int, int);
template <> CharString Var<char>::tostring() const;

template <class TYPE, class NTYPE, class HTYPE>
void AddVariable(Menu *m, NTYPE nm, HTYPE help, 
		 TYPE &x, void (*f)(const CharString&)); 
template <class TYPE, class NTYPE, class HTYPE>
void AddVariable(Menu *m, NTYPE nm, HTYPE help, TYPE &x); 
template <class TYPE, class NTYPE, class HTYPE>
void AddVariable(Menu *m, NTYPE nm, HTYPE help, 
		 TYPE &x, void (*f)(const CharString&),
		 const MenuAttribute &att); 
template <class TYPE, class NTYPE, class HTYPE>
void AddVariable(Menu *m, NTYPE nm, HTYPE help, TYPE &x,
		 const MenuAttribute &att); 

template <class TYPE, class NTYPE>
void AddArgument(CommandM *cmd, NTYPE nm, TYPE &x);

template <class TYPE, class NTYPE>
void AddArgument(CommandM *cmd, NTYPE nm, TYPE &x, const MenuAttribute &att);

// -------------------------------------------------------------- //

/* Waiter classes.
 * Pointers to the base waiter class are passed to the Commands. Otherwise, 
 * there's no reason to have the TextWaiter and FormWaiter classed be
 * derived from a common base class.
 */

const int wbufsiz = 1024;

class Waiter {
  friend class Menu;
  friend class Command;
  friend class Command0;
  friend class CommandS;
  friend class CommandM;
  friend class Variable;
  friend class MenuOption;
  friend class VarToggle;
  friend Waiter *garcon();
protected:
  CharString makeprompt(const char *); // constructs prompt from promptfmt
  CharString promptfmt;	// format for prompts
  void saywhat(const char*);	// bad input, error message
  void makecurrent();	// label this one the current waiter
  WordList path;        // path to currently executing cmd, set by pronto
  CharString args;      // set by commands so that user fns can unparse args
  CharString errhm;     // sort of clearing the throat to get attention

  Menu *rootmenu;		// the menu for the last order()

  int (*quitcmd)();  // Command to call when quitting
	
  // stuff for defining macros
  int macromode;
  Menu *macromenu;
  CharString macrostring;
  WordList macropath;
  int macrocount;
  void startmacro(Menu *m, const WordList &pth) {
    macromenu = m;
    macromode = 1;
    macrostring = "";
    macropath = pth;
  }

  void record(const CharString &s) { macrostring += s; }
  void recorddelim();
  void endmacro();
  void definemacro(const CharString &); // put it in the menu
  void trashmacro() { macromode = 0; macrostring = ""; }
  virtual CharString getmacroname() = 0;
  CharString defaultmacroname();

  TrueFalse logging_;
  int inhibit_logging_;   // to temporarily disable logging
  ofstream *logfile;
  static TrueFalse logdirty_;
  static Vec<CharString*> logbook;
public:
  Waiter(MenuStream&);
  Waiter(MenuStream*);
  virtual ~Waiter();
  virtual void order(Menu*) = 0;	    // take orders
  void setprompt(const char *);	    // set format for prompts
  CharString get_command(int=0) const;  // get part of command path
  int get_depth() const;                // get depth of command path
  CharString get_path() const { return path.concat(); }
  CharString get_path(int n, int m) const {// path skipping n at start, m at end
    return path.concat(n, m);
  }
  const CharString &get_args() const { return args; }
  MenuStream &msout;		// output stream
  TrueFalse echo;		// echo inputs?

 // allows functions to read data embedded in scripts, but may return
 // 0 for Waiters for which this doesn't make sense
  virtual istream *inputstream() { return 0; }

  virtual void quiet() { echo = 0;}
  virtual void talkative() { echo = 1;}
 // read text input from a file
  void runfile(FILE*, Menu*, int, bool (*stp)()=0);
  void runfile(istream&, Menu*, int, bool (*stp)()=0);
  // ask the user for a string
  virtual CharString getstring(const CharString&, const CharString&) = 0;
  // ask a yes/no question
  virtual int question(const CharString&, int) = 0;
  // wait a bit
  static CommandFn pause();

  void loadmacros(FILE*);
  void showmacros(FILE*);
  void showmacros(ostream&);
  TrueFalse logging() const { return logging_; }      // log 
  void startlog(const CharString&, const TrueFalse &append=TF_FALSE);
  void stoplog();
  void log(const CharString &);
  void inhibit_logging() { ++inhibit_logging_; }
  void disinhibit_logging() { --inhibit_logging_; }
  void printlog(ostream&) const;
  void readlog(istream&);
  TrueFalse logdirty() const { return logdirty_; }

  // direct output to a file as well as to the screen
  // "mode" is the second argument to fopen()
  int tee(const CharString &file, const char *mode) {
    return msout.tee(file, mode);
  }
  void teestop() { msout.teestop(); } // stop doing so
  // choose which kinds of output get teed
  void tee_normal(int on) { msout.tee_normal(on); }
  void tee_errors(int on) { msout.tee_error(on); }
  void tee_prompt(int on) { msout.tee_prompt(on); }
  void tee_inform(int on) { msout.tee_info(on); }

  // set fn to be called when quitting. returns 1 if really should quit.
  void quitcommand(int (*f)()) { quitcmd = f; }

  int graphics();	// is graphics being used?
  // (ie has a FormWaiter been invoked?)
#ifdef MENUGFX
  friend class MenuForm;
  friend class CommandMForm;
  friend class Command0Form;
public:
  // stuff that's only meaningful for FormWaiter, but is defined as no-op
  // for all waiters for convenience
  virtual void graphics_off() {}	// suppress graphics output
  virtual void graphics_on() {}	// restore graphics output
  virtual void set_cursor(int) {} 
#endif // MENUGFX
};

class TextWaiter : public Waiter {	    // waiters handle menus
  friend class Menu;
  friend class CommandM;
  friend class Command0;
  friend class CommandS;
  friend class Variable;
  friend class SubMenu;
  friend class MenuOption;
  friend class Waiter;
protected:
  void twstartup();	// used by constructors
  FILE *infile;		// typed input
  CharString buffer;	// current input line
  int repeatword;		// repeat last word read?
  int grouplevel;		// parenthesis depth
  int eoffound;		// was end-of-file encountered?
  TrueFalse showprompts;	// print prompts?
  CommandFn (*eolfn)();	// function called at end-of-line;
  // methods called by Menu...
  void order1(Menu*);	// take a single order
  CharString word(void);			// get next word of input
  // prompt for next word of input
  CharString inputword(const CharString &prompt);
  // prompt for next line of input
  virtual CharString getline(const CharString &prompt);
  CharString argword(void);		// get word from arg list
  void listoptions(const Menu*);		// print choices
  void trashline();			// discard rest of input
  int help(Menu* menu);
  char *(*inputhandler)(const CharString&, MenuStream&, FILE*);
  int getvar(const Variable*);	// ask for variable value
  void echovar(const Variable*);	// print variable value and path
  int setargs(Vec<Variable*> &);	// set args for commands
  virtual void do_cmdm(CommandM*);
  virtual CharString getmacroname();
  WordList cmdlist;  // for keeping track of calling sequence
  Menu *currentmenu;

  bool (*stop)();	// returns 1 when it's time to stop taking orders

  static char *defaultinputhandler(const CharString&, MenuStream&, FILE*);

  // This is really ugly. The TextWaiter has to be able to return an
  // istream for the inputstream() method, but it only has a FILE*. So
  // if it needs to it creates an istream and stores it here. The
  // ugliness is that the StreamWaiter class is derived from
  // TextWaiter, but the StreamWaiter doesn't need to construct an
  // istream; it has one already.  StreamWaiter should *not* be
  // derived from TextWaiter; it should *be* TextWaiter, and we
  // shouldn't use FILEs at all!
  istream *inputstrm_;

public:
  TextWaiter(bool (*stop)()=0);
  TextWaiter(FILE* ifile, FILE *pfile, bool (*stop)()=0);
  TextWaiter(char *(*ih)(const CharString&, MenuStream&, FILE*),
	     bool (*stop)()=0);
  TextWaiter(FILE* ifile, FILE *pfile,
	     char *(*ih)(const CharString&, MenuStream&, FILE*),
	     bool (*stop)()=0);
  TextWaiter(MenuStream &ms, FILE* ifile = stdin, 
	     char *(*ih)(const CharString&,
			 MenuStream&,FILE*)=defaultinputhandler,
	     bool (*stop)()=0);
  TextWaiter(MenuStream &ms, FILE* ifile, bool (*stop)());
  virtual ~TextWaiter();
  virtual istream *inputstream();
  virtual void order(Menu*);	// perform an action from menu
  virtual void quiet() { echo = 0; showprompts = 0;}
  virtual void talkative() { echo = 1; showprompts = 1;}
  virtual int question(const CharString &, int);
  void set_eolfn(CommandFn (*)());    // set end-of-line function
  // check for end-of-file on input
  int eof() {return eoffound;}
  virtual CharString getstring(const CharString&, const CharString&);
  /* inbuf would be protected if templates could be friends */
  istrstream *inbuf;	// for reading current input line
};

class ScriptWaiter : public TextWaiter {
private:
  CharString script;
  int already_read;
  virtual CharString getline(const CharString&);
  virtual void do_cmdm(CommandM*);
public:
  ScriptWaiter(const CharString&, MenuStream&, bool (*stop)()=0);
  virtual void talkative() { echo = 1; showprompts = 0;}
};

class StreamWaiter : public TextWaiter {
  // use an existing istream for input, without any inputhandler for now.
private:
  virtual CharString getline(const CharString&);
  istream &inputstream_;
public:
  StreamWaiter(MenuStream &output, istream &input,
	       bool (*stop)()=0);
  virtual istream *inputstream() { return &inputstream_; }
};

#ifdef MENUGFX
class FormWaiter : public Waiter {
  friend class Menu;
  friend class Command;
  friend class Variable;
  friend class SubMenu;
  friend class MenuOption;
  friend class VarToggle;
private:
  virtual CharString getmacroname();
  MenuForm *rootform;
public:
  FormWaiter(int*, char*[], XrmOptionDescList = 0, int = 0);
  ~FormWaiter() {}
	
  virtual void order(Menu*);
	
  virtual void graphics_off();
  virtual void graphics_on();
  virtual void set_cursor(int);

  void show_homeform() const;

  virtual CharString getstring(const CharString&, const CharString&);
  virtual int question(const CharString &, int);
};
#endif // MENUGFX

extern Waiter *garcon();    // pointer to current Waiter

class LoopCommand : public CommandM {
private:
  CharString script, beginscript, endscript;
  int iterations;
  Menu *loopmenu;
  int fixscripts;	// are scripts not to appear in argument list?
public:
  LoopCommand(const CharString&, const CharString&, Menu*, 
	      const CharString&, const CharString& = "", const CharString& = "",
	      int=1);
  virtual CommandFn func();
  virtual CommandM *clone() const;
  void setscript(const CharString &s) { script = s; }
};

class LoadMacroCmd : public CommandM {
private:
  CharString filename;
  CharString cmdname;
public:
  LoadMacroCmd(const CharString &cname);
  virtual CommandFn func();
  virtual CommandM *clone() const;
};

class SaveMacroCmd : public CommandM {
private:
  CharString filename;
  CharString cmdname;
public:
  SaveMacroCmd(const CharString &cname);
  virtual CommandFn func();
  virtual CommandM *clone() const;
};

// ------------------------------------------------------- //

#include "menuDef.C"

#endif /* MENUDEF_H */
