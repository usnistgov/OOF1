// -*- C++ -*-
// $RCSfile: linklist2.C,v $
// $Revision: 1.7 $
// $Author: langer $
// $Date: 2000-10-31 19:15:11 $

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */



#ifndef LINKLIST2_C
#define LINKLIST2_C

#include "linklist2.h"

template <class TYPE>
LinkList2<TYPE>::LinkList2()
  : head(0), size_(0), tail(0)
{
}

template <class TYPE>
LinkList2<TYPE>::~LinkList2() {
  clear();
}

template <class TYPE>
void LinkList2<TYPE>::clear() {
  LinkList2Node<TYPE> *node = head;
  while(node) {
    LinkList2Node<TYPE> *next = node->next;
    delete node;
    node = next;
  }
  head = 0;
  size_ = 0;
  tail = 0;
}

template <class TYPE>
LinkList2Node<TYPE>::LinkList2Node(TYPE const &xx)
  : x(xx),
    next(0),
    prev(0)
{
}


template <class TYPE>
LinkList2Node<TYPE>::LinkList2Node(const LinkList2Node<TYPE> &other,
				   LinkList2Node<TYPE> *previous)
{
  // copy constructor for nodes -- copies recursively in the forward direction
  x = other.x;
  prev = previous;
  if(other.next)
    next = new LinkList2Node<TYPE>(*other.next, this);
  else
    next = 0;
}

template <class TYPE>
LinkList2<TYPE>::LinkList2(const LinkList2<TYPE> &other) {
  copy(other);
}

template <class TYPE>
void LinkList2<TYPE>::copy(const LinkList2<TYPE> &other) {
  if(other.head)
    head = new LinkList2Node<TYPE>(*other.head, 0); // copies recursively
  else
    head = 0;
  size_ = other.size_;
}

template <class TYPE>
LinkList2<TYPE> &LinkList2<TYPE>::operator=(const LinkList2<TYPE> &other) {
  if(this == &other)
    return *this;
  clear();
  copy(other);
  return *this;
}

template <class TYPE>
void LinkList2<TYPE>::AddHead(const TYPE &x) {
  LinkList2Node<TYPE> *newnode = new LinkList2Node<TYPE>(x);
  newnode->next = head;
  newnode->prev = 0;
  if(size_ == 0)
    head = tail = newnode;
  else {
    head->prev = newnode;
    head = newnode;
  }
  ++size_;
}

template <class TYPE>
void LinkList2<TYPE>::AddTail(const TYPE &x) {
  LinkList2Node<TYPE> *newnode = new LinkList2Node<TYPE>(x);
  newnode->next = 0;
  newnode->prev = tail;
  if(size_ == 0)
    head = tail = newnode;
  else {
    tail->next = newnode;
    tail = newnode;
  }
  ++size_;
}

template <class TYPE>
void LinkList2<TYPE>::Remove(LinkList2Iterator<TYPE> &i) {
  if(!head || !i.node)
    return;
  LinkList2Node<TYPE> *defunct = i.node;
  if(defunct->prev)
    defunct->prev->next = defunct->next;
  else
    head = defunct->next;	// deleting head
  if(defunct->next)
    defunct->next->prev = defunct->prev;
  else
    tail = defunct->prev;	// deleting tail
  ++i;
  --size_;
  delete defunct;
}

template <class TYPE>
void LinkList2<TYPE>::Insert(const LinkList2Iterator<TYPE> &i,
				    const TYPE &x)
{
  LinkList2Node<TYPE> *node = i.node;	// insert after this node
  if(!node) return;
  LinkList2Node<TYPE> *newnode = new LinkList2Node<TYPE>(x);
  newnode->next = node->next;
  newnode->prev = node;
  if(node->next)
    node->next->prev = newnode;
  else
    tail = newnode;
  node->next = newnode;
  ++size_;
}

template <class TYPE>
bool operator==(const LinkList2<TYPE> &lla, const LinkList2<TYPE> &llb) {
  if(lla.size() != llb.size())
    return 0;
  for(LinkList2Iterator<TYPE> ia = lla.begin(), ib = llb.begin();
      !ia.end() && !ib.end(); ++ia, ++ib)
    if(!(lla[ia] == llb[ib]))
      return 0;
  return 1;
}

template <class TYPE>
bool operator==(const LinkList2Iterator<TYPE> &i,
		const LinkList2Iterator<TYPE> &j)
{
  // compare pointers, since a node can be in only one list
  return i.node == j.node;
}

template <class TYPE>
LinkList2Iterator<TYPE> &LinkList2Iterator<TYPE>::operator+=(int d) {
  if(d > 0)
    while(node && d--)
      node = node->next;
  else if(d < 0)
    while(node && d++)
      node = node->prev;
  return *this;
}

template <class TYPE>
LinkList2Iterator<TYPE> operator+(const LinkList2Iterator<TYPE> &i, int j) {
  LinkList2Iterator<TYPE> iter(i);
  iter += j;
  return iter;
}

template <class TYPE>
LinkList2Iterator<TYPE> &LinkList2Iterator<TYPE>::operator-=(int d) {
  if(d > 0)
    while(node && d--)
      node = node->prev;
  else if(d < 0)
    while(node && d++)
      node = node->next;
  return *this;
}

template <class TYPE>
LinkList2Iterator<TYPE> operator-(const LinkList2Iterator<TYPE> &i, int j) {
  LinkList2Iterator<TYPE> iter(i);
  iter -= j;
  return iter;
}

#endif // LINKLIST2_C

