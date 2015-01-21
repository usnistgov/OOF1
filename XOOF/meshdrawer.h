// -*- C++ -*-
// $RCSfile: meshdrawer.h,v $
// $Revision: 1.4 $
// $Author: langer $
// $Date: 2001-02-18 02:34:49 $


/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


#ifndef MESHDRAWER_H
#define MESHDRAWER_H

class MeshDrawer;

#include "drawer.h"
#include "truefalse.h"
#include "colormap.h"
#include "coorddashboard.h"
#include "infodashboard.h"

class SelectDrawer;
class AttributesDashboard;

class MeshDrawer: public Drawer {
private:
  Color black_;
  Color white_;
  Color wallpaper_fg_;
  Color wallpaper_bg_;
  Color edgecolor;
  Color rubberbandcolor;
  Color plotlinecolor;
protected:
  static DrawerRegistration registration;

  virtual int showingempties() const;

  GrayMap graymap;
  static const int Ngrays;

  virtual void draw();
  virtual void drawelements();
  virtual void drawelement(const Element*);
  virtual void drawedges();
  virtual void drawedges(const Element*);
  virtual void drawnodes();
  virtual void drawnode(const Node*);
  virtual void drawextras() {}

  virtual void draw_ps(FILE*, const Rectangle&);
  virtual void drawelements_ps(FILE*, const Rectangle&);
  virtual void drawedges_ps(FILE*, const Rectangle&) const;
  virtual void drawextras_ps(FILE*, const Rectangle&) const {}
  virtual void drawnodes_ps(FILE*, const Rectangle&) const {}

//   virtual void drawelement_ps(const Element*, FILE*, const Rectangle&) const;
//   virtual void drawedges_ps(const Element*, FILE*, const Rectangle&) const {}
//   virtual void drawnode_ps(const Node*, FILE*, const Rectangle&) const {}


  // colors used by all drawers, but pixel value may differ from
  // colormap to colormap
  virtual unsigned long wallpaper_fg() const { return wallpaper_fg_.pixel; }
  virtual unsigned long wallpaper_bg() const { return wallpaper_bg_.pixel; }
  virtual unsigned long black() const { return black_.pixel; }
  virtual unsigned long white() const { return white_.pixel; }
  // pixel values to be used in RW colormaps
  static const unsigned long BLACKPIXEL;
  static const unsigned long WHITEPIXEL;
  static const unsigned long WALLPAPER_FG;
  static const unsigned long WALLPAPER_BG;
  static const unsigned long EDGECOLOR;
  static const unsigned long RUBBERBANDCOLOR;
  static const unsigned long PLOTLINECOLOR;
  static const unsigned long FREECOL;

  CoordDashboard coorddashboard;
  AttributesDashboard *attributes;
  ElementInfo elementinfo;
  NodeInfo nodeinfo;

  // called whenever the coordinates of the visible region change
  virtual void scrollcallback(const MeshCoord&, const MeshCoord&);

public:
  MeshDrawer(Grid*, FormDrawer*);
  virtual ~MeshDrawer();
  virtual unsigned long rubberbandpixel() const {
    return rubberbandcolor.pixel;
  }
  void writeppm(const CharString&) const;

  friend class PlotDashboard;
  friend class InfoDashboard;
  friend class ElementInfo;
  friend class NodeInfo;
};

#endif






