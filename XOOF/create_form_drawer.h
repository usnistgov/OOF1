// -*- C++ -*-
// $RCSfile: create_form_drawer.h,v $
// $Revision: 1.3 $
// $Author: langer $
// $Date: 2000-09-06 20:10:50 $


/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


#ifndef CREATE_FORM_DRAWER_H
#define CREATE_FORM_DRAWER_H

class ColorDashboard;
class CoordDashboard;
class ElementInfo;
class FormDrawer;
class NodeInfo;
class SelectAttributes;
class SimpleAttributes;
class QuitForm;
class PlotForm;

void create_form_form_drawer(FormDrawer *fdui);
void create_form_simple_attributes(SimpleAttributes *fdui);
void create_form_select_attributes(SelectAttributes *fdui);
void create_form_coordinates(CoordDashboard *fdui);
void create_form_color_control(ColorDashboard *fdui);
void create_form_elementinfo(ElementInfo *fdui);
void create_form_nodeinfo(NodeInfo *fdui);
void create_form_quitform(QuitForm *fdui);
void create_form_plot(PlotForm *fdui);
#endif
