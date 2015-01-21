# -*- makefile -*-
# $RCSfile: sourcefiles.mk,v $
# $Revision: 1.7 $
# $Author: langer $
# $Date: 2000-12-26 14:36:49 $

# This software was produced by NIST, an agency of the U.S. government,
# and by statute is not subject to copyright in the United States.
# Recipients of this software assume all responsibilities associated
# with its operation, modification and maintenance. However, to
# facilitate maintenance we ask that before distributing modifed
# versions of this software, you first contact the authors at
# oof_manager@ctcms.nist.gov. 


ABQCFILES = binaryread.C createelement.C element.C elementgroups.C	\
emptyelement.C eulerangle.C genelement.C grid.C groups.C initfile.C	\
isoelement.C main.C meshcoord.C node.C nodegroups.C trielement.C	\
griditerator.C typeregistry.C cijkl.C symmmatrix.C random.C		\
addfilter.C twotwo.C lookfor.C mutableelement.C myfread.C		\
cubicelement.C hexelement.C trigonal.C cubic_crack_element.C		\
damageelement.C damisoelement.C griffith_element.C tetragonal.C		\
kessler.C orthorhombic.C orthoexpand.C twissel.C zimmer_element.C

ABQHFILES = binaryread.h createelement.h element.h elementgroups.h	\
emptyelement.h eulerangle.h genelement.h grid.h groups.h initfile.h	\
isoelement.h main.h meshcoord.h node.h nodegroups.h trielement.h	\
fatalerror.h inputflags.h beginheader.h endheader.h griditerator.h	\
typeregistry.h readbinary.h myfread.h iterator.h cijkl.h symmmatrix.h	\
random.h addfilter.h twotwo.h lookfor.h mutableelement.h sparselink.h	\
cubicelement.h hexelement.h trigonal.h cubic_crack_element.h		\
damageelement.h damisoelement.h griffith_element.h tetragonal.h		\
kessler.h orthorhombic.h orthoexpand.h twissel.h zimmer_element.h

ABQTEMPLATES = readbinary.C groups.C createelement.C readbinary.C	\
enumio.C sparselink.C

MENUCFILES = menu.C charstring.C editlineinputhandler.C truefalse.C	\
specialchars.C word.C filename.C menustream.C menuinterrupt.C

MENUHFILES = menuDef.h menustream.h charstring.h			\
editlineinputhandler.h truefalse.h vec.h tocharstring.h specialchars.h	\
word.h enum.h filename.h menuform.h menuinterrupt.h varobjects.h

MENUTEMPLATES = vec.C menuDef.C tocharstring.C enum.C enumio.C

# sparselib++, mv++, and iml++ files
SPARSEHFILES = mvvd.h mvvind.h mvvrf.h

SPARSECFILES = mvvd.C


CFILES = $(ABQCFILES) $(MENUCFILES) $(SPARSECFILES)
HFILES = $(ABQHFILES) $(MENUHFILES) $(SPARSEHFILES)
TEMPLATES = $(ABQTEMPLATES) $(MENUTEMPLATES)

SOURCEFILES = $(CFILES) $(HFILES) $(TEMPLATES)
