// -*- C++ -*-
// $RCSfile: menustream.h,v $
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

#ifndef MENUSTREAM_H
#define MENUSTREAM_H

class MenuStream;
class MessageForm;
class CharString;

#include <iostream.h>
#include <stdio.h>

// FormMenuStream and TextMenuStream are both derived from MenuStream,
// which is derived from ostream. FormMenuStream sends its output to
// the Message Window. TextMenuStream sends its output to the
// screen. Both can also tee the output to a file, and can work in
// different modes. The real work is done by the FormMenuBuf and
// TextMenuBuf classes, which are both derived from MenuBuf, which is
// derived from streambuf. MenuBuf keeps a pointer to the MenuStream,
// so that it can find out what the current mode and tee files are.


class MenuBuf : public streambuf {
private:
  void put_buffer(void);	// write out current buffer
protected:
  MenuStream *mstream;
  virtual void putstring(const char*) = 0;
  virtual int overflow(int);	// derived from streambuf
  virtual int sync();		// derived from streambuf
public:
  MenuBuf(MenuStream *ms);
  ~MenuBuf();
};

#ifdef MENUGFX
class FormMenuBuf : public MenuBuf { // write to message window
private:
  FormMenuBuf(MenuStream *ms) : MenuBuf(ms) {}
  virtual void putstring(const char*);
  static MessageForm *msgform;
  friend class FormMenuStream;
};
#endif // MENUGFX

class TextMenuBuf : public MenuBuf { // write to cout
private:
  TextMenuBuf(MenuStream *ms) : MenuBuf(ms) {}
  virtual void putstring(const char*);
  friend class TextMenuStream;
};

class FileMenuBuf : public MenuBuf { // write to file
private:
  FileMenuBuf(MenuStream *ms, FILE *f) : MenuBuf(ms), file(f) {}
  FILE *file;
  virtual void putstring(const char*);
  friend class FileMenuStream;
};

//-----------------------------------------------//

class MenuStream : public ostream {
public:
  typedef unsigned char Mode;
  static const Mode NORMAL, ERROR, PROMPT, INFO;
  //  enum Mode {NORMAL=1, ERROR=2, PROMPT=4, INFO=8};
  MenuStream(MenuBuf *mbuf)
    : ostream(mbuf),
      teefile(0),
      ms_mode(NORMAL),
      tee_mode(NORMAL)
  {}
  void setmode(Mode m) { ms_mode = m; }
  int tee(const CharString &filename, const char *m);
  void teestop();
  void tee_normal(int on) { toggle_tee_mode(NORMAL, on); }
  void tee_error(int on) { toggle_tee_mode(ERROR, on); }
  void tee_prompt(int on) { toggle_tee_mode(PROMPT, on); }
  void tee_info(int on) { toggle_tee_mode(INFO, on); }
private:
  FILE *teefile;		// file for tee'd output
  FILE *tee_file() const { return teefile; }
  Mode ms_mode;
  Mode tee_mode;
  void toggle_tee_mode(Mode, int);
  friend class MenuBuf;
#ifdef MENUGFX
  friend class FormMenuBuf;
#endif // MENUGFX
  friend ostream &ms_normal(ostream&);
  friend ostream &ms_error(ostream&);
  friend ostream &ms_prompt(ostream&);
  friend ostream &ms_info(ostream&);
};

#ifdef MENUGFX
class FormMenuStream : public MenuStream {
public:
  FormMenuStream() : MenuStream(new FormMenuBuf(this)) {}
};
#endif // MENUGFX

class TextMenuStream : public MenuStream {
public:
  TextMenuStream() : MenuStream(new TextMenuBuf(this)) {}
};

class FileMenuStream : public MenuStream {
private:
  FILE *file;
public:
  FileMenuStream(FILE *f)
    : MenuStream(new FileMenuBuf(this, f)), file(f) {}
  ~FileMenuStream();
};

//------------------------------------------------//

// manipulators for changing output modes

ostream &ms_normal(ostream&);
ostream &ms_error(ostream&);
ostream &ms_prompt(ostream&);
ostream &ms_info(ostream&);

#endif
