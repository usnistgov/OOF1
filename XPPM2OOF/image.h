// -*- C++ -*-
// $RCSfile: image.h,v $
// $Revision: 1.10 $
// $Author: langer $
// $Date: 2001-02-16 14:53:54 $

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */

#ifndef IMAGE_H
#define IMAGE_H

class Image;

#include "array.h"
#include "color.h"
#include "colortree.h"
#include <iostream.h>
#include <stdio.h>

class CharString;
class Rectangle;
class Stencil;

class Image : public Array<Color> 
{
private:
  ColorTree<Cell_coordinate> colortree;
  int read_ppmraw(istream &file);
  int read_ppm(istream &file);
  int read_pgmraw(istream &file);
  int read_pgm(istream &file);
  int read_pbmraw(istream &file);
  int read_pbm(istream &file);
  bool grayscale;		// is it all gray?
  double totalGray(const ImageRect&, double) const;
  double totalBright(const ImageRect&, double) const;
public:
  Image(bool gray=0);
  Image(int w, int h, bool gray=0);
  virtual ~Image();

  int read_image(istream &file);
  void copy_image(const Image &oldimage);

  void save(const CharString &filename) const; // write to a file
  void save(ostream&) const;
  void draw_ps(FILE*, const Rectangle&) const;

  void getpixels(int nreserved, Colormap);
  int changed; // set this to one if any pixels have changed!

  void makegray();
  void no_longer_gray() { grayscale = 0; } // call this when adding color!
  bool isgray() const { return grayscale; }

  void maxcontrast();
  void smooth(double dt, int nsteps);
  void nonlinear_smooth(double alpha, double dt, int nsteps);
  void stencil(const Stencil&, int nsteps);

  void imageStats();
  void averageSmooth(int, int);

  friend ostream &operator<<(ostream &file, const Image &image) {
    image.save(file);
    return file;
  }
  friend istream &operator>>(istream &file, Image &image) {
    image.read_image(file);
    return file;
  }

  // these can be done with Color::operator=
  //   void change_cell(const Cell_coordinate, const Color newcolor);
  //   void change_cell(const long x, const long y, const Color newcolor);
  //   void change_cell(const int index , const Color newcolor);

  // these shouldn't be needed
  //  unsigned long operator() (const Cell_coordinate the_coordinate) const;
  //  unsigned long operator() (const long x, const long y) const;
  //  unsigned long & operator() (const Cell_coordinate the_coordinate);
  //  unsigned long & operator() (const long x, const long y);
  //  unsigned long & operator() (const int index);
  // these are now done by the Array operators
  //  Color query_color(const Cell_coordinate the_coordinate) const;
  //  Color query_color(const long x , const long y) const;
  //  Color query_color(const int index) const;

};

#endif






