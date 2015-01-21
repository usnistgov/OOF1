// -*- C++ -*-
// $RCSfile: menuform.C,v $
// $Revision: 1.10 $
// $Author: langer $
// $Date: 2000-11-03 14:46:32 $

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


#ifdef MENUGFX

#include "ctype.h"
#include "stdlib.h"
#include <unistd.h>
#include <limits.h>
#include <string.h>
#include <X11/cursorfont.h>
#include "forms.h"
#include "menuDef.h"
#include "menucolor.h"
#include "menuform.h"
#include "varobjects.h"

// General Form methods
Vec<Form*> Form::allforms;

Form::Form(const CharString &nm)
    : name(nm), 
      frozen(0),
      the_form(0)
{
    allforms.grow(1, this);
}

Form::~Form() {
    /* have to reset the event queue here so that after a form is deleted
     * it doesn't get any events!
     */
    //fl_qreset();
    if(the_form->visible) fl_hide_form(the_form);
    fl_free_form(the_form);
    allforms.remove(this);
}

void Form::gotoform(Form *nextform) {
    if(!frozen) {
	nextform->show(this);	// show new form at old position
	hide();			// hide current form
    }
    else {			// don't hide current form
	nextform->show(0);	// show new form at user-selected position
    }
}

/* show form. If the form is already visible, it is brought to the top.
 * If it's not visible, and "other" is non-zero, the form takes its position
 * from the other form. If other is zero, the mouse is used to place the
 * form.
 */

void Form::show(Form *other) {
  if(the_form->visible)   // get position from self
    other = this;
  if(other) {			// get position from other form
    hide();			// hide first, so that "show" raises form to top
    sameposition(other);
    fl_show_form(the_form, FL_PLACE_GEOMETRY, FL_FULLBORDER, name.charstar());
  }
  else
#ifdef sgi
    // On the SGI, if I use FL_PLACE_FREE instead of FL_PLACE_SIZE
    // here, then the new windows aren't placed in the location of the
    // old windows.  I don't know if this is a problem with the window
    // manager or the XForms version. Using FL_PLACE_SIZE sets the
    // minimum size of the window to be its initial size, but that's
    // easier to deal with than having to place every window by hand
    // when changing menus.
    fl_show_form(the_form, FL_PLACE_SIZE, FL_FULLBORDER, name.charstar());   	
#else
    fl_show_form(the_form, FL_PLACE_FREE, FL_FULLBORDER, name.charstar());   	
#endif
  Nforms()++;		    // do this even if form was already showing,
  // since it was hidden to enforce a raise.
}

// simply make a form visible if it's not already. Don't bring it to the top.

void Form::makevisible() {
    if(the_form->visible) return;
    fl_show_form(the_form, FL_PLACE_SIZE, FL_FULLBORDER, name.charstar());
    Nforms()++;
}

void Form::hide() {
    if(the_form->visible) {
	Nforms()--;
	fl_hide_form(the_form);
    }
}

static int ngeneralforms = 0;

int &Form::Nforms() {
    return ngeneralforms;
}

void Form::sameposition(const Form *other) {
    // make this form have same position as the other one
    FL_FORM *otherform = other->the_form;
    long x = otherform->x;
    long y = otherform->y;
    fl_set_form_position(the_form, x, y);
}

void Form::samesize(const Form *other) {
    // make this form have same size as the other one
    FL_FORM *otherform = other->the_form;
    FL_Coord w = otherform->w;
    FL_Coord h = otherform->h;
    fl_set_form_size(the_form, w, h);
}

// there's only one formwaiter
// this is set by MenuForm::makeroot()
FormWaiter *Form::the_formwaiter = 0;

FormWaiter &Form::formwaiter() {
    return *the_formwaiter;
}

void Form::setinputvar(FL_OBJECT *obj, Variable *var) {
    fl_freeze_form(the_form);
    if(var) {
	CharString str(var->tostring());
	// display variable value
	fl_set_input(obj, str.charstar());
	// change displayed name
	str = var->name + " =";
	fl_set_object_label(obj, str.charstar());
    }
    else {
	fl_set_input(obj, "");
	fl_set_object_label(obj, "");
    }
    fl_redraw_form(the_form);   // or else old var name is still visible  
    fl_unfreeze_form(the_form);
}

// ----------------------------------------------------------- //

// things that happen to all forms at once

void Form::haltall() {
  fl_deactivate_all_forms(); 
  setcursor(XC_watch);
  for(int i=0; i<allforms.capacity(); i++)
    fl_freeze_form(allforms[i]->the_form);
}

void Form::resumeall() {
  setcursor(XC_top_left_arrow); // this is the default xforms cursor
                                // fl_reset_cursor doesn't seem to work
  fl_activate_all_forms();
  for(int i=0; i<allforms.capacity(); i++) {
    if(allforms[i]->the_form->frozen)
      fl_unfreeze_form(allforms[i]->the_form);
  }
}

void Form::setcursor(int cursornumber) {
  // the cursor isn't actually changed until fl_check_only_forms is called
  // (it seems) so we need to stick a call to it here. But we don't want
  // events processed here, so the forms are deactivated first.
  fl_deactivate_all_forms();
  for(int i=0; i<allforms.capacity(); i++) {
    if(allforms[i]->the_form->window)
      fl_set_cursor(allforms[i]->the_form->window, cursornumber);
  }
  fl_check_only_forms();
  fl_activate_all_forms();
}

// ---------------------------------------------------------- //

int Form::exists(long ptr) {
    for(int i=0; i<allforms.capacity(); i++)
	if(allforms[i] == (Form*)ptr) return 1;
    return 0;
}

// ---------------------------------------------------------- //

// MenuForm specific stuff

// this should be a static member of the MenuForm class
// it's set by MenuForm::makeroot()
static MenuForm *rootform = 0;		// there's only one root form

// double click time in seconds. don't exceed 1 second
static const double doubleclicktime = 0.4;
MenuForm::ClickMode MenuForm::clickmode = MenuForm::DOUBLE_NOW;


MenuForm::MenuForm(Menu *menu, const CharString &errhm,
		   const WordList &cpath,
		   const CharString &helpstr)
  : Form(errhm + cpath.concat()), path(cpath),
    selectedvar(-1), isroot(0), the_menu(menu), current_varobj(0)
{
  create_form_generic_menu(this, menu->prompt, helpstr);
    
  // stuff added by hand:
  // make the browser lists readable
  fl_set_browser_fontstyle(MF_submenu_list, FL_BOLD_STYLE);
  fl_set_browser_fontstyle(MF_command_list, FL_BOLD_STYLE);
  fl_set_browser_fontstyle(MF_variable_list, FL_BOLD_STYLE);
  fl_set_browser_fontsize(MF_submenu_list, 12);
  fl_set_browser_fontsize(MF_command_list, 12);
  fl_set_browser_fontsize(MF_variable_list, 12);

  fl_set_object_color(background, COL_BACKGROUND, 0);
  fl_set_object_lcol(MF_menu_name, COL_MF_TITLE);
  fl_set_object_color(MF_submenu_list, COL_MF_BROWBG, COL_MF_BROWSELECTION);
  fl_set_object_color(MF_command_list, COL_MF_BROWBG, COL_MF_BROWSELECTION);
  fl_set_object_color(MF_variable_list, COL_MF_BROWBG, COL_MF_BROWSELECTION);
  fl_set_object_lcol(MF_submenu_list, COL_MF_BROWTEXT);
  fl_set_object_lcol(MF_command_list, COL_MF_BROWTEXT);
  fl_set_object_lcol(MF_variable_list, COL_MF_BROWTEXT);
  fl_set_object_color(inputregion, COL_MF_INPUTREGION, 0);
  fl_set_object_color(MF_frozen, COL_OFF_LIGHT, COL_FREEZE_LIGHT);
  fl_set_object_color(MF_record, COL_OFF_LIGHT, COL_MF_RECORD_LIGHT);

  fl_set_object_shortcutkey(MF_return_to_home_menu, XK_Home);

  fl_set_form_atclose(the_form, closemenuform, this);

  fl_set_form_dblbuffer(the_form, 1);

  // put the menu entries into the forms
  update();
  
  // record this form in the list in the menu
  menu->form.grow(1, this);
}

MenuForm::~MenuForm() {
    the_menu->form.remove(this);
}

void MenuForm::update() {
    fl_freeze_form(the_form);
    fl_clear_browser(MF_submenu_list);
    fl_clear_browser(MF_command_list);
    fl_clear_browser(MF_variable_list);
    submenus.resize(0);
    variables.resize(0);
    commands.resize(0);
    for(int i=0; i<the_menu->option.capacity(); i++)
	the_menu->option[i]->AddToForm(this);
    fl_unfreeze_form(the_form);
}


void MenuForm::makeroot(FormWaiter *w) {
    isroot = 1;
    rootform = this;
    the_formwaiter = w;
    fl_set_object_label(MF_back_up, "Quit");
    fl_deactivate_object(MF_return_to_home_menu);
    fl_set_object_lcol(MF_return_to_home_menu, FL_INACTIVE_COL);
}

static int Nmenuforms = 0;

int &MenuForm::Nforms() {
    return Nmenuforms;
}

// --------------------------------------------------------- //

// MenuOption methods for creating browser entries

CharString MenuOption::browserline() const {
    static const CharString highlighter("@i");	// italic
    static const CharString activator("@C1");	// color 1 = red
    CharString line;
    if(drawactive) line += activator;
    if(highlighted) line += highlighter;
    if(drawactive && isdigit(name[0]) && !highlighted) line += " ";
	/* have to insert an extra space so that the first digit in the
	 * name doesn't look like part of the number following the 'C'.
	 */
    line += name;
    return line;
}

void SubMenu::AddToForm(MenuForm *mf) {
  if(attribute(MOPT_SECRET)) return;
  fl_add_browser_line(mf->MF_submenu_list, browserline().charstar());
  SubMenu *thees = this;	// some compilers object falsely
  mf->submenus.grow(1, thees);
}

void Command::AddToForm(MenuForm *mf) {
  if(attribute(MOPT_SECRET)) return;
  fl_add_browser_line(mf->MF_command_list, browserline().charstar());
  Command *thees = this;
  mf->commands.grow(1, thees);
}

void Variable::AddToForm(MenuForm *mf) {
  if(attribute(MOPT_SECRET)) return;
  CharString line(browserline() + " = " + tostring());
  fl_add_browser_line(mf->MF_variable_list, line.charstar());
  Variable *thees = this;
  mf->variables.grow(1, thees);
  if(!varobj) {
    fl_freeze_form(mf->the_form);
    fl_addto_form(mf->the_form);
    varobj = create_varobject(this, mf, VO_PROACTIVE,
			      mf->dummyinput->x, mf->dummyinput->y,
			      mf->dummyinput->w, mf->dummyinput->h);
    varobj->hide();
    fl_end_form();
    fl_unfreeze_form(mf->the_form);
  }
}

// --------------------------------------------------------- //

// call back routines

// first, a routine that gets the index in the list of menu options,
// taking secret options into account. Also correct for the
// fortran-like indexing of the xforms browser lines.

template <class T>
int optionindex(const Vec<T*> &optionlist, int chosen) {
  for(int i=0; i<optionlist.capacity(); i++) {
    if(!optionlist[i]->attribute(MOPT_SECRET)) chosen--;
    if(chosen == 0) return i;
  }
  cerr << "Error in optionindex!" << endl;
  return -1;   // shouldn't ever get here!
}

//-------------------------------------------------------//

// submenu callbacks

void MFCB_submenuhandler(FL_OBJECT *obj, long ptr) {
  if(Form::exists(ptr))
    ((MenuForm *) ptr)->submenuhandler(obj);
}

void MenuForm::submenuhandler(FL_OBJECT *obj) {
  int which = optionindex(submenus, fl_get_browser(obj));
  if(which < 0) return;   // nothing selected
  SubMenu *submenu = submenus[which];
  
  if(!submenu->form)			// create form if necessary
    submenu->form =
      new MenuForm(submenu->menu, formwaiter().errhm,
		   path + submenu->name, submenu->helpstring);
  submenu->form->previous = this;	// remember where it came from
  gotoform(submenu->form);
}

//-------------------------------------------------------//

// command callbacks

/* Check for double clicks on the command list by setting a timer on
 * the first click. If the timer expires before another click is
 * found, then the click was a single click.  If another click is
 * found before the timer expires, then it's a double click.
 */

#include <signal.h>
#include <sys/time.h>

// Pass signals along to XForms. This handler is installed by
// interrupt_init().  If interrupt_init() hasn't been called, then
// signals are handled directly by XForms.
#ifdef VARARG_SIGNALS
void MenuForm::signalhandler(int sig, ...) {
  fl_signal_caught(sig);
  signal(sig, MenuForm::signalhandler);	// reinstall handler
}
#else
void MenuForm::signalhandler(int sig) {
  fl_signal_caught(sig);
  signal(sig, MenuForm::signalhandler);	// reinstall handler
}
#endif

void MFCB_commandhandler(FL_OBJECT *obj, long ptr) {
  if(Form::exists(ptr))
    ((MenuForm *) ptr)->commandhandler(obj);
}

static int clickcount = 0; // how many clicks have been processed?
static MenuForm *clickedform = 0;
static int clickedline;


void MenuForm::singleclick_callback(int, void *ptr) {
  // timer expired without receiving a second click
  ((MenuForm*) ptr)->do_click(1);
  fl_remove_signal_callback(SIGALRM);
  fl_add_signal_callback(SIGALRM, ignorealarm, 0);
  clickcount = 0;
  clickedform = 0;
  clickedline = 0;
}

void MenuForm::ignorealarm(int, void*) {}

void MenuForm::commandhandler(FL_OBJECT *obj) {
  struct itimerval timer, oldtimer;
  clickcount++;
  if(clickcount == 1) { // wait for second click
    clickedform = this;
    clickedline = fl_get_browser(obj);
    timer.it_value.tv_usec = (long) (1000000*doubleclicktime);
    timer.it_value.tv_sec = 0;
    timer.it_interval.tv_usec = 0;
    timer.it_interval.tv_sec = 0;
    fl_remove_signal_callback(SIGALRM);
    fl_add_signal_callback(SIGALRM, singleclick_callback, this);
    setitimer(ITIMER_REAL, &timer, &oldtimer);
  }
  else {  // got second click!
    // turn off timer
    timer.it_value.tv_usec = 0;
    timer.it_value.tv_sec = 0;
    setitimer(ITIMER_REAL, &timer, &oldtimer);
    fl_remove_signal_callback(SIGALRM);
    // sometimes the signal is still generated although the timer has
    // supposedly been turned off, so there should always be a signal
    // handler installed.
    fl_add_signal_callback(SIGALRM, ignorealarm, 0);

    int secondline = fl_get_browser(obj);
    if(secondline == clickedline && clickedform == this)
      do_click(2);
    clickedform = 0;
    clickedline = 0;
    clickcount = 0;
  }
}

void MenuForm::do_click(int nclicks) {
  if((clickmode == SINGLE_NOW && nclicks == 1)
     || (clickmode == DOUBLE_NOW && nclicks == 2))
    execute_fn_now();
  else
    open_fn_window();
}


void MenuForm::execute_fn_now() {
  int which = optionindex(commands, clickedline);
  Command *thecmd = commands[which];
  thecmd->drawactive = 1;
  fl_replace_browser_line(MF_command_list, clickedline,
			  thecmd->browserline().charstar());
  fl_redraw_form(the_form);
  XFlush(fl_get_display()); // make sure redrawn line is displayed now
  /* when the line is redrawn after the command is finished, we
   * can't assume that's it's still in the same location in the
   * browser. So we have to save a copy of the line to find out
   * which to redraw.
   */
  CharString curline(fl_get_browser_line(MF_command_list, clickedline));
  
  // execute command
  thecmd->pronto(formwaiter(), path + thecmd->name);
  
  // revert to original color
  thecmd->drawactive = 0;
  int nlines = fl_get_browser_maxline(MF_command_list);
  for(int i=1; i<=nlines; i++) {
    if(curline == fl_get_browser_line(MF_command_list, i)) {
      fl_replace_browser_line(MF_command_list, i,
			      thecmd->browserline().charstar());
      fl_redraw_form(the_form);
      XFlush(fl_get_display());
      break;
    }
  }
}

void MenuForm::open_fn_window() {
  int which = optionindex(commands, clickedline);
  CommandForm *cmdform = commands[which]->makecmdform(path);
  cmdform->show(0);
}

//------------------------------------------------------------//

// variable callbacks

void MFCB_variablehandler(FL_OBJECT *obj, long ptr) {
  if(Form::exists(ptr))
    ((MenuForm *) ptr)->variablehandler(obj);
}

void MenuForm::variablehandler(FL_OBJECT *obj) {
  fl_freeze_form(the_form);
  // hide last variable's fl_object
  if(current_varobj) {
    current_varobj->hide();
    current_varobj->vsbl = 0;
  }
  current_varobj = 0;

  // get new variable
  selectedvar = optionindex(variables, fl_get_browser(obj));
  if(selectedvar < 0) {  // nothing selected
    fl_set_object_label(MF_varhelp, "");
  }
  else {
    Variable *var = variables[selectedvar];

    // show new variable's fl_object
    current_varobj = var->varobj;
    current_varobj->show();
    current_varobj->vsbl = 1;
    
    // display help string
    fl_set_object_label(MF_varhelp, var->helpstring.charstar());
    fl_redraw_form(the_form);
  }
  fl_unfreeze_form(the_form);
}

// routine called by variable's callback fn
void MenuForm::setvariable(Variable *var, const CharString &val) {
  if(formwaiter().macromode) {
    formwaiter().record(path.concat() + " " + var->name + "=" + val);
    formwaiter().recorddelim();
  }
  else {
    var->fromstring(val, formwaiter());
    
    // update value in variable browser and print to message window
    CharString line = updatevariable(var, selectedvar);
    // update other instances of this same variable:
    VariableRegistry::vr().call_function(var, &Variable::update_forms);
    if(formwaiter().echo)
      formwaiter().msout << ms_prompt << name << " " << line
			 << endl << ms_normal;
    if(!var->attribute(MOPT_NOLOG))
      formwaiter().log(path.concat() + " " + line);
    
    // call hook function
    if(var->hook) {
      formwaiter().path = path + line;
      formwaiter().makecurrent();
      (var->hook)(var->name);
    }
  }
}

// update varible in browser, when its position in browser is known

CharString MenuForm::updatevariable(Variable *var, int which) {
  // use tostring() rather than val to get standard form
  CharString line(var->name + " = " + var->tostring());
  fl_freeze_form(the_form);
  fl_replace_browser_line(MF_variable_list, which+1, line.charstar());
  fl_redraw_form(the_form);
  fl_unfreeze_form(the_form);
  return line;
}

// update variable in browser when its position is not known

void MenuForm::updatevariable(Variable *var) {
  for(int i=0; i<variables.capacity(); i++)
    if(var == variables[i]) {
      updatevariable(var, i);
      if(var->varobj->visible())
	var->varobj->show();	// refreshes value displayed
      return;
    }
}

void MFCB_close_form(FL_OBJECT*, long ptr) {
    if(Form::exists(ptr))
	((MenuForm *) ptr)->close_form();
}

void MenuForm::close_form() {
  if(Nforms() == 1) {
    if(!isroot)
      gotoform(rootform);
  }
  else {
    hide();
  }
}

// called when "close" is chosen from the window manager menu
int MenuForm::closemenuform(FL_FORM*, void *ptr) { 
  ((MenuForm *) ptr)->close_form();
  return FL_IGNORE;
}

void MFCB_go_back(FL_OBJECT*, long ptr) {
    if(Form::exists(ptr))
	((MenuForm *) ptr)->go_back();
}

void MenuForm::go_back() {
  if(isroot) {    // "Back" is "Quit" on the root menu form
    if(garcon()->quitcmd) {
      if((*garcon()->quitcmd)())
	exit(0);
    }
    else {
      if(formwaiter().question("Do you really want to quit?",1))
	exit(0);
    }
  }
  else
    gotoform(previous);
}

void MFCB_go_home(FL_OBJECT*, long ptr) {
    if(Form::exists(ptr))
	((MenuForm *) ptr)->go_home();
}

void MenuForm::go_home() {
    gotoform(rootform);
}

void MFCB_freeze(FL_OBJECT *obj, long ptr) {
    if(Form::exists(ptr))
	((MenuForm *) ptr)->freeze(obj);
}

void MFCB_record(FL_OBJECT *obj, long ptr) {
    if(Form::exists(ptr)) {
	if(fl_get_button(obj))
	    ((MenuForm *) ptr)->record_on(obj);
	else
	    ((MenuForm *) ptr)->record_off(obj);
    }
}

static int recording = 0;

void MenuForm::record_on(FL_OBJECT *obj) {
    if(recording) { // only record on one menu at once
	fl_set_button(obj, 0);
    }
    else {
	fl_set_object_label(obj, "Stop");
	formwaiter().startmacro(the_menu, path);
	recording = 1;
    }
}

void MenuForm::record_off(FL_OBJECT *obj) {
    if(!recording) {	// don't stop if this isn't the right menu
	fl_set_button(obj, 1);
    }
    else {
	fl_set_object_label(obj, "Record");
	formwaiter().endmacro();
	recording = 0;
    }
}

// ------------------------------------------------------------ //


static const int DEFAULTFONT = FL_FIXED_STYLE;
static const int DEFAULTSIZE = 12;  // in points
const int MessageForm::fudge = 40;  // includes width of vertical scroll bar

static FL_OBJECT *fontmenu;
void MessageForm::addfonts(const char *fontname) {
  fl_addto_menu(fontmenu, fontname);
}

static int closeit(FL_FORM*, void *ptr) {
  ((MessageForm*) ptr)->hide();
  return FL_IGNORE;
}

Vec<MessageForm*> MessageForm::allmessageforms;

MessageForm::MessageForm(const CharString &nm)
  : Form(nm),
    lastlinecomplete(1),
    selectedfont(DEFAULTFONT),
    selectedsize(DEFAULTSIZE)
{
  allmessageforms.grow(1, this);
  create_form_messageform(this);

  fl_set_browser_fontsize(MG_text, selectedsize);
  fl_set_browser_fontstyle(MG_text, selectedfont);
  fontmenu = MG_font;
  fl_enumerate_fonts(addfonts, 1);
  fl_set_menu_item_mode(MG_font, selectedfont+1, FL_PUP_CHECK);

  fl_set_counter_return(MG_size, FL_RETURN_END_CHANGED);
  fl_set_counter_value(MG_size, DEFAULTSIZE);
  fl_set_counter_bounds(MG_size, 1.0, 40.0);
  fl_set_counter_step(MG_size, 1.0, 1.0);
  fl_set_counter_precision(MG_size, 0);

  fl_set_object_color(MG_text, COL_MG_TEXTBG, 0);
  fl_set_object_lcol(MG_text, COL_MG_TEXT);
  fl_set_object_color(background, COL_MG_BACKGROUND, 0);

  fl_set_object_shortcutkey(MG_home, XK_Home);
  fl_set_form_atclose(this->the_form, closeit, this);
}

MessageForm::~MessageForm() {
  allmessageforms.remove(this);
}

void MessageForm::makevisible() {
    if(the_form->visible) return;
    fl_show_form(the_form, FL_PLACE_FREE, FL_FULLBORDER, name.charstar());
    Nforms()++;
}

void MessageForm::addtext(const char *str) {
  /* add lines of text. fl_addto_browser doesn't like \n's, so look for
   * them and add each bit separately. Furthermore, remember if the last
   * line ended with a \n, so it can be appended to if necessary.
   */
  fl_freeze_form(the_form);
    
  char line[1024];
    
  int nlines;
  if(!lastlinecomplete && (nlines = fl_get_browser_maxline(MG_text))) {
    // remove incomplete line from browser and append new string
    strcpy(line, fl_get_browser_line(MG_text, nlines));
    strcat(line, str);
    fl_delete_browser_line(MG_text, nlines);
  }
  else
    strcpy(line, str);

  char *ptr = line;
    
  char *newline;
  do {
    newline = strchr(ptr, '\n');
    if(!newline) {
      // any remaining stuff in str is an incomplete line
      if(*ptr != '\0') {
	fl_addto_browser(MG_text, ptr);
	lastlinecomplete = 0;
      }
    }
    else {
      *newline = '\0';
      fl_addto_browser(MG_text, ptr);
      lastlinecomplete = 1;
      ptr = ++newline;	// first char after '\n'
    }
  } while(newline);
  
  fl_unfreeze_form(the_form);
}

void MessageForm::setcolor(int color) {
  if(lastlinecomplete)		// only change color at beginning of line!
    addtext("@C" + to_charstring(color) + "@N");
  // @C changes the color, @N is only used as a terminator so that
  // if the next character is a digit it isn't interpreted as part
  // of the color.
}

void MGCB_text(FL_OBJECT*, long ptr) {
  if(Form::exists(ptr))
    ((MessageForm *) ptr)->addtext("Shouldn't do this!\n");
}

void MGCB_clear(FL_OBJECT*, long ptr) {
  if(Form::exists(ptr))
    ((MessageForm *) ptr)->cleartext();
}

void MessageForm::cleartext() {
  fl_clear_browser(MG_text);
}

void MGCB_close(FL_OBJECT*, long ptr) {
  if(Form::exists(ptr))
    ((MessageForm *) ptr)->hide();
}

void MGCB_save(FL_OBJECT*, long ptr) {
  if(Form::exists(ptr))
    ((MessageForm *) ptr)->save();
}

void MessageForm::save() {
  // open a file with the file selection box
  static char dir[100] = "";
  static char pat[100] = "";
  static char dflt[100] = "logfile";
  const char *filename = fl_show_file_selector("log file", dir, pat, dflt);
  if(!filename) return;   // cancelled
  FILE *file = fopen(filename, "w");
  
  int nlines = fl_get_browser_maxline(MG_text);
  for(int i=1; i<=nlines; i++) {
    const char *line = fl_get_browser_line(MG_text, i);
    // skip browser formating symbols of the form "@X" 
    while(line[0] == '@') {	// skip @
      line++;
      if(line[0] == 'C') {	// "C<number>" sets the line color
	line++;
	while(isdigit(line[0]))
	  line++;
      }
      else if(line[0] == '@')	// "@@" prints as "@"
	break;
      else
	line++;			// skip X
    }
    fprintf(file, "%s\n", line);
  }
  fclose(file);
}

void MGCB_font(FL_OBJECT*, long ptr) {
  if(Form::exists(ptr))
    ((MessageForm *) ptr)->font();
}

void MessageForm::font() {
  fl_freeze_form(the_form);
  int which = fl_get_menu(MG_font);
  if(which <= 0) return;
  fl_set_browser_fontstyle(MG_text, which-1);
  fl_set_menu_item_mode(MG_font, selectedfont+1, FL_PUP_NONE);
  selectedfont = which-1;
  fl_set_menu_item_mode(MG_font, selectedfont+1, FL_PUP_CHECK);
  fl_unfreeze_form(the_form);
}

void MGCB_size(FL_OBJECT*, long ptr) {
  if(Form::exists(ptr))
    ((MessageForm *) ptr)->size();
}

void MessageForm::size() {
  fl_freeze_form(the_form);
  selectedsize = int(fl_get_counter_value(MG_size));
  fl_set_browser_fontsize(MG_text, selectedsize);
  fl_unfreeze_form(the_form);
}

void MGCB_home(FL_OBJECT*, long ptr) {
  if(Form::exists(ptr))
    ((MessageForm*) ptr)->home();
}

void MessageForm::home() {
  formwaiter().show_homeform();
}

// -------------------------------------------------------------- //

/* When using fl_get_input, it is important not to modify the string
 * it returns. This routine makes a copy of the string returned from
 * fl_get_input, so it's safe.
 */
 
CharString forminputstring(FL_OBJECT *obj) {
  return CharString(fl_get_input(obj));
}

// -------------------------------------------------------------- //

// my own version of fl_question

int FormWaiter::question(const CharString &qstr, int dflt) {
  fl_deactivate_all_forms();
  QuestionForm qf(qstr, dflt);
  qf.makevisible();
  do
    fl_check_only_forms();
  while(!qf.done());
  fl_activate_all_forms();
  return qf.answer();
}

QuestionForm::QuestionForm(const CharString &question, const TrueFalse &dflt)
  : done_(0), Form("?")
{
  create_form_questionform(this);
  fl_set_object_color(background, COL_BACKGROUND, 0);
  fl_set_object_color(question_text, COL_BACKGROUND, 0);
  fl_set_object_label(question_text, question);
  if(dflt)
    fl_set_form_hotobject(the_form, yes_button);
  else
    fl_set_form_hotobject(the_form, no_button);
}

void QuestionForm::makevisible() {
  if(the_form->visible) return;
  fl_show_form(the_form, FL_PLACE_HOTSPOT, FL_TRANSIENT, name.charstar());
  Nforms()++;
}

void QuestionForm::yes() {
  answer_ = 1;
  done_ = 1;
}

void QuestionForm::no() {
  answer_ = 0;
  done_ = 1;
}

void QFCB_yesbutton(FL_OBJECT *, long ptr) {
  if(Form::exists(ptr))
    ((QuestionForm*) ptr)->yes();
}

void QFCB_nobutton(FL_OBJECT *, long ptr) {
  if(Form::exists(ptr))
    ((QuestionForm*) ptr)->no();
}

// -------------------------------------------------------------- //

// my own version of fl_show_input

StringForm::StringForm(const CharString &prompt, const CharString &dflt)
    : Form("rsvp"), notyet(1)
{
  create_form_stringform(this);
  fl_set_object_label(SF_prompt, prompt.charstar());
  fl_set_object_color(background, COL_BACKGROUND, 0);
  fl_set_input(SF_input, dflt.charstar());
}

void StringForm::makevisible() {
  if(the_form->visible) return;
  fl_show_form(the_form, FL_PLACE_MOUSE, FL_TRANSIENT, name.charstar());
  Nforms()++;
}

void SFCB_okbutton(FL_OBJECT*, long ptr) {
  if(Form::exists(ptr))
    ((StringForm *) ptr)->ok();
}

void StringForm::ok() {
    // see comment in commands.C in CommandMForm::perform()
    // hitting ok generates an input event, which this line catches:
    //(void) fl_check_only_forms();
    answer = forminputstring(SF_input);
    notyet = 0;
}

#ifdef FORMFORM
CharString FormWaiter::getstring(const CharString &prompt,
					const CharString &dflt) {
    CharString answer(fl_show_input(prompt.charstar(), dflt.charstar()));
    return answer;
}
#endif // FORMFORM

CharString FormWaiter::getstring(const CharString &prompt,
					const CharString &dflt) {
  fl_deactivate_all_forms();
  StringForm sf(prompt, dflt);
  sf.makevisible();
  do
    fl_check_only_forms();
  while(sf.notyet);
  fl_activate_all_forms();
  return sf.answer;
}

//-----------------------------------

PauseForm *PauseForm::pauseform = 0;

void PauseForm::pause() {	// static
  static PauseForm *pauseform = 0;
  fl_deactivate_all_forms();
  if(!pauseform) {
    pauseform = new PauseForm;
    pauseform->show(0);
  }
  pauseform->wait();
  fl_activate_all_forms();
}

//  void PauseForm::makevisible() {
//    if(fl_form_is_visible(the_form)) return;
//    fl_show_form(the_form, FL_PLACE_MOUSE, FL_TRANSIENT, name.charstar());
//    Nforms()++;
//  }

void PauseForm::wait() {
  FL_OBJECT *obj;
  fl_activate_form(the_form);
  if(!fl_form_is_visible(the_form))
    fl_show_form(the_form, FL_PLACE_GEOMETRY, FL_FULLBORDER, name.charstar());
  fl_raise_form(the_form);
  while(obj = fl_do_forms()) {
    if(obj == button_continue) {
      fl_deactivate_form(the_form);
      fl_hide_form(the_form);
      return;
    }
  }
}

PauseForm::PauseForm()
  : Form("pause")
{
  create_form_pauseform(this);
}

#endif // MENUGFX
