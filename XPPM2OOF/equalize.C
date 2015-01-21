// -*- C++ -*-
// $RCSfile: equalize.C,v $
// $Revision: 1.2 $
// $Author: langer $
// $Date: 2000-09-06 20:11:30 $

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */

#include "menuDef.h"
#include "image.h"
#include "ishape.h"
#include "goof.h"
#include <math.h>

//**************************************************************************

double Image::totalGray(const ImageRect &window, double averageValue) const {
  double sum = 0;
  Cell_coordinate cell;
  ArrayIterator iter(window);
  while(iter(cell)) {
    if(contains(cell))
      //get the total of the cell colors in the window
      sum+=(*this)[cell].query_red();
    else
      sum+=averageValue;
  }
  return sum;
}

//**************************************************************************

double Image::totalBright(const ImageRect &window, double averageValue) const {
  double sum = 0;
  Cell_coordinate cell;
  ArrayIterator iter(window);
  while(iter(cell)) {
    if(contains(cell))
      //get the total of the cell colors in the window
      sum+=(*this)[cell].brightness();
    else
      sum+=averageValue;
  }
  return sum;
}

//**************************************************************************
	
void Image::averageSmooth(int sizeX, int sizeY)
{
  //equalizes the image by replacing each pixel with the average of a window 
  //(specified by the function parameters) of its neighbors

  const Cell_coordinate yhat(0, 1);  //unit vector j, <0, 1>
  const Cell_coordinate xhat(1, 0);  //unit vector i, <1, 0>

  sizeX=abs(sizeX);
  sizeY=abs(sizeY);
  int windowArea=(2*sizeX+1)*(2*sizeY+1);
  Cell_coordinate diagonal(sizeX, sizeY);
  Cell_coordinate diagonal2(-sizeX, sizeY);
  double averageValue;

  //create a copy of the calling object so calculations aren't based
  //on things that have already been equalized
  Image newImage(query_width(), query_height(), isgray());
 
  if(isgray()){

    //compute average gray of entire image
    double grayTotal=0;
    ArrayIterator it(*this);
    Cell_coordinate cc;
    while(it(cc)){
      grayTotal+=(*this)[cc].query_red();
    }
    averageValue=grayTotal/(query_height()*query_width());

    //pixels that don't have enough surrounding pixels to make up the window
    //have the image's average gray substituted in for these "missing" pixels 

    grayTotal=0;
    bool first=true;  //true for the first iteration of the loop
    ArrayIterator iter(*this);
    Cell_coordinate cell, lastPixel;

    while(iter(cell))
      if(current_goof->active(cell)){

	ImageRect window(cell+diagonal, cell-diagonal);  //averaging region
	
	if(first){
	  grayTotal = totalGray(window, averageValue);
	  first=false;
	}
	else{  //now beyond the very first pixel
	  if(cell-lastPixel==xhat){
	    //window has moved one pixel in the x direction
	    
	    Cell_coordinate mover1=cell+diagonal2;  //bottom left of window
	    Cell_coordinate mover2=cell+diagonal;   //bottom right of window
	    
	    for(int i=0; i<window.height(); i++){
	      
	      if(contains(mover1)){
		grayTotal-=(*this)[mover1].query_red();
	      } 
	      else
		grayTotal-=averageValue;
	      
	      if(contains(mover2)){
		grayTotal+=(*this)[mover2].query_red();	      
	      }
	      else
		grayTotal+=averageValue;

	      mover1=mover1-yhat;
	      mover2=mover2-yhat;
	    }
	  }
	  else{
	    //window has moved one pixel in the y direction OR
	    //window has moved somewhere else randomly
	    //so recalculate the entire window sum
	    grayTotal = totalGray(window, averageValue);
	  }

	}
	
	if(grayTotal != 0) {
	  //get the average gray of the window
	  double regionGrayAvg=grayTotal/windowArea;
	  //replace the current cell on the new image with the average
	  //that was just obtained
	  newImage[cell]=Color(((*this)[cell].query_red()*averageValue)/regionGrayAvg);
	}
	else
	  newImage[cell] = (*this)[cell];
	lastPixel=cell;  //the last central pixel that was visited
      } // loop over active cells
    
  }
  else{				
    // image is not grayscale
    //compute average brightness of entire image
    double brightTotal=0;
    ArrayIterator it(*this);
    Cell_coordinate cc;
    while(it(cc)){
      brightTotal+=(*this)[cc].brightness();
    }
    averageValue=brightTotal/(query_height()*query_width());

    //pixels that don't have enough surrounding pixels to make up the
    //window have the image's average brightness substituted in for
    //these "missing" pixels

    brightTotal=0;
    bool first=true;  //true for the first iteration of the loop
    ArrayIterator iter(*this);
    Cell_coordinate cell, lastPixel;

    while(iter(cell))
      if(current_goof->active(cell)){

	ImageRect window(cell+diagonal, cell-diagonal);  //averaging region
	
	if(first) {
	  brightTotal = totalBright(window, averageValue);
	  first=false;
	}
	else{  //now beyond the very first pixel
	  if(cell-lastPixel==xhat){
	    //window has moved one pixel in the x direction
	    
	    Cell_coordinate mover1=cell+diagonal2;  //bottom left of window
	    Cell_coordinate mover2=cell+diagonal;   //bottom right of window
	    
	    for(int i=0; i<window.height(); i++){
	      
	      if(contains(mover1))
		brightTotal-=(*this)[mover1].brightness();
	      else
		brightTotal-=averageValue;
	      
	      if(contains(mover2))
		brightTotal+=(*this)[mover2].brightness();	      
	      else
		brightTotal+=averageValue;

	      mover1=mover1-yhat;
	      mover2=mover2-yhat;
	    }
	  }
	  else{
	    //window has moved one pixel in the y direction OR
	    //window has moved somewhere else randomly
	    //so recalculate the entire window sum
	    brightTotal= totalBright(window, averageValue);
	  }

	}
	
	if(brightTotal != 0) {
	  //get the average brightness of the window
	  double regionBrightAvg=brightTotal/windowArea;
	  // scale the brightness of the pixel in the new image to
	  // make the window as bright as the whole image
	  double scale = averageValue/regionBrightAvg;
	  newImage[cell] = (*this)[cell].dim(scale);
	}
	else 
	  newImage[cell] = (*this)[cell];

	lastPixel=cell;  //the last central pixel that was visited
      } // loop over active cells

  }
 
  (*this).copy_image(newImage);  //replace old image with newImage
  garcon()->msout << ms_info << "Equalization complete: average value = "
		  << averageValue << endl << ms_normal;
}
