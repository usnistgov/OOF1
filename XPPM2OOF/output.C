// -*- C++ -*-
// $RCSfile: output.C,v $
// $Revision: 1.5 $
// $Author: langer $
// $Date: 2000-10-30 19:23:28 $

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */

#include "goof.h"
#include "pixelgroups.h"
#include "material.h"
#include "menuDef.h"
#include "output.h"
#include "ppm2oof.h"
#include "readbinary.h"

void endlist(FILE *f) { fprintf(f, "%d\n", -1); }

const int OutputFlags::end_marker = -1;
const int OutputFlags::xy_node = 0;
const char OutputFlags::no_displacement = 0;

static void do_output(const ImageRect&, const CharString&);
static void printgoofheader(const Goof *goof, FILE *file);

static int nodeno(const int x, const int y, const ImageRect &rect) {
    return (rect.width()+1) * (y-rect.ymin) + x - rect.xmin;
}

// ------------------------------------------------ //

class goof_element {
private:
  int id;
  int s1;
  int s2;
  int s3;
  const Material *material;
public:
  goof_element(const Material *mat, int ii, int side1, int side2, int  side3)
    : id(ii), s1(side1), s2(side2), s3(side3), material(mat)
  {
  }
  int compare(const Material *mat,
	       int side1, int side2, int side3) const
  {
    return (*material == *mat && s1 == side1 && s2 == side2 && s3 == side3);
  }
  int which() const {
    return id;
  }
};

// ------------------------------------------------ //

SubRegionOutputCmd::SubRegionOutputCmd()
  : CommandM("subregion", "write an oof file"),
    filename("out.goof")
{
  AddArgument(this, "filename", filename);
  AddArgument(this, "xmin", c1.x);
  AddArgument(this, "xmax", c2.x);
  AddArgument(this, "ymin", c1.y);
  AddArgument(this, "ymax", c2.y);
  c1.x = c1.y = 0;
  if(current_goof) {
    c2.x = current_goof->query_width();
    c2.y = current_goof->query_height();
  }
  else
    c2.x = c2.y = 0;
}

CommandFn SubRegionOutputCmd::func() {
    if(!goofinit()) return;
    do_output(ImageRect(c1, c2), filename);
}

CommandFn OutputCmd::func() {
    if(!goofinit()) return;
    do_output(current_goof->entirety(), filename);
}

static void do_output(const ImageRect &rect, const CharString &filename) {
  FILE *fip;
  int i, j;
  
  if(rect.xmin < 0 || rect.ymin < 0 || rect.xmax > current_goof->query_width()
     || rect.ymax > current_goof->query_height()) {
    garcon()->msout << ms_error << "rectangle is too big!" << endl << ms_normal;
    return;
  }


  Cell_coordinate pixel;
  
  // check to see if default material is set, if necessary.
  if(!defaultmaterial) {
    ArrayIterator iter(rect, ARI_EXCLUSIVE);
    while(iter(pixel)) 
      if(!current_goof->material[pixel]) {
	garcon()->msout << ms_error << "No default material type!"
			<< endl << ms_normal;
	return;
      }
  }

  if(use_stdout) 
    fip = stdout;
  else
    {
      if ( !(fip = fopen(filename,"w")))
	{
	  garcon()->msout << ms_error << "Could not open file for writing: "
			  << filename << endl << ms_normal;
	  return;
	}
    }
  printgoofheader(current_goof, fip);

  // nodes
  float dx = physical_width/current_goof->query_width();
  ArrayIterator iter2(rect, ARI_INCLUSIVE);
  while(iter2(pixel)) {
    writebinary(fip,OutputFlags::xy_node);
    writebinary(fip,OutputFlags::no_displacement);
    writebinary(fip,nodeno(pixel.x, pixel.y, rect));
    writebinary(fip, dx*pixel.x);
    writebinary(fip, dx*pixel.y);
  }
  writebinary(fip,OutputFlags::end_marker);
  
  // elements
  Vec<goof_element*> unique_elements;
  ArrayIterator iter3(rect, ARI_EXCLUSIVE);
  int recycled = 0;
  int count = 0;
  while(iter3(pixel)) {
    int ix = pixel.x;
    int iy = pixel.y;
    Material *material = current_goof->material[pixel];
    if(!material) material = defaultmaterial;
 
    int ilook;
    for (ilook = 0; bluebox && ilook < unique_elements.capacity() &&
	   !unique_elements[ilook]->compare(material,2,1,1) ;
	 ilook++);
    
    unsigned char flag = 0;
    if(bluebox && ilook < unique_elements.capacity()) {
      material->output(fip, flag,
		       nodeno(ix, iy, rect),
		       nodeno(ix+1, iy+1, rect),
		       nodeno(ix, iy+1, rect),
		       unique_elements[ilook]->which());
      recycled++;
/*
      cerr << "Recycling  " << ix << "," << iy << "   "
	   << ix+1 << "," << iy+1 << "   "
	   << ix   << "," << iy+1 << " same as "
	   << unique_elements[ilook]->which() <<endl;
*/
	      
    }
    else {
      unique_elements.grow(1, new goof_element(material,count,2,1,1));
      material->output(fip, flag,
		       nodeno(ix, iy, rect),
		       nodeno(ix+1, iy+1, rect),
		       nodeno(ix, iy+1, rect));
      /*
      cerr << endl << "Initiating " << ix << "," << iy << "   "
	   << ix+1 << "," << iy+1 << "   "
	   << ix   << "," << iy+1 << " as "
	   << unique_elements[ilook]->which() << endl;
      */
    }
    count++;
  
  
    for (ilook = 0; bluebox && ilook < unique_elements.capacity() && 
	   !unique_elements[ilook]->compare(material,1,1,2) ;
	 ilook++);

    flag = 0;
  
    if(bluebox && ilook < unique_elements.capacity() ) {
      material->output(fip, flag,
		       nodeno(ix, iy, rect),
		       nodeno(ix+1, iy, rect),
		       nodeno(ix+1, iy+1, rect),
		       unique_elements[ilook]->which());
/*
      cerr << "Recycling  " << ix << "," << iy << "   "
		<< ix+1 << "," << iy << "   "
		<< ix+1   << "," << iy+1 << " same as "
		<< unique_elements[ilook]->which() <<endl;
*/
	   recycled++;
    }
    else {
      unique_elements.grow(1, new goof_element(material,count,1,1,2));
      material->output(fip, flag,
		       nodeno(ix, iy, rect),
		       nodeno(ix+1, iy, rect),
		       nodeno(ix+1, iy+1, rect));
/*
      cerr << endl << "Initiating " << ix << "," << iy << "   "
	   << ix+1 << "," << iy << "   "
	   << ix+1   << "," << iy+1 << " as "
	   << unique_elements[ilook]->which() << endl;
*/
    }
    count++;
  }

  garcon()->msout << ms_info << "Recycled " << recycled << " elements."
		  << endl << ms_normal;

  writebinary(fip,OutputFlags::end_marker);  
  for(i=0; i<unique_elements.capacity(); i++)
    delete unique_elements[i];
  
  // node groups
  // right
  for(i = rect.ymin ; i <= rect.ymax ; i++)
    writebinary(fip, nodeno(rect.xmax, i, rect));
  writebinary(fip,OutputFlags::end_marker);

  // left
  for(i = rect.ymin ; i <= rect.ymax   ; i++)
    writebinary(fip,nodeno(rect.xmin, i, rect));
  writebinary(fip,OutputFlags::end_marker);


  for(i = rect.xmin ; i <= rect.xmax ; i++)
    writebinary(fip,nodeno(i, rect.ymax, rect));
  writebinary(fip,OutputFlags::end_marker);
 
  for(i = rect.xmin ; i <= rect.xmax ; i++)
    writebinary(fip,nodeno(i, rect.ymin, rect));
  writebinary(fip,OutputFlags::end_marker);

  // upperleft
  writebinary(fip, nodeno(rect.xmin, rect.ymax, rect));
  writebinary(fip,OutputFlags::end_marker);

  // lowerleft
  writebinary(fip, nodeno(rect.xmin, rect.ymin, rect));
  writebinary(fip,OutputFlags::end_marker);

  // upperright
  writebinary(fip, nodeno(rect.xmax, rect.ymax, rect));
  writebinary(fip,OutputFlags::end_marker);

  // lowerright
  writebinary(fip, nodeno(rect.xmax, rect.ymin, rect));
  writebinary(fip,OutputFlags::end_marker);

  // element groups
  for (i = 0; i < current_goof->grouplist.capacity(); i++) {
    PixelGroup *cg = current_goof->grouplist[i];
    cg->weed();
    count = 0;
    for(j = 0; j < cg->size(); j++) {
      Cell_coordinate cc = (*cg)[j];
      if(rect.contains(cc)) {
	int id = 2*(rect.width()*(cc.y - rect.ymin) + cc.x-rect.xmin);
	writebinary(fip,id);
	writebinary(fip,id+1);
	count++;
      }
    }
    writebinary(fip,OutputFlags::end_marker);
    garcon()->msout << ms_info << "Wrote " << 2*count << " elements in group \""
		    << cg->query_name()  << "\"" << endl << ms_normal;
  }


  if(fip != stdout)
    fclose(fip);
  return;
}

// there is a slightly different version of this in adaptmesh.C

static void printgoofheader(const Goof *goof, FILE *file) {
  int i;
#ifdef THERMAL
  fputs("program = thermal_oof\n", file);
#else  // !THERMAL
  fputs("program = oof\n", file);
#endif // THERMAL
  fprintf(file,"version number = 5\n");
  fprintf(file, "Nelements = %d\n", 2*goof->query_height()*goof->query_width());
  fprintf(file, "Nnodes = %d\n",
	  (goof->query_width()+1)*(goof->query_height()+1));
  fprintf(file,"type = b\n");
  fprintf(file,"elements\n");
  Vec<MaterialTypeRegistration*> &registry = material_registry();
  for(i=0; i<registry.capacity(); i++) {
    fprintf(file,"%s\n",registry[i]->name().charstar());
  }
  endlist(file);
  fprintf(file,"nodes\n");
  fprintf(file,"xy\n");
  endlist(file);
  fprintf(file,"nodegroups\n");
  fprintf(file,"right\n");
  fprintf(file,"left\n"); 
  fprintf(file,"top\n");
  fprintf(file,"bottom\n");
  fprintf(file,"upperleft\n");
  fprintf(file,"lowerleft\n");
  fprintf(file,"upperright\n");
  fprintf(file,"lowerright\n");
  endlist(file);
  if(goof->grouplist.capacity() > 0) {
    fprintf(file, "elementgroups\n");
    for(i = 0; i < goof->grouplist.capacity();i++)
      fprintf(file,"%s\n",goof->grouplist[i]->query_name().charstar());
    endlist(file);
  }
  
  endlist(file);
}
