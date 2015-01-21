// -*- C++ -*-
// $RCSfile: commands.C,v $
// $Revision: 1.4 $
// $Author: langer $
// $Date: 2000-10-31 19:15:09 $

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


#ifdef MENUGFX

// routines for command forms

#include <string.h>
#include "stdlib.h"
#include "menuDef.h"
#include "forms.h"
#include "menuform.h"
#include "menucolor.h"
#include "vec.h"

CommandForm::CommandForm(const WordList &cmdpath, const CharString &cmdname)
    : path(cmdpath), Form((cmdpath + cmdname).concat())
{
}

int CommandForm::closecommandform(FL_FORM*, void *ptr) {
  ((CommandForm *) ptr)->dismiss();
  return FL_IGNORE;
}


CommandMForm::CommandMForm(CommandM *command, const WordList &cmdpath)
  : cmd(command->duplicate()),
    mastercmd(command), 
    CommandForm(cmdpath, command->name)
{
  int i;
  int Nargs = command->arg.capacity();
  cmd->form = this;

  // count secret args (args that aren't displayed)
  int secret = 0;
  for(i=0; i<Nargs; i++)
    if(cmd->arg[i]->attribute(MOPT_SECRET))
      secret++;
  varobj.resize(Nargs-secret);

  create_form_commandMform(this, cmd->name);
  // make sure to get dummy arg geometry before resizing the form!
  int argx = dummyarg->x;
  int argy = dummyarg->y;
  int argw = dummyarg->w;
  int argh = dummyarg->h;
  fl_freeze_form(the_form);
  fl_set_form_size(the_form, the_form->w, the_form->h+(Nargs-secret-1)*argh);
  fl_addto_form(the_form);

  int ac = 0; // number of args drawn
  for(i=0; i<Nargs; i++) {
    if(!cmd->arg[i]->attribute(MOPT_SECRET)) {
      varobj[ac] = 
	cmd->arg[i]->create_varobject(cmd->arg[i], this, VO_REACTIVE,
				      argx, argy+ac*argh, argw, argh);
      varobj[ac]->show();
      ac++;
    }
  }
  fl_end_form();
  fl_set_object_color(background, COL_BACKGROUND, 0);
  fl_set_object_color(CF_freeze, COL_OFF_LIGHT, COL_FREEZE_LIGHT);
  fl_set_object_lcol(CF_CommandButton, COL_CF_MAINTEXT);
  fl_set_object_lcol(CF_helpbutton, COL_CF_HELPTEXT);
  fl_set_object_lcol(CF_dismiss, COL_CF_OTHERTEXT);
  fl_set_object_lcol(CF_freeze, COL_CF_OTHERTEXT);
  fl_set_object_lcol(CF_copy, COL_CF_OTHERTEXT);
  fl_set_object_lcol(CF_default, COL_CF_OTHERTEXT);
  fl_set_form_atclose(the_form, closecommandform, this);
  fl_set_object_shortcutkey(CF_homebutton, XK_Home);
  fl_unfreeze_form(the_form);
}

CommandForm::~CommandForm() {
  /* have to reset the event queue here so that after a form is deleted
   * it doesn't get any events!
   */
  hide();
  //fl_qreset();
}

CommandMForm::~CommandMForm() {
  for(int i=0; i<varobj.capacity(); i++)
    delete varobj[i];
}

int CommandMForm::getinputs() {
  fl_freeze_form(the_form);
  int status = 1;
  // get values of all inputs, to make sure they are up-to-date.
  for(int i=0; i<varobj.capacity(); i++) {
    // get input
    CharString str(varobj[i]->get());
    // set value
    if(!varobj[i]->var->fromstring(str, formwaiter())) {
      garcon()->msout << ms_error
		      << "=== Argument \"" << varobj[i]->var->name
		      << "\" is badly formed." << endl << ms_normal;
      status = 0;
    }
    varobj[i]->show();
  }
  fl_unfreeze_form(the_form);
  return status;
}

// --------------------------------------------------------------- //

// commands w/out arguments

CommandZeroForm::CommandZeroForm(const WordList &cmdpath,
				const CharString &nm)
    : CommandForm(cmdpath, nm)
{
  create_form_command0form(this, nm);
  fl_set_object_color(background, COL_BACKGROUND, 0);
  fl_set_object_color(CF_freeze, COL_OFF_LIGHT, COL_FREEZE_LIGHT);
  fl_set_object_shortcutkey(CF_homebutton, XK_Home);
  fl_set_form_atclose(the_form, closecommandform, this);
}


// normal commands
Command0Form::Command0Form(Command0 *command, const WordList &cmdpath)
    : cmd(command), CommandZeroForm(cmdpath, command->name)
{}

// scripted commands
CommandSForm::CommandSForm(CommandS *command, const WordList &cmdpath)
    : cmd(command), CommandZeroForm(cmdpath, command->name)
{}

// ----------- Call Back Functions --------------------------------- //

void CFCB_freeze(FL_OBJECT *obj, long ptr) {
    if(Form::exists(ptr))
	((Command0Form *) ptr)->freeze(obj);
}

void CFCB_helpbutton(FL_OBJECT*, long ptr) {
    if(Form::exists(ptr))
	((CommandForm *) ptr)->help();
}

/* help() is not simply a CommandForm member, since cmd lives only
 * in the derived CommandMForm and Command0Form classes.
 */
void Command0Form::help() {
  garcon()->msout << ms_info << name << ": " << cmd->helpstring
		  << endl << ms_normal;
}

void CommandSForm::help() {
  garcon()->msout << ms_info << name << ": macro {" << cmd->script << "}"
		  << endl << ms_normal;
}

void CommandMForm::help() {
  garcon()->msout << ms_info << name << ": " << cmd->helpstring
		  << endl << ms_normal;
}

void CFCB_CommandButton(FL_OBJECT *obj, long ptr) {
    if(Form::exists(ptr))
	((CommandForm *) ptr)->perform();
    if(Form::exists(ptr))   // check again in case command has deleted the form!
	fl_set_button(obj, 0);  // unpush button
}

void CommandMForm::perform() {
    /* if an argument has been changed, hitting "return" will call
     * CFCB_CommandButton and also queue a CFCB_argument event. Processing
     * the argument event after the command is the wrong thing to do, and
     * can dump core. So check for an event here.
     */
    (void) fl_check_only_forms();
    if(!getinputs()) {
      garcon()->msout << ms_error << "=== Command \""
		      << cmd->name << "\" not executed." << endl << ms_normal;
	return;
    }
    fl_set_object_color(CF_backbox, COL_CF_BACK_ACTIVE, 0);
    fl_redraw_form(the_form);
    XFlush(fl_get_display());  // make sure form is redrawn before cmd runs
    cmd->pronto(formwaiter(), path + cmd->name);
    if(!frozen)
	dismiss();
    else {
	fl_set_object_color(CF_backbox, COL_CF_BACK_IDLE, 0);
	fl_redraw_form(the_form);
	XFlush(fl_get_display());
    }
}

void CommandZeroForm::perform0(Command *cmd) {
    fl_set_object_color(CF_backbox, COL_CF_BACK_ACTIVE, 0);
    fl_redraw_form(the_form);
    XFlush(fl_get_display());  // make sure form is redrawn before cmd runs
    cmd->pronto(formwaiter(), path + cmd->name);
    if(!frozen)
	dismiss();
    else {
	fl_set_object_color(CF_backbox, COL_CF_BACK_IDLE, 0);
	fl_redraw_form(the_form);
	XFlush(fl_get_display());
    }
}

void Command0Form::perform() {
    perform0(cmd);
}

void CommandSForm::perform() {
    perform0(cmd);
}

void CFCB_dismiss(FL_OBJECT*, long ptr) {
    if(Form::exists(ptr))
	((CommandForm *) ptr)->dismiss();
}

void CommandForm::dismiss() {
    hide();
    (void) fl_check_only_forms();   // see comment in CommandMForm::perform()
    delete this;
}

void CommandMForm::dismiss() {
    hide();
    (void) fl_check_only_forms();   // see comment in CommandMForm::perform()
    if(!cmd->ismaster)
	delete cmd;	// will delete the form!
    else
	delete this;	// should never occur
}

void CMCB_copy(FL_OBJECT*, long ptr) {
    if(Form::exists(ptr))
	((CommandMForm *) ptr)->copy();
}

void CommandMForm::copy() {
  // make a new form with a new argument list
  (void) fl_check_only_forms();   // see comment in CommandMForm::perform()
  if(!getinputs()) {
    garcon()->msout << ms_error << "=== Command not copied!"
		    << endl << ms_normal;
    return;
  }
  CommandMForm *newform = new CommandMForm(cmd, path);
  newform->mastercmd = mastercmd;
  newform->show(0);
}

void CMCB_default(FL_OBJECT*, long ptr) {
    if(Form::exists(ptr))
	((CommandMForm *) ptr)->setdefault();
}

void CommandMForm::setdefault() {
  (void) fl_check_only_forms(); // see comment in CommandMForm::perform()
  if(!getinputs()) {
    garcon()->msout << ms_error << "=== Defaults not set." << endl
		    << ms_normal;
    return;
  }
  for(int i=0; i<cmd->arg.capacity(); i++)
    mastercmd->arg[i]->copyvalue(cmd->arg[i]);
}

void CommandForm::show_homeform() {
  formwaiter().show_homeform();
}

void CFCB_homebutton(FL_OBJECT*, long ptr) {
  if(Form::exists(ptr))
    ((CommandForm*) ptr)->show_homeform();
}

#endif // MENUGFX
