// -*- C++ -*-
// $RCSfile: charstring.h,v $
// $Revision: 1.1 $
// $Author: langer $
// $Date: 2000-12-22 14:11:09 $


/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


#ifndef CHARSTRING_H
#define CHARSTRING_H

#include <string.h>
#include <iostream.h>

class CharString {
    protected:
	char *str;
    public:
    // null constructor
	CharString() : str(new char[1]) { str[0] = '\0'; }
    // construct empty string with given length
	CharString(int i) : str(new char[i]) { str[0] = '\0'; }
    // construct given string
	CharString(const char *s) : str(new char[s?(strlen(s)+1):1]) {
	    if(s)
		strcpy(str, s);
	    else
		str[0] = '\0';
	}
	CharString(const char c) : str(new char[2]) {
	    str[0] = c;
	    str[1] = '\0';
	}
    // copy constructor
	CharString(const CharString &s) : str(new char[strlen(s.str)+1]) {
	    strcpy(str, s.str);
	}
    // destructor
	virtual ~CharString() { if(str) delete [] str; }
    // conversion to const char*
	typedef const char *cchar;
	operator cchar() const { return str; }
    // conversion to char* is provided only because some library routines
    // have const char* args that aren't declared const
	char *charstar() const { return str; }
    // conversion to char
	char &operator[](const int i) { return str[i]; }
	const char &operator[](const int i) const { return str[i]; }
    // extraction
        CharString head(int) const; // extract beginning n characters
        CharString tail(int) const; // extract final n characters
    // assignment operators
	CharString &operator=(const CharString &);
	CharString &operator=(const char*);
	CharString &operator=(const char);
    // concatenation with CharString and char*
	CharString &operator+=(const CharString &);
	CharString &operator+=(const char *);
	CharString &operator+=(const char);
	friend CharString operator+(const CharString&, const CharString&);
	friend CharString operator+(const char*, const CharString&);
	friend CharString operator+(const CharString&, const char*);
	friend CharString operator+(const CharString&, const char);
	friend CharString operator+(const char, const CharString&);
	friend size_t strlen(const CharString &);
    // append with an intervening space
	CharString spaceappend(const CharString &) const;
    // replace spaces with underscores
       CharString despace(char substitute = '_') const;
    // unary not
	int operator!() const { return str[0]==0 ? 1 : 0; }
    // equality
	friend int operator==(const CharString &s1, const CharString &s2) {
	    return !strcmp(s1.str, s2.str);
	}
	friend int operator==(const CharString &s1, const char *s2) {
	    return !strcmp(s1.str, s2);
	}
	friend int operator==(const char *s1, const CharString &s2) {
	    return !strcmp(s1, s2.str);
	}
	friend int operator!=(const CharString &s1, const CharString &s2) {
	    return strcmp(s1.str, s2.str);
	}
	friend int operator!=(const CharString &s1, const char *s2) {
	    return strcmp(s1.str, s2);
	}
	friend int operator!=(const char *s1, const CharString &s2) {
	    return strcmp(s1, s2.str);
	}
	
	friend ostream &operator<<(ostream &os, const CharString &cs);
	friend istream &operator>>(istream &is, CharString &cs);
};

// a couple of utilities...
const char *skipspace(const CharString&);
void skipspace(const char**);
void skipspace(char**);
const char *skipspace(const char*);
char *skipspace(char*);


// this is included separately so that the prelinker knows where to
// find the template body
#include "tocharstring.h"

#endif
