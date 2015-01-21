
// $RCSfile: version.C,v $
// $Revision: 1.52 $
// $Author: langer $
// $Date: 2006-01-12 16:59:53 $

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

const CharString &oofversion() {
  // Don't change the format of the following line, or the webinstall
  // script won't work.
  static const CharString version("1.1.22");// VERSIONNUMBER
  return version;
}

/*

version 0.1.0  This is the first numbered version.

6/3/97 Thermal color map.

6/4/97 Bug fix: disconnected nodes weren't reconnected when empty
       elements were replaced with non-empty elements.

version 0.1.1

6/4/97 Replace spaces in group names with underscores. Spaces
       couldn't be parsed properly.

version 0.1.2

6/5/97 Installed new menu routines. Changes in variables set in
       scripts are now reflected in GUI menus.

version 0.1.3

6/5/97 Moved preconditioner setting from separate menu to enumerated
       variable with pull down menu.

6/5/97 Got rid of conversion from StressStrain to MV_Vector_double,
       which was confusing the 64 bit compiler.

version 0.1.4

6/5/97 Moved statistics type to enumerated variable w/ pull down menu.

version 0.1.5

6/6/97 Print all settable variables in binary goof file. Print empty
       strings with quotation marks so that they don't look like
       missing arguments when reading the goof file.

version 0.1.6

6/10/97 Print all settable variables in ascii goof file too.

version 0.2
  Identical to version 0.1.6, put on the web site.

version 0.2.1

6/24/97 Increased blocksize for groups so they can be allocated faster.

version 0.2.2

7/1/97 Fixed bug in arithmetic with stress & strain classes that was
       causing "Incompatible vector length" errors.

version 0.3


7/2/97 Reorganized "distort" menu. This breaks (slightly) existing
         goof files. The distortions stored in them won't be read
         correctly. But it turns out that if the files were written by
         oof, they weren't written correctly since version 0.1.5
         anyway.

version 0.3.1

7/7/97 Got rid of old "axis" output commands. Nobody used them anyway,
       presumably.

version 0.3.2

7/9/97 Changed 11 to XX in menus, to avoid confusion. We're told that
       11 is the principle component, not the xx component, of stress.

version 0.3.3

7/15/97 Fixed bug wrt proliferation of disconnected nodes.

version 0.3.4

7/30/97 "distort set reset_params" now works properly.

version 0.3.5

8/5/97 clumsy bug fix. bc & distort commands read from goof file are
       stored in cmdlist with "oof" prefix, since that's the way they
       appear in the file. But then they're written with another "oof"
       prefix, so the next time they're read errors are
       generated. Clumsy fix is to put a oof->oof loop in the root
       menu.  The problem with this is that the goof file keeps
       getting bigger each time it's written.

version 0.3.6

8/6/97 fixed clumsiness in 0.3.5. Goof file still keeps getting
       bigger, because commands are repeated, but oof->oof loop isn't
       needed.

       Plot principal values of stress & strain.

       Got rid of "suffix" variable.

version 0.4.0

8/21/97 8-bit graphics, new ScrollCanvas

version 0.4.1

8/22/97 pixmap backing for color bar, so that it draws faster, and
        draws correctly in 8-bit mode. Not working yet.

version 0.4.2

9/5/97 don't include "principal" and "shear" in element info eigenvalues.
       They were nonsense.

version 0.4.3

9/9/97 change colors in colorbar by clicking on them. Left mouse
       changes color, right mouse prints color info, middle mouse
       reverts to default color.

version 0.5

9/16/97 Nothing new. Changed number to have a round number to put on the web.

version 0.5.1

9/16/97 Can turn wallpaper on and off. Can't change color, though.

version 0.5.2

9/23/97 "Save" button now works in graphics windows.

version 0.5.3

9/25/97 Removed "bc force" command. Forces can be applied to all nodes.

9/25/97 New "distort clear" menu to clear incremental and absolute
        distortions from node groups.

version 0.5.4

10/30/97 Got rid of "-t" command line option, and disabled graphics
         menu when "-g" isn't specified.

         Fancy text start-up screen.

version 0.5.5

10/31/97 Fixed bugs wrt saving macros and defining macros that set variables.

11/3/97 Use XForms code for handling command line arguments. -l
        <logfile> added. -f <cmdfilelist> now requires comma delimited
        list of files, without spaces. -f can't be specified multiple
        times anymore.
        *** -g is no longer needed. Use -t to use the text interface. ***

version 0.5.6

11/19/97 Mouse clicks are recorded in log files and can be played back.
Colorbar editing temporarily disabled.

11/19/97 Record window size changes in log file.

11/19/97 Catch "close" events from window manager, and ignore them, mostly.

11/19/97 Ask to save log file when quitting.

version 0.5.7

11/20/97 Fixed bug in colorbar editing. Middle mouse made colors
revert to slightly wrong colors.

11/20/97 Reinstated color bar editing. Mouse clicks on the colorbar
         are now recorded in the log file.

11/20/97 X cursors are defined only once and freed at the end, instead
         of being defined in each drawer.

version 0.5.8

1/14/98 Inserted missing space in ascii output for hexagonal elements.

1/18/98 No longer require element indices to be in order in ascii goof files.

1/18/98 "Empty Elements" button in Attributes dashboard now works.
         Had been broken in 0.5.6, probably.

version 0.6.0

2/3/98 Installed XForms version 0.88

2/3/98 Changed -f option to -file, since XForms 0.88 uses -f.

2/5/98 Fixed bug. Coordinates Dashboard would sometimes not show limits
       of visible region.

2/5/98 Got rid of single character command line options. XForms
       processes its own options before the application's options, so
       single character options can be mistaken for abbreviated XForms
       options.

2/6/98 Cleaned up memory leaks in dashboards.

2/10/98 Use fl_share_canvas_colormap. Should allow windows to be
        closed w/out crashing on 8 bit displays.

2/24/98 Fixed bug where empty elements replaced with non-empty elements
        weren't equilibrating properly.

version 0.6.1

<3/17/98 Workaround for bugs caused by new XForms library.

3/17/98 Fixed bug with spaces in EulerAngle input.

3/17/98 Allowed E format for floating point inputs.

version 0.6.2

3/18/98 More workarounds for bugs caused by new XForms library.
        Color dashboards with fullscale turned off would crash
        when switching or closing drawers.

version 0.6.3

3/26/98 Upgraded to XForms version 0.88.1 from 0.88.

version 0.6.4

3/27/98 Weight statistical output by element area.

version 0.6.5

4/17/98 Griffith element.

version 0.6.6

5/4/98  Disable "debug Xdebug" in text mode.

version 0.6.7

5/15/98 Don't use fast malloc library -- it uses too much space!

version 0.6.8

5/26/98 Avoid resizing arrays, improved connectivity detection.
        Add Nelements and Nnodes lines to the goof files.

version 0.6.9

5/27/98 Tie degrees of freedom together, so that all the nodes on an
        edge can move in unison. (Not working yet...)

6/1/98 Bug fix: internally generated grids put the wrong node in the topright
        node group unless nx == ny.

6/1/98 Graphics output for shear stress & strain.

version 0.7.0

6/5/98 Slaved nodes finally working, I think.
       Zimmer & Griffith elements revert to unmutated state properly when reset.

6/8/98 Changed single/double click behavior in menus. Single clicks
       now bring up function windows, and double clicks execute
       functions immediately. This is the safe way to do it if you're
       running over a slow connection and the double clicks aren't
       getting through in time. You can recover the old behavior by
       starting with the -singleclickfns option.

version 0.7.1

7/1/98 "Home" key on keyboard brings the root menu to the front.

7/20/98 "select node number" command

7/29/98 "select element number" command

version 0.7.2

7/30/98 Added "griffith_element2" as a stopgap measure. It's still not
        quite correct.

version 0.7.3

8/3/98 Didn't succeed in fixing "output energy" command to account for
       stress free strains.  "output force" is still broken.
8/4/98 "log save" command to save log file now, without quitting.
8/4/98 Fixed InfoDashboard "node type" button.

version 0.7.4

8/26/98 Fixed "output force" command (sign error was causing bad output when
        deltaT was non-zero).
8/27/98 Fixed "output force" -- possible wrong output if force measured after
        nodes were freed.

version 0.7.5

9/15/98 Mutated elements are stored in goof files correctly.

version 1.0

9/15/98 Just renamed version 0.7.5

version 1.0.1

9/29/98 Made the question mark cursor like the one in PPM2OOF, with
        the hotspot below the dot.

version 1.0.2

10/30/98 Fixed bug in "output force". Program would crash if system
         hadn't been equilibrated.

version 1.0.3

11/4/98  Fixed bug in application of incremental distortions when a
         node was in two fixed groups. For example,
            OOF >> bc fix both (top; right)
            OOF >> distort (set (xshift = .1; right) increment)
         didn't move the top right node.

version 1.0.4

1/7/99  Use new ostream class for output. Colored text in message window.
        New commands "output tee ..."

version 1.0.5

1/27/99 New button in Element Info dashboard, for printing element index.

1/29/99 Fixed bug in which applied forces weren't being applied! Bug
        existed since version 1.0.3.

version 1.0.6

3/2/99  Installed kludge that lets programs compiled under IRIX 6.5
        run under IRIX 6.2.

version 1.0.7

3/10/99 Fixed bug in unmutating elements. Zimmer elements didn't
        revert to original orientation. Fixing the bug invalidates old
        goof files that store mutated elements, though.

3/11/99 Mutate and equilibrate commands can be interrupted with "STOP" button.
        Fixed crash when reading ascii goof file after "init uniform ..."
        Avoid double redraw after reading goof file.
        Correct placement of initial image after reading ascii goof file.

4/1/99  New more robust vec.C.

version 1.0.8

5/18/99 Fixed a bug in which the shear stresses weren't being reported
        properly for some elements.

5/25/99 Fixed a bug in which forces weren't being reported correctly
        for enslaved nodes.  The sign convention for forces has also
        been changed -- a positive force on the right edge now means
        that the system is being pulled to the right. (The old
        convention was that "output force" showed the force that the
        sample exerted on the boundaries, rather than the force the
        boundaries exerted on the sample.)
	Changed "applied force" button in NodeInfo dashboard to "force".

	Ported to Sun!

6/9/99  Clicking in the wrong spot on the dashboard selector would bring up
        the main menu window. Fixed by moving a hidden button.

6/15/99 When mutating elements, the principal stress is computed in
         3D, not just in the XY plane, in order to determine the
         coordinate system in which the knockdown is performed.

         Fixed bug: Damage element applied knockdown2 to C_45 twice.

         Restructured Element Info dashboard. All components of stress
         and/or strain are printed when the "components" button is
         lit, and all invariants are printed when the "invariants"
         button is lit.

	 Fixed bug in which not all element info buttons were recorded
         properly in the log file.

	 Strain outputs ("output strain" command, and graphics
	 windows) report the total strain, not the elastic strain.
	 The strain reported in previous versions was the
	 two-dimensional elastic strain used in plane stress and plane
	 strain calculations, which is not the true elastic strain
	 when using plane strain and thermal expansion.

6/16/99 Fixed bug. The griffith2 element hadn't been appearing in the
        menus, but the griffith menu options created griffith2
        elements.  Binary files written by PPM2OOF were read
        correctly.  Binary files written by OOF created only griffith
        elements.  Ascii files written by OOF created only griffith2
        elements.

6/17/99 "STOP" button now doesn't take up 25% of the CPU, uses forked process.

6/23/99 comp_equil now stops if the equilibration step fails to converge.

7/16/99 Fixed plane stress calculation. It no longer crashes when
        out-of-plane moduli vanish.

1.0.8b11
7/22/99 "STOP" button no longer uses forked process, uses XEventsQueued to
        avoid sucking up CPU.

1.0.8b12
8/10/99 Fixed bug in "plot out" -- files now flushed properly.
        "tee" output is now line buffered.

1.0.8b13
9/21/99 Got rid of garbage at beginning of lines in Message Window
         "save" output.
9/29/99 Drawers now remember their settings.
9/30/99 Files written by "output individual ..." now sort elements by
        numerical index.

1.0.8b14
10/1/99 Fixed bug introduced in 1.0.8b13, wrt drawing dashboards.

1.0.8b15
11/9/99 Fixed bug that made "reset" fail to work properly when zimmer
        elements had been mutated more than once.

1.0.8b16
11/9/99  Added trigonal element.
11/10/99 Added Zimmer2 element.

1.0.8b17

11/12/99 Fixed bug where log file contained bad value for
         elementinfo/elementtype flag.
11/12/99 Added trigonalA, trigonalB, and trigonalCrack element, and got rid
         of trigonal.

1.0.8b18

11/17/99 Unknown element types in the binary goof file header no
         longer cause an error unless they're actually used in the
         mesh.

1.0.8b19

12/10/99 Fixed bug in which drawers would refuse to update. Bug was
         introduced in version 1.0.8b13


1.0.8b20

1/28/00  Tetragonal element.
2/2/00   Kessler element.
2/24/00  Work around ios::trunc bug in g++ by removing it.  Bug had broken
         output/individual commands.

1.0.8b21

3/6/00  Added Nnodes and Nelements to ascii goof file.
3/9/00  Fixed a bug in kessler element that made it crash during binary write.

1.0.8b22

4/5/00 Added "enhancement" variable to main menu for enhancing the
       displayed displacements.  Nodes are drawn at their original
       positions plus enhancement times their displacements, so
       enhancement=1 is the normal display and enhancement=0 displays
       all nodes at their original positions.

4/11/00 Added the Plot dashboard to the Drawers, for plotting the displayed
        quantity along a line.

1.0.8.b23
4/20/00 Changed "output/stress/statistics/type=invariant" to
        "eigenvalues", and added a new option called "invariants"
        that prints statistics on the first, second, and third invariants
        of the stress.  The same change applies to "output/strain/statistics".

        Changed the name of the Plot dashboard. It's now called "Cross Section".

1.0.8b24
4/26/00 Added the output/crosssection menu, giving access to the functionality
        of the Cross Section dashboard even in text mode.

5/3/00  Fixed a bug that prevented Drawers from being updated if they had been
        opened but were not visible when the mesh was created.

1.0.8b25
5/12/00 Finally got unequally spaced plot points working in the Cross Section
        dashboard.

1.0.8b26
5/15/00 Moved enhancement variable from the main menu to the graphics menu.

1.0.8b27

5/16/00 Split /output/crosssection/<f> to
        /output/crosssection/uniform/<f> and
        /output/crosssection/nonuniform/<f>.  The tolerance parameter
        had been missing from the old version, so nonuniform point
        spacing didn't work.

	Changed the way in which unequally spaced cross section plots
        work. Now points are plotted at element boundaries, even if
        the function value doesn't change at that boundary.

1.0.8 beta 28

5/18/00 Added output for Shear Stress 2D at Mark's request.

1.0.8 beta 29
5/25/00 Added thermal forces into node::force output.
5/26/00 Added pause command.

1.0.8 beta 30
6/15/00 Added "rename" command for element and node groups.
6/21/00 Added "output/individual/group"

1.0.8 beta 31
7/6/00  Made "output/stress/statistics" use the original element areas for
        weights, rather than the disorted areas.

        Check for and warn about nonpositive areas after equilibrating.

        Added "output/strain/statistics" menu, which does for total strain
        what the "output/stress/statistics" menu does for stress.

7/20/00 Fixed the arclength button on the cross section dashboard.
7/28/00 Added "output/current_area/<group>" and "output/original_area/<group>"
        to get the total area of a group of elements.
        Added "output/stress/all" and "output/strain/all" to get average
        stress and strain on the whole system.
7/21/00 After equilibration, move disconnected nodes to the average position
        of their neighbors, so that they don't screw up the warning about 
        nonpositive element areas.
7/27/00 Avoid nan's in statistics output when roundoff error makes the variance
        slightly negative.

1.0.8 beta 32

8/30/00 Give a warning when one or both ends of the line are off the
        edge of the grid in the cross section dashboard, and try to do
        something sensible.

1.0.8 beta 33

9/28/00 Don't close the whole program when closing the color browser with
        a window-manager close button.

1.1T    Added thermal fields.

        Changed the way that enslaved degrees of freedom are
  	implemented.  Now it's possible to enslave two groups that
  	share the same degrees of freedom.

	Changed the way that boundary conditions and distortions are
	stored in the goof files.  Instead of storing the command
	list, the distortion and bc state of each nodegroup is stored
	with the group information.  Storing the command list could
	lead to errors when a group had been enslaved and then
	emancipated before writing the goof file.

1.1.1

10/13/00 Merged source for thermal and regular versions. Compiling
         with the compiler flag -DTHERMAL gives the thermal version.
	 Both versions now have the same version number.

	 Added Doug Twisselmann's Twissel Element (isotropic with a
	 T independent stress free strain).

1.1.2

10/17/00 Workaround for XForms bug which makes command line argument
         parsing fail when the program name has more than one dot in
         it (as in "oof1.1.1").

1.1.3

10/19/00 Added "gas" element to thermal version.

1.1.4

10/23/00 Added select/nodes/nearest and select/nodes/rectangle to select
         nodes in scripts.
10/27/00 Added output/position/<group>.

1.1.5

11/02/00 Added a -start command line option, ala ppm2oof.  It's really just a
         clone of the -file option.  Files and commands given with -start are
         executed before those given with -file.

         Upgraded to XForms library version 0.89.5.

11/03/00 Added the cubic_crack element.  It's like the griffith2 element, but
         has cubic symmetry before mutating.

1.1.6

11/08/00 Fixed a mistaken error message when -start was used and -file wasn't.

11/09/00 Reworked the way distortions are applied, since they weren't
         working properly for fixed enslaved groups.  There's not much
         need for fixed enslaved groups, but they could arise by
         accident.

11/10/00 Added bc/emancipate/all command.

11/15/00 Fixed problems with distortions on enslaved groups.

1.1.7

11/18/00 Fixed a bug recently (I hope) introduced into lookfor().
11/20/00 Added "output/comment" to allow annotation in output/tee files.

11/20/00 Postpone the actual application of enslave and emancipate
         commands until they're absolutely necessary. This means that
         enslaving n nodegroups doesn't require o(n^2) cpu cycles, but
         it also means that the display may not be updated all the
         time.  The nodegroups are updated before equilibrating, when
         node info is requested, or when the "bc/apply" command is
         explicitly executed.

1.1.8

11/24/00 Restored the preconditioner variable in the non-thermal main
         menu.  It had somehow been lost when the thermal code was added.

1.1.9

02/06/01 Added "output/traction" to print stress*normal integrated
         over an edge (ie nodegroup).  It should be the same as
         "output/force", but treats corners better in some cases.  It
         will get confused if the nodegroup does not represent a real
         edge.

02/17/01 PostScript output from the Save button on the graphics window.

1.1.10

06/04/01 Reordered operations on exit to avoid a rare seg fault.  Grid
         is now deleted from within quitcmd() instead of from exitfn().

1.1.11

06/27/01 Added "effective_property" menu to compute effective macroscopic
         properties.

1.1.12

11/08/01 Added "gradyation" element for Grady White.
11/26/01 Fixed "effective_property" commands in thermal version.

1.1.13

11/30/01 Fixed bug in effective_property commands.  They had been neglecting
         to divide by the edge length.

1.1.14

11/30/01 Fixed bug in emptyelements causing crashes during thermal
         equilibration.


1.1.15

12/11/01 Corrected the TrigonalA and TrigonalCrack modului.  C46 and C56
         had extra factors of 1/2.

1.1.16

02/05/02 Added some code to make gcc 2.96 happy, possibly.

1.1.17

10/15/02 Added fdstream and conditional compilation to make gcc 3.1 happy
         on OS X 10.2.  No new features.
1.1.18

08/19/03 Added valeria element contributed by Valeria Cannilo.

1.1.19

10/30/03 Minor changes to make gcc 3.3 happy in OS X 10.3.

1.1.20

10/20/04 Added oof2 output.

1.1.21 

8/1/05  Minor changes to make gcc 3.4 happy.

1.1.22

1/12/06 Minor changes to make gcc 4.0.2 happy.

  */

