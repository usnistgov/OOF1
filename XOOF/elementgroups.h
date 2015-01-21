// -*- C++ -*-
// $RCSfile: elementgroups.h,v $
// $Revision: 1.3 $
// $Author: langer $
// $Date: 2000-09-06 20:10:56 $


/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


#ifndef ELEMENTGROUPS_H
#define ELEMENTGROUPS_H

class ElementGroup;

#include "charstring.h"
#include "element.h"
#include "groups.h"
#include "menuDef.h"
#include <iostream.h>

class ElementGroup : public Group<Element> {
    public:
	ElementGroup(const CharString &name) : Group<Element>(name) {}
	virtual ~ElementGroup();
	virtual void Add(Element*);
	virtual void Remove(Element*);
};

ostream &operator<<(ostream&, const ElementGroup&);

CommandFn selectelemgroup();
Menu *elemgroupmenu();
ElementGroup *Egroupname();

#endif
