// -*- C++ -*-
// $RCSfile: menuform.h,v $
// $Revision: 1.5 $
// $Author: langer $
// $Date: 2000-09-06 20:11:06 $

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


#ifdef MENUGFX

class Form;
class MenuForm;
class CommandForm;
class Command0Form;
class CommandMForm;
class CommandSForm;
class MessageForm;

#ifndef MENUFORM_H
#define MENUFORM_H

#include "forms.h"
#include "menuDef.h"
#include "vec.h"
#include "charstring.h"
#include "truefalse.h"
#include "varobjects.h"
#include "word.h"

class Form {
private:
  static Vec<Form*> allforms;
protected:
  FL_OBJECT *background;
  CharString name;
  int frozen;
  void sameposition(const Form *);    // copy position
  void samesize(const Form *);	    // copy size
  void gotoform(Form *);		    // transfer control
  void freeze(FL_OBJECT *obj) { frozen = fl_get_button(obj); }
  virtual int &Nforms();// counter for number of visible forms
  //                       can be overloaded if specialized count needed
  FormWaiter &formwaiter();
  static FormWaiter *the_formwaiter;
  void setinputvar(FL_OBJECT*, Variable*);
	
  static void haltall();	    // does a fl_freeze_form on all forms
  static void resumeall();    // undoes it
  static void setcursor(int);
public:
  Form(const CharString &);
  virtual ~Form();
  FL_FORM *the_form;
  static int exists(long);
  void show(Form *);
  void hide();
  virtual void makevisible();
  void activate() { fl_activate_form(the_form); }
  void deactivate() { fl_deactivate_form(the_form); }

  /* setvariable needs to be declared by MenuForm and CommandMForm,
   * and is used only by MenuForm, but I don't want another level of class
   * derivation, so I'll put it here.
   */
  virtual void setvariable(Variable*, const CharString&) {};

  friend void MFCB_freeze(FL_OBJECT *, long);
  friend void CFCB_freeze(FL_OBJECT *, long);
  friend void create_form_generic_menu(MenuForm*, const char*,const char*);
  friend class FormWaiter;
  friend class Command;
  friend class CommandM;
};


class MenuForm : public Form {
private:
  int isroot;	// only one form is the root form.
  void makeroot(FormWaiter *);
  Menu *the_menu;
  WordList path;
  FL_OBJECT	// stuff in the form
  *MF_menu_name,
    *MF_return_to_home_menu,
    *MF_back_up,
    *MF_close_me,
    *MF_frozen,
    *MF_command_list,
    *MF_submenu_list,
    *MF_variable_list,
    *MF_varhelp, 
    *MF_help_message, 
    *MF_record,
    *inputregion,
    *dummyinput;
  MenuForm *previous;
  int selectedvar;
  // lists of browser entries
  Vec<Variable*> variables;
  Vec<SubMenu*> submenus;
  Vec<Command*> commands;
  // not quite the call back routines
  void close_form();
  static int closemenuform(FL_FORM *, void *);
  void go_back();
  void go_home();
  void submenuhandler(FL_OBJECT *);
  void commandhandler(FL_OBJECT *);
  void do_click(int nclicks);
  void execute_fn_now();
  void open_fn_window();
  static void singleclick_callback(int, void*);
  static void ignorealarm(int, void*);
  void variablehandler(FL_OBJECT *);
  void inputvariable(FL_OBJECT *);
  VarObject* current_varobj;
  void record_on(FL_OBJECT *);
  void record_off(FL_OBJECT *);
  virtual int &Nforms();
  CharString updatevariable(Variable*, int);
  void updatevariable(Variable*);
public:
  MenuForm(Menu*, const CharString&, const WordList&,
	   const CharString&);
  ~MenuForm();
  void update();	// redraw browsers
  virtual void setvariable(Variable*, const CharString&);

  // does a single or double click execute functions immediately?
  enum ClickMode {SINGLE_NOW, DOUBLE_NOW};
  static ClickMode clickmode;
	
#ifdef VARARG_SIGNALS
  static void signalhandler(int, ...);
#else
  static void signalhandler(int);
#endif

  // call back routines can't be class members!
  // so these friends use the long argument to call the right class member
  friend void MFCB_close_form(FL_OBJECT *, long);
  friend void MFCB_go_back(FL_OBJECT *, long);
  friend void MFCB_go_home(FL_OBJECT *, long);
  friend void MFCB_submenuhandler(FL_OBJECT *, long);
  friend void MFCB_commandhandler(FL_OBJECT *, long);
  friend void MFCB_variablehandler(FL_OBJECT *, long);
  friend void MFCB_record(FL_OBJECT *, long);

  friend void create_form_generic_menu(MenuForm*, const char*, const char*);

  friend class FormWaiter;
  friend class SubMenu;
  friend class Variable;
  friend class Command;
  friend class CallBack;
};


// -------------------------------------------------------- //

class CommandForm : public Form {
protected:
  FL_OBJECT
  *CF_CommandButton,
    *CF_dismiss,
    *CF_freeze,
    *CF_helpbutton, 
    *CF_backbox,
    *CF_homebutton;
  virtual void perform() = 0;
  virtual void dismiss();
  virtual void help() = 0;
  WordList path;
  static int closecommandform(FL_FORM*, void*);
  void show_homeform();
public:
  CommandForm(const WordList&, const CharString&);
  virtual ~CommandForm();

  friend void CFCB_helpbutton(FL_OBJECT *, long);
  friend void CFCB_CommandButton(FL_OBJECT *, long);
  friend void CFCB_dismiss(FL_OBJECT *, long);
  friend void CFCB_homebutton(FL_OBJECT*, long);
};

// form for a command with multiple arguments

class CommandMForm : public CommandForm {
private:
  CommandM *cmd;
  CommandM *mastercmd;
  Vec<VarObject*> varobj;
  FL_OBJECT *CF_copy, *CF_default, *dummyarg;
  // methods called by the call back routine
  virtual void perform();
  virtual void help();
  void copy();
  void setdefault();
  void set_arg(FL_OBJECT *);
  int getinputs();
  virtual void dismiss();
public:
  CommandMForm(CommandM*, const WordList&);
  virtual ~CommandMForm();

  friend void create_form_commandMform(CommandMForm*, const char*);

  friend void CFCB_CommandButton(FL_OBJECT *, long);
  friend void CMCB_copy(FL_OBJECT *, long);
  friend void CMCB_default(FL_OBJECT *, long);
  friend void CMCB_argument(FL_OBJECT *, long);
  friend void CFCB_helpbutton(FL_OBJECT *, long);
  friend class CommandM;
};

// form for a command with no arguments

class CommandZeroForm : public CommandForm {
protected:
  virtual void perform() = 0;
  void perform0(Command *);
  virtual void help() = 0;
public:
  CommandZeroForm(const WordList&, const CharString&);
  virtual ~CommandZeroForm() {}
  friend void create_form_command0form(CommandZeroForm*, const char*);
};

class Command0Form : public CommandZeroForm {
private:
  Command0 *cmd;
  virtual void perform();
  virtual void help();
public:
  Command0Form(Command0*, const WordList&);
  virtual ~Command0Form() {}

  friend void CFCB_CommandButton(FL_OBJECT *, long);
  friend void CFCB_dismiss(FL_OBJECT *, long);
  friend void CFCB_helpbutton(FL_OBJECT *, long);

  friend void create_form_command0form(CommandZeroForm *);
};

class CommandSForm : public CommandZeroForm {
private:
  CommandS *cmd;
  virtual void perform();
  virtual void help();
public:
  CommandSForm(CommandS*, const WordList&);
  virtual ~CommandSForm() {}

  friend void CFCB_CommandButton(FL_OBJECT *, long);
  friend void CFCB_dismiss(FL_OBJECT *, long);
  friend void CFCB_helpbutton(FL_OBJECT *, long);
};

// ------------------------------------------------------- //

class MessageForm : public Form {
private:
  FL_OBJECT *MG_text;
  FL_OBJECT *MG_clear;
  FL_OBJECT *MG_close;
  FL_OBJECT *MG_save;
  FL_OBJECT *MG_font; 
  FL_OBJECT *MG_size; 
  FL_OBJECT *MG_input;
  FL_OBJECT *MG_home;

  void cleartext();
  void save();
  void font();
  void size();
  void home();

  int lastlinecomplete;
  int selectedfont;
  int selectedsize;
  static void addfonts(const char*);
  CharString longestline;
  static const int fudge;
  static Vec<MessageForm*> allmessageforms;
public:
  MessageForm(const CharString &);
  ~MessageForm();
  void addtext(const char *);
  void setcolor(int);
  virtual void makevisible();
  friend void create_form_messageform(MessageForm*);
  friend void MGCB_text(FL_OBJECT *, long);
  friend void MGCB_clear(FL_OBJECT *, long);
  friend void MGCB_close(FL_OBJECT *, long);
  friend void MGCB_save(FL_OBJECT *, long);
  friend void MGCB_font(FL_OBJECT *, long);
  friend void MGCB_size(FL_OBJECT *, long);
  friend void MGCB_home(FL_OBJECT*, long);
};


CharString forminputstring(FL_OBJECT *);

// ------------------------------------------------------------- //

// form that prompts for an input string.
// probably only used via FormWaiter::getstring()

class StringForm : public Form {
private:
  FL_OBJECT *SF_prompt;
  FL_OBJECT *SF_input;
  FL_OBJECT *SF_okbutton;
  void ok();
  CharString answer;
  int notyet;
public:
  StringForm(const CharString &prompt, const CharString &dflt);
  virtual ~StringForm() {}
  virtual void makevisible();
  friend void create_form_stringform(StringForm*);
  friend void SFCB_okbutton(FL_OBJECT*, long);
  friend class FormWaiter;
};

class QuestionForm : public Form {
private:
  FL_OBJECT *yes_button;
  FL_OBJECT *no_button;
  FL_OBJECT *question_text;
  int done_, answer_;
  void yes();
  void no();
public:
  QuestionForm(const CharString &question, const TrueFalse &dflt);
  virtual void makevisible();
  int done() const { return done_; }
  int answer() const { return answer_; }
  friend void create_form_questionform(QuestionForm*);
  friend void QFCB_yesbutton(FL_OBJECT*, long);
  friend void QFCB_nobutton(FL_OBJECT*, long);
};


class PauseForm : public Form {
private:
  FL_OBJECT *button_continue;
  PauseForm();			// private constructor for singleton object
  static PauseForm *pauseform;
  void wait();
public:
  static void pause();		// this calls the constructor
  friend void create_form_pauseform(PauseForm*);
};

#endif

#endif // MENUGFX




