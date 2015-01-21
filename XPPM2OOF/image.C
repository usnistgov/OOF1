// -*- C++ -*-
// $RCSfile: image.C,v $
// $Revision: 1.9 $
// $Author: langer $
// $Date: 2002-10-15 18:31:31 $


/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


#include "stdlib.h"
#include <stdio.h>
#include <string.h>
#include "ctype.h"

#include "array.h"
#include "charstring.h"
#include "colorutils.h"
#include "image.h"
#include "ppm2oof.h"
#include "readbinary.h"

Image::Image(bool gray)
  : grayscale(gray)
{
}

Image::Image(int w, int h, bool gray)
  : Array<Color>(h, w),
    changed(1),
    grayscale(gray)
{
}

Image::~Image() {}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

// stuff for reading images from ppm files

// Return next white-space delimited word from file, ignoring
// everything between '#' and end of line. Return "" at end of file.

static char *nextword(istream &file) {
  // have to read character by character so that binary data at the
  // end of the header isn't accidentally read too soon.
  char c;
  static char buf[100];
  char *ptr = buf;

  while(file.get(c) && isspace(c));  // skip initial whitespace
  if(!file) {
    buf[0] = '\0';
    return buf;
  }
  if(c == '#')  {		// skip comment
    while(file.get(c) && c != '\n');
    return nextword(file);
  }
  *ptr++ = c;
  while(file.get(c) && !isspace(c) && c != '#') {
    *ptr++ = c;
  }
  if(c == '#')  // skip comment
    while(file.get(c) && c != '\n');
  *ptr = '\0'; // terminate the string
  return buf;
}

int Image::read_image(istream &file) {
  char filetype = '\0';
  // find what kind of PPM/PBM/PGM file this is:
  char *word = nextword(file);
  if(word[0] == '\0') {
    garcon()->msout << ms_error << "Can't read file!" << endl << ms_normal;
    return 0;
  }
  if(word[0] != 'P') {
    garcon()->msout << ms_error << "File is not a PPM/PGM/PBM file!"
		    << endl << ms_normal;
    return 0;
  }
  filetype = word[1];

  // find size
  int w = atoi(nextword(file));
  int h = atoi(nextword(file));
  if(h <= 0 || w <= 0) {
    garcon()->msout << ms_error << "Invalid image size!" << endl
		    << ms_normal;
    return 0;
  }
  changed = 1;
  resize(Cell_coordinate(w, h));

  switch(filetype) {
  case '1':
    return read_pbm(file);
  case '2':
    return read_pgm(file);
  case '3':
    return read_ppm(file);
  case '4':
    return read_pbmraw(file);
  case '5':
    return read_pgmraw(file);
  case '6':
    return read_ppmraw(file);
  default:
    garcon()->msout << ms_error << "Unknown file type!" << endl
		    << ms_normal;
    return 0;
  }
  // not reached
}

int Image::read_ppmraw(istream &file) {
  int maxpixval = atoi(nextword(file));
  if(maxpixval == 0) {
    garcon()->msout << ms_error << "Invalid maxpixval in raw ppm image file!"
		    << endl << ms_normal;
    return 0;
  }
  double scale = 1./maxpixval;
  for(int i=0; i<height; i++)
    for(int j=0; j<width; j++) {
      unsigned char rgb[3];
      file.read(reinterpret_cast<char*>(rgb), 3);
      if(file.gcount() != 3) {
	garcon()->msout << ms_error << "Incomplete raw ppm image file! "
			<< endl << ms_normal;
	return 0;
      }
      (*this)[Cell_coordinate(j, height-i-1)] =
	Color(scale*rgb[0], scale*rgb[1], scale*rgb[2]);
    }
  return 1;
}

int Image::read_ppm(istream &file) {
  int maxpixval = atoi(nextword(file));
  if(maxpixval == 0) {
    garcon()->msout << ms_error << "Invalid maxpixval in ppm image file!"
		    << endl << ms_normal;
    return 0;
  }
  double scale = 1./maxpixval;
  for(int i=0; i<height; i++)
    for(int j=0; j<width; j++) {
      int r = atoi(nextword(file));
      int g = atoi(nextword(file));
      int b = atoi(nextword(file));
      if(!file) {
	garcon()->msout << ms_error << "Incomplete ppm image file!"
			<< endl << ms_normal;
	return 0;
      }
      (*this)[Cell_coordinate(j, height-i-1)] =
	Color(scale*r, scale*g, scale*b);
    }
  return 1;
}

int Image::read_pgmraw(istream &file) {
  int maxpixval = atoi(nextword(file));
  if(maxpixval == 0) {
    garcon()->msout << ms_error << "Invalid maxpixval in raw pgm image file!"
		    << endl << ms_normal;
    return 0;
  }
  double scale = 1./maxpixval;
  for(int i=0; i<height; i++)
    for(int j=0; j<width; j++) {
      unsigned char gray;
      file.read((char*)&gray, 1);
      if(file.gcount() != 1) {
	garcon()->msout << ms_error << "Incomplete raw pgm image file!"
			<< endl << ms_normal;
	return 0;
      }
      double g = scale*gray;
      (*this)[Cell_coordinate(j, height-i-1)] = Color(g, g, g);
    }
  grayscale = 1;
  return 1;
}

int Image::read_pgm(istream &file) {
  int maxpixval = atoi(nextword(file));
  if(maxpixval == 0) {
    garcon()->msout << ms_error << "Invalid maxpixval in pgm image file!"
		    << endl << ms_normal;
    return 0;
  }
  double scale = 1./maxpixval;
  for(int i=0; i<height; i++)
    for(int j=0; j<width; j++) {
      int g = atoi(nextword(file));
      if(!file) {
	garcon()->msout << ms_error << "Incomplete pgm image file!"
			<< endl << ms_normal;
	return 0;
      }
      double gg = scale*g;
      (*this)[Cell_coordinate(j, height-i-1)] = Color(gg, gg, gg);
    }
  grayscale = 1;
  return 1;
}

int Image::read_pbm(istream &file) {
  for(int i=0; i<height; i++)
    for(int j=0; j<width; j++) {
      char c;
      while(file.get(c) && isspace(c));
      if(!file) {
	garcon()->msout << ms_error << "Incomplete pbm image file!"
			<< endl << ms_normal;
	return 0;
      }
      if(c == '1')
	(*this)[Cell_coordinate(j, height-i-1)] = Color(0.0, 0.0, 0.0);
      else if(c == '0')
	(*this)[Cell_coordinate(j, height-i-1)] = Color(1.0, 1.0, 1.0);
      else {
	garcon()->msout << ms_error << "Incomprehensible pbm file!"
			<< endl << ms_normal;
	return 0;
      }
    }
  grayscale = 1;
  return 1;
}

int Image::read_pbmraw(istream &file) {
  Color black(0.0, 0.0, 0.0);
  Color white(1.0, 1.0, 1.0);

#ifdef WRONG
  /* This section, I think, does what the pbm manual says it should,
   * but doesn't read files written by xv. It assumes that the images
   * bits are stored sequentially, with no padding, even if the width
   * isn't a multiple of 8.
   */

  int nbytes = height*width/8;
  if(height*width > 8*nbytes) nbytes++;
  unsigned char *raster = new unsigned char[nbytes];

  file.read(raster, nbytes);
  if(file.gcount() != nbytes) {
    garcon()->msout << ms_error << "Incomplete raw pbm file!"
		    << endl << ms_normal;
    delete [] raster;
    return 0;
  }

  int i = 0;
  int j = 0;
  for(int k=0; k<nbytes; k++) {                               // loop over bytes
    for(unsigned char mask = 1 << 7; mask != 0; mask >>= 1) { // loop over bits
      (*this)[Cell_coordinate(j, height-i-1)] =
	(mask & raster[k]) ? black : white;
      j++;
      if(j == width) {  // go to next line of image
	j = 0;
	i++;
	if(i == height) {  // done!
	  delete [] raster;
	  return 1;
	}
      }
    }
  }

  garcon()->msout << ms_error << "This shouldn't happen!" << endl << ms_normal;
  delete [] raster;
  grayscale = 1;
  return 0;
#endif

  /* This section assumes that each horizontal line is padded to
   * contain an integer number of bytes. The pbm manual page doesn't
   * say that lines should be padded, but that appears to be what xv does.
   */
  int nbytes = width/8;
  if(width > 8*nbytes) nbytes++;
  unsigned char *raster = new unsigned char[nbytes];
  for(int i=0; i<height; i++) {
    // read a line
    file.read((char*)raster, nbytes);
    if(file.gcount() != nbytes) {
      garcon()->msout << ms_error << "Incomplete raw pbm file!"
		      << endl << ms_normal;
      delete [] raster;
      return 0;
    }
    int j = 0; // bit counter
    for(int k=0; k<nbytes; k++)   // loop over bytes
      for(unsigned char mask = 1<<7; mask!=0 && j<width; mask >>= 1, j++)
	(*this)[Cell_coordinate(j, height-i-1)] =
	  (mask & raster[k]) ? black : white;
  }
  delete [] raster;
  grayscale = 1;
  return 1;
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

void Image::save(const CharString &filename) const {
  ofstream file(filename);
  if(!file) {
    garcon()->msout << ms_error << "Can't save image to \""
		    << filename << "\"!" << endl << ms_normal;
    return;
  }
  save(file);
}

void Image::save(ostream &file) const {
  if(!grayscale) {
    // write raw PPM format
    file << "P6" << endl;
    file << "# CREATOR: ppm2oof" << endl;
    file << width << " " << height << endl;
    file << "255" << endl;
    for(int i=0; i<height; i++) {
      for(int j=0; j<width; j++) {
	file.put((*this)[Cell_coordinate(j, height-i-1)].query_red());
	file.put((*this)[Cell_coordinate(j, height-i-1)].query_green());
	file.put((*this)[Cell_coordinate(j, height-i-1)].query_blue());
      }
    }
  } 
  else {			// gray scale image
    // write raw PGM format
    file << "P5" << endl;
    file << "# CREATOR: ppm2oof" << endl;
    file << width << " " << height << endl;
    file << "255" << endl;
    for(int i=0; i<height; i++)
      for(int j=0; j<width; j++)
	file.put((*this)[Cell_coordinate(j, height-i-1)].query_red());
  }
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//


void Image::copy_image(const Image &initimage) {
  changed = 1;
//  resize(Cell_coordinate(initimage.query_width(), initimage.query_height()));
//    ArrayIterator iter(*this);
//    Cell_coordinate cell;
//    while(iter(cell))
//      (*this)[cell] = initimage[cell];
  this->Array<Color>::operator=(initimage);
  colortree = initimage.colortree;
  grayscale = initimage.grayscale;
}  

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//


//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

void Image::getpixels(int nreserved, Colormap colormap) {
  if(!changed) return;
  changed = 0;

  ArrayIterator iter(*this);
  Cell_coordinate cell;
  colortree.clear();
  while(iter(cell))
    colortree.add(cell, (*this)[cell]);

  if(gfxinfo.c_class() == PseudoColor) {
    // reduce number of colors so that it will fit in colormap
    if(restricted_colors != 0)
      colortree.reduce(restricted_colors);
    else
      colortree.reduce(gfxinfo.colormap_size() - nreserved);
    // Make a list of XColors
    XColor *xcolor = new XColor[colortree.Nreducedcolors()];
    int ncolors = 0;  // number of colors being set
    for(int i=0; i<colortree.Nreducedcolors(); i++) {
      ColorTreeNode<Cell_coordinate> &node = colortree[i];
      if(node.elected) {
	xcolor[ncolors] = node.color.xcolor();
	xcolor[ncolors].pixel = nreserved + ncolors;
	xcolor[ncolors].flags = DoRed | DoGreen | DoBlue;
	node.color.pixel = xcolor[ncolors].pixel;
	ncolors++;
      }
    }
    XStoreColors(gfxinfo.display(), colormap, xcolor, ncolors);
    delete [] xcolor;
  }

  else if(gfxinfo.c_class() == TrueColor) {
    if(restricted_colors != 0)
      colortree.reduce(restricted_colors);
    else 
      colortree.electall();
    for(int i=0; i<colortree.Nreducedcolors(); i++) {
      if(colortree[i].elected)
	colortree[i].color.RO_pixel();
    }
  }

  else {
    cerr << "Image::getpixels --- bad X visual!\n" << endl;
    exit(1);
  }

  // copy pixel values from colortree to image
  for(int i=0; i<colortree.Ncolors(); i++) {
    ColorTreeNode<Cell_coordinate> &node = colortree[i];
    unsigned long pixel;
    if(node.elected)
      pixel = node.color.pixel;
    else
      pixel = colortree[node.nearest].color.pixel;
    for(LinkListIterator<Cell_coordinate> j=node.list.begin();!j.end();++j)
      (*this)[node.list[j]].pixel = pixel;
  }
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

// ostream &operator<<(ostream &os, const Image &image) {
//   ArrayIterator iter(image);
//   Cell_coordinate cell;
//   while(iter(cell))
//     os << cell << ": " << image[cell] << endl;
//   return os;
// }
