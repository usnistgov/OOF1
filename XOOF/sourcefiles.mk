# -*- makefile -*-
# $RCSfile: sourcefiles.mk,v $
# $Revision: 1.18 $
# $Author: langer $
# $Date: 2004-10-14 21:32:04 $

# This software was produced by NIST, an agency of the U.S. government,
# and by statute is not subject to copyright in the United States.
# Recipients of this software assume all responsibilities associated
# with its operation, modification and maintenance. However, to
# facilitate maintenance we ask that before distributing modifed
# versions of this software, you first contact the authors at
# oof_manager@ctcms.nist.gov. 


OOFCFILES = displacements.C distort.C equilibrate.C grid.C init.C	\
main.C mvmult.C output.C node.C initfile.C bc.C element.C shape.C	\
colormap.C cell_coordinate.C meshcoord.C formdrawer.C drawer.C		\
meshdrawer.C colordrawer.C stressdrawer.C selectdrawer.C symmmatrix.C	\
matrixconvert.C modifygrid.C debug.C stiffness.C preconditioner.C	\
timer.C color.C binaryread.C graphics.C twotwo.C colorutils.C		\
scrollcanvas.C oofcanvas.C plotout.C select.C replace.C random.C	\
rubberband.C griditerator.C typeregistry.C addfilter.C trielement.C	\
isoelement.C emptyelement.C nodegroups.C elementgroups.C		\
cubicelement.C eulerangle.C cijkl.C genelement.C damisoelement.C	\
strainstress.C hexelement.C orthorhombic.C orthoexpand.C		\
damageelement.C statistics.C create_form_drawer.C pixmap.C version.C	\
lookfor.C colorbrowser.C mouseclick.C robomouse.C dashboard.C		\
coorddashboard.C infodashboard.C attributes.C colordashboard.C quit.C	\
cursor.C delete_fl_group.C griffith_element.C zimmer_element.C		\
memusage.C freedom.C mutableelement.C myfread.C timestamp.C		\
eigenvalues.C trigonal.C tetragonal.C kessler.C plotdashboard.C		\
elementfunctionbase.C trielementT.C kij.C planarity.C twissel.C		\
gaselement.C substring.C cubic_crack_element.C normtype.C flux.C psgfx.C \
effective.C grady_element.C valeria.C oof2oof2.C
# cubic_aniso_crack_element.C 

OOFHFILES = main.h cg.h debug.h displacements.h element.h generate.h	\
grid.h mvmult.h node.h timer.h hashmesh.h shape.h parameters.h		\
stiffness.h symmmatrix.h bc.h distort.h select.h random.h		\
preconditioner.h sparselink.h output.h replace.h addfilter.h		\
formdrawer.h drawer.h meshdrawer.h colordrawer.h stressdrawer.h		\
beginheader.h endheader.h strainstress.h twotwo.h initfile.h plotout.h	\
color.h binaryread.h readbinary.h createelement.h colorutils.h		\
griditerator.h typeregistry.h trielement.h isoelement.h emptyelement.h	\
groups.h nodegroups.h elementgroups.h cubicelement.h eulerangle.h	\
cijkl.h genelement.h damisoelement.h hexelement.h orthorhombic.h	\
iterator.h orthoexpand.h damageelement.h statistics.h colormap.h	\
pixmap.h selectdrawer.h zoomcursor.h zoommask.h graphics.h		\
infocursor.h infomask.h version.h cell_coordinate.h meshcoord.h		\
scrollcanvas.h oofcanvas.h rubberband.h lookfor.h mouseclick.h		\
robomouse.h dashboard.h coorddashboard.h infodashboard.h attributes.h	\
colordashboard.h quit.h cursor.h delete_fl_group.h griffith_element.h	\
zimmer_element.h memusage.h freedom.h mutableelement.h inputflags.h	\
create_form_drawer.h myfread.h timestamp.h symeig3.h		\
eigenvalues.h trigonal.h tetragonal.h kessler.h stdlib.h ctype.h	\
plotdashboard.h elementfunction.h elementfunctionbase.h linklist2.h	\
trielementT.h kij.h planarity.h twissel.h gaselement.h substring.h	\
cubic_crack_element.h normtype.h effective.h grady_element.h valeria.h
# cubic_aniso_crack_element.h

OOFTEMPLATES = cg.C readbinary.C sparselink.C generate.C groups.C	\
createelement.C hashmesh.C symeig3.C tntlu.h tntfmat.h tntvec.h         \
tntsubscrpt.h elementfunction.C linklist2.C




MENUCFILES = menu.C menuform.C commands.C charstring.C fdcode.C	\
menucolor.C editlineinputhandler.C menuinterrupt.C truefalse.C	\
specialchars.C word.C varobjects.C filename.C menustream.C 

MENUHFILES = menuDef.h menuform.h menustream.h charstring.h		\
editlineinputhandler.h menuinterrupt.h truefalse.h vec.h		\
tocharstring.h specialchars.h word.h enum.h varobjects.h	\
filename.h forms.h menucolor.h menuinterruptform.h

MENUTEMPLATES = vec.C menuDef.C tocharstring.C enum.C enumio.C

# sparselib++, mv++, and iml++ files
SPARSEHFILES = mvvd.h mvvi.h mvvind.h mvmd.h mvblasd.h mvv.h mvvrf.h	\
 mvmrf.h comprow_double.h compcol_double.h coord_double.h		\
 icpre_double.h ilupre_double.h diagpre_double.h qsort_double.h		\
 qsort_int.h vecdefs.h spblas.h

SPARSECFILES = mvvd.C mvvi.C mvmd.C spmm.C spsm.C mvblasd.C		\
comprow_double.C diagpre_double.C ilupre_double.C icpre_double.C	\
qsort_double.C qsort_int.C

CFILES = $(OOFCFILES) $(MENUCFILES) $(SPARSECFILES)
HFILES = $(OOFHFILES) $(MENUHFILES) $(SPARSEHFILES)
TEMPLATES = $(OOFTEMPLATES) $(MENUTEMPLATES)

SOURCEFILES = $(CFILES) $(HFILES) $(TEMPLATES)

ALLFILES = $(SOURCEFILES) GNUmakefile Makefile makefile.generic	\
oofstipple.xbm sourcefiles.mk dependencies.mk
