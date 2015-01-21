// -*- C++ -*-
// $RCSfile: menustream.C,v $
// $Revision: 1.1 $
// $Author: langer $
// $Date: 2000-12-22 14:11:10 $

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */

// These routines are vaguely based on the wstream class by Dietmar
// Kuehl.  See http://www.informatik.uni-konstanz.de/~kuehl/iostream/

#include "charstring.h"
#include "menuform.h"
#include "menustream.h"

static const int bufsize = 1024;

const MenuStream::Mode MenuStream::NORMAL(1);
const MenuStream::Mode MenuStream::ERROR(2);
const MenuStream::Mode MenuStream::INFO(4);
const MenuStream::Mode MenuStream::PROMPT(8);

MenuBuf::MenuBuf(MenuStream *ms)
  : mstream(ms)
{
  char *ptr = new char[bufsize];
  setp(ptr, ptr + bufsize);	// "put" buffer
  setg(0, 0, 0);		// "get" buffer (unused)
}

MenuBuf::~MenuBuf() {
  sync();
  delete [] pbase();
}

int MenuBuf::overflow(int c) {
  put_buffer();
  if(c != EOF)
    sputc(c);
  return 0;
}

int MenuBuf::sync() {
  put_buffer();
  return 0;
}

void MenuBuf::put_buffer() {
  if(pbase() != pptr()) {	// check for null string
    // copy string out of buffer
    int len = pptr() - pbase();
    char *buffer = new char[len+1];
    strncpy(buffer, pbase(), len);
    buffer[len] = 0;

    putstring(buffer);		// display contents of buffer

    if(mstream->ms_mode & mstream->tee_mode) {
      FILE *teefile = mstream->tee_file();
      if(teefile)
	fprintf(teefile, buffer);
    }

    // reset
    setp(pbase(), epptr());
    delete [] buffer;
  }
}

//--------------------------------------------------//

#ifdef MENUGFX

MessageForm *FormMenuBuf::msgform(0);

void FormMenuBuf::putstring(const char *str) {
  if(!msgform)
    msgform = new MessageForm("messages");
  if(mstream->ms_mode == MenuStream::ERROR)
    msgform->setcolor(1);	// FL_RED
  else if(mstream->ms_mode == MenuStream::PROMPT)
    msgform->setcolor(4);	// FL_BLUE
  else if(mstream->ms_mode == MenuStream::INFO)
    msgform->setcolor(22);	// FL_DARKTOMATO
  else if(mstream->ms_mode == MenuStream::NORMAL)
    msgform->setcolor(0);	// FL_BLACK

  msgform->addtext(str);
  msgform->makevisible();

  if(mstream->ms_mode == MenuStream::ERROR)
    fl_raise_form(msgform->the_form);
}

#endif // MENUGFX

void TextMenuBuf::putstring(const char *str) {
  cout << str << flush;
}

void FileMenuBuf::putstring(const char *str) {
  fputs(str, file);
}

FileMenuStream::~FileMenuStream() {
  fflush(file);
}

//--------------------------------------------------//

int MenuStream::tee(const CharString &filename, const char *m) {
  teestop();
  teefile = fopen(filename, m);
  if(!teefile) return 0;
  setvbuf(teefile, 0, _IOLBF, 0); // line buffering for output
  return 1;
}

void MenuStream::teestop() {
  if(teefile)
    fclose(teefile);
  teefile = 0;
}

void MenuStream::toggle_tee_mode(Mode whichmode, int on) {
  if(on)
    tee_mode |= whichmode;
  else
    tee_mode &= ~whichmode;
}

//------------------------------------------------//

// manipulators

ostream &ms_normal(ostream &ms) { 
  // flush so that previous output is handled before mode is changed
  ms << flush;
#ifdef OLDCASTS
  ((MenuStream&) ms).ms_mode = MenuStream::NORMAL;
#else
  dynamic_cast<MenuStream&>(ms).ms_mode = MenuStream::NORMAL;
#endif
  return ms;
}

ostream &ms_error(ostream &ms) {
  ms << flush;
#ifdef OLDCASTS
  ((MenuStream&) ms).ms_mode = MenuStream::ERROR;
#else
  dynamic_cast<MenuStream&>(ms).ms_mode = MenuStream::ERROR;
#endif
  return ms;
}

ostream &ms_prompt(ostream &ms) {
  ms << flush;
#ifdef OLDCASTS
  ((MenuStream&) ms).ms_mode = MenuStream::PROMPT;
#else
  dynamic_cast<MenuStream&>(ms).ms_mode = MenuStream::PROMPT;
#endif
  return ms;
}

ostream &ms_info(ostream &ms) {
  ms << flush;
#ifdef OLDCASTS
  ((MenuStream&) ms).ms_mode = MenuStream::INFO;
#else
  dynamic_cast<MenuStream&>(ms).ms_mode = MenuStream::INFO;
#endif
  return ms;
}

