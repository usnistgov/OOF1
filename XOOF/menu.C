// -*- C++ -*-
// $RCSfile: menu.C,v $
// $Revision: 1.15 $
// $Author: langer $
// $Date: 2002-10-15 21:08:26 $

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */

// Detailed instructions are in README.


#include "charstring.h"
#include "fatalerror.h"
#ifdef NEED_FDSTREAM
#include "fdstream.hpp"
#endif
#include "menuDef.h"
#include "menucolor.h"
#include "menuform.h"
#include "specialchars.h"
#include "truefalse.h"
#include "word.h"
#include <string.h>
#include "stdlib.h"
#include "ctype.h"
#ifdef HAVE_MALLOC_H
#include <malloc.h>
#endif
#include <signal.h>
#include <stdio.h>
#include <iostream.h>


static Waiter *currentwaiter = 0; // Keep track of which waiter is in control. 
#ifdef MENUGFX
static int using_graphics = 0;	// set in FormWaiter constructor, never unset.
#endif // MENUGFX

void Waiter::makecurrent() {
  currentwaiter = this;
}

Waiter *garcon() {
  Waiter *w = currentwaiter;
  if(!w)    // make a temporary Waiter
    w = new TextWaiter;
  return w;
}

int Waiter::graphics() {
#ifdef MENUGFX
  return using_graphics;
#else
  return 0;
#endif // MENUGFX
}

// ------------- TextWaiter Execution routines -------------------------//

void Menu::execute(const CharString &input, TextWaiter& w) {
    int cnum = getcnum(input, option);
    if(cnum < 0) {
      w.saywhat(w.cmdlist.concat()  + " " + input /*input*/);
	w.trashline();
	return;
    }
    w.cmdlist += option[cnum]->name ;
    w.currentmenu = this;
    option[cnum]->execute(w);
    w.cmdlist.strip(1);
    return;
}

void SubMenu::execute(TextWaiter& w) {
  w.order1(menu);
}

void Command0::execute(TextWaiter &w) {
  pronto(w, w.cmdlist);
}

void CommandS::execute(TextWaiter &w) {
  pronto(w, w.cmdlist);
}

/* CommandM's are executed by this sequence of functions, so that
 * TextWaiters and ScriptWaiters behave slightly differerently.
 * Waiter::do_cmdm calls CommandM::koko on either the original
 * command (TW), or a copy (SW). koko sets arguments and then
 * calls pronto, which really calls the command. koko is distinct
 * from pronto because pronto is used elsewhere to run the command
 * without setting arguments.
 * When defining a macro, however, always use a copy of the command.
 */

void CommandM::execute(TextWaiter &w) {
    w.do_cmdm(this);
}

void TextWaiter::do_cmdm(CommandM *cmd) {
    cmd->koko(*this, macromode);
}

void ScriptWaiter::do_cmdm(CommandM *cmd) {
    cmd->koko(*this, 1);
}

void CommandM::koko(TextWaiter &w, int copy) {	// the lord high executioner
  if(!copy) {		    // execute command itself
    if(w.setargs(arg))
      pronto(w, w.cmdlist);
    else
      w.msout << ms_error << "=== Command \"" << name << "\" not executed."
	      << endl << ms_normal;
  }
  else {		    // make a copy and execute it
    CommandM *dup(duplicate());
    if(w.setargs(dup->arg))
      dup->pronto(w, w.cmdlist);
    else
      w.msout << ms_error << "=== Command \"" << name << "\" not executed."
	      << endl << ms_normal;
    delete dup;
  }
}

CommandM *CommandM::duplicate() const {
    CommandM *newcmd = clone();
    newcmd->ismaster = 0;
    newcmd->clonelist = clonelist;
    clonelist->grow(1, newcmd);
    return newcmd;
}


// -------------- Menu assembly routines -------------------------- //

const MenuAttribute MOPT_NORMAL(0);
const MenuAttribute MOPT_NOLOG(1);
const MenuAttribute MOPT_SECRET(2);
const MenuAttribute MOPT_NOPRINT(4);

Menu::Menu(const CharString &prmpt) : prompt(prmpt), option(0) {}

Menu::~Menu() {}

// Add an already constructed generic option to a menu
void Menu::AddOption(MenuOption* newopt) {
  int duplicated = 0;	// does this option duplicate a previous one?
  for(int i=0; i<option.capacity() && !duplicated; i++) {
    if(option[i]->name == newopt->name) {
      delete option[i];
      option[i] = newopt;
      duplicated = 1;
    }
  }
  if(!duplicated) option.grow(1, newopt);
  newopt->menu = this;
  update_forms();
}

// Construct an empty menu named "nm" and add it to the menu.
Menu* Menu::AddSubMenu(const CharString &nm, const CharString &help,
			const CharString &prmpt, const MenuAttribute &att)
{
    // create the option in this menu
    SubMenu* newoption = new SubMenu(nm, help, att);
    // create the submenu in the option. 
    Menu* newsubmenu = new Menu(prmpt == ""? nm : prmpt);
    newoption->menu = newsubmenu;
    AddOption(newoption);	    // store the option in this menu
    return newsubmenu;		    // return pointer to new submenu
}

// Add an existing menu to the menu as a submenu
Menu* Menu::AddSubMenu(const CharString &nm, const CharString &help,
			Menu *submenu, const MenuAttribute &att)
{
    SubMenu* newoption = new SubMenu(nm, help, att);
    newoption->menu = submenu;
    AddOption(newoption);
    return submenu;
}

MenuOption::MenuOption(const CharString &nm, const CharString &hs, char t,
		       const MenuAttribute &att)
  : name(nm),
    helpstring(hs),
    tag(t),
    active(0),
    highlighted(0),
    drawactive(0),
    menu(0),
    attribute(att)
{}

void Menu::AddCommand(const CharString &nm, const CharString &help,
			CommandFn (*func)(), const MenuAttribute &att)
{
    Command0 *newcommand = new Command0(nm, help, func, att);
    AddOption(newcommand);
}

// creates a CommandM derived command
void Menu::AddCommand(Command *cmd, const MenuAttribute &att) {
  cmd->attribute = cmd->attribute | att;
  AddOption(cmd);
}

void Menu::RemoveOption(const CharString &nm) {
    for(int i=0; i<option.capacity(); i++) {
	if(nm == option[i]->name) {
	    if(!option[i]->active)
		delete option[i];
	    // get rid of space in the option list
	    for(int j=i+1; j<option.capacity(); j++)
		option[j-1] = option[j];
	    option.grow(-1);
	    
	    update_forms(); // update menu forms
	    return;
	}
    }
    garcon()->msout << ms_error
		    << "=== Failed attempt to remove option " << nm
		    << " from Menu " << prompt << endl << ms_normal;
}

void Menu::RemoveOption(const MenuOption &opt) {
    RemoveOption(opt.name);
}

bool Menu::RenameOption(const CharString &oldname, const CharString &newname) {
  int which = -1;
  for(int i=0; i<option.capacity(); i++) {
    if(newname == option[i]->name)
      return 0;			// don't allow conflicting names
    if(oldname == option[i]->name)
      which = i;
  }
  if(which > -1) {
    option[which]->rename(newname);
    update_forms();
    return 1;
  }
  return 0;
}

// --------------------------------------------------------------- //

// Find entry in menu corresponding to input word
// this isn't a Menu method anymore, so that it can be used
// on any list of MenuOptions, particularly CommandM::arg.

// should be templated, but template functions can't be friends

int getcnum(const CharString &word, const Vec<MenuOption*> &option) {
    if(!word) return -1;		    // error
    int nmatches = 0;			    // number of items matching word
    int lastmatch;
    int i;
    // Look for matches up to the length of the input word.
    for(i=0; i<option.capacity(); i++) {
	if(!strncmp(word, option[i]->name, strlen(word))) {
	    lastmatch = i;
	    nmatches++;
	}
    }
    if(nmatches == 1) return lastmatch;	    // Just one match
    if(nmatches == 0) return -1;	    // No match
    
    // Resolve multiple matches by looking for exact one
    for(i=0; i<option.capacity(); i++) {
	if(!strcmp(word, option[i]->name)) return i;
    }
    return -1;				    // No exact match
}

int getcnum(const CharString &word, const Vec<Variable*> &option) {
    if(!word) return -1;		    // error
    int nmatches = 0;			    // number of items matching word
    int lastmatch;
    int i;
    // Look for matches up to the length of the input word.
    for(i=0; i<option.capacity(); i++) {
	if(!strncmp(word, option[i]->name, strlen(word))) {
	    lastmatch = i;
	    nmatches++;
	}
    }
    if(nmatches == 1) return lastmatch;	    // Just one match
    if(nmatches == 0) return -1;	    // No match
    
    // Resolve multiple matches by looking for exact one
    for(i=0; i<option.capacity(); i++) {
	if(!strcmp(word, option[i]->name)) return i;
    }
    return -1;				    // No exact match
}

// ----------------------------------------------------------------- //

void Menu::highlight(const CharString &what) {
    int cnum = getcnum(what, option);
    if(cnum < 0) return;
    option[cnum]->highlighted = 1;
    update_forms();
}

void Menu::highlight_only(const CharString &what) {
    for(int i=0; i<option.capacity(); i++)
	option[i]->highlighted = 0;
    int cnum = getcnum(what, option);
    if(cnum < 0) return;
    option[cnum]->highlighted = 1;
    update_forms();
}

void Menu::unhighlight() {
    for(int i=0; i<option.capacity(); i++)
	option[i]->highlighted = 0;
    update_forms();
}

void Menu::unhighlight(const CharString &what) {
    int cnum = getcnum(what, option);
    if(cnum < 0) return;
    option[cnum]->highlighted = 0;
    update_forms();
}

// ----------------------------------------------------------------- //

void Menu::update_forms() {
#ifdef MENUGFX
    for(int j=0; j<form.capacity(); j++)
	form[j]->update();
#endif // MENUGFX
}

// ----------------------------------------------------------------- //

// Routines to maintain stack of commands being executed

/* the Waiter path variable is set by pronto() just before calling a
 * CommandFn. It stores the complete path to the current command.
 */

#ifdef OLD
void TextWaiter::push_cmd(const char* name) {
    cmdlist.grow(1, strdup(name));
}

void TextWaiter::pop_cmd() {
    free(cmdlist[cmdlist.capacity()-1]);
    cmdlist.grow(-1);
}

void TextWaiter::pop_allcmds() {
    for(int i=0; i<cmdlist.capacity(); i++)
	free(cmdlist[i]);
    cmdlist.resize(0);
}

CharString TextWaiter::getpath(int d) const {
    // d is how far short of the end of the list to stop. default=0
    CharString thepath;
    for(int i=0; i<cmdlist.capacity()-d; i++)
	thepath = thepath.spaceappend(cmdlist[i]);
    return thepath;
}
#endif

// return command n-deep in heirarchy

CharString Waiter::get_command(int n) const {
  // return nth word from the end of the path
  int s = path.size();
  if(n >= s)
    return CharString("");
  else
    return path[s-n-1];
}

int Waiter::get_depth() const {	// count number of words in path
  return path.size();
}

// ------------------- Waiter routines ---------------------------- //

Waiter::Waiter(MenuStream &ms)
  : msout(ms),
    promptfmt("%s >> "),
    macrocount(0), macromode(0),
    quitcmd(0)
{
  makecurrent();
}

Waiter::Waiter(MenuStream *ms)
  : msout(*ms),
    promptfmt("%s >> "),
    macrocount(0), macromode(0),
    quitcmd(0)
{
  makecurrent();
}

Waiter::~Waiter() {
  if(currentwaiter == this)	// avoid dangling pointer
    currentwaiter = 0;
}

void TextWaiter::twstartup() {
    inbuf = 0;
    repeatword = 0;
    grouplevel = 0;
    showprompts = TF_TRUE;
    echo = TF_FALSE;
    logging_ = TF_FALSE;
    inhibit_logging_ = 0;
    eoffound = 0;
    eolfn = 0;
    inputstrm_ = 0;
}

TextWaiter::TextWaiter(bool (*stp)())
  : infile(stdin),
    inputhandler(defaultinputhandler), 
    Waiter(new TextMenuStream),
    stop(stp)
{
    twstartup();
}

TextWaiter::TextWaiter(FILE* ifile, FILE *pfile, bool (*stp)())
  : infile(ifile),
    inputhandler(defaultinputhandler), 
    Waiter(new FileMenuStream(pfile)),
    stop(stp)
{
  twstartup();
}

TextWaiter::TextWaiter(char *(*ih)(const CharString&, MenuStream&, FILE*),
		       bool (*stp)())
  : infile(stdin),
    inputhandler(ih),
    Waiter(new TextMenuStream),
    stop(stp)
{
  twstartup();
}

TextWaiter::TextWaiter(FILE* ifile, FILE *pfile,
		       char *(*ih)(const CharString&, MenuStream&, FILE*),
		       bool (*stp)())
  : infile(ifile),
    inputhandler(ih),
    Waiter(new FileMenuStream(pfile)),
    stop(stp)
{
  twstartup();
}

// this version uses an existing output message stream
TextWaiter::TextWaiter(MenuStream &ms, FILE* ifile,
		       char *(*ih)(const CharString&, MenuStream&, FILE*),
		       bool (*stp)())
  : infile(ifile),
    inputhandler(ih),
    Waiter(ms),
    stop(stp)
{
  twstartup();
}

TextWaiter::TextWaiter(MenuStream &ms, FILE* ifile, bool (*stp)())
  : infile(ifile),
    inputhandler(defaultinputhandler),
    Waiter(ms),
    stop(stp)
{
  twstartup();
}

TextWaiter::~TextWaiter() {
  if(inputstrm_)
    delete inputstrm_;
}

istream *TextWaiter::inputstream() {
  if(!inputstrm_) {
    fflush(infile);		// synchronizes FILE and istream
#ifdef NEED_FDSTREAM
    inputstrm_ = new boost::fdistream(fileno(infile));
#else
    inputstrm_ = new ifstream(fileno(infile));
#endif
  }
  return inputstrm_;
}

ScriptWaiter::ScriptWaiter(const CharString &s, MenuStream &ms, bool (*stp)())
  : script(s),
    TextWaiter(ms, stdin, defaultinputhandler, stp),
    already_read(0)
{
  showprompts = TF_FALSE; // overrides setting in TextWaiter constructor
}

StreamWaiter::StreamWaiter(MenuStream &ms, istream &is, bool (*stp)())
  : TextWaiter(ms, stdin, defaultinputhandler, stp),
    inputstream_(is)
{
}


#ifdef MENUGFX

// BadWindow errors can be generated by clicking on a window that's
// about to disappear. This catches and ignores those events.

static int (*old_error_handler)(Display*, XErrorEvent*);

static int new_error_handler(Display *disp, XErrorEvent *err) {
  if(err->error_code == BadWindow) {
    cerr << "X Error (BadWindow) ignored" << endl;
    return 0;
  }
  if(err->error_code == BadDrawable) {
    cerr << "X Error (BadDrawable) ignored" << endl;
    return 0;
  }
  (*old_error_handler)(disp, err);
  fatalerror();
  return 0;
}

FormWaiter::FormWaiter(int *argc, char *argv[], 
		       XrmOptionDescList options, int noptions)
  : Waiter(new FormMenuStream())
{
  echo = TF_TRUE;
  logging_ = TF_FALSE;
  inhibit_logging_ = 0;
  using_graphics = 1;
  fl_initialize(argc, argv, "XFormMenu", options, noptions);
  fl_set_app_nomainform(1);
  setmenucolors();

  // SIGALRM is used to handle double clicks. A handler is always
  // installed so that if the alarm goes off accidentally the program
  // won't crash.
  fl_add_signal_callback(SIGALRM, MenuForm::ignorealarm, 0);

  //old_error_handler = XSetErrorHandler(new_error_handler);
}

void FormWaiter::graphics_off() {
  Form::haltall();
}

void FormWaiter::graphics_on() {
  Form::resumeall();
}

void FormWaiter::set_cursor(int cursor) {
  Form::setcursor(cursor);
}

#endif // MENUGFX

/* construct prompt from promptfmt, replacing %s with the given string */

CharString Waiter::makeprompt(const char *str) {
    char *promptbuf = new char[strlen(promptfmt) + strlen(str) + 1];
    const char *f = promptfmt;
    char *p = promptbuf;
    while (*f) {
	if(*f == '%' && *(f+1) == 's') {    // check for "%s"
	    const char *s = str;
	    while(*s) *p++ = *s++;	    // replace "%s" in fmt with str
	    f += 2;
	}
	else
	    *p++ = *f++;		    // copy verbatim from fmt string
    }
    *p = '\0';
    return CharString(promptbuf);
}

void Waiter::setprompt(const char *fmt) {
    promptfmt = CharString(fmt);
}

/* Take orders and serve commands */

void TextWaiter::order(Menu *menu) {	// take orders until eof
    rootmenu = menu;
    while(!eof() && (!stop || !(*stop)()))
	order1(rootmenu);
}

void TextWaiter::order1(Menu *menu) {	// take a single order
  int startlevel = grouplevel;
  int repeat = 0;
  do {
    CharString input(inputword(menu->prompt));
    repeat = 0;
    if(eoffound || input == "") {	// end of file on input
      eoffound = 1;
      return;
    }
    switch (input[0]) {
    case BEGINGROUP:
      grouplevel++;
      break;
    case ENDGROUP:
      grouplevel--;
      if(grouplevel < 0) {
	fputs("Too many right parentheses!\n", stderr);
	grouplevel = 0;
      }
      break;
    case COMMENTCHAR:
      trashline();
      repeat = 1;
      break;
    case DELIMITER:
      repeat = 1;
      break;
    case ABORTCHAR:
      break;
    case BEGINMACRO:
      if(macromode) {
	fputs("Nested macro definitions are not allowed!\n", stderr);
	trashline();
      }
      else
	startmacro(menu, cmdlist);
      break;
    case ENDMACRO:
      if(!macromode) {
	fputs("Too many right braces!\n", stderr);
	trashline();
      }
      else
	endmacro();
      break;
    case HELPCHAR:
      if(input[1] == HELPCHAR)    // "??" --> long help
	help(menu);
      else			    // "?" --> short help
	listoptions(menu);
      repeat = 1;
      break;
    default:
      menu->execute(input, *this);
    }
  } while((repeat || grouplevel > startlevel) && !eoffound);
}

#ifdef MENUGFX
void FormWaiter::order(Menu *menu) {
  WordList pth;
  rootmenu = menu;
  errhm = makeprompt(menu->prompt);
  rootform = new MenuForm(menu, errhm, pth, "");
  rootform->makeroot(this);
  rootform->show(NULL);

  for(;;) fl_do_forms();
  // doesn't return from here!
}

void FormWaiter::show_homeform() const {
  rootform->show(NULL);
}
#endif // MENUGFX

void TextWaiter::set_eolfn(CommandFn (*f)()) {
    eolfn = f;
}

// ------------- Help routines -------------------------//

// help is like order, but it just prints the help string.
// it doesn't allow for parentheses or continue to the next line.
// help returns 1 if it finds something to provide help on, 0 otherwise.
// The actual help is provided by MenuOption::help() and its variants.

int TextWaiter::help(Menu* menu) {
    CharString input(word()); // get a word from this input line 
    if(input == "" || is_special(input[0])) return 0;
    return menu->help(input, *this);
}

// return 1 if help has been provided, 0 otherwise
// cf Menu::execute
int Menu::help(const CharString &input, TextWaiter& w) const {
    int cnum = getcnum(input, option);
    if(cnum < 0) {
	w.saywhat(input);
	w.trashline();
	return 1;
    }
    return option[cnum]->help(w);
}

int SubMenu::help(TextWaiter& w) const {
  if(!w.help(menu))
    w.msout << ms_info << name << ": " << helpstring << endl << ms_normal;
  return 1;
}

int Variable::help(TextWaiter &w) const {
  w.msout << ms_info << name << " = " << tostring() << ": " << helpstring
	  << endl << ms_normal;
  return 1;
}

int MenuOption::help(TextWaiter& w) const {
  w.msout << ms_info << name << ": " << helpstring << endl << ms_normal;
  return 1;
}

int CommandS::help(TextWaiter &w) const {
  w.msout << ms_info << name << ": macro {" << script << "}"
	  << endl << ms_normal;
  return 1;
}

// Print list of commands or error message

void Waiter::saywhat(const char* word) {
  msout << endl << ms_error;
  msout << "=== Command \"" << word << "\" not understood!" << endl;
  msout << "=== Use '" << HELPCHAR << "' to list possible commands." << endl;
  msout << ms_normal;
}

void TextWaiter::listoptions(const Menu* menu) {
  msout << ms_info
	<< "=== Type \"" << HELPCHAR << HELPCHAR
	<< " xxx\" to get help on option \"xxx\"." << endl;
  msout << "=== Choices are (command* menu/ variable=) :   " << endl;
  for(int i=0; i<menu->option.capacity(); i++) {
    if(!menu->option[i]->attribute(MOPT_SECRET)) {
      if(menu->option[i]->highlighted) msout << "<<";
      msout << menu->option[i]->name << menu->option[i]->tag;
      if(menu->option[i]->highlighted) msout << ">>";
      msout << " ";
    }
  }
  msout << endl << ms_normal;
}

// -------------- Print routines ----------------------------- //

void Menu::print(FILE *file, const TrueFalse &recursive) const {
  print(file, CharString(""), recursive);
}

void Menu::print(ostream &os, const TrueFalse &recursive) const {
  print(os, CharString(""), recursive);
}

void Menu::print(FILE *file, CharString path, const TrueFalse &recursive) const
{
  for(int i=0; i<option.capacity(); i++)
    option[i]->print(file, path, recursive);
}

void Menu::print(ostream &os, CharString path, const TrueFalse &recursive) const
{
  for(int i=0; i<option.capacity(); i++)
    option[i]->print(os, path, recursive);
}

void SubMenu::print(FILE *file, CharString path,
		    const TrueFalse &recursive) const
{
  if(!recursive || attribute(MOPT_NOPRINT)) return;
  menu->print(file, path.spaceappend(name), recursive);
}

void SubMenu::print(ostream &os, CharString path,
		    const TrueFalse &recursive) const
{
  if(!recursive || attribute(MOPT_NOPRINT)) return;
  menu->print(os, path.spaceappend(name), recursive);
}

void Variable::print(FILE *file, CharString path, const TrueFalse&) const
{
  if(attribute(MOPT_NOPRINT)) return;
  fprintf(file, "%s %s\n", path.charstar(), printstr().charstar());
}

void Variable::print(ostream &os, CharString path, const TrueFalse&) const
{
  if(attribute(MOPT_NOPRINT)) return;
  os << path << " " << printstr() << endl;
}

// -------------- Waiter Input routines ---------------------- //

// Read new non-blank line into buffer.

CharString TextWaiter::getline(const CharString &menuprompt) {
    char *ptr;
    CharString copy;
    CharString prompt(showprompts ? makeprompt(menuprompt) : CharString(""));
    do {
	ptr = (*inputhandler)(prompt, msout, infile);
	if(ptr) {
	    copy = CharString(skipspace(ptr));
	    free(ptr);	// inputhandler allocates space
	}
    } while(!(eoffound = feof(infile)) && ptr &&
			(copy[0] == '\0' || copy[0] == '\n'));
    if(eoffound) return CharString("");
    return copy;
}

CharString ScriptWaiter::getline(const CharString &) {
    if(already_read) return 0;	// read only once!
    already_read = 1;
    return script;
}

CharString StreamWaiter::getline(const CharString &menuprompt) {
  CharString prompt(showprompts? makeprompt(menuprompt) : CharString(""));
  static int bufsiz = 1024;
  CharString input(bufsiz);
  do {
    inputstream_.getline(input.charstar(), bufsiz, '\n');
    //inputstream >> input;
  } while(!(eoffound = (!inputstream_ || inputstream_.eof())) &&
	  (input[0]=='\0' || input[0]=='\n'));
  if(eoffound) return CharString("");
  return input;
}

char *TextWaiter::defaultinputhandler(const CharString &prompt,
				      MenuStream &pstrm,
				      FILE *ifile)
{
    char line[1024];
    pstrm << ms_prompt << prompt << flush << ms_normal;
    if(fgets(line, sizeof(line), ifile))
	return strdup(line);
    else
	return 0;
}

// Return one word from buffer, without prompting

CharString TextWaiter::word() {
    static Word lastword;
    if(repeatword) {
	repeatword = 0;
	return lastword;
    }
    if(!inbuf || !*inbuf) return CharString("");
    (*inbuf) >> lastword;
    return lastword;
}

// Return one word from buffer, prompting for input

CharString TextWaiter::inputword(const CharString &menuprompt) {
    CharString wrd = word();
    if(!inbuf || !*inbuf || wrd == "") {    // need more input
	if(inbuf && eolfn) {	// Call end-of-line function, but not
	    makecurrent();	// before the first line has been read. If
	    (*eolfn)();		// it's been read, then inbuf != 0.
	}
	buffer = getline(menuprompt);  // Get another line of input
	if(buffer == "") return buffer;	// signals eof
	if(inbuf) delete inbuf;
	inbuf = new istrstream(buffer, strlen(buffer));
	return word();
    }
    return wrd;
}
    
    
// Kill rest of input line.

void TextWaiter::trashline() {
    inbuf->clear(ios::badbit|inbuf->rdstate());
}

// ----------------- Routines to define macros ---------------//

class MacroList {
public:
  Vec<CharString*> defn;
  Vec<CharString*> name;
  Vec<WordList*> path;
  void add(const CharString &n, const WordList &p, const CharString &d) {
    defn.grow(1, new CharString(d));
    name.grow(1, new CharString(n));
    path.grow(1, new WordList(p));
  }
  void remove(const CharString &n) {
    for(int i=0; i<name.capacity(); i++)
      if(n == *name[i]) {
	delete name[i];
	delete path[i];
	delete defn[i];
	name.remove(name[i]);   // inefficient
	defn.remove(defn[i]);
	path.remove(path[i]);
	break;
      }
  }
  ~MacroList() {
    for(int i=0; i<name.capacity(); i++) {
      delete name[i];
      delete path[i];
      delete defn[i];
    }
  }
};

static MacroList macrolist;


void Waiter::endmacro() {
  macromode = 0;
  definemacro(getmacroname());
}

CharString TextWaiter::getmacroname() {
  CharString arg(argword());
  if(arg != "") {
    return arg;
  }
  else {  // nothing provided, create a dummy string Variable and ask for name
    // construct a default name
    CharString name(defaultmacroname());
    Var<CharString> namevar("name", "macro name", name);
    if(getvar(&namevar))
      return name;
    else
      return CharString("");
  }
}

void Waiter::recorddelim() {
  // not in menuDef.h just because DELIMITER isn't defined there
  macrostring += DELIMITER;
}

#ifdef MENUGFX
CharString FormWaiter::getmacroname() {
  return getstring("Enter macro name", defaultmacroname());
}
#endif // MENUGFX

CharString Waiter::defaultmacroname() {
  char namebuf[20];
  sprintf(namebuf, "macro%d", macrocount);
  return CharString(namebuf);
}

static CharString macrodefstr(const WordList &path,
			      const CharString &defn, const CharString &name) {
  return path.concat() + " {" + defn + "} " + name;
}

void Waiter::definemacro(const CharString &macroname) {
  if(macroname == "") return;
  if(is_special(macroname[0])) {
    msout << ms_error << "=== Macro names can't begin with '" << macroname[0]
	  << "'" << endl << ms_normal;
    return;
  }
  msout << ms_info << "defining macro \"" << macroname << "\""
	<< endl << ms_normal;
  if(macrostring == "") {
    macromenu->RemoveOption(macroname);
    macrolist.remove(macroname);
  }
  else {
    macrocount++;
    macromenu->AddCommand(new CommandS(macroname, macrostring, rootmenu));
    macrolist.add(macroname, macropath, macrostring);
    log(macrodefstr(macropath, macrostring, macroname));
  }
}

void Waiter::showmacros(ostream &os) {
  for(int i=0; i<macrolist.name.capacity(); i++)
    os << macrodefstr(*macrolist.path[i],
		      *macrolist.defn[i],
		      *macrolist.name[i])
			<< endl;
}

void Waiter::showmacros(FILE *f) {
  for(int i=0; i<macrolist.name.capacity(); i++)
    fprintf(f, "%s\n", macrodefstr(*macrolist.path[i],
				   *macrolist.defn[i],
				   *macrolist.name[i]).charstar());
}

void Waiter::loadmacros(FILE *f) {
  runfile(f, rootmenu, 0);
}


// ----------------- Routines to get arguments ---------------//

int TextWaiter::setargs(Vec<Variable*> &arglist) {
  int i;
  CharString argwd;
  int nset = 0;   // number of args set so far
  Vec<int> isset(arglist.capacity(), 0);
    
  // read arguments
  ignorechar(COMMA);
  while(nset < arglist.capacity() && (argwd = argword()) && argwd != "") {
    int which = getcnum(argwd, arglist);	    // look up argument name
    if(which >= 0) {			    // found arg name
      Variable *thearg = arglist[which];
      argwd = argword();		// get equals sign
      if(argwd[0] != EQUALS) {
	msout << ms_error
	      << "=== Badly formed argument list! Missing '='."
	      << endl << ms_normal;
	trashline();
      }
      else {  // argwd[0] == EQUALS
	thearg->fromstring(*this);
	if(*inbuf) {	// was read correctly
	  if(!isset[which]) {
	    isset[which] = 1;
	    nset ++;
	  }
	}
	else {
	  msout << endl << ms_error << "=== Argument \"" << thearg->name
		<< "\" is badly formed." << endl << ms_normal;
	  return 0;
	}
      }
    }
    else {	// not a recognizable argument name
      msout << endl << ms_error << "=== \"" << argwd
	    << "\" is not a recognizable argument!" << endl << ms_normal;
      trashline();
      return 0;
    }
  }
  noticechar(COMMA);

  // ask for unset arguments
  for(i=0; i<arglist.capacity(); i++) {
    if(!isset[i] && !arglist[i]->attribute(MOPT_SECRET))
      if(!getvar(arglist[i]))
	return 0;
  }
  return 1;    
}

/* argword returns one command line argument in an allocated buffer
 * which must be freed.
 */

CharString TextWaiter::argword() {
  CharString str(word());	    // gets one word or one special character
  if(str == "")		    // no input, no more args
    return 0;
  
  if(is_special(str[0])) {

    if(isignored(str[0]))
      return argword();
    
    if(str[0] == BEGINGROUP) {  // is this right?
      grouplevel++;
      return 0;
    }
    
    if(str[0] == ENDGROUP) {
      grouplevel--;
      return 0;
    }
    
    if(str[0] == ENDMACRO) {
      repeatword = 1;  // closing brace is processed by order1() next time...
      return 0;
    }
    
    if(str[0] == DELIMITER)
      return 0;
    
    if(str[0] == COMMENTCHAR) {
      trashline();
      return 0;
    }
    
    if(isterminal(str[0]))	    // no more args
      return 0;
    
    // return special character
    CharString spl(2);
    spl[0] = str[0];
    spl[1] = '\0';
    return spl;
  }
  return str;
}

// ------------------------------------------------------ //

// prompt for and return a string independent of the command stream.

CharString TextWaiter::getstring(const CharString &prompt,
				    const CharString &dflt) {
  msout << ms_prompt << prompt << " [" << dflt << "]: " << flush
	<< ms_normal;
  char str[200];
  fgets(str, sizeof(str), stdin);
  // get rid of trailing newline
  char *ptr = strchr(str, '\n');
  if(ptr) *ptr = '\0';
  // skip leading spaces
  ptr = str;
  skipspace(&ptr);
  if(*ptr == '\0') return dflt;
  return CharString(str);
}

// return 1 for yes, 0 for no
int TextWaiter::question(const CharString &qstr, int dflt) {
  CharString answer;
  for(;;) {
    msout << ms_prompt << qstr << " [" << (dflt?"yn":"ny") << "] "
	  << flush << ms_normal;
    cin >> answer;
    if(answer[0] == 'y')  return 1;
    if(answer[0] == 'n')  return 0;
    if(answer[0] == '\0') return dflt;
  }
}

CommandFn Waiter::pause() {	// static so it can be put in menus
#ifdef MENUGFX
  if(garcon()->graphics()) {
     PauseForm::pause();
     return;
  }
#endif
  CharString dummy;
  garcon()->msout << ms_prompt << "Hit return to continue:" << flush
		  << ms_normal;
  cin >> dummy;
}


// ------------------ Variables -----------------------------//

// executing a variable means either printing its value (if no arg is
// provided), or assigning a value (if an arg is provided).\

// print variable name and value

CharString Variable::printstr() const {
  return name + " = " + tostring();
}

// void Variable::printvar(ostream &os) const {
//     os << name << " = " << tostring() << endl;
// }

// print variable together with the path, used when echoing commands
void TextWaiter::echovar(const Variable *var)
{
  WordList leadingpath(path);
  leadingpath.strip(1);
  CharString stringpath(leadingpath.concat());
  if(echo)
    msout << ms_prompt << stringpath << " " << var->printstr() << endl
	  << ms_normal;
  if(!var->attribute(MOPT_NOLOG))
    log(stringpath + " " + var->printstr());
}

void Variable::execute(TextWaiter& w) {
  w.path = w.cmdlist;
  if(w.macromode) {
    // this has to be rewritten to look for only name=arg
    w.record(w.path.concat());
    CharString value(w.argword()); // look for value
    if(value == EQUALS) value = w.argword();
    if(value != "")
      w.record("=" + value);
    w.record(DELIMITER);
  }
  else {
    //noticechar(EQUALS);
    CharString arg(w.argword());
    if(arg == "") {	    // "var"
      w.msout << printstr() << endl;
    }
    else {
      if(arg[0] == EQUALS) {	    // "var = ???"
	savevalue();
	fromstring(w);		    // new value? (destroys old value)
	if(*w.inbuf)	{	    // found it?
	  w.echovar(this);	    // yes
	  update_forms();
	}
	else {
	  restorevalue();    // restore old value
	  if(!w.getvar(this)) {	    // not already typed -- request value
	    // error reading new value
	    if(!attribute(MOPT_NOLOG))
	      w.log(printstr());
	    return;
	  }
	}
	if(hook) {  // execute hook function
	  w.path = w.cmdlist + " = " + tostring();
	  w.makecurrent();
	  (*hook)(name);
	  w.path.strip(2); // remove "= value" from path
	}
      }
      else {			    // "var value" is illegal
	w.repeatword = 1; // the word that wasn't '=' is the next command
	w.msout << printstr() << endl;
      }
    }
    //ignorechar(EQUALS);
  }
}

// prompt for a new value of var, using current value as default.

int TextWaiter::getvar(const Variable *var) {
  msout << ms_prompt << "*Enter " << var->name << " [" << var->tostring()
	<< "]: " << flush << ms_normal;

  char line[wbufsiz];
  char *ptr = fgets(line, sizeof(line), infile);
  if(!ptr) return 0;	// eof probably, will be caught later.
  
  // get rid of trailing newline
  char *c = strchr(ptr, '\n');
  if(c) *c = '\0';
  
  skipspace(&ptr);
  if(*ptr == '\0') return 1;    // keep old value
  return var->fromstring(ptr, *this);
}

void Variable::update_forms() {
#ifdef MENUGFX
  Vec<MenuForm*> &formlist = menu->form;
  for(int j=0; j<formlist.capacity(); j++)
    formlist[j]->updatevariable(this);
#endif
}

// ------------------------------------------------- //

// check that a string represents a number, for Variable input verification

static int check_number(const char *str, int founddot, int foundexp,
							    int beginning)
{
    const char c = *str;
    if(c == '\0' || c == '\n') return 1;    // ok -- got to the end
    if((c == '-' || c == '+') && !beginning) return 0;
    beginning = 0;
    if(c == '.') {
	if(founddot) return 0;
	founddot = 1;
    }
    if(isalpha(c)) {
	if(foundexp || (c != 'e' && c != 'E')) return 0;
	foundexp = 1;	// found "e"
	beginning = 1;
    }
    return check_number(str+1, founddot, foundexp, beginning);
}

static int checknumber(const CharString &str, Waiter &w) {
  CharString cs(skipspace(str));	    // makes a copy
  char *c = (char*) strchr(cs, '\n');
  if(c) *c = '\0';
  if(!check_number(cs, 0, 0, 1)) {
    w.msout << ms_error << "=== \"" << cs << "\" is not a number!"
	    << endl << ms_normal;
    return 0;
  }
  return 1;
}

// ------------------------------------------------- //

// Variable member functions for menu template classes

Variable::Variable(const CharString &nm, const CharString &hs, void *x, 
		   void (*f)(const CharString&), const MenuAttribute &att)
  : MenuOption(nm, hs, '=', att), address(x), hook(f), oldvalue(0)
#ifdef MENUGFX
     , varobj(0)
#endif
{
  VariableRegistry::vr().add_variable(this);
}

Variable::Variable() : oldvalue(0) {}

Variable::~Variable() {
  //  if(oldvalue) delete oldvalue;
#ifdef MENUGFX
  if(varobj) delete varobj;
#endif
}

template <>
CharString Var<char>::tostring() const {
    return to_charstring(*(char*) address);
}

// The VariableRegistry is a list of lists of Variables. Each sublist
// consists of Variables in different menus that refer to the same
// underlying variable (ie have the same address). There is only one
// VariableRegistry. It's allocated in the following function, to
// guarantee that there's only one and that it exists when needed.

VariableRegistry &VariableRegistry::vr() {
  // this function is a static member of the VariableRegistry class.
  static VariableRegistry *v = 0;
  if(v) return *v;
  v = new VariableRegistry;
  return *v;
}

VariableRegistry::~VariableRegistry() {
  for(int i=0; i<varlist.capacity(); i++)
    delete varlist[i];
}

void VariableRegistry::add_variable(Variable *var) {
  for(int i=0; i<varlist.capacity(); i++) {
    if((*varlist[i])[0]->address == var->address) {
      // new variable duplicates one in the list
      varlist[i]->grow(1, var);
      return;
    }
  }
  // enlarge varlist with a new list of Variables. The new list just
  // contains var.
  varlist.grow(1, new Vec<Variable*>(1, var));
}

void VariableRegistry::call_function(const Variable *var, void (Variable::*f)())
  const 
{
  for(int i=0; i<varlist.capacity(); i++) {
    if((*varlist[i])[0]->address == var->address) {
      Vec<Variable*> &vlist = *varlist[i];
      for(int j=0; j<vlist.capacity(); j++) {
	if(vlist[j] != var)	// don't call for var itself
	  (vlist[j]->*f)();
      }
    }
  }
}

// -------------------------------------------- // 


/* Override the templates for tostring and fromstring and Var,
 * to provide a bit more error handling than the default and to handle
 * quotation marks and spaces in CharStrings.
 */

template <>
int Var<double>::fromstring(const CharString &str, Waiter& w) const {
    if(checknumber(str, w)) {
	*(double *) address = atof(str);
	return 1;
    }
    return 0;
}

template <>
void Var<double>::fromstring(TextWaiter& w) const {
    Word str;
    *w.inbuf >> str;
    if(checknumber(str, w))
	*(double *) address = atof(str);
    else
	w.inbuf->clear(ios::badbit|w.inbuf->rdstate());
}

template <>
int Var<float>::fromstring(const CharString &str, Waiter& w) const {
    if(checknumber(str, w)) {
	*(float *) address = atof(str);
	return 1;
    }
    return 0;
}

template <>
void Var<float>::fromstring(TextWaiter& w) const {
    Word str;
    *w.inbuf >> str;
    if(checknumber(str, w))
	*(float *) address = atof(str);
    else
	w.inbuf->clear(ios::badbit|w.inbuf->rdstate());
}

template <>
int Var<int>::fromstring(const CharString &str, Waiter& w) const {
    if(checknumber(str, w)) {
	*(int*) address = atoi(str);
	return 1;
    }
    return 0;
}

template <>
void Var<int>::fromstring(TextWaiter& w) const {
    Word str;
    *w.inbuf >> str;
    if(checknumber(str, w))
	*(int*) address = atoi(str);
    else
	w.inbuf->clear(ios::badbit|w.inbuf->rdstate());
}

template <>
int Var<CharString>::fromstring(const CharString &str, Waiter&) const {
    CharString temp(str);    // make a temporary copy
    char *c;
    if(c = (char*) strchr(temp, '\n')) *c = '\0'; // get rid of possible \n
    // strip quotation marks
    if(temp[0] == QUOTE && (c=(char*)strchr(&temp[1], QUOTE))) {
	*c = '\0';
	*(CharString*) address = CharString(&temp[1]);
    }
    else
	*(CharString*) address = temp;
    return 1;
}

template <>
void Var<CharString>::fromstring(TextWaiter &w) const {
    Word str;	    // read as a Word so that special chars are handled
    *w.inbuf >> str;
    *(CharString*) address = str;
}

template <>
CharString Var<CharString>::tostring() const {
    CharString cs(*(CharString*) address);
    if(cs == "") return CharString("\"\""); // quote empty strings!
    // if string contains spaces and doesn't begin with QUOTE, quote it.
    // strings with embedded quotes will be confusing.
    //    if(strchr(cs, SPACE) && cs[0] != QUOTE)

    if(cs[0] != QUOTE) {
      for(int i=0; i<strlen(cs); i++) {
	if(cs[i] == SPACE || is_special(cs[i]))
	  return QUOTE + cs + QUOTE;
      }
    }
    return cs;
}

// ---------  Commands, Arguments, etc. -------------------- //

/* Commands with arguments need to have separate instances for each form,
 * since the forms can have different values for the arguments. Only the
 * master copy of the command keeps a list of the forms. The cloned copies
 * just have a pointer to the list.
 * 
 * Each command keeps a pointer to its own form.
 */

Command::~Command() {
#ifdef MENUGFX
  if(form) {
    if(!active) {
      delete form;
      form = 0;
    }
    else
      form->frozen = 0;	// so form will be deleted when cmd is done
  }
#endif // MENUGFX
}

CommandM::~CommandM() {
  if(!ismaster) {
    clonelist->remove(this);
  }
  else {
    /* do this in reverse order, since each command removes itself from
     * the list as it is deleted.
     */
    for(int i=clonelist->capacity()-1; i>=0; i--) {
      CommandM *cmd = (*clonelist)[i];
      if(cmd != this && !cmd->active) // don't delete the active command
	delete cmd;
#ifdef MENUGFX
      else
	cmd->form->frozen = 0;	// make sure it's deleted when it's done
      // active command should be deleted by CommandMForm::dismiss()
#endif // MENUGFX
    }
  }
}

CharString CommandM::argstring() const {
  CharString str;
  for(int i=0; i<arg.capacity(); i++)
    if(!arg[i]->attribute(MOPT_SECRET))
       str = str.spaceappend(arg[i]->name + "=" + arg[i]->tostring());
  return str;
}

#ifdef MENUGFX
CommandForm *Command0::makecmdform(const WordList &cmdpath) {
  return new Command0Form(this, cmdpath);
}

CommandForm *CommandS::makecmdform(const WordList &cmdpath) {
  return new CommandSForm(this, cmdpath);
}

CommandForm *CommandM::makecmdform(const WordList &cmdpath) {
  return new CommandMForm(this, cmdpath);
}
#endif // MENUGFX

void CommandM::pronto(Waiter &w, const WordList &path) {
  w.makecurrent();
  w.path = path;
  w.args = argstring();
  CharString fullcmd(w.path.concat() + " " + w.args);
  if(w.echo)
    w.msout << ms_prompt << w.errhm << fullcmd << endl << ms_normal;
  if(!attribute(MOPT_NOLOG))
    w.log(fullcmd);
  if(w.macromode) {
    w.record(fullcmd);
    w.recorddelim();
  }
  else {
    active = 1;
    func();
    active = 0;
  }
}

void Command0::pronto(Waiter &w, const WordList &path) {
  w.makecurrent();
  w.path = path;
  w.args = "";
  CharString fullcmd(path.concat());
  if(w.echo)
    w.msout << ms_prompt << w.errhm << fullcmd << endl << ms_normal;
  if(!attribute(MOPT_NOLOG))
    w.log(fullcmd);
  if(w.macromode) {
    w.record(fullcmd);
    w.recorddelim();
  }
  else {
    active = 1;
    (*func)();
    active = 0;
  }
}

void CommandS::pronto(Waiter &w, const WordList &path) {
  w.makecurrent();
  w.path = path;
  CharString fullcmd(path.concat());
  if(w.echo)
    w.msout << ms_prompt << w.errhm << fullcmd << endl << ms_normal;
  if(!attribute(MOPT_NOLOG))
    w.log(fullcmd);
  if(w.macromode) {
    w.record(fullcmd);
    w.recorddelim();
  }
  else {
    ScriptWaiter sw(script, w.msout);
    active = 1;
    sw.order(menu);
    active = 0;
    w.makecurrent();
  }
}

// ---------  Auxiliary routines -------------------- //

void Waiter::runfile(FILE* file, Menu* menu, int verbose, bool (*stop)()) {
  if(!file) return;
  TextWaiter w(msout, file, stop);
  if(inhibit_logging_) w.inhibit_logging();
  if(verbose)
    w.talkative();
  else
    w.quiet();
  w.order(menu);
  if(w.showprompts)
    msout << endl;		// so that two prompts don't appear on one line
  makecurrent();		// original waiter regains control
}

void Waiter::runfile(istream &is, Menu *menu, int verbose, bool (*stop)()) {
  if(!is) return;
  StreamWaiter w(msout, is, stop);
  if(inhibit_logging_) w.inhibit_logging();
  if(verbose)
    w.talkative();
  else
    w.quiet();
  w.order(menu);
  if(w.showprompts)
    msout << endl;
  makecurrent();
}

// Command log
//  startlog(), stoplog() control logging to a file.
//  A log of the whole session is also kept automatically, and it
//  can be printed with printlog();

Vec<CharString*> Waiter::logbook(0, BlockSize(50));

TrueFalse Waiter::logdirty_(0);

void Waiter::startlog(const CharString &file, const TrueFalse &append) {
  stoplog();
  if(append)
    logfile = new ofstream(file, ios::app);
  else
    logfile = new ofstream(file);
  if(*logfile) logging_ = TF_TRUE;
}

void Waiter::stoplog() {
  if(logging_) {
    logging_ = TF_FALSE;
    logfile->close();
  }
}

void Waiter::log(const CharString &str) {
  if(inhibit_logging_ > 0) return;
  if(logging_ && !macromode)  // explicit logging
    *logfile << str << endl;
  if(!macromode) {            // automatic logging
    logbook.grow(1, new CharString(str));
    logdirty_ = TF_TRUE;
  }
#ifdef DEBUGVERBOSE
  cerr << "Waiter::log: " << str << endl;
#endif
}

void Waiter::printlog(ostream &os) const {
  for(int i=0; i<logbook.capacity(); i++)
    os << *logbook[i] << endl;
  logdirty_ = TF_FALSE;
}

void Waiter::readlog(istream &is) {
  for(int i=0; i<logbook.capacity(); i++)
    delete logbook[i];
  logbook.resize(0);
  char line[1024];
  while(is.getline(line, 1024)) {
    logbook.grow(1, new CharString(line));
  }
}

// ------------------------------------------------------------- //

// Specific command classes

LoopCommand::LoopCommand(const CharString &nm, const CharString &hs, Menu *mnu,
			 const CharString &s, const CharString &bs,
			 const CharString &es, int f)
  : iterations(0), script(s), beginscript(bs), endscript(es), 
    CommandM(nm, hs), loopmenu(mnu), fixscripts(f)
{
  if(!fixscripts) {
    AddArgument(this, "initialize", beginscript);
    AddArgument(this, "body", script);
    AddArgument(this, "finish", endscript);
  }
  AddArgument(this, "iterations", iterations);
}

CommandFn LoopCommand::func() {
  Interrupter stopnow;

  if(beginscript != "") {
    ScriptWaiter sw(beginscript, garcon()->msout);
    sw.order(loopmenu);
  }

  for(int i=0; i<iterations && !stopnow(); i++) {
    ScriptWaiter sw(script, garcon()->msout);
    sw.order(loopmenu);
  }

  if(stopnow())
    garcon()->msout << ms_error << "=== Interrupted!\n" << endl << ms_normal;

  if(endscript != "") {
    ScriptWaiter sw(endscript, garcon()->msout);
    sw.order(loopmenu);
  }
}

CommandM *LoopCommand::clone() const {
  LoopCommand *lc = new LoopCommand(name, helpstring, loopmenu,
				    script, beginscript, endscript,
				    fixscripts);
  lc->iterations = iterations;
  return lc;
}

// ----------------------------------------------------- //

LoadMacroCmd::LoadMacroCmd(const CharString &cname) : cmdname(cname), 
    CommandM(cname, "read macro definitions from a file"),
    filename("macrofile")
{
    AddArgument(this, "filename", filename);
}

CommandFn LoadMacroCmd::func() {
  FILE *f = fopen(filename, "r");
  if(!f)
    garcon()->msout << ms_error << "=== Can't read macros from file "
		    << filename << "!" << endl << ms_normal;
  else {
    garcon()->loadmacros(f);
    fclose(f);
  }
}

CommandM *LoadMacroCmd::clone() const {
  LoadMacroCmd *lm = new LoadMacroCmd(cmdname);
  lm->filename = filename;
  return lm;
}

SaveMacroCmd::SaveMacroCmd(const CharString &cname) : cmdname(cname), 
  CommandM(cname, "save macro definitions to a file"),
  filename("macrofile")
{
  AddArgument(this, "filename", filename);
}

CommandFn SaveMacroCmd::func() {
  FILE *f = fopen(filename, "w");
  if(!f)
    garcon()->msout << ms_error << "=== Can't write macros to file "
		    << filename << "!" << endl << ms_normal;
  else {
    garcon()->showmacros(f);
    fclose(f);
  }
}

CommandM *SaveMacroCmd::clone() const {
  SaveMacroCmd *sm = new SaveMacroCmd(cmdname);
  sm->filename = filename;
  return sm;
}
