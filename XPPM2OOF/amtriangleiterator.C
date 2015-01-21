// -*- C++ -*-
// $RCSfile: amtriangleiterator.C,v $
// $Revision: 1.8 $
// $Author: langer $
// $Date: 2000-11-01 17:07:44 $


/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */

// Iterate over pixels underlying a triangle
// Usage:
//  Triangle tri = ...;
//  for(AMTriangleIterator i(tri); !i.end(); ++i) {
//     Cell_coordinate pixel = tri[i];
//     ...
//  }

#include "adaptmesh.h"
#include "amtriangle.h"
#include "amtriangleiterator.h"
#include "goof.h"

// Divide the triangle into two triangles with a horizontal division
// through one vertex. Loop over the bottom half first.

AMTriangleIterator::AMTriangleIterator(const AMTriangle &triangle)
  : btm_done(0),		// has the bottom half been done?
    top_done(0),		// has the top half been done?
    maxx(triangle.mesh->goof->query_width()-1),
    maxy(triangle.mesh->goof->query_height()-1)
{
  // find botttom node
  btmnode = triangle.node[0];
  double btmy = btmnode->coord().y;
  int btmi = 0;
  for(int i=1; i<3; i++) {
    double y = triangle.node[i]->coord().y;
    if(y < btmy) {
      btmy = y;
      btmnode = triangle.node[i];
      btmi = i;
    }
  }
  // find middle node and top nodes
  int n1 = (btmi+1)%3;
  int n2 = (btmi+2)%3;
  if(triangle.node[n1]->coord().y < triangle.node[n2]->coord().y) {
    midnode = triangle.node[n1];
    topnode = triangle.node[n2];
  }
  else {
    midnode = triangle.node[n2];
    topnode = triangle.node[n1];
  }

  // is the middle node to the right or left of the line joining top and btm?
  midright = ((midnode->coord() - btmnode->coord()) %
	      (topnode->coord() - btmnode->coord()) > 0);

  // find equations for sides
  if(btmnode->coord().y == midnode->coord().y)
    btm_done = 1;		// there is no bottom triangle
  else {
    // compute slopes of edges, assuming that the midpoint is on the left side
    lslope_btm = (midnode->coord().x - btmnode->coord().x)
      / (midnode->coord().y - btmnode->coord().y);
    rslope_btm = (topnode->coord().x - btmnode->coord().x)
      / (topnode->coord().y - btmnode->coord().y);
    if(midright) {		// wrong assumption!
      double temp = lslope_btm;
      lslope_btm = rslope_btm;
      rslope_btm = temp;
    }
    if(rslope_btm == lslope_btm)
      btm_done = 1;
  }
  if(midnode->coord().y == topnode->coord().y)
    top_done = 1;		// there is no top triangle
  else {
    // compute slopes of edges, assuming that the midpoint is on the left side
    lslope_top = (topnode->coord().x - midnode->coord().x)
      / (topnode->coord().y - midnode->coord().y);
    rslope_top = (topnode->coord().x - btmnode->coord().x)
      / (topnode->coord().y - btmnode->coord().y);
    if(midright) {		// wrong assumption
      double temp = rslope_top;
      rslope_top = lslope_top;
      lslope_top = temp;
    }
    if(rslope_top == lslope_top)
      top_done = 1;
  }
  if(!btm_done) {
    current.y = (short) btmnode->coord().y;
    current.x = (short) xmin_btm(current.y);
    current_xmax = (int) xmax_btm(current.y);
  }
  else {
    current.y = (short) midnode->coord().y;
    current.x = (short) xmin_top(current.y);
    current_xmax = (int) xmax_top(current.y);
  }
}

double AMTriangleIterator::xmin_btm(double y) const {
  // Find the minimum x value for a pixel within the bottom triangle
  // at this y. Use the equation for the left edge of the triangle,
  // but be careful if the edge has an end within this row of pixels
  // (ie between y and y+1).
  double min;
  double yint;			// y at minimum value of x

  if(lslope_btm > 0) {
//                edge
//   y+1  ---------/------------------------------------/-------
//                /  yint is the lowest y              /
//               /   for the edge in this row         B
//     y  ------/-------------------------------------+---------
//            xmin                                   xmin when btm node
//                                                   is within the row
    yint = y;
    if(y < btmnode->coord().y)	// bottom node is in this row of pixels
      yint = btmnode->coord().y;
  }
  else {			// lslope_btm < 0
    yint = y + 1;
    if(!midright && midnode->coord().y < yint) // left edge ends within this row
      yint = midnode->coord().y;
  }
  
  min = floor(btmnode->coord().x + lslope_btm*(yint - btmnode->coord().y));
  if(min < 0) min = 0;
  if(min > maxx) min = maxx;
  return min;
}

double AMTriangleIterator::xmax_btm(double y) const {
  double max;
  double yint;
  if(rslope_btm > 0) {
    yint = y + 1;
    if(midright && midnode->coord().y < yint) // midnode is within this row
	yint = midnode->coord().y;
  }
  else {			// rslope_btm < 0
    yint = y;
    if(y < btmnode->coord().y)	// bottom node is in this row of pixels
      yint = btmnode->coord().y;
  }
  max = floor(btmnode->coord().x + rslope_btm*(yint - btmnode->coord().y));
  if(max > maxx) max = maxx;
  if(max < 0) max = 0;
  return max;
}

double AMTriangleIterator::xmin_top(double y) const {
  double min;
  double yint;
  if(lslope_top > 0) {
    yint = y;
    if(!midright && midnode->coord().y > yint) // middle node is within this row
	yint = midnode->coord().y;
  }
  else {			// lslope_top < 0
    yint = y + 1;
    if(topnode->coord().y < y + 1)
      yint = topnode->coord().y;
  }
  min = floor(topnode->coord().x + lslope_top*(yint - topnode->coord().y));
  if(min < 0) min = 0;
  if(min > maxx) min = maxx;
  return min;
}

double AMTriangleIterator::xmax_top(double y) const {
  double max;
  double yint;
  if(rslope_top > 0) {
    yint = y + 1;
    if(topnode->coord().y < y + 1)
      yint = topnode->coord().y;
  }
  else {			// rslope_top < 0
    yint = y;
    if(midright && midnode->coord().y > y)
      yint = midnode->coord().y;
  }
  max = floor(topnode->coord().x + rslope_top*(yint - topnode->coord().y));
  if(max > maxx) max = maxx;
  if(max < 0) max = 0;
  return max;
}

void AMTriangleIterator::operator++() {
  if(!btm_done) {
    current.x++;		// next pixel in this row
    if(current.x > current_xmax) { // done with this row?
      current.y++;		// go up to next row
      if(current.y > maxy || current.y > topnode->coord().y) {
	top_done = btm_done = 1;
      }
      else if(current.y > midnode->coord().y) { // done with btm triangle?
	btm_done = 1;
	// initial settings for top triangle
	current.x = (short) xmin_top(current.y);
	current_xmax = (int) xmax_top(current.y);
      }
      else {			// new row in bottom triangle
	current.x = (short) xmin_btm(current.y);
	current_xmax = (int) xmax_btm(current.y);
      }
    }
  }
  else if(!top_done) {
    current.x++;
    if(current.x > current_xmax) { // done with this row?
      current.y++;
      if(current.y > maxy || current.y > topnode->coord().y)
	top_done = 1;
      else {
	current.x = (short) xmin_top(current.y);
	current_xmax = (int) xmax_top(current.y);
      }
    }
  }
}

Cell_coordinate AMTriangle::operator[](AMTriangleIterator &iter) const {
  return iter.current;
}
