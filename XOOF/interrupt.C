// -*- C++ -*-
// $RCSfile: interrupt.C,v $
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

// Fork an interrupt handler

#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>

static pid_t parent;
static FL_FORM *interruptform = 0;
static FL_OBJECT *stopbutton;
static FL_OBJECT *counter;
static int depth = 0;
static int awake = 0;

static void setlabel() {
  ++depth;
  char label[10];
  sprintf(label, "%d", n);
  fl_set_object_label(counter, label);
}


static void wakeup(int, void*) { // signal was received from parent process
  ++depth;
  setlabel();
  if(!awake) {			// wake up
    awake = 1;
    fl_show_form(interruptform, FL_PLACE_MOUSE, FL_TRANSIENT, "Stop");
  }
}

static void gotobed(int, void*) { // signal was received from parent process
  --depth;
  if(depth == 0) {
    awake = 0;
    fl_hide_form(interruptform);
  }
  setlabel();
}

static void callback(FL_OBJECT*, long) { // button was pressed
  --depth;
  if(depth == 0)
    fl_hide_form(interruptform);
  else
    setlabel();
  // send signal to parent process
  kill(parent, SIGUSR1);
}

static void gui_interrupt_handler(int argc, char *argv[]) {
  parent = getppid();

  // Create form
  FL_OBJECT *obj;
  interruptform = fl_bgn_form(FL_NO_BOX, 160, 180);
  fl_add_box(FL_UP_BOX,0,0,160,180,"");
  stopbutton = obj = fl_add_button(FL_NORMAL_BUTTON,10,10,140,140,"STOP");
    fl_set_object_color(obj,FL_RED,62);
    fl_set_object_lsize(obj,FL_HUGE_SIZE);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);
    fl_set_object_callback(obj,callback,0);
  counter = obj = fl_add_text(FL_NORMAL_TEXT,10,150,140,20,"1");
    fl_set_object_boxtype(obj,FL_NO_BOX);
    fl_set_object_color(obj,FL_COL1,FL_COL1);
    fl_set_object_lsize(obj,11);
    fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
  fl_end_form();  

  // Start forms...
  fl_initialize(&argc, argv, "XFormMenu", 0, 0);

  // SIGUSR1 from parent means it's time to start working
  fl_add_signal_callback(SIGUSR1, wakeup);
  // SIGUSR2 means it's time to stop
  fl_add_signal_callback(SIGUSR2, gotobed);

  for(;;) fl_do_forms();
}

void interrupt_init(int argc, char *argv[]) {
  pid_t pid = fork();
  switch(pid) {
  case -1:
    perror("can't fork interrupt handler");
    return 0;
  case 0:			// child
    gui_interrupt_handler(argc, argv); // doesn't return
  }
  return pid;
}

