// -*- C++ -*-
// $RCSfile: imagemandash.h,v $
// $Revision: 1.4 $
// $Author: langer $
// $Date: 2000-09-06 20:11:35 $


/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */

#ifndef IMAGEMANDASH_H
#define IMAGEMANDASH_H

#include "dashboard.h"
#include "menuDef.h"

class NamedImage;

class ImageManDashboard : public Dashboard {
private:
  ImageManDashboard(ImageForm*, FL_OBJECT*);
  ~ImageManDashboard();

  void buildmenu(Menu*, MenuAttribute);

  void delete_image(int which);
  static void CB_button_delete(FL_OBJECT*, long);
  void delete_gui();
  class DeleteCmd : public CommandM {
  private:
    ImageManDashboard *imd;
    int which;
    DeleteCmd(ImageManDashboard*);
    virtual CommandFn func();
    virtual CommandM *clone() const;
    friend class ImageManDashboard;
  };
  DeleteCmd delete_cmd;
  friend class DeleteCmd;

  void store(const CharString&);
  static void CB_button_store(FL_OBJECT*, long);
  void store_gui();
  class StoreCmd : public CommandM {
    // This command is only executed by the textwaiter, not via the gui.
  private:
    CharString imagename;
    ImageManDashboard *imd;
    StoreCmd(ImageManDashboard*);
    virtual CommandFn func();
    virtual CommandM *clone() const;
    friend class ImageManDashboard;
  };
  StoreCmd store_cmd;
  friend class StoreCmd;
  CharString dflt_name() const;

  void retrieve_gui();
  void retrieve(int);
  static void CB_browser_imagelist(FL_OBJECT*, long); // single click
  static void CB_button_retrieve(FL_OBJECT*, long); // dbl click, or button
  class RetrieveCmd : public CommandM {
  private:
    ImageManDashboard *imd;
    int which;			// which image to retrieve
    RetrieveCmd(ImageManDashboard*);
    virtual CommandFn func();
    virtual CommandM *clone() const;
    friend class ImageManDashboard;
  };
  RetrieveCmd retrieve_cmd;
  friend class RetrieveCmd;

  static void CB_button_save_selected(FL_OBJECT*, long);
  void save_selected_gui();
  class SaveSelectedCmd : public CommandM {
  private:
    ImageManDashboard *imd;
    FileName filename;
    int which;
    SaveSelectedCmd(ImageManDashboard*);
    virtual CommandFn func();
    virtual CommandM *clone() const;
    friend class ImageManDashboard;
  };
  SaveSelectedCmd save_selected_cmd;
  friend class SaveSelectedCmd;

  static void CB_button_save_current(FL_OBJECT*, long);
  void save_current_gui();
  class SaveCurrentCmd : public CommandM {
  private:
    ImageManDashboard *imd;
    FileName filename;
    SaveCurrentCmd(ImageManDashboard*);
    virtual CommandFn func();
    virtual CommandM *clone() const;
    friend class ImageManDashboard;
  };
  SaveCurrentCmd save_current_cmd;
  friend class SaveCurrentCmd;

  FileName get_filename() const;

  void update_gallery_listing(const Vec<NamedImage*> &);
  friend class ImageForm;
  friend class ImageFormGfx;
  friend class ImageFormTxt;
  friend FD_image_drawer *create_form_image_drawer();
};

#endif
