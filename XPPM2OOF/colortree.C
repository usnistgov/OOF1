// -*- C++ -*-
// $RCSfile: colortree.C,v $
// $Revision: 1.3 $
// $Author: langer $
// $Date: 2000-09-06 20:11:26 $


/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */

#ifndef COLORTREE_C
#define COLORTREE_C

#include "colortree.h"
#include "stdlib.h"

template <class TYPE>
ColorTree<TYPE>::ColorTree()
  : root(0),
    nnodes(0),
    oldpoplist(1),
    nelected(0)
{
}

template <class TYPE>
ColorTree<TYPE>::~ColorTree() {
  if(root) delete root;
}

template <class TYPE>
void ColorTree<TYPE>::clear() {
  if(root)
    delete root;
  root = 0;
  nnodes = 0;
  oldpoplist = 1;
  nelected = 0;
}

template <class TYPE>
ColorTreeNode<TYPE>::ColorTreeNode(const TYPE &cell, const Color &col)
  : color(col),
    popularity(1),
    elected(0),
    mindistance(200000), // bigger than any L2_dist2
    nearest(-1),
    left(0), right(0)
{
  list.AddHead(cell);
}

template <class TYPE>
ColorTreeNode<TYPE>::~ColorTreeNode() {
  if(left == this || right == this)
    exit(1);
  if(left)
    delete left;
  if(right)
    delete right;
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

template <class TYPE>
ColorTreeNode<TYPE>::ColorTreeNode(const ColorTreeNode &other) {
  copy(other);
}

template <class TYPE>
void ColorTreeNode<TYPE>::copy(const ColorTreeNode &other) {
  color = other.color;
  popularity = other.popularity;
  elected = other.elected;
  mindistance = other.mindistance;
  nearest = other.nearest;
  list = other.list;
  if(other.left)
    left = new ColorTreeNode<TYPE>(*other.left);
  else
    left = 0;
  if(other.right)
    right = new ColorTreeNode<TYPE>(*other.right);
  else
    right = 0;
}

template <class TYPE> ColorTreeNode<TYPE>
&ColorTreeNode<TYPE>::operator=(const ColorTreeNode<TYPE> &other) {
  if(this == &other)
    return *this;
  copy(other);
  return *this;
}

template <class TYPE>
ColorTree<TYPE> &ColorTree<TYPE>::operator=(const ColorTree<TYPE> &other) {
  if(this == &other)
    return *this;
  nnodes = other.nnodes;
  oldpoplist = 1; // don't copy popularity list!
  nelected = other.nelected;
  furthest = other.furthest;
  if(root) delete root;
  if(other.root)
    root = new ColorTreeNode<TYPE>(*other.root);
  else
    root = 0;
  return *this;
}

template <class TYPE>
ColorTree<TYPE>::ColorTree(const ColorTree<TYPE> &other) {
  nnodes = other.nnodes;
  oldpoplist = 1;
  nelected = other.nelected;
  furthest = other.furthest;
  if(other.root)
    root = new ColorTreeNode<TYPE>(*other.root);
  else
    root = 0;
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

// Add nodes to the tree

template <class TYPE>
void ColorTree<TYPE>::add(const TYPE &cell, const Color &color) {
  oldpoplist = 1;
  if(root)
    nnodes += root->add(cell, color);
  else {
    root = new ColorTreeNode<TYPE>(cell, color);
    nnodes = 1;
  }
}

template <class TYPE>
int ColorTreeNode<TYPE>::add(const TYPE &cell, const Color &col) {
  if(col == color) {
    list.AddHead(cell);
    popularity++;
    return 0;   // no new nodes in the tree
  }
  else if(col < color) {
    if(left)
      return left->add(cell, col);
    else {
      left = new ColorTreeNode<TYPE>(cell, col);
      return 1;
    }
  }
  else {   // col > color
    if(right)
      return right->add(cell, col);
    else {
      right = new ColorTreeNode<TYPE>(cell, col);
      return 1;
    }
  }
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

// make a linear list of nodes in the tree, sorted by popularity

template <class TYPE>
int ColorTree<TYPE>::popcompare(const void *one, const void *two) {
  ColorTreeNode<TYPE> *n1 = *(ColorTreeNode<TYPE>**) one;
  ColorTreeNode<TYPE> *n2 = *(ColorTreeNode<TYPE>**) two;
  if(n1->popularity > n2->popularity) return -1;
  if(n2->popularity > n1->popularity) return 1;
  return 0;
}

template <class TYPE>
void ColorTree<TYPE>::makelist() {
  oldpoplist = 0;
  popularitylist.resize(nnodes);
  int i = 0;
  root->makelist(i, popularitylist); // make a linear list of nodes
  ::qsort(&popularitylist[0],	// sort the list by popularity
	popularitylist.capacity(), sizeof(ColorTreeNode<TYPE>*), popcompare);
}

// convert tree to a linear list of nodes
template <class TYPE>
void ColorTreeNode<TYPE>::makelist(int &where, Vec<ColorTreeNode<TYPE>*> &lst)
{
  if(left)
    left->makelist(where, lst);
  lst[where++] = this;
  if(right)
    right->makelist(where, lst);
}

// return a node in the linear list

template <class TYPE>
ColorTreeNode<TYPE> &ColorTree<TYPE>::operator[](int which) {
  if(oldpoplist)
    makelist();
  return *popularitylist[which];
}

template <class TYPE>
void ColorTree<TYPE>::electall() {
  if(oldpoplist)
    makelist();
  for(int i=0; i<nnodes; i++)
    popularitylist[i]->elected = 1;
  nelected = nnodes;
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

// use the Diversity Algorithm from xv to reduce the number of colors
// in the linear list

template <class TYPE>
void ColorTree<TYPE>::reduce(int ndesired) {
  int i;
  if(oldpoplist)
    makelist();
  if(ndesired > nnodes) {
    nelected = nnodes;
    for(int i=0; i<nnodes; i++)
      popularitylist[i]->elected = 1;
    return;
  }

  cerr << "Reducing image from " << nnodes
       << " to " << ndesired << " colors."
       << endl;

  // make sure nobody's elected before the election
  nelected = 0;
  for(i=0; i<nnodes; i++) {
    popularitylist[i]->elected = 0;
    popularitylist[i]->nearest = -1;
    popularitylist[i]->mindistance = 200000;
  }

  // elect colors alternating between diversity and popularity,
  // starting with popularity and then a bunch of diversities.
  enum {DIVERSITY, POPULARITY} method = POPULARITY;
  while(nelected < ndesired) {
    if(method == DIVERSITY) {
      elect(furthest);
      if(nelected > 11) method = POPULARITY;
    }
    else {  // method == POPULARITY
      elect(nelected);
      method = DIVERSITY;
    }
  }
}

template <class TYPE>
void ColorTree<TYPE>::elect(int candidate) {
  int i;
  ColorTreeNode<TYPE> * const winner = popularitylist[candidate];
  // elected nodes are at the front of the list. Put the candidate there,
  // while keeping the remaining entries in order.
  for(i=candidate; i>nelected; i--)
    popularitylist[i] = popularitylist[i-1];
  popularitylist[nelected] = winner;
  winner->elected = 1;
  nelected++;
  
  // update distances from unelected colors to elected colors
  int maxdistance = -1;  // maximum distance to any elected color
  for(i=nelected; i<nnodes; i++) {
    int d2 = L2_dist2(winner->color, popularitylist[i]->color);
    if(d2 < popularitylist[i]->mindistance) {
      // newly elected node is closer than previously elected nodes
      popularitylist[i]->mindistance = d2;
      popularitylist[i]->nearest = nelected - 1;
    }
    if(popularitylist[i]->mindistance > maxdistance) {
      // this color is the furthest from any elected color
      maxdistance = popularitylist[i]->mindistance;
      furthest = i;
    }
  }
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

// include template definitions

#include "linklist.C"

#endif
