// -*- C++ -*-
// $RCSfile: amintersection.C,v $
// $Revision: 1.7 $
// $Author: langer $
// $Date: 2000-11-02 13:59:21 $


/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */

// Find the intersection of a mesh triangle and a pixel.

#include "amtriangle.h"
#include "amtriangleiterator.h" // only used for DEBUG code
#include "adaptmesh.h"
#include "stdlib.h"

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

class ConvexPolygon {
private:
  Vec<MeshCoord> vertex;
  static int compare(const void*, const void*);	// for sorting vertices 
  static int compare0(const void*, const void*);
  static MeshCoord referencept; // for sorting vertices
  bool sorted;
  int sort();
public:
  ConvexPolygon(int n=0)	// n is number of vertices to preallocate
    : sorted(0)
  {
    vertex.setphysicalsize(n);
  }
  const MeshCoord &operator()(int i) const { return vertex[i]; }
  int npts() const { return vertex.capacity(); }
  void add(const MeshCoord &pt);
  double area();
};

void ConvexPolygon::add(const MeshCoord &pt) {
  // Don't bother checking for duplicate points here.  That is done by
  // ConvexPolygon::sort(), which is called by ConvexPolygon::area().
  vertex.grow(1, pt);
  sorted = 0;
}

double ConvexPolygon::area() {
  if(vertex.capacity() < 3) return 0.0;
  if(!sort()) 
    return 0;			// can't sort vertices if polygon is degenerate
  double a = 0;
  for(int j=2; j<vertex.capacity(); j++)
    a += trianglearea(vertex[0], vertex[j-1], vertex[j]);
  return a;
}

MeshCoord ConvexPolygon::referencept;

int ConvexPolygon::sort() {
  // Sort vertices so that they proceed counterclockwise. The sorting
  // starts with vertex[1], not vertex[0], since the starting vertex
  // (referencept) has to be chosen arbitrarily.  If the same point
  // appears twice in the list, then the sorting algorithm can fail,
  // since it relies on the cross product of vectors drawn between
  // points. In particular, it WILL fail if the referencept is a
  // redundant point.  So first look for a nonredundant reference
  // point, then sort the vertices, then remove the redundant points.
  // This is more efficient than searching for all redundant points in
  // an unsorted list.

  if(sorted) return 1;

  if(vertex.capacity() < 3) {
    sorted = true;
    return 0;
  }

  // Most of the time all the vertices will be unique, so do a quick
  // check on vertex[0] first.
  int duplicate = 0;
  for(int j=1; j<vertex.capacity() && !duplicate; j++)
    if(vertex[0] == vertex[j])
      duplicate = j;

  if(!duplicate) {
    // vertex[0] is unique.  Putting vertices in order will work, even
    // if other vertices are not unique
    referencept = vertex[0];
    ::qsort(&vertex[1], vertex.capacity()-1, sizeof(MeshCoord), compare);

    // remove non-unique vertices
    Vec<int> dup(vertex.capacity(), 0);
    int dups = 0;
    for(int k=1; k<vertex.capacity(); k++) {
      dup[k] = (vertex[k] == vertex[k-1]);
      dups = 1;
    }
    if(dups)
      vertex.remove_conditional(dup);
  }
  else {
    // vertex[0] isn't unique.  We have to sort to find the redundant
    // vertices, remove them, and then put the remaining vertices in
    // counterclockwise order.

    // sort vertices alphabetically to find redundant ones
    ::qsort(&vertex[0], vertex.capacity(), sizeof(MeshCoord), compare0);

    // mark redundant vertices
    Vec<int> dup(vertex.capacity(), 0);
    for(int i=1; i<vertex.capacity(); i++)
      dup[i] = (vertex[i] == vertex[i-1]);
    // remove redundant vertices
    vertex.remove_conditional(dup);

    // put vertices in counterclockwise order
    referencept = vertex[0];
    ::qsort(&vertex[1], vertex.capacity()-1, sizeof(MeshCoord), compare);
  }

  sorted = 1;
  return 1;			// success
}

int ConvexPolygon::compare(const void *p1, const void *p2) {
  double a = trianglearea(referencept, *(MeshCoord*) p1, *(MeshCoord*) p2);
  if(a > 0) return -1;
  if(a < 0) return 1;
  return 0;
}

int ConvexPolygon::compare0(const void *p1, const void *p2) {
  MeshCoord &pp1 = *(MeshCoord*) p1;
  MeshCoord &pp2 = *(MeshCoord*) p2;
  if(pp1 < pp2) return -1;
  if(pp1 > pp2) return 1;
  return 0;
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

// Find the area of intersection between a pixel and a triangle

// does a pixel contain a point?
inline int pixelcontains(const Cell_coordinate &pixel, const MeshCoord &pt) {
  return (pixel.x <= pt.x && pixel.x + 1 >= pt.x &&
	  pixel.y <= pt.y && pixel.y + 1 >= pt.y);
}

// find intersection of a pixel side (p1, p2) with a triangle side
// (t1, t2) assuming that the pixel side is vertical and that the
// triangle side is not. ***The segments are closed (contain their
// endpoints).***

static int yintercept(const MeshCoord &t1, const MeshCoord &t2,
		      const MeshCoord &p1, const MeshCoord &p2,
		      MeshCoord &intercept)
{
  // rename triangle corners so that ta is to the left of tb
  const MeshCoord &ta = (t1.x < t2.x ? t1 : t2);
  const MeshCoord &tb = (&ta == &t1 ? t2 : t1);
  // check that triangle side straddles the (extended) pixel side
  if(ta.x > p1.x || tb.x < p1.x)
    return 0;
  // rename pixel corners so that pa is below pb
  const MeshCoord &pa = (p1.y < p2.y ? p1 : p2);
  const MeshCoord &pb = (&pa == &p1 ? p2 : p1);
  intercept.x = pa.x;
  intercept.y = ta.y + (tb.y - ta.y)*(pa.x - ta.x)/(tb.x - ta.x);
  if(intercept.y > pb.y || intercept.y < pa.y)
    return 0;
  return 1;
}

// find intersection of a pixel side (p1, p2) with a triangle side
// (t1, t2) assuming that the pixel side is horizontal and that the
// triangle side is not. ***The segments are closed (contain their
// endpoints).***

static int xintercept(const MeshCoord &t1, const MeshCoord &t2,
		      const MeshCoord &p1, const MeshCoord &p2,
		      MeshCoord &intercept)
{
  // rename triangle corners so that ta is below tb
  const MeshCoord &ta = (t1.y < t2.y ? t1 : t2);
  const MeshCoord &tb = (&ta == &t1 ? t2 : t1);
  // check that triangle side straddles the (extended) pixel side
  if(ta.y > p1.y || tb.y < p1.y)
    return 0;
  // rename pixel corners so that pa is to the left of pb
  const MeshCoord &pa = (p1.x < p2.x ? p1 : p2);
  const MeshCoord &pb = (&pa == &p1 ? p2 : p1);
  intercept.y = pa.y;
  intercept.x = ta.x + (tb.x - ta.x)*(pa.y - ta.y)/(tb.y - ta.y);
  if(intercept.x > pb.x || intercept.x < pa.x)
    return 0;
  return 1;
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

// find the area of intersection of the given pixel with this triangle

double AMTriangle::intersection(const Cell_coordinate &pixel) const {
  if(area() == 0.0) return 0;
  int i;
  MeshCoord pixelcorner[4];
  ConvexPolygon intersection(30);
  // The argument to the ConvexPolygon constructor is a guess at the
  // maximum number of vertices that a polygon will have.  It doesn't
  // hurt to make it too big.  The intersection of a triangle and a
  // square can have at most 7 vertices, but during its construction a
  // vertex may be added more than once.  A vertex certainly can't be
  // added more than 4 times, though, so 30 is a safe upper bound.

  // labels for corners of pixel
  static const int LL = 0;	// lower left
  static const int LR = 1;	// lower right
  static const int UR = 2;	// upper right
  static const int UL = 3;	// upper left

  pixelcorner[LL].x = pixel.x;
  pixelcorner[LL].y = pixel.y;
  pixelcorner[LR].x = pixel.x + 1;
  pixelcorner[LR].y = pixel.y;
  pixelcorner[UR].x = pixel.x + 1;
  pixelcorner[UR].y = pixel.y + 1;
  pixelcorner[UL].x = pixel.x;
  pixelcorner[UL].y = pixel.y + 1;
  // Identify corners of the intersection region

  // When checking vertices, use <= and >= to see if triangle vertices
  // are inside the pixel (and vice versa), but < and > to see if triangle
  // edges intersect pixel edges.  This prevents vertices of one shape
  // that lie on an edge of the other shape from being included in the
  // list twice.

  // check corners of pixel to see if they're in the triangle
  for(i=0; i<4; i++)		// loop over pixel corners
    if(this->contains(pixelcorner[i]))
      intersection.add(pixelcorner[i]);
  int np = intersection.npts();	// no. of pixel corners w/in triangle
  if(np == 4)			// pixel is entirely w/in triangle
    return 1.0;
  // check corners of triangle to see if they're in the pixel
  for(i=0; i<3; i++)		// loop over triangle corners
    if(pixelcontains(pixel, node[i]->coord()))
      intersection.add(node[i]->coord());
  int nt = intersection.npts() - np; // no. of triangle vertices w/in pixel
  if(nt == 3)			// triangle is completely inside pixel
    return area();
  // look for intersections of edges
  for(i=0; i<3; i++) {		// loop over triangle edges
    const MeshCoord &t1 = node[i]->coord();
    const MeshCoord &t2 = node[(i+1)%3]->coord();
    MeshCoord intercept;
    // intersections with horizontal pixel edges
    if(t1.y != t2.y) { // triangle edge not horizontal
      // Don't have to look at horizontal triangle edges, since if
      // they lie along a horizontal pixel edge, the intersections
      // will have been detected when checking to see if the triangle
      // corners are inside the pixel.
      if(xintercept(t1, t2, pixelcorner[LL], pixelcorner[LR], intercept))
	intersection.add(intercept);
      if(xintercept(t1, t2, pixelcorner[UR], pixelcorner[UL], intercept))
	intersection.add(intercept);
    }
    // intersections with vertical pixel edges
    if(t1.x != t2.x) {		// triangle edge not vertical
      // Don't have to look at vertical triangle edges.
      if(yintercept(t1, t2, pixelcorner[LR], pixelcorner[UR], intercept))
	intersection.add(intercept);
      if(yintercept(t1, t2, pixelcorner[LL], pixelcorner[UL], intercept))
	intersection.add(intercept);
    }
  }

  return intersection.area();
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

#ifdef DEBUG

void AMTriangle::dump_intersections() const {
//    cerr << "-----------------" << endl
//         << "Nodes: " << *node[0] << " " << *node[1] << " " << *node[2] << endl;
//    double total = 0;
//    cerr << "Intersections:" << endl;
//    for(AMTriangleIterator it(*this); !it.end(); ++it) {
//      const Cell_coordinate pixel = (*this)[it];
//      double intersect = intersection(pixel);
//      total += intersect;
//      cerr << pixel << " " << intersect << endl;
//    }
//    cerr << "Total area = " << total << endl;
}

#endif // DEBUG
