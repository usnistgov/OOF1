// -*- C++ -*-
// $RCSfile: colortree.h,v $
// $Revision: 1.4 $
// $Author: langer $
// $Date: 2000-10-24 14:35:17 $


/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


// Maintain tree of colors used in the Image class

#ifndef COLORTREE_H
#define COLORTREE_H

#include "color.h"
#include "vec.h"
#include "linklist.h"
#include "cell_coordinate.h"

template <class TYPE> class ColorTree;

template <class TYPE>
class ColorTreeNode {
private:
  void makelist(int &, Vec<ColorTreeNode<TYPE>*> &);
  ColorTreeNode &operator=(const ColorTreeNode<TYPE>&);
  ColorTreeNode(const ColorTreeNode<TYPE>&);
  void copy(const ColorTreeNode<TYPE>&);
public:
  Color color;
  int popularity;  // number of times this color appears in the image
  int elected;     // has this color been chosen to appear in the display?
  int mindistance; // minimum squared distance to an elected color
  int nearest;     // nearest elected color (index into popularity list)
  LinkList<TYPE> list;  // cells using this color
  ColorTreeNode *left, *right;
  ColorTreeNode(const TYPE&, const Color&);
  ~ColorTreeNode();
  int add(const TYPE&, const Color&);
  friend class ColorTree<TYPE>;
};

template <class TYPE>
class ColorTree {
private:
  ColorTreeNode<TYPE> *root;
  int nnodes;
  Vec<ColorTreeNode<TYPE>*> popularitylist;
  bool oldpoplist;  // is the popularity list out of date?
  void makelist();  // construct popularity list
  static int popcompare(const void*, const void*);

  int nelected;  // number of colors elected during color reduction
  void elect(int);

  // index in popularitylist of color furthest from any elected color
  int furthest;

public:
  ColorTree();
  ColorTree(const ColorTree<TYPE>&);
  ~ColorTree();
  int Ncolors() const { return nnodes; }
  int Nreducedcolors() const { return nelected; }
  void add(const TYPE&, const Color&);
  ColorTreeNode<TYPE> &operator[](int);
  void electall();
  void clear();
  
  ColorTree<TYPE> &operator=(const ColorTree<TYPE>&);

  // reduce to n colors, leaving elected ones at head of popularitylist
  void reduce(int n);
};

#include "colortree.C"

#endif
