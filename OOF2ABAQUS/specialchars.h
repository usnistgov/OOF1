// -*- C++ -*-
// $RCSfile: specialchars.h,v $
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


#ifndef SPECIALCHARS_H
#define SPECIALCHARS_H

#include "ctype.h"

const char DELIMITER = ';';
const char COMMENTCHAR = '%';
const char BEGINGROUP = '(';
const char ENDGROUP = ')';
const char QUOTE = '"';
const char HELPCHAR = '?';
const char EQUALS = '=';
const char COMMA = ',';
const char ABORTCHAR = '<';
const char BEGINMACRO = '{';
const char ENDMACRO = '}';
const char SPACE = ' ';

const unsigned char SPL = 01;	// special character
const unsigned char IGN = 02;	// ignored character (when leading args)
const unsigned char TRM = 04;	// terminates arg list

class SpecialChars {
    friend inline int is_special(const char c);
    friend inline void makespecial(const char c);
//  friend void makeordinary(const char c);
    friend inline int isignored(const char c);
    friend inline void ignorechar(const char c);
    friend inline void noticechar(const char c);
    friend inline int isterminal(const char c);
    friend inline void maketerminal(const char c);
    private:
	unsigned char ctype[128];
    public:
	SpecialChars();
};

extern SpecialChars spx;

inline int is_special(const char c) { return isascii(c) && (SPL & spx.ctype[c]); }
inline void makespecial(const char c) { spx.ctype[c] |= SPL; }
//inline void makeordinary(const char c) { spx.ctype[c] &= ~SPL; }
inline int isignored(const char c) { return isascii(c) && (IGN & spx.ctype[c]); }
inline void ignorechar(const char c) { spx.ctype[c] |= IGN; }
inline void noticechar(const char c) { spx.ctype[c] &= ~IGN; }
inline int isterminal(const char c) { return isascii(c) && (TRM & spx.ctype[c]); }
inline void maketerminal(const char c) { spx.ctype[c] |= TRM; }

#endif
