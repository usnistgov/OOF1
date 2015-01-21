// -*- C++ -*-
// $RCSfile: automation.C,v $
// $Revision: 1.31 $
// $Author: langer $
// $Date: 2000-09-06 20:11:24 $

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */

#include "burn.h"
#include "burner.h"
#include "goof.h"
#include "image.h"
#include <math.h>
#include "menuinterrupt.h"
#include "pixelgroups.h"
#include "random.h"
#include "stencil.h"
#include "vec.h"

void Goof::autoSelect(const Image & theImage,
		      const CharString &grainGroupName,
		      const CharString &boundaryGroupName,
		      const Color &lowerColorBound,
		      const Color &upperColorBound,
		      const int localBurn, const int globalBurn,
		      const int minSize,
		      const double minAspect, const double maxAspect,
		      const double maxPerimeter, bool verbose, bool auto_redraw)
{
  //automated selection of regions on an image as Group A or B

  garcon()->msout << ms_info << "Starting automatic selection..." << endl;

  PixelGroup *grain, *boundary; //the two materials
  
  if(!(grain=get_group(grainGroupName)))
    grain=add_group(grainGroupName);
  if(!(boundary=get_group(boundaryGroupName)))
    boundary=add_group(boundaryGroupName);

  PixelGroup toBurn;
  toBurn.setphysicalsize(height*width);  //so the group doesn't have to be
                                         //resized before every addition
                                         //(saves a lot of time)

  ArrayIterator iter(theImage);
  Cell_coordinate cell;
  while(iter(cell)){
    if(theImage[cell].query_red()>=lowerColorBound.query_red() && 
       theImage[cell].query_red()<=upperColorBound.query_red() &&
       theImage[cell].query_blue()>=lowerColorBound.query_blue() && 
       theImage[cell].query_blue()<=upperColorBound.query_blue() &&
       theImage[cell].query_green()>=lowerColorBound.query_green() && 
       theImage[cell].query_green()<=upperColorBound.query_green())
      toBurn.append(cell);
  }
  int initialSize = toBurn.size();

  if(verbose)
    garcon()->msout << "  " << initialSize << " pixels to burn" << endl;

  if(toBurn.size()!=0){

    Interrupter stop;
    
    Burn burner;
    //set burn parameters (Class Goof is a friend to Class Burn)
    burner.lcl=localBurn;
    burner.glbl=globalBurn;

    init_selection(0);


    //while there are unslected pixels still left	
    while((toBurn.size()>0) && !stop()){
      
      //pick random cell coordinate to burn and scale it to toBurn accordingly
      double random=rndm();
      int index=(int)floor((random*(toBurn.size())));
      if(index==toBurn.size())
	index--;

      burner.igniteNoUndo(theImage, toBurn[index], 0);
      (void) current_goof->doDespeckle(5);

      double aspect=selected().aspectRatio();
      double perimeter=selected().perimeter();

      if(verbose)
	garcon()->msout << "At " << toBurn[index] << " selected "
			<< selected().size() << " pixels that were " ;
    
      if(selected().size()<minSize){
	//selection is too small to use aspectRatio or perimeter

	//remove the chosen pixel from toBurn group
	toBurn.remove(toBurn[index]);

	if(verbose)
	  garcon()->msout << "not enough." << endl;
      }
      else if(((aspect>minAspect) && (perimeter>maxPerimeter))
	      || (aspect>maxAspect)){
	//selection is probably part of Group B (boundary)
	toBurn.remove(toBurn[index]);
	if(verbose)
	  garcon()->msout << "probably part of Group B." << endl;
	if(auto_redraw)
	  redraw();
    
      }
      else{
	//selection is part of Group A (grain)
	toBurn.remove(selected());
	grain->append(selected());
	if(verbose)
	  garcon()->msout << "added to Group A." << endl;
	if(auto_redraw)
	  redraw();
      }
    
      if(verbose)
	garcon()->msout << "Pixels left to burn: " << toBurn.size()
			<< " (" << toBurn.size()*100./initialSize << "%)"
			<< endl;
    }


    grain->weed();
    if(verbose)
      garcon()->msout << "Total Group A pixels selected before expand: "
		      << grain->size() << endl;
    expandgroup(*grain, 1);
    if(verbose)
      garcon()->msout << "Total Group A pixels selected after expand: "
		      << grain->size() << endl;

    //invert Group A to get Group B
    select(*grain);
    select_complement();
    boundary->append(selected());
    unselect_all();

    garcon()->msout << "Automated Selection Complete." << endl;
    
  }
  else
    garcon()->msout << ms_error << "No pixels in color range!" << endl
		    << ms_normal;
}
