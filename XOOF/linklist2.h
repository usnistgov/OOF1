// -*- C++ -*-
// $RCSfile: linklist2.h,v $
// $Revision: 1.8 $
// $Author: langer $
// $Date: 2000-10-31 19:15:12 $


/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */

// Doubly linked list

#ifndef LINKLIST2_H
#define LINKLIST2_H

template <class TYPE> class LinkList2;
template <class TYPE> class LinkList2Iterator;

template <class TYPE>
class LinkList2Node {
  // The Sun CC compiler screws up on nested template classes, so this
  // class isn't nested within LinkList2<>.
  private:
    LinkList2Node(const LinkList2Node&); // prohibited
    TYPE x;
    LinkList2Node<TYPE> *next;
    LinkList2Node<TYPE> *prev;
    LinkList2Node(const TYPE &);
    LinkList2Node(const LinkList2Node<TYPE>&, LinkList2Node<TYPE>*);
  friend class LinkList2<TYPE>;
  friend class LinkList2Iterator<TYPE>;
};

template <class TYPE>
class LinkList2 {
private:
  LinkList2Node<TYPE> *head;
  LinkList2Node<TYPE> *tail;
public:
  LinkList2();
  ~LinkList2();
  LinkList2(const LinkList2<TYPE>&);
  LinkList2<TYPE> &operator=(const LinkList2<TYPE>&);
  void AddHead(const TYPE&);	// add given item to head of list
  void AddTail(const TYPE&);	// add given item to tail of list
  void Remove(LinkList2Iterator<TYPE> &);
  void clear();

  // insert after given position
  void Insert(const LinkList2Iterator<TYPE>&, const TYPE &x);

  // operator{}(LinkList2Iterator) probably shouldn't exist. It allows
  // constructions like list[iterator], which looks nice, but is
  // misleading, since "list" isn't actually used. list1[iterator] and
  // list2[iterator] both return the same object, which is in the list
  // used in iterator's constructor.
  TYPE &operator[](const LinkList2Iterator<TYPE> &i) {
    return i.node->x;
  }
  const TYPE &operator[](const LinkList2Iterator<TYPE> &i) const {
    return i.node->x;
  }


  LinkList2Iterator<TYPE> begin() const {
    return LinkList2Iterator<TYPE>(*this);
  }
  
  int size() const { return size_; }
private:
  void copy(const LinkList2<TYPE>&);
  int size_;
  friend class LinkList2Iterator<TYPE>;
};

template <class TYPE>
class LinkList2Iterator {
private:
  LinkList2Node<TYPE> *node;
public:
  LinkList2Iterator(const LinkList2<TYPE> &ll) : node(ll.head) {}
  int end() { return node == 0; }
  void operator++() { if(node) node = node->next; }
  void operator--() { if(node) node = node->prev; }
  LinkList2Iterator &operator+=(int);
  LinkList2Iterator &operator-=(int);
  TYPE *operator*() const { return &node->x; }
  operator bool() const { return (node != 0); }
  friend class LinkList2<TYPE>;
};

template <class TYPE>
bool operator==(const LinkList2<TYPE>&, const LinkList2<TYPE>&);

template <class TYPE>
bool operator==(const LinkList2Iterator<TYPE>&, const LinkList2Iterator<TYPE>&);

template <class TYPE>
LinkList2Iterator<TYPE> operator+(const LinkList2Iterator<TYPE>&, int);

template <class TYPE>
LinkList2Iterator<TYPE> operator-(const LinkList2Iterator<TYPE>&, int);

#include "linklist2.C"

#endif // LINKLIST2_H


