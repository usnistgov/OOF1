// -*- C++ -*-
// $RCSfile: select_by_color.C,v $
// $Revision: 1.6 $
// $Author: langer $
// $Date: 2000-10-30 19:23:29 $

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */
#include "color.h"
#include "goof.h"
#include "imageform.h"
#include "menuDef.h"
#include "ppm2oof.h"
#include "select_by_color.h"
#include "selectdash.h"
#include <assert.h>
#include "stdlib.h"


int SelectDashboard::dev_gray_dflt = 10;

DemogCmd::DemogCmd(SelectDashboard *dash)
  : CommandM("demography",
	     "select pixels with a similar color to the clicked color"),
  sd(dash)
{
  AddArgument(this, "plus_or_minus", sd->dev_gray);
}

CommandFn DemogCmd::func() {
  sd->dev_gray_dflt = sd->dev_gray;
}

CommandM *DemogCmd::clone() const {
  return new DemogCmd(sd);
}

bool DemogCmd::compare_color(const Color &col) const
{
  assert(current_goof != NULL);
  return (L1_dist(col, picked_color) <= sd->dev_gray);
}

void DemogCmd::select(const Image &img, const Cell_coordinate &where,
		      int shiftkey)
{
  assert(current_goof != NULL);
  current_goof->init_selection(shiftkey);
  Vec<Cell_coordinate> list;
  list.setphysicalsize(current_goof->query_height()*
		       current_goof->query_width());
  picked_color = img[where];
  ArrayIterator iter(img);
  Cell_coordinate cell;
  while(iter(cell))
    if(current_goof->active(cell) && compare_color(img[cell]))
      list.grow(1, cell);
	
  current_goof->select(list);
  garcon()->msout << ms_info << "Selected " << list.capacity()
		  << " pixels similar to " << picked_color
		  << " at " << where << "." << endl << ms_normal;
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

static int compare_colors(const Color &, const Color &, const Color &);
static int compare_grays(double, int, int);
 // first arg is double because Color::brightness is double

DemograyfyCmd::DemograyfyCmd(const ImageForm *imgfrm)
  : CommandM("demograyfy", "select pixels by specifying gray value"),
    grayvalue(125),dev_gray(5),replace(1),
    imageform(imgfrm)
{
  AddArgument(this, "gray_value", grayvalue);
  AddArgument(this, "plus_or_minus", dev_gray);
  AddArgument(this, "replace", replace);
}

CommandFn DemograyfyCmd::func() {
  assert(current_goof != NULL);
  Vec<Cell_coordinate> list;
  list.setphysicalsize(current_goof->query_height()*
		       current_goof->query_width());
  Cell_coordinate cell;
  ArrayIterator iter(imageform->current_picture());
  current_goof->init_selection(!replace);
  while(iter(cell))
    
    if(current_goof->active(cell)
       && compare_grays(Color(imageform->current_picture()[cell]).brightness(),
			grayvalue, dev_gray))
      list.grow(1, cell);
  
  current_goof->select(list);
  garcon()->msout << ms_info << list.capacity() << " pixel"
		  << (list.capacity() == 1? "" : "s") << " identified."
		  << endl << ms_normal;
  current_goof->redraw();
}

CommandM *DemograyfyCmd::clone() const {
  DemograyfyCmd *dc = new DemograyfyCmd(imageform);
  dc->grayvalue = grayvalue;
  dc->dev_gray = dev_gray;
  dc->replace = replace;
  return dc;
}



DemoRGBCmd::DemoRGBCmd(const ImageForm *imgfrm)
  : CommandM("demoRGB", "select pixels by specifying RGB values"),
    redvalue(125),greenvalue(75),bluevalue(175),
    dev_r(27),dev_g(32),dev_b(12),replace(1),
    imageform(imgfrm)
{
  AddArgument(this, "red_value", redvalue);
  AddArgument(this, "green_value", greenvalue);
  AddArgument(this, "blue_value", bluevalue);
  AddArgument(this, "+/-__red", dev_r);
  AddArgument(this, "+/-__green", dev_g);
  AddArgument(this, "+/-__blue", dev_b);
  AddArgument(this, "replace", replace); 
}

CommandFn DemoRGBCmd::func() {
  assert(current_goof != NULL);
  Color comp_color(redvalue,greenvalue,bluevalue);
  Color dev_color(dev_r,dev_g,dev_b);
  Vec<Cell_coordinate> list;
  list.setphysicalsize(current_goof->query_height()*
		       current_goof->query_width());
  current_goof->init_selection(!replace);
  Cell_coordinate cell;
  ArrayIterator iter(imageform->current_picture());
  while(iter(cell))

    if(current_goof->active(cell) &&
       compare_colors(Color(imageform->current_picture()[cell]),
		      comp_color,dev_color))
      list.grow(1, cell);
  
  current_goof->select(list);
  garcon()->msout << ms_info << list.capacity() << " pixel"
		  << (list.capacity() == 1? "" : "s") << " identified."
		  << endl << ms_normal;
  current_goof->redraw();
}

CommandM *DemoRGBCmd::clone() const {
  DemoRGBCmd *dc = new DemoRGBCmd(imageform);
  dc->redvalue = redvalue;
  dc->greenvalue = greenvalue;
  dc->bluevalue = bluevalue;
  dc->dev_r = dev_r;
  dc->dev_g = dev_g;
  dc->dev_b = dev_b;
  dc->replace = replace;
  return dc;
}

// ---------------------------------------------------------------- //

static int compare_colors(const Color &acol,
			  const Color &bcol,
			  const Color &devcol)
{
  return (
	  abs(acol.query_red() - bcol.query_red()) 
	  <= abs(devcol.query_red()) 
	  &&
	  abs(acol.query_green() - bcol.query_green())
	  <= abs(devcol.query_green())
	  &&
	  abs(acol.query_blue() - bcol.query_blue())
	  <= abs(devcol.query_blue())
	  );  
}


static int compare_grays(double agray, int bgray, int devgray) {
  return fabs(agray - bgray) <= devgray;  
}
