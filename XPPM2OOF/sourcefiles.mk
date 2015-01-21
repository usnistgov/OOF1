# -*- makefile -*-
# $RCSfile: sourcefiles.mk,v $
# $Revision: 1.17 $
# $Author: langer $
# $Date: 2004-10-22 13:51:09 $

# This software was produced by NIST, an agency of the U.S. government,
# and by statute is not subject to copyright in the United States.
# Recipients of this software assume all responsibilities associated
# with its operation, modification and maintenance. However, to
# facilitate maintenance we ask that before distributing modifed
# versions of this software, you first contact the authors at
# oof_manager@ctcms.nist.gov. 

PPMCFILES = burn.C goof.C ppm2oof.C color.C pixelgroups.C material.C	\
materialT.C isomaterial.C emptymaterial.C output.C select_by_color.C	\
despeckle.C selectpixel.C adaptmesh.C amtriangle.C meshcmds.C		\
cubicmaterial.C eulerangle.C random.C damagematerial.C			\
orthoexmaterial.C image.C imageform.C pixmap.C version.C gfx.C		\
arraybase.C scrollcanvas.C imagecanvas.C mouseclick.C meshcoord.C	\
robomouse.C cell_coordinate.C lookfor.C quit.C colorutils.C		\
hexmaterial.C orthorhombicmaterial.C imagedrawer.C dashboard.C		\
coordinates.C cursor.C rubberband.C displaydash.C selectdash.C		\
infodash.C elector.C timestamp.C burner.C zimmermaterial.C		\
griffithmaterial.C myfread.C imagemandash.C imagemoddash.C imagemod.C	\
ishape.C expandsel.C fiddlemesh.C amintersection.C timer.C		\
amtriangleiterator.C noderubberband.C restrict.C shape.C trigonal.C	\
memusage.C saveconfig.C addfilter.C compressedfile.C equalize.C		\
automation.C autodash.C system.C planarity.C twismaterial.C		\
gasmaterial.C substring.C cubic_crack_material.C psgfx.C tetragonal.C   \
gradymaterial.C valeria.C ppm2oof2.C
## arson.C

PPMHFILES = burn.h goof.h color.h groups.h array.h material.h		\
materialT.h readbinary.h isomaterial.h emptymaterial.h output.h		\
select_by_color.h despeckle.h selectpixel.h ppm2oof.h adaptmesh.h	\
amtriangle.h meshcmds.h meshcoord.h cubicmaterial.h eulerangle.h	\
random.h damagematerial.h orthoexmaterial.h cell_coordinate.h pixmap.h	\
version.h image_drawer2.h scrollcanvas.h imagecanvas.h mouseclick.h	\
gfx.h imageform.h robomouse.h lookfor.h quit.h materialcmd.h		\
colorutils.h colortree.h linklist.h hexmaterial.h			\
orthorhombicmaterial.h dashboard.h coordinates.h cursor.h rubberband.h	\
displaydash.h selectdash.h infodash.h infocursor.h infomask.h		\
zoomcursor.h zoommask.h checkcursor.h checkmask.h pixelgroups.h		\
meshgroups.h elector.h timestamp.h burner.h zimmermaterial.h		\
griffithmaterial.h myfread.h image_drawer2.c imagemandash.h		\
imagemoddash.h rastack.h namedimage.h stencil.h ishape.h expandsel.h	\
fiddlemesh.h timer.h amtriangleiterator.h noderubberband.h		\
sparselink.h circlestack.h restrict.h shape.h trigonal.h memusage.h	\
stdlib.h ctype.h saveconfig.h addfilter.h beginheader.h endheader.h	\
image.h compressedfile.h autodash.h system.h inputflags.h planarity.h	\
twismaterial.h gasmaterial.h substring.h cubic_crack_material.h         \
tetragonal.h gradymaterial.h valeria.h
## arson.h

PPMTEMPLATES = readbinary.C materialcmd.C linklist.C groups.C array.C	\
rastack.C sparselink.C circlestack.C colortree.C

MENUCFILES = menu.C menuform.C commands.C charstring.C fdcode.C	\
menucolor.C editlineinputhandler.C menuinterrupt.C truefalse.C	\
specialchars.C word.C varobjects.C filename.C menustream.C

MENUHFILES = menuDef.h menuform.h menustream.h charstring.h		\
editlineinputhandler.h menuinterrupt.h truefalse.h vec.h		\
tocharstring.h specialchars.h word.h enum.h varobjects.h filename.h	\
forms.h menucolor.h menuinterruptform.h

MENUTEMPLATES = vec.C menuDef.C tocharstring.C enum.C enumio.C

SPARSEHFILES = mvvd.h mvvind.h mvvrf.h mvmd.h mvmrf.h

SPARSECFILES = mvvd.C mvmd.C

BITMAPS = oofstipple.xbm

CFILES = $(PPMCFILES) $(MENUCFILES) $(SPARSECFILES)
HFILES = $(PPMHFILES) $(MENUHFILES) $(SPARSEHFILES)
TEMPLATES = $(PPMTEMPLATES) $(MENUTEMPLATES)

SOURCEFILES = $(CFILES) $(HFILES) $(TEMPLATES) $(BITMAPS)

ALLFILES = $(SOURCEFILES) GNUmakefile Makefile makefile.generic	\
sourcefiles.mk dependencies.mk
