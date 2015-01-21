// -*- C++ -*-
// $RCSfile: version.C,v $
// $Revision: 1.80 $
// $Author: langer $
// $Date: 2006-01-12 16:59:54 $

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */

#include "version.h"
#include "charstring.h"

const CharString &ppm2oofversion() {
  // Don't change the format of the following line, or the webinstall
  // script won't work.
  static const CharString version("1.1.30"); // VERSIONNUMBER
  return version;
}

/*

version 0.1.0  This is the first numbered version.

version 0.2.0  Has (almost) all the functionality of the GL version, plus a bit.

version 0.2.1  GUI for setting colors.

version 0.3

7/2/97 Selected mesh triangles drawn. Now all functions of the GL
version are working in the X version.

version 0.3.1

7/3/97 Avoid core dump when attempting to add groups when no image has
been loaded.

version 0.3.2

7/7/97 Fixed inversion of image in goof file. Bug was introduced in
conversion to X.

7/8/97 Top and bottom node groups also had to be switched!

version 0.3.3

7/17/97 Support for 8 bit displays. Menus can be unreadable if other
apps are using too many colors, but images are ok. Color reduction
(from 24 to 8 bit images) isn't as good as it could be.

version 0.3.4

7/17/97 Use X resources & fl_get_app_resources instead of getopt for
command line arguments.

7/30/97 Fixed bugs in color reduction that could cause crashes.  Save
"select method" settings when loading new image, and don't crash when
setting is made when no image is loaded.

version 0.3.5

9/11/97 Updated scrollcanvas and related files with new versions from
OOF.

version 0.4

9/16/97 Nothing new, except a nice round version number to put on the
web.

version 0.4.1

11/04/97 Hexagonal material.

11/04/97 Changed -x option to -s. -x is an undocumented XForms option,
so it can't be used when using XForms' option handling code.

11/04/97 Orthorhombic material.

11/04/97 Changed order of arguments in material functions so that they
correspond to OOF's argument lists.

version 0.4.2

11/05/97 Prohibit spaces in group names.  Do all PPM/PGM/PBM reading
ourselves, no longer link to libppm.a, etc. Raw PBM files aren't
working...

11/07/97 Fixed raw PBM problem. The pbm man page and xv have slightly
different ideas about what the pbm format is. We're assuming for now
that xv is right.

version 0.4.3

11/18/97 Fixed bug where despeckle was reporting the wrong number of
additional pixels selected.

11/18/97 New menu routines, fixing bug in displaying commands executed
from log files.

11/18/97 Firewalls!

11/18/97 Log file records scroll bars (not quite working) and zoom button.

version 0.5.0

2/13/98 Upgrade to XForms 0.88.
        Changed command line options!

version 0.5.1

3/24-31/98 Adaptive mesh using newest node bisection.

version 0.5.2

4/16/98 Output of adaptive mesh.

version 0.6.0

6/18/98 Catch and ignore BadWindow errors which can arise from clicking
        on a window as it's disappearing.

[67]/-/98  New controls on image windows!

7/1/98  "Home" key on keyboard brings up root menu.
        "also_select" command in groups menu to select a group without
        unselecting current selection.

7/6/98  Mesh triangles can be put into groups.
7/7/98  Pixels and triangles can be selected by dragging the mouse.

7/9/98  Mesh triangles can inherit groups from pixels.
        Mesh groups are written in goof file.
	Command to save log file immediately, not just while quitting.

7/-/98  Completely revised adaptive_mesh menus.

version 0.6.1

7/23/98  Put undo/redo/clear/none back into pixel selection menu.
7/29/98  Select the intersection of a group with the set of selected pixels.
7/30/98  Unselect pixel groups.

version 0.6.2

8/28/98  Remove pixels from a group.
         Moved firewall submenu to root menu.
         Sub-pixel triangles in adaptive mesh now associated w/correct pixel.

version 0.6.3

9/2/98   Order in which materials appear in menus is same as in OOF.
         Included Zimmer element.
9/3/98   Included Griffith & Griffith2 elements.
         Fixed bug that was flipping simple_mesh output upside down.

version 0.6.4

9/10/98  Fixed bug introduced while fixing flipped mesh in 0.6.3!

version 1.0

9/16/98  Just renamed version 0.6.4

version 1.0.1

10/29/98  Fixed memory leak in ColorTree::operator=
          New Demograyfy and DemoRGB commands for pixel selection from menu.

version 1.0.2

11/4/98  Sped up Demograyfy and DemoRGB and added "replace" argument.

version 1.0.3

12/22/98 Fixed bug that dumped core when saving adaptive meshes when
         there were empty pixel groups.

version 1.0.4

1/6/99  Image modification tools. Smooth, non-linear smooth, convert to gray.
        Crude interface, though.
1/12/99 New interface routines, colored text in message window.

version 1.0.5

1/21/99 GUI to image modification tools. Undo/redo for image modifications.
        Each graphics window can have separate modifications. Image gallery.

1/22/99 When adding pixels to groups, the new group size is reported
        correctly even when a pixel is added to a group to which it
        already belongs.  The "pixelgroup info" command reports the
        fraction of the image occupied by the group.

1/25/99 Avoided core dump when loading a non-existent ppmfile.  Fixed
        bug whereby misplaced hidden "Home" button in function windows
        could sometimes be triggered.

1/27/99 Select elliptical and circular regions with pixel selection tool.

1/29/99 Changed button for selecting pixel drag shape into a pulldown menu.

2/1/99  Expand selection to include neighboring pixels.
        Shrink selection to remove pixels near edges.

version 1.0.6

2/10/99 Avoided another core dump when attempting to load a non-existent
        ppmfile.

2/11/99 Fixed bug in which pixels on the boundary of adaptive mesh
        triangles weren't assigned to the correct triangles.

2/12/99 Info dashboard can mark the selected pixel and the pixels
        belonging to adaptive mesh triangles.

version 1.0.7

2/19/99 Added orientation to zimmer element argument list. It had been in
        the goof file, but there had been no way to set it.

version 1.0.8

3/2/99  Installed kludge that lets programs compiled under IRIX 6.5
        run under IRIX 6.2.

version 1.0.9

4/1/99  Fixed crash when unselecting an empty pixelgroup.
        New more robust vec.C.


version 1.1
6/1/99  Cancelling a "save" in the image gallery works.
        Better adaptive meshing:

6/24/99 Better "STOP" button, controlled by forked process.

1.1b5
7/22/99 Still better "STOP" button, using XEventsQueued. No forked
        process, graphics windows are updated, but cpu isn't sucked
        up.

        Fixed bug in which elements could mysteriously become selected
        during edge swaps.

1.1b6
8/17/99 Laplacian mesh smoothing.
8/25/99 Undo/Redo commands for mesh modifications.

1.1b7
9/7/99 -quit command line option.

1.1b8
9/8/99  Fixed bugs in transferring pixel groups to mesh triangles.
9/10/99 Can now define an active region -- selections, mesh operations, etc
        take place only within the region.

1.1b9
9/13/99 New commands "adaptive_mesh refine interface" and
        "adaptive_mesh select interface" for acting on triangles
	whose neighbors have different materials assigned to them.

1.1b10
9/15/99  Fixed mesh refinement bug wrt tracking which triangles touch
         a node. The results of the smooth and anneal commands were affected.

9/16/99  Mesh triangles are faded when not in the active region.
9/21/99  Got rid of garbage at beginning of lines in Message Window
         "save" output.
         New refinement method "adaptive_mesh refine divison=smallest_E"


1.1b11
9/27/99  "min_area" variable to limit mesh subdivision.

1.1b12
10/25/99  Fixed a compilation flag for Linux PC version, so that it correctly
          swaps bytes in binary goof files.

1.1b13
11/9/99  Added trigonal element.
11/10/99 Added zimmer2 element.

1.1b14
11/10/99 Changed trigonal to trigonalA and trigonalB.
11/12/99 Added trigonalCrack.

1.1b15
11/16/99 Fixed crash when setting adaptive_mesh/stack/stacksize before
         creating the mesh. stacksize=1 no longer has a memory leak.

11/17/99 Changed order of libraries in SGI version to avoid memory
         fragmentation problems (!).

1.1b16
11/18/99 Fixed a completely inconsequential memory leak.

1.1b17
12/7/99  Fixed node group assignments in simple mesh output.

1.1b18
2/22/00  Fixed a core dump when using the menu "quit" command (not the
        "Quit" button) when certain dashboards were open.

1.1b19
3/10/00  Added "adaptive_mesh refine bumps", which doesn't work quite
         as expected.

1.1b20
3/20/00  New commands to save current state: "configuration save"
         and "configuration load".
3/20/00  Prohibit pixelgroups with no name.
3/22/00  Added ability to read gzipped ppm files.
3/23/00  Display version number on main window and in title bars.
3/28/00  Fixed a bug in which zooming the window would change which pixel was
         marked in the Pixel dashboard.
3/28/00  Fixed a bug in which the highlighted triangle sometimes wasn't
         updated correctly in the Mesh dashboard.
3/30/00  -start command line option can now read configuration files.
         The quit dialog box saves configuration files, not log files.
3/31/00  Fixed a bug in the application of stencils to gray images in the
         Image_Modification dashboard.
4/3/00   New images are displayed immediately instead of just being added to
         the gallery list. If the previous image has been modified, the new
         image isn't displayed unless the user says so, so as not to lose
         the modifications.

1.1b21
5/26/00  Added pause command.

1.1b22
6/2/00   Changed Demography command so that it considers red, green, and blue
         separately, instead of looking only at their average.

1.1b23
6/6/00   Sped up selection of rectangles, circles, and ellipses.

1.1b24
6/9/00   Rewrote the burn algorithm to avoid seg faults on very very
         large burns.

1.1b25
6/22/00  Brought menu code up-to-date with changes in OOF version.
         Wrote a function to display images statistics, including the mode
	 and average pixel values (on 0-255).  (DMV)
6/30/00  Wrote a function to calculate the Eigenvalues for a selection region
         of the image to determine if the selection is a grain or the material
	 between grains. (DMV)
7/5/00   Added GUI and menu components for selection statistics and image
         equalization.
7/9/00   Fixed a bug in reading raw pgm image files.
7/10/00  Completed the equalization function. (DMV)
7/13/00  Worked around an XForms/Intel bug that prevented the "Info for..."
         menu in the Mesh dashboard from working correctly.

1.1b26
7/18/00  Terminate the recursive division of triangles if the area gets
         too small.

1.1b27   
7/20/00  Do it correctly....  Added "adaptive_mesh/refine/max_divisions".
7/24/00  Added the automation dashboard.
7/27/00  Mark selected pixels with a translucent color.
7/27/00  Sped up image copying, somewhat.
7/28/00  Removed the restriction against alpha_E=1 and division=smallest_E, 
         since the limit on max_divisions takes care of it.
7/28/00  Added "redraw" variable to adaptive_mesh menu.

1.1b28
8/16/00  Fixed a bug in which the close button on graphics windows wasn't
         logged correctly.
8/16/00  Fixed a bug which could cause crashes when running scripts without
         opening a valid image file.
8/16/00  Fixed a bug that could lead to infinite loops in mesh refinement,
         under hopefully obscure circumstances.

1.1b29
8/22/00  Rewrote the mesh refinement code to really fix the bug that wasn't
         quite fixed in 1.1b28.  NO  -- IT'S STILL THERE
8/24/00  Finally fixed the mesh refinement bug.
8/25/00  Added an "opacity" variable to the graphics menu, controlling
         the opacity of the selected pixel color.

1.1b30
8/27/00  Added text mode.
8/28/00  Added area info to adaptive_mesh/group/info commands.

1.1b31
8/30/00  Fixed a bug that led to infinitely large configuration files when
         adaptive_mesh/stack/stacksize=1.

1.1b32
10/4/00  The only_once flag for the damage element wasn't stored properly
          in the .goof files.

1.1T   Thermal conductivity.

1.1.1

10/13/00 Merged thermal and regular versions. Using the -DTHERMAL
         compiler flag produces the thermal version.

	 Added Doug Twisselmann's Twissel Element (isotropic with a T
	 independent stress free strain).

	 Sorted the materials/default menu correctly.

1.1.2

10/17/00 Non-thermal version accidentally printed THERMAL on startup.
         
         Workaround for XForms bug which makes command line argument
         parsing fail when the program name has more than one dot in
         it (as in "ppm2oof1.1.1").

1.1.3

10/18/00  Added random seed variable to the main menu.
10/19/00  Added gas material.

1.1.4

10/27/00 Found a bug in removing pixels from groups, that could lead to
         problems in the automation dashboard.

         Rewrote the pixel/triangle intersection code to be more robust
         when nodes fall exactly on pixel boundaries and vice versa.

11/01/00 Fixed a bug that could lead to crashes or nonsense after deleting
         pixel groups.

1.1.5

11/02/00 Can now specify multiple files with the -start flag.  Files must
         be separated by commas.  Names beginning with an exclamation point
         are interpreted as ppm2oof commands.  For example,
	 ppm2oof -start "file1,file2,\!adaptive_mesh save filename=output"
         runs file1, then runs file2, then runs the command "adaptive mesh
         save" with the given argument.  The exclamation point is preceded by
         a backslash to prevent the Unix shell from interpreting it, and the
         whole string is enclosed in quotation marks because it contains spaces.
         Commas that aren't delimiting file names or commands must be
         preceded with a backslash.

         Upgraded to XForms library version 0.89.5.

11/03/00 Added cubic_crack material.

1.1.6

11/18/00 Fixed a bug recently (I hope) introduced in lookfor() that made it
         impossible to read configuration files.

1.1.7

12/26/00 Fixed a bug in comparing empty materials to other material
         types, that messed up the assignment of materials to
         triangles in the adaptive mesh.  Triangles on the edges of
         empty regions were sometimes not marked empty when they
         should have been.

1.1.8

01/11/01 Triangle's materials sometimes weren't updated correctly
         when nodes had moved.

1.1.9

01/12/01 Fixed a bug in which mouse clicks were being misinterpreted.
         It only happened if the pixmap was the same size as the last
         pixmap clicked on, but the image was a different size.  Fixed
         by adding a pixind_outofdate flag to ImageCanvas, to force
         ImageCanvas::mouselocation to recompute its lookup tables.

1.1.10

01/30/01 Fixed a bug in emptyelement (again!) that was preventing
         empty elements with one color from being replaced with empty
         elements of another color.

         Fixed a bug in materials/reset_selected that could crash the
         program. 

1.1.11

02/23/01 Hacked PostScript output from the Save button on the Graphics window.

1.1.12

03/13/01 Make sure groups are weeded before computing element homogeneity.
         Make sure that each pixel thinks it's in each group at most once.


1.1.13

04/02/01 Added "adaptive_mesh/material/set" commands to explicitly set
         the material of the selected triangles.  Any subsequent
         action that changes the triangles (eg refining, moving nodes)
         will make them forget the material assigned by hand and use
         the material inherited from the pixels instead.  So use this
         menu only to correct problems just before saving a mesh.

	 Removed the material_cell datum from AMTriangle

1.1.14
04/06/01 Fixed a bug introduced in 1.1.13.  The inhibit_inheritance flag
         wasn't being set correctly when copying mesh triangles.

1.1.15

04/12/01 Fixed a bug in loading meshes from configuration files. The
         "sanity check" function would fail for refined meshes loaded
         from configuration files.  I don't know if there were any other
         effects. 

04/18/01 Fixed a longstanding bug that was making the Image Gallery
         dashboard ask if you wanted to overwrite a modified image,
         even if the image had just been saved.


1.1.17

04/24/01 Fixed an infinite loop in /adaptive_mesh/groups/subtract.
         I guess nobody ever used that function before.


1.1.18   

05/09/01 Groups assigned by hand to triangles with /adaptive/mesh/groups/add
         and /adaptive/mesh/groups/subtract now persist.

         Changed the format of configuration files so that groups and materials
         assigned to triangles by hand are always correct.

1.1.19

06/06/01 Don't dump core, just quit when fatalerror() is called, unless DEBUG
         is defined at compilation time.

06/13/01 Added oof2 output.  This isn't useful to the general public yet.

1.1.20

11/08/01 Added tetragonal element, finally.
         Added gradyation element, for Grady White.

1.1.21

02/05/02 Put explicit cast to int in switch statements in amtriangle.C,
         to see if it would compile under gcc 2.96.

1.1.22

10/15/02 Added fdstream and conditional compilation to make gcc 3.1 happy
         on OS X 10.2.  No new features.

1.1.23

02/20/03 Changed the way Color(double,double,double) converts it args to 
	   ints, to avoid roundoff errors on some systems.

1.1.24

08/19/03 Added valeria element contributed by Valeria Cannilo.

1.1.25

09/10/03 Back-port of OOF2's postscript image output routine.  Image output
         is much more compact now.

1.1.26

10/07/03 Fixed a bug in timestamp.C that could cause errors if the
         timestamp overflowed at just the wrong point.  Very unlikely.

1.1.27

10/30/03 Minor changes to make gcc3.3 in OS X 10.3 happy.

1.1.28

10/22/04 Added oof2 output of adaptive meshes.

1.1.29

8/1/05  Minor changes to make gcc 3.4 happy.

1.1.30

1/12/06 Minor changes to make gcc 4.0.2 happy. 
*/

