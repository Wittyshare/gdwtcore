/*
 * Copyright (C) 2006-Today Guy Deleeuw
 *
 * See the LICENSE file for terms of use.
*/

#ifndef __gdFHtmlEditor__
#define __gdFHtmlEditor__

#include <Wt/WContainerWidget>
#include <Wt/WTextEdit>
#include <Wt/WText>

#include "gdToolbar.h"
#include "gdWFileDialog.h"

/*! \class gdFHtmlEditor
 *  \brief Small editor based on wt + tinyMce
*/
 
class gdFHtmlEditor : public Wt::WContainerWidget
 {
  public :
                       /*! \brief Constructor.  */
                       gdFHtmlEditor(const Wt::WString& rText = Wt::WString(), bool bUseToolBar = true, Wt::WContainerWidget* parent = 0);
                       /*! \brief Destructor.  */
                       ~gdFHtmlEditor();

                       /*! \brief Get the text.  */
   const Wt::WString& text() const;
                       /*! \brief Set the text.  */
   void               setText(const Wt::WString& rText);
                       /*! \brief Get the current root path. */
   const std::string& rootPath() const;
                       /*! \brief Set the root path, for the dialog open/save/saveAs.
                                   the root path define the level in the root file system that become the root for the user.
                                   By default it is empty and in this case the dialog cannot be opened.
                       */
   void               setRootPath(const std::string& newPath);
                       /*! \brief Get the current path selected.  */
   const std::string& currentPath() const;
                       /*! \brief Set the current path.  */
   void               setCurrentPath(const std::string& newPath);
                       /*! \brief Enable/disable the toolbar, default true.  */
   void               setUseToolbar(bool bUseToolbar);
                       /*! \brief Is toolbar enabled ?  */
   bool               useToolbar();
                       /*! \brief Build the toolbar.  */
   Wt::WWidget*       buildToolbar();
                       /*! \brief Read the file and set the text.  */
   bool               readFile();
                       /*! \brief Render the file dialog.  */
   void               startDlg(gdwtcore::viewType vt);
                       /*! \brief Manage the dialog result.  */
   void               endDlg(gdWDialogData* pDataEmit);

  public slots :
   void               doOpen();
   void               doSave();
   void               doSaveAs();

  private :
   Wt::WTextEdit*          m_wte;
   bool                    m_bUseToolbar;
   gdToolbarItem*          m_tbiOpen;
   gdToolbarItem*          m_tbiSave;
   gdToolbarItem*          m_tbiSaveAs;
   std::string             m_sRootPath; // the file to edit
   std::string             m_sPath;     // the file to edit
 };

#endif // ifdef __gdFHtmlEditor__

