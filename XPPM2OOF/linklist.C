// -*- C++ -*-
// $RCSfile: linklist.C,v $
// $Revision: 1.5 $
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



#ifndef LINKLIST_C
#define LINKLIST_C

#include "linklist.h"

template <class TYPE>
LinkList<TYPE>::LinkList()
  : head(0), size_(0)
{
}

template <class TYPE>
LinkList<TYPE>::~LinkList() {
  clear();
}

template <class TYPE>
void LinkList<TYPE>::clear() {
  LinkListNode *node = head;
  while(node) {
    LinkListNode *next = node->next;
    delete node;
    node = next;
  }
  head = 0;
  size_ = 0;
}

template <class TYPE>
LinkList<TYPE>::LinkListNode::LinkListNode(const TYPE &xx)
  : x(xx),
    next(0)
{
}


template <class TYPE>
LinkList<TYPE>::LinkListNode::LinkListNode(const LinkListNode &other) {
  x = other.x;
  if(other.next)
    next = new LinkListNode(*other.next);
  else
    next = 0;
}

template <class TYPE>
LinkList<TYPE>::LinkList(const LinkList<TYPE> &other) {
  copy(other);
}

template <class TYPE>
void LinkList<TYPE>::copy(const LinkList<TYPE> &other) {
  if(other.head)
    head = new LinkListNode(*other.head); // copies recursively
  else
    head = 0;
  size_ = other.size_;
}

template <class TYPE>
LinkList<TYPE> &LinkList<TYPE>::operator=(const LinkList<TYPE> &other) {
  if(this == &other)
    return *this;
  copy(other);
  return *this;
}

template <class TYPE>
void LinkList<TYPE>::AddHead(const TYPE &x) {
  LinkListNode *newnode = new LinkListNode(x);
  newnode->next = head;
  head = newnode;
  ++size_;
}

template <class TYPE>
void LinkList<TYPE>::Remove(const TYPE &x) {
  if(!head)
    return;
  if(head->x == x) {		// remove the head of this list
    LinkListNode *oldnode = head;
    head = oldnode->next;
    delete oldnode;
    --size_;
    return;
  }

  LinkListNode *node = head;
  while(node->next && node->next->x != x) // look for item to remove
    node = node->next;
  if(!node->next)		// didn't find anything to remove
    return;
  if(node->next->x == x) {	// found something
    LinkListNode *oldnode = node->next;
    node->next = oldnode->next;
    --size_;
    delete oldnode;
  }
}

template <class TYPE>
void LinkList<TYPE>::Insert(const TYPE &x, int (*f)(const TYPE&, const TYPE&)) {
  int comparison;
  if(!head || (comparison = (*f)(x, head->x)) < 0) {
    AddHead(x);
    return;
  }
  if(head && comparison == 0) return; // don't add duplicates
  LinkListNode *node = head;
  while(node->next && (comparison = (*f)(node->next->x, x)) < 0)
    node = node->next;

  if(node->next && comparison == 0) return; // don't add duplicates

  LinkListNode *newnode = new LinkListNode(x);
  newnode->next = node->next;
  node->next = newnode;
  ++size_;
}

template <class TYPE>
int operator==(const LinkList<TYPE> &lla, const LinkList<TYPE> &llb) {
  if(lla.size() != llb.size())
    return 0;
  for(LinkListIterator<TYPE> ia = lla.begin(), ib = llb.begin();
      !ia.end() && !ib.end(); ++ia, ++ib)
    if(!(lla[ia] == llb[ib]))
      return 0;
  return 1;
}

#endif
