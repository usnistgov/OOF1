// -*- C++ -*-
// $RCSfile: truefalse.C,v $
// $Revision: 1.3 $
// $Author: langer $
// $Date: 2000-09-06 20:11:50 $


/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


#include "truefalse.h"
#include "menuDef.h"
#include "word.h"

const TrueFalse TF_TRUE(1);
const TrueFalse TF_FALSE(0);

static int istrue(const char *str) {
    return !strcmp(str, "1")   || !strcmp(str, "y")    ||
	!strcmp(str, "Y")   || !strcmp(str, "t")    ||
	!strcmp(str, "T")   || !strcmp(str, "true") ||
	!strcmp(str, "True")|| !strcmp(str, "TRUE") ||
	!strcmp(str, "yes") || !strcmp(str, "YES")  ||
	!strcmp(str, "da") || !strcmp(str, "vrai")  ||
	!strcmp(str, "dwei") || !strcmp(str, "totally")  ||
	!strcmp(str, "way") || !strcmp(str, "ja") ||
	!strcmp(str, "Yes") || !strcmp(str, "sure") ||
	!strcmp(str, "on")  || !strcmp(str, "ON")   ||
	!strcmp(str, "ok")  || !strcmp(str, "OK")   ||
	!strcmp(str, "what_the_hell") || !strcmp(str, "yeah") ||
	!strcmp(str, "yep") || !strcmp(str, "yup")  ||
	!strcmp(str, "fine");
}

static int isfalse(const char *str) {
    return !strcmp(str, "0")	|| !strcmp(str, "n")	    ||
	!strcmp(str, "N")	|| !strcmp(str, "no")	    ||
	!strcmp(str, "No")	|| !strcmp(str, "NO")	    ||
	!strcmp(str, "nyet")	|| !strcmp(str, "faux")	    ||
	!strcmp(str, "budwei")	|| !strcmp(str, "nein")	    ||
	!strcmp(str, "f")	|| !strcmp(str, "F")	    ||
	!strcmp(str, "false")	|| !strcmp(str, "False")  ||
	!strcmp(str, "FALSE")	|| !strcmp(str, "no_way") ||
	!strcmp(str, "nope")	|| !strcmp(str, "naw")    ||
	!strcmp(str, "off")	|| !strcmp(str, "OFF");
}

ostream & operator<<(ostream &os, const TrueFalse &flag) {
    if(flag.flag)
	os << "true";
    else
	os << "false";
    return os;
}

istream &operator>>(istream &is, TrueFalse &flag) {
    Word str;
    is >> str;
    if(istrue(str))
	flag = TF_TRUE;
    else if(isfalse(str))
	flag = TF_FALSE;
    else {
	is.clear(ios::badbit|is.rdstate());
	garcon()->msout << ms_error
			<< "=== Illegal value \"" << str
			<< "\" for boolean variable!!"
			<< endl << ms_normal;
    }
    return is;
}
