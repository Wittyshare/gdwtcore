/*
 * Copyright (C) 2008-Today Guy Deleeuw
 *
 * See the LICENSE file for terms of use.
*/

/*! \file gdWFileDialog.h */

#ifndef __gdWFileDialog__
#define __gdWFileDialog__

#include <Wt/WLineEdit>

#include "gdWDialog.h"
#include "gdWFileView.h"

/*! \class gdWFileDialogData
*/
class gdWFileDialogData : public gdWDialogData
 {
  public :
               gdWFileDialogData();
   std::string dirDepth();

  public :
   bool              fileSelected;
   std::string       file;
   std::string       directory;
   int               type;
   std::string       rootPath;
   bool              bCheckOnly; // extra parameter for reporting tool
 };

/*! \class gdWFileDialog
 *  \brief Objet permettant la selection d'un fichier pour Open/Save/Save As/Link
*/
 
class gdWFileDialog : public gdWDialog
 {
  public :
   gdWFileDialog(gdwtcore::viewType dlgType, const std::string& rootPath, gdWFileDialogData* refData = 0,
                 gdWFileView::tbFlags vFlags = gdWFileView::tbDefault);
   ~gdWFileDialog();
   void setCurrentPath(const std::string& curPath);

  public slots :
   void onAccepted();
   void onRejected();
   void onFinished(DialogCode dlgRes);
   void onFileSelected(gdWFileView::signalType theSignal, std::string file);
   void onNameChanged();

  private :
   gdwtcore::viewType                  m_dlgType;
   std::string                         m_strRootPath;
   std::string                         m_strCurPath;
   gdWFileDialogData*                  m_refData;
   gdWFileView::tbFlags                m_tbFlags;
   gdWFileView*                        m_pFileView;
   Wt::WLineEdit*                      m_pLEFileName;
   Wt::WLineEdit*                      m_pLEError;
 };

#endif // ifdef __gdWFileDialog__

