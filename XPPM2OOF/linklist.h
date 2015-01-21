// -*- C++ -*-
// $RCSfile: linklist.h,v $
// $Revision: 1.6 $
// $Author: langer $
// $Date: 2001-03-13 19:21:01 $

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */

#ifndef LINKLIST_H
#define LINKLIST_H

template <class TYPE> class LinkListIterator;

template <class TYPE>
class LinkList {
public:     // would be private if LinkListIterator<TYPE> could be a friend
  class LinkListNode {
  public:
    TYPE x;
    LinkListNode *next;
    LinkListNode(const TYPE &);
    LinkListNode(const LinkListNode&);
  };
  LinkListNode *head;
public:
  LinkList();
  ~LinkList();
  LinkList(const LinkList<TYPE>&);
  LinkList<TYPE> &operator=(const LinkList<TYPE>&);
  void AddHead(const TYPE&);           // add given item to head of list
  void Remove(const TYPE&);
  void clear();

  // insert x before the first element for which f(x, entry) >=
  // 0. Don't insert at all if f(x,entry) == 0 for some entry. f is a
  // comparator function of the type that qsort() uses: f(a,b) > 0 if
  // a > b, f(a,b) < 0 if a < b, and f(a,b) == 0 if a==b.
  void Insert(const TYPE &x, int (*f)(const TYPE&, const TYPE&));

  TYPE &operator[](const LinkListIterator<TYPE> i) {
    return i.node->x;
  }
  const TYPE &operator[](const LinkListIterator<TYPE> i) const {
    return i.node->x;
  }

  LinkListIterator<TYPE> begin() const { return LinkListIterator<TYPE>(*this); }
  
  int size() const { return size_; }
private:
  void copy(const LinkList<TYPE>&);
  int size_;
};

// Iterate over the elements of a linked list like this:
//  LinkList<TYPE> list;
//  for(LinkListIterator<TYPE> i=list.begin(); !i.end(); ++i)
//     TYPE x = list[i];

template <class TYPE>
class LinkListIterator {
public:
  typename LinkList<TYPE>::LinkListNode *node;

  LinkListIterator(const LinkList<TYPE> &ll) : node(ll.head) {}
  int end() { return node == 0; }
  void operator++() { if(node) node = node->next; }
};

template <class TYPE>
int operator==(const LinkList<TYPE>&, const LinkList<TYPE>&);

#include "linklist.C"

#endif


