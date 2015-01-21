// -*- C++ -*-
// $RCSfile: typeregistry.h,v $
// $Revision: 1.1 $
// $Author: langer $
// $Date: 2000-12-22 14:11:10 $


/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


#ifndef TYPEREGISTRY_H
#define TYPEREGISTRY_H

class NodeTypeRegistration;
class ElementTypeRegistration;

#include <stdio.h>
#include "charstring.h"
#include "element.h"
#include "vec.h"

/* This keeps a list of binary readers for Node and Element derived types,
 * associating their names with the functions that do the reading.
 * The functions should probably be static member functions of the
 * derived class, and are of the form
 *	int func(FILE*).
 * They use the global grid pointer in parameters.h.
 * They return 1 if successful,  0 otherwise.
 * Each Node or Element must have a static XXXXTypeRegistry member.
 * The name of this static member is irrelevant, since the registry
 * constructor keeps a list of all readers, and all access is
 * done through this list.
 *
 * TYPE is either "Node" or "Element".
 */
 
/* these functions allocate and return a pointer to the registries. 
 * doing it via a function ensures (hopefully) that the registry is
 * allocated before it is used.
 */
 
Vec<NodeTypeRegistration*> &node_registry();
Vec<ElementTypeRegistration*> &element_registry();

class NodeTypeRegistration {
    private:
	CharString label;
    public:
	int (*create)(FILE*);

	NodeTypeRegistration(const CharString &nm, int (*fn)(FILE*))
	    : label(nm), create(fn)
	{
	    node_registry().grow(1, this);
	}
	
	const CharString &name() { return label; }
	
};

class ElementTypeRegistration {
    private:
	CharString label;
    public:
	Element *(*create)(FILE*, TrueFalse&);
	
	ElementTypeRegistration(const CharString &nm,
					    Element *(*fn)(FILE*, TrueFalse&))
	    : label(nm), create(fn)
	{
	    element_registry().grow(1, this);
	}
	const CharString &name() { return label; }

    friend class ElementTypeIterator;
};

class ElementTypeIterator {
    private:
	int index;
    public:
	ElementTypeIterator() : index(-1) {}
	CharString const *operator()() {
	    index++;
	    if(index < element_registry().capacity())
		return &element_registry()[index]->label;
	    else
		return 0;
	}
	
};

#endif
