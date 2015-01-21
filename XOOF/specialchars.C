// -*- C++ -*-
// $RCSfile: specialchars.C,v $
// $Revision: 1.3 $
// $Author: langer $
// $Date: 2000-09-06 20:11:15 $


/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


/* is_special(char c) is modelled after the ctype routines. This is fast
 * and lets specialness be turned on and off easily. Using a class to
 * handle the array of ctype flags lets the constructor do the work of
 * setting up the array. If there will be more than 8 flags to check, 
 * change the array from unsigned char to unsigned int.
 * Here are the special characters:
 */

#include "specialchars.h"

SpecialChars spx;

SpecialChars::SpecialChars() {
    for(int i = 0; i<128; i++) ctype[i] = 0;
    makespecial(DELIMITER);	maketerminal(DELIMITER);
    makespecial(COMMENTCHAR);	maketerminal(COMMENTCHAR);
    makespecial(BEGINGROUP);	maketerminal(BEGINGROUP);
    makespecial(ENDGROUP);	maketerminal(ENDGROUP);
    makespecial(QUOTE);
    makespecial(HELPCHAR);	maketerminal(HELPCHAR);
    makespecial(EQUALS);
    //ignorechar(EQUALS);
    makespecial(COMMA);
    makespecial(ABORTCHAR);	maketerminal(ABORTCHAR);
    makespecial(BEGINMACRO);	maketerminal(BEGINMACRO);
    makespecial(ENDMACRO);	maketerminal(ENDMACRO);
}

