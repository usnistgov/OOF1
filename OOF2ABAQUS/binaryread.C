// -*- C++ -*-
// $RCSfile: binaryread.C,v $
// $Revision: 1.2 $
// $Author: langer $
// $Date: 2000-12-26 14:36:48 $

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


// read and write a binary grid file
// uses global grid pointer

#include "charstring.h"
#include "grid.h"
#include "fatalerror.h"
#include "element.h"
#include "elementgroups.h"
#include "initfile.h"
#include "inputflags.h"
#include "main.h"
#include "node.h"
#include "nodegroups.h"
#include "griditerator.h"
#include "vec.h"
#include "readbinary.h"
#include "ctype.h"
#include <stdio.h>
#include "stdlib.h"
#include <string.h>

static Vec<CharString> elementtags;
static Vec<CharString> nodetags;
static Vec<CharString> grouptags;
static Vec<CharString> egrouptags;

static const int ENDLIST = -1;

static int readtags(Vec<CharString>&, FILE*);
static CharString readtag(FILE*);
static int readnodes(FILE*);
static int readelements(FILE*);
static int readgroups(FILE*);
static int readEgroups(FILE*);

// --------------------------------------------------- //

int binaryread2(FILE *file) {
  if(!readtags(elementtags, file)) return 0;
  if(!readtags(nodetags, file)) return 0;
  if(!readtags(grouptags, file)) return 0;
  if(!readnodes(file)) return 0;
  if(!readelements(file)) return 0;
  if(!readgroups(file)) return 0;
  return 1;
}

int binaryread(FILE *file) {
  if(InitInfo::inputversion < 3) {
    if(!binaryread2(file))
      return 0;
  }
  else {
    for(;;) {
      CharString tag;
      tag = readtag(file);
      if(tag == "" || atoi(tag) == ENDLIST) {
	break;
      }
      else if(tag == "elements") {
	if(!readtags(elementtags, file)) return 0;
      }
      else if(tag == "nodes") {
	if(!readtags(nodetags, file)) return 0;
      }
      else if(tag == "nodegroups") {
	if(!readtags(grouptags, file)) return 0;
      }
      else if(tag == "elementgroups") {
	if(!readtags(egrouptags, file)) return 0;
      }
    }
    if(!readnodes(file)) return 0;
    Element::reset_defaults();
    if(!readelements(file)) return 0;
    if(!readgroups(file)) return 0;
    if(!readEgroups(file)) return 0;
  }
  
  return 1;
}


// --------------------------------------------------- //

static CharString readtag(FILE *file) {	// return a single line
    char line[1024];
    if(fgets(line, sizeof(line), file)) {
	char *ptr = line;
	while(isspace(*ptr) && *ptr != '\n') ptr++; // skip initial space
	char *c = strchr(ptr, '\n');		    // terminate cleanly
	if(c) *c = '\0';
	return CharString(ptr);
    }
    else
	return CharString("");
}

static int readtags(Vec<CharString>& taglist, FILE *file) {
    char line[1024];
    taglist.resize(0);
    while(fgets(line, sizeof(line), file)) {
	char *ptr = line;
	while(isspace(*ptr) && *ptr != '\n') ptr++; // skip initial space
	char *c = strchr(ptr, '\n');		    // terminate cleanly
	if(c) *c = '\0';
	if(atoi(ptr) == ENDLIST) return 1;  // done reading this list
	taglist.grow(1, ptr);
    }
    garcon()->msout << ms_error << "EOF encountered while reading tags!"
		    << endl << ms_normal;
    return 0;
}

// --------------------------------------------------- //

static int readnodes(FILE *file) {
  // short hand...
  Vec<NodeTypeRegistration*> &noderegister = node_registry();

    /* sort readers by tag number. -1 indicates this type doesn't
     * occur in the data file
     */
  Vec<int> registrationnumber(nodetags.capacity(), -1);
  for(int i=0; i<nodetags.capacity(); i++) {
    for(int j=0; j<noderegister.capacity(); j++) {
      if(noderegister[j]->name() == nodetags[i]) {
	registrationnumber[i] = j;
	break;
      }
    }
    if(registrationnumber[i] == -1) {
      garcon()->msout << ms_error << "Unknown node type \"" << nodetags[i]
		      << "\" found" << endl << ms_normal;
      return 0;
    }
  }
    
  // read and create nodes
  int type;
  int status = 1;
  int count = 0;
  while(readbinary(file, type) && type != ENDLIST && status) {
    if(type >= registrationnumber.capacity()) {
      garcon()->msout << ms_error << "Bad node type " << type
		      << endl << ms_normal;
      status = 0;
    }
    else {
      status = noderegister[registrationnumber[type]]->create(file);
      count++;
    }
  }
  return status;
}

// ---------------------------------------------------------- //

static int readelements(FILE *file) {
  // short hand...
  Vec<ElementTypeRegistration*> &elemregister = element_registry();

    // Assign tag numbers used in the data file to registration
    // numbers (indices into the element registry).  Unknown element
    // tags are marked with -1.
  Vec<int> registrationnumber(elementtags.capacity(), -1);
  for(int i=0; i<elementtags.capacity(); i++) {
    for(int j=0; j<elemregister.capacity(); j++) {
      if(elemregister[j]->name() == elementtags[i]) {
	registrationnumber[i] = j;
	break;
      }
    }
  }
    
  // read and create elements
  int type;
  int status = 1;
  int count = 0;
  int recyclecount = 0;
  while(readbinary(file, type) && type != ENDLIST && status) {
    if(type >= registrationnumber.capacity()) {
      garcon()->msout << ms_error << "Bad element type " << type
		      << endl << ms_normal;
      status = 0;
    }
    else {
      /* the "create" member of the element registration is the
       * element's binaryread routine.
       */
      TrueFalse ok;
      Element::index_dflt = count;
      int regno = registrationnumber[type];
      if(regno == -1) {
	garcon()->msout << ms_error << "Unknown element type \""
			<< elementtags[type] << "\"" << endl << ms_normal;
	status = 0;
      }
      else {
	Element *elem = elemregister[regno]->create(file, ok);
	status = (elem != 0);
	count++;
	if(Element::inputformatflag & DUPLICATE) recyclecount++;
      }
    }
  }
  if(!status) garcon()->msout << ms_error << "Failed to read elements!"
			      << endl << ms_normal;
  return status;
}


// ---------------------------------------------------------- //

static int readgroups(FILE *file) {
    for(int i=0; i<grouptags.capacity(); i++) {
	NodeGroup *group = grid->getgroup(grouptags[i], TF_TRUE);
	int nodenumber;
	while(readbinary(file, nodenumber)) {
	    if(nodenumber == ENDLIST) break;
	    if(nodenumber >= grid->node.capacity()) {
	      garcon()->msout << ms_error << "Failed to add node "
			      << nodenumber << " to group " << grouptags[i]
			      << "!" << endl << ms_normal;
		return 0;
	    }
	    group->add(grid->node[nodenumber]);
	}
    }
    return 1;
}

// ---------------------------------------------------------- //

static int readEgroups(FILE *file) {
    for(int i=0; i<egrouptags.capacity(); i++) {
	ElementGroup *group = grid->getEgroup(egrouptags[i], TF_TRUE);
	int elemnumber;
	while(readbinary(file, elemnumber)) {
	    if(elemnumber == ENDLIST) break;
	    if(elemnumber >= grid->interiorE.capacity()) {
	      garcon()->msout << ms_error << "Failed to add element "
			      << elemnumber << " to group " << egrouptags[i]
			      << "!" << endl << ms_normal;
		return 0;
	    }
	    group->Add(grid->interiorE[elemnumber]);
	}
    }
    return 1;
}

