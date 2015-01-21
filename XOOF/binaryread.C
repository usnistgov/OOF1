// -*- C++ -*-
// $RCSfile: binaryread.C,v $
// $Revision: 1.6 $
// $Author: langer $
// $Date: 2000-10-31 19:15:09 $

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

//#include "cmdlist.h"
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
#include "parameters.h"
#include "timer.h"
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
template <class TYPE> void writetags(const TYPE&, FILE*);
static void writenodes(FILE*);
template <class TYPE> int listindex(const TYPE &, const CharString &);
static void writeelements(FILE*, const int);
static void writegroups(FILE*);
static void writeEgroups(FILE*);

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
  
  grid->redraw();
  return 1;
}

void binarywrite(FILE *file, const int longform) {

  // The output of element groups doesn't work unless all the elements
  // are in the same list.
  grid->unshufflelists();

  fprintf(file, "program = %s\n", programname.charstar());
  fprintf(file, "version number = %d\n", InitInfo::outputversion);
  fprintf(file, "Nnodes = %d\n", grid->node.capacity());
  fprintf(file, "Nelements = %d\n",
	  grid->interiorE.capacity() + grid->boundaryE.capacity());
  fputs("type = b\n", file);
  fputs("elements\n", file);
  writetags(element_registry(), file);
  fputs("nodes\n", file);
  writetags(node_registry(), file);
  if(grid->nodegroup.capacity() > 0) {
    fputs("nodegroups\n", file);
    writetags(grid->nodegroup, file);
  }
  if(grid->elementgroup.capacity() > 0) {
    fputs("elementgroups\n", file);
    writetags(grid->elementgroup, file);
  }
  fputs("-1\n", file);
  
  // binary stuff
  writenodes(file);
  writeelements(file, longform);
  writegroups(file);
  writeEgroups(file);
  
  // end of binary stuff. Remainder is parsed by loadmenu().

  // this section repeats code from initfile.C. It's ugly to repeat it
  // here, but initfile.C uses iostream and this uses stdio.
  for(int i=0; i<grid->nodegroup.capacity(); i++) {
    NodeGroup *ng = grid->nodegroup[i];
    fputs("nodegroup (\n", file);
    fprintf(file, " label=%s\n", ng->name().charstar());
    if(ng->xflag.fixed()) fputs(" fix_x\n", file);
    if(ng->yflag.fixed()) fputs(" fix_y\n", file);
    if(ng->xflag.enslaved()) fputs(" enslave_x\n", file);
    if(ng->yflag.enslaved()) fputs(" enslave_y\n", file);
#ifdef THERMAL
    if(ng->phiflag.fixed()) fputs(" fix_T\n", file);
    if(ng->phiflag.enslaved()) fputs(" enslave_T\n", file);
#endif // THERMAL
    fputs(" incremental_distortion=", file);
    ng->incremental_distortion.print(file);
    fputs("\n", file);
    fputs(" incremental_shift=", file);
    ng->incremental_shift.print(file);
    fputs("\n", file);
    fputs(" incremental_force=", file);
    ng->incremental_force.print(file);
    fputs("\n", file);
    fputs(" absolute_distortion=", file);
    ng->absolute_distortion.print(file);
    fputs("\n", file);
    fputs(" absolute_shift=", file);
    ng->absolute_shift.print(file);
    fputs("\n", file);
    fputs(" absolute_force=", file);
    ng->absolute_force.print(file);
    fputs("\n", file);
#ifdef THERMAL
    fputs(" incremental_thermaldistortion=", file);
    ng->incremental_thermaldistortion.print(file);
    fputs("\n", file);
    fputs(" absolute_thermaldistortion=", file);
    ng->absolute_thermaldistortion.print(file);
    fputs("\n", file);
#endif // THERMAL
    fputs(")\n", file);
  }
  //  grid->bccmds->show(file, "oof");
  //  grid->distortcmds->show(file, "oof");
  get_rootmenu()->print(file, "oof", TF_TRUE);
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

template <class TYPE>
void writetags(const TYPE &list, FILE *file) {
  for(int i=0; i<list.capacity(); i++)
    fprintf(file, "%s\n", list[i]->name().charstar());
  fprintf(file, "%d\n", ENDLIST);
}

template <class TYPE>
int listindex(const TYPE &list, const CharString &which) {
    for(int i=0; i<list.capacity(); i++)
	if(which == list[i]->name())
	    return i;
    garcon()->msout << ms_error << "This really should not happen!"
		    << endl << ms_normal;
    fatalerror();
    /*NOTREACHED*/
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
  //  grid->nodes_done();
  if(verbose)
    garcon()->msout << ms_info << "  Read " << count << " nodes"
		    << endl << ms_normal;
  return status;
}

static void writenodes(FILE *file) {
    for(int i=0; i<grid->node.capacity(); i++) {
	writebinary(file,
	    listindex(node_registry(), grid->node[i]->tag()));
	grid->node[i]->binarywrite(file);
    }
    writebinary(file, ENDLIST);
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
    if(verbose) garcon()->msout << ms_info
				<< "  Read " << count << " elements" << endl
				<< "  Recycled " << recyclecount << " elements"
				<< endl << ms_normal;
    return status;
}

static void writeelements(FILE *file, const int longform) {
    ElementIterator iter(EI_ALL, grid);
    Element *el;
    while(el = iter()) {
	writebinary(file,
	    listindex(element_registry(), el->tag()));
	el->binarywrite(file, (longform? STIFFNESS : char(0)));
    }
    writebinary(file, ENDLIST);
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
	    group->Add(grid->node[nodenumber]);
	}
    }
    return 1;
}

static void writegroups(FILE *file) {
    for(int i=0; i<grid->nodegroup.capacity(); i++) {
	NodeGroup *group = grid->nodegroup[i];
	GroupIterator<Node> iter(*group);
	Node *n;
	while(n = iter())
	    writebinary(file, n->index);
	writebinary(file, ENDLIST);
    }
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

static void writeEgroups(FILE *file) {
  for(int i=0; i<grid->elementgroup.capacity(); i++) {
    ElementGroup *group = grid->elementgroup[i];
    GroupIterator<Element> iter(*group);
    Element *elem;
    while(elem = iter())
      writebinary(file, elem->index);
    writebinary(file, ENDLIST);
  }
}
