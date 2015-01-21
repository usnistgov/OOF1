// -*- C++ -*-
// $RCSfile: checkmask.h,v $
// $Revision: 1.3 $
// $Author: langer $
// $Date: 2000-09-06 20:11:25 $

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */

#define checkmask_width 16
#define checkmask_height 16
#define checkmask_x_hot 6
#define checkmask_y_hot 14
static unsigned char checkmask_bits[] = {
   0x00, 0x60, 0x00, 0xf0, 0x00, 0xf8, 0x00, 0x7c, 0x00, 0x3e, 0x1c, 0x1f,
   0xbe, 0x0f, 0xbe, 0x07, 0xfc, 0x07, 0xfc, 0x03, 0xf8, 0x03, 0xf8, 0x03,
   0xf0, 0x01, 0xf0, 0x01, 0xe0, 0x00, 0x40, 0x00};
