/*
 * Copyright (C) 2008-Today Guy Deleeuw
 *
 * See the LICENSE file for terms of use.
*/

#include <stdio.h>

#include <boost/algorithm/string/replace.hpp>

#include <Wt/WApplication>
#include <Wt/WPushButton>
#include <Wt/WText>
#include <Wt/WTable>
#include <Wt/WCssDecorationStyle>
#include <Wt/WTheme>

#include "gdWFileDialog.h"

using namespace Wt;
using namespace gdwtcore;

gdWFileDialogData::gdWFileDialogData() : gdWDialogData(), fileSelected(false), bCheckOnly(false)
 { }

std::string gdWFileDialogData::dirDepth()
 {
  if ( directory.empty() || directory.size() <= 0 ) return "";
  // Si on ne trouve pas rootPath dans directory on retourne 1 chaine vide;
  std::string::size_type loc = directory.find(rootPath, 0);
  if ( loc == std::string::npos )
    return "";
  // Sinon on enleve rootPath de directory
  std::string str = directory;
  boost::algorithm::replace_all(str, rootPath, "");
  return str;
 }



gdWFileDialog::gdWFileDialog(viewType dlgType, const std::string& rootPath, gdWFileDialogData* refData, gdWFileView::tbFlags vFlags)
 : gdWDialog("", refData, gdWDialog::buttonNone), m_dlgType(dlgType), m_strRootPath(rootPath), m_strCurPath(rootPath), m_refData(refData), m_tbFlags(vFlags)
 {
  if ( WString::tr("byObjectStyleSheet").narrow() == "true" )
     wApp->useStyleSheet(wApp->theme()->resourcesUrl() + "gdwtcore/Css/gdWFileDialog.css");
  wApp->messageResourceBundle().use(wApp->docRoot() + wApp->resourcesUrl() + "/gdwtcore/Transl/gdWFileDialog");
  addStyleClass("gdWFileDialog");
  
  if ( m_refData ) m_refData->type = dlgType;
  if ( m_refData ) m_refData->rootPath = m_strRootPath;

  switch(m_dlgType) {
   case typeOpen          : setWindowTitle(WString::tr("gdwfiledlg-Open"));              break;
   case typeAppend        : setWindowTitle(WString::tr("gdwfiledlg-Append"));            break;
   case typeSave          : setWindowTitle(WString::tr("gdwfiledlg-Save"));              break;
   case typeSaveAs        : setWindowTitle(WString::tr("gdwfiledlg-SaveAs"));            break;
   case typeSaveChecked   : setWindowTitle(WString::tr("gdwfiledlg-SaveChecked"));       break;
   case typeSaveAsChecked : setWindowTitle(WString::tr("gdwfiledlg-SaveAsChecked"));     break;
   case typeImport        : setWindowTitle(WString::tr("gdwfiledlg-Import"));            break;
   case typeLink          : setWindowTitle(WString::tr("gdwfiledlg-Link"));              break;
   case typeWebLink       : setWindowTitle(WString::tr("gdwfiledlg-WebLink"));           break;
   case typeWebDownload   : setWindowTitle(WString::tr("gdwfiledlg-WebDownload"));       break;
  }

  switch(m_dlgType) {
   case typeOpen              : setWindowIcon(wApp->theme()->resourcesUrl() + "gdwtcore/Icons/folder.png");        break;
   case typeAppend            : setWindowIcon(wApp->theme()->resourcesUrl() + "gdwtcore/Icons/folder.png");        break;
   case typeSave              : setWindowIcon(wApp->theme()->resourcesUrl() + "gdwtcore/Icons/folder.png");        break;
   case typeSaveAs            : setWindowIcon(wApp->theme()->resourcesUrl() + "gdwtcore/Icons/folder.png");        break;
   case typeSaveChecked       : setWindowIcon(wApp->theme()->resourcesUrl() + "gdwtcore/Icons/folder.png");        break;
   case typeSaveAsChecked     : setWindowIcon(wApp->theme()->resourcesUrl() + "gdwtcore/Icons/folder.png");        break;
   case typeImport            : setWindowIcon(wApp->theme()->resourcesUrl() + "gdwtcore/Icons/folder.png");        break;
   case typeLink              : setWindowIcon(wApp->theme()->resourcesUrl() + "gdwtcore/Icons/folder.png");        break;
   case typeWebLink           : setWindowIcon(wApp->theme()->resourcesUrl() + "gdwtcore/Icons/folder.png");        break;
   case typeWebDownload       : setWindowIcon(wApp->theme()->resourcesUrl() + "gdwtcore/Icons/folder.png");        break;
  }

  int nTmp = 0;
  if ( m_tbFlags == gdWFileView::tbDefault )
    {
     nTmp |= gdWFileView::tbUseToolbar;
     nTmp |= gdWFileView::tbBreadCrumb;
    }
   else  nTmp = m_tbFlags;
  m_pFileView = new gdWFileView((viewType) m_dlgType, m_strRootPath, (gdWFileView::tbFlags) nTmp, getContent()); 
  m_pFileView->fileSelected().connect(SLOT(this, gdWFileDialog::onFileSelected));

// ======== Ajout du filter et filename via getBottom() =============
  WTable* pTable = new WTable(getBottom());
  pTable->setStyleClass("t-dlgview " + pTable->styleClass());
  new WText(WString::tr("gdfiledlg_filename"), pTable->elementAt(0, 0));
  m_pLEFileName = new WLineEdit(pTable->elementAt(0, 1));
  m_pLEFileName->setTextSize( 48);
  m_pLEFileName->changed().connect(SLOT(this, gdWFileDialog::onNameChanged));

//  m_pLEFileName->setMinimumSize(WLength(100, WLength::Percentage), WLength());
//  m_pLEFileName->setMaximumSize(WLength(100, WLength::Percentage), WLength());
  if ( m_dlgType == typeSave ||  m_dlgType == typeSaveAs || m_dlgType == typeSaveChecked || m_dlgType == typeSaveAsChecked )
     m_pLEFileName->setReadOnly(false);
   else
     m_pLEFileName->setReadOnly(true);

  m_pLEError = new WLineEdit(pTable->elementAt(1, 1));
  m_pLEError->decorationStyle().setForegroundColor(red);
  m_pLEError->decorationStyle().setTextDecoration(WCssDecorationStyle::Blink);
  m_pLEError->decorationStyle().setBorder(WBorder(WBorder::None));
  m_pLEError->setReadOnly(true);
  m_pLEError->setTextSize( 48);

// ======== Ajout des buttons via addButton() =============
  double  buttonWidth  = 100.0; // TODO : Normalement 0.0 mais width().value retourne 0 sur un button

  WPushButton* bOk = new WPushButton(windowTitle());
  bOk->clicked().connect(SLOT(this, gdWFileDialog::onAccepted));
  addButton(bOk);
  if ( bOk->width().value() > buttonWidth )
     buttonWidth  = bOk->width().value();

  WPushButton* bCancel = new WPushButton(WString::tr("gdwdialog_btn-cancel"));
  bCancel->clicked().connect(SLOT(this, gdWFileDialog::onRejected));
  addButton(bCancel);
  if ( bCancel->width().value() > buttonWidth )
     buttonWidth  = bCancel->width().value();

  bOk    ->setWidth(buttonWidth);
  bCancel->setWidth(buttonWidth);


  finished().connect(SLOT(this, gdWFileDialog::onFinished));
 }

gdWFileDialog::~gdWFileDialog()
 { }

void gdWFileDialog::setCurrentPath(const std::string& curPath)
 {
  m_strCurPath = curPath;
  m_pFileView->setCurrentPath(curPath);
 }

void gdWFileDialog::onAccepted()
 {
  m_pLEError->setText("");
  m_refData->directory.clear();
  if ( m_refData->fileSelected )
    {
     m_refData->directory = m_pFileView->selectedDirectory();
     gdWDialog::onAccepted();
     return;
    }
  m_pLEError->setText(WString::tr("gdfiledlg_novalidsel"));
 }

void gdWFileDialog::onRejected()
 { gdWDialog::onRejected(); }

void gdWFileDialog::onFinished(DialogCode dlgRes)
 { gdWDialog::onFinished(dlgRes); }

void gdWFileDialog::onFileSelected(gdWFileView::signalType theSignal, std::string file)
 {
  m_pLEError->setText("");
  if ( m_refData )
    {
     m_refData->file         = file;
     if ( theSignal != gdWFileView::none )
        m_refData->fileSelected = true;
      else
        m_refData->fileSelected = false;
     boost::filesystem::path p(file);
     m_pLEFileName->setText(p.leaf().string());
    }
  if ( theSignal == gdWFileView::doubleClick )
     onAccepted();
 }

void gdWFileDialog::onNameChanged()
 {
  std::string fic = m_pLEFileName->text().narrow();
  std::string dir = m_pFileView->selectedDirectory();
  dir += "/" + fic;
  fprintf(stderr, "onNameChanged : %s\n", dir.c_str());
  m_refData->fileSelected = true;
  m_refData->file = dir;
 }

