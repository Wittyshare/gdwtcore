/*
 * Copyright (C) 2006-Today Guy Deleeuw
 *
 * See the LICENSE file for terms of use.
*/

#include <iostream>
#include <fstream>

#include <Wt/WApplication>
#include <Wt/WLogger>
#include <Wt/WVBoxLayout>
#include <Wt/WTheme>

#include <gdcore/gdCore.h>

#include "gdFHtmlEditor.h"

/*
Gautier Hankenne : <script type="text/javascript">
tinyMCE.init({
    // General options
    mode : "textareas",
    theme : "advanced",
    plugins : "autolink,lists,spellchecker,pagebreak,style,layer,table,save,advhr,advimage,advlink,emotions,iespell,inlinepopups,insertdatetime,preview,media,searchreplace,print,contextmenu,paste,directionality,fullscreen,noneditable,visualchars,nonbreaking,xhtmlxtras,template,imagemanager,filemanager",
    
    // Theme options
    theme_advanced_buttons1 : "save,newdocument,|,styleprops,spellchecker,|,bold,italic,underline,strikethrough,|,justifyleft,justifycenter,justifyright,justifyfull,|,cite,abbr,acronym,del,ins,attribs,|,styleselect,formatselect,|,visualchars,nonbreaking,template,blockquote,pagebreak",
    theme_advanced_buttons2 : "cut,copy,paste,pastetext,pasteword,|,search,replace,|,bullist,numlist,|,outdent,indent,blockquote,|,undo,redo,|,link,unlink,anchor,image,cleanup,help,code,|,insertdate,inserttime,preview",
    theme_advanced_buttons3 : "tablecontrols,|,hr,removeformat,visualaid,|,sub,sup,|,charmap,emotions,iespell,media,advhr,|,print,|,fullscreen",
    theme_advanced_toolbar_location : "top",
    theme_advanced_toolbar_align : "left",
    theme_advanced_statusbar_location : "bottom",
    theme_advanced_resizing : true,
    width: "100%",
    height: "400"
});
</script>
*/
using namespace Wt;

gdFHtmlEditor::gdFHtmlEditor(const WString& rText, bool bUseToolBar, WContainerWidget* parent)
 : WContainerWidget(parent), m_wte(0), m_bUseToolbar(bUseToolBar)
 {
  addStyleClass("gdFHtmlEditor");

  wApp->messageResourceBundle().use(wApp->docRoot() + wApp->resourcesUrl() + "/gdwtcore/Transl/gdFHtmlEditor");

//  resize(WLength(100, WLength::Percentage), WLength(100, WLength::Percentage));
//  setOverflow(WContainerWidget::OverflowHidden);

  m_wte = new WTextEdit(rText);
//  m_wte->setConfigurationSetting("document_base_url", std::string("/web03/"));
//  m_wte->setConfigurationSetting("relative_urls", false);

//  m_wte->setExtraPlugins("advlink,style,advimage,paste,table,preview,fullscreen");
  m_wte->setExtraPlugins("autolink,lists,spellchecker,pagebreak,style,layer,table,save,advhr,advimage,advlink,emotions,iespell,inlinepopups,insertdatetime,preview,media,searchreplace,print,contextmenu,paste,directionality,fullscreen,noneditable,visualchars,nonbreaking,xhtmlxtras,template");

  std::string strCfg;// = m_wte->toolBar(0) + ",";
//  strCfg += "styleselect,|,tablecontrols,|,image,|,preview,|,fullscreen,|,code,|,pastetext,pasteword,selectall,|,link,unlink";
  strCfg += "styleprops,spellchecker,|,bold,italic,underline,strikethrough,|,justifyleft,justifycenter,justifyright,justifyfull,|,cite,abbr,acronym,del,ins,attribs,|,styleselect,formatselect,|,visualchars,nonbreaking,template,blockquote,pagebreak";
  m_wte->setToolBar(0, strCfg);

  strCfg = "cut,copy,paste,pastetext,pasteword,|,search,replace,|,bullist,numlist,|,outdent,indent,blockquote,|,undo,redo,|,link,unlink,anchor,image,cleanup,help,code,|,insertdate,inserttime,preview";
  m_wte->setToolBar(1, strCfg);

  strCfg = "tablecontrols,|,hr,removeformat,visualaid,|,sub,sup,|,charmap,emotions,iespell,media,advhr,|,print,|,fullscreen";
  m_wte->setToolBar(2, strCfg);

  WVBoxLayout* vBox = new WVBoxLayout();

  if ( m_bUseToolbar ) {
     vBox->addWidget(buildToolbar(), 0);
    }

  vBox->addWidget(m_wte, 1);

  setLayout(vBox);
 }

gdFHtmlEditor::~gdFHtmlEditor()
 { }

const WString& gdFHtmlEditor::text() const
 { return m_wte->text(); }

void gdFHtmlEditor::setText(const WString& rText)
 {
//  wApp->log("notice") << "gdFHtmlEditor::setText = " << rText;
  m_wte->setText(rText);
//  wApp->log("notice") << "gdFHtmlEditor::setText retrieve the text = " << text();
 }
const std::string& gdFHtmlEditor::rootPath() const
 { return m_sRootPath; }

void gdFHtmlEditor::setRootPath(const std::string& newPath)
 { m_sRootPath = newPath; }

const std::string& gdFHtmlEditor::currentPath() const
 { return m_sPath; }

void gdFHtmlEditor::setCurrentPath(const std::string& newPath)
 { m_sPath = newPath; }

void gdFHtmlEditor::setUseToolbar(bool bUseToolbar)
 { m_bUseToolbar = bUseToolbar; }

bool gdFHtmlEditor::useToolbar()
 { return m_bUseToolbar; }

WWidget* gdFHtmlEditor::buildToolbar()
 {
  gdToolbar* m_pToolbar = new gdToolbar();

  m_tbiOpen           = m_pToolbar->addToolbarItem(wApp->theme()->resourcesUrl() + "gdwtcore/Icons/project-open-3.png",
                                                   WString::tr("fhtmlEditor_tbi_open").toUTF8(),
                                                   WString::tr("fhtmlEditor_tbi_open_tooltips").toUTF8());
  m_tbiOpen->clicked().connect(SLOT(this, gdFHtmlEditor::doOpen));

  m_tbiSave           = m_pToolbar->addToolbarItem(wApp->theme()->resourcesUrl() + "gdwtcore/Icons/filesave.png",
                                                   WString::tr("fhtmlEditor_tbi_save").toUTF8(),
                                                   WString::tr("fhtmlEditor_tbi_save_tooltips").toUTF8());
  m_tbiSave->clicked().connect(SLOT(this, gdFHtmlEditor::doSave));

  m_tbiSaveAs         = m_pToolbar->addToolbarItem(wApp->theme()->resourcesUrl() + "gdwtcore/Icons/filesaveas.png",
                                                   WString::tr("fhtmlEditor_tbi_saveas").toUTF8(),
                                                   WString::tr("gdFHtmlEditor_tbi_saveas_tooltips").toUTF8());
  m_tbiSaveAs->clicked().connect(SLOT(this, gdFHtmlEditor::doSaveAs));

  return m_pToolbar;
 }

bool gdFHtmlEditor::readFile()
 {
  setText("");
  if ( !gdcore_isPathFile(m_sPath) ) return false;
  std::string     fileContent;
  if ( !gdcore_file_content2string(m_sPath.c_str(), fileContent) ) return false;
  setText(fileContent);
  return true;
 }

void gdFHtmlEditor::doOpen()
 {
  startDlg(gdwtcore::typeOpen);
 }

void gdFHtmlEditor::doSave()
 {
  startDlg(gdwtcore::typeSave);
 }

void gdFHtmlEditor::doSaveAs()
 {
  startDlg(gdwtcore::typeSaveAs);
 }

void gdFHtmlEditor::startDlg(gdwtcore::viewType vt)
 {
  if ( m_sRootPath.empty() || !m_sRootPath.size() ) {
     WMessageBox::show(WString::tr("fhtmlEditor_error_title"), WString::tr("fhtmlEditor_error_norootpath"), Ok);
     return;
    }

  gdWFileDialogData* m_pData = new gdWFileDialogData();
  m_pData->callerCallback = boost::bind(&gdFHtmlEditor::endDlg, boost::ref(*this), m_pData);

  int nTmp = gdWFileView::tbDefault;
  nTmp |= gdWFileView::tbUseToolbar;
  nTmp |= gdWFileView::tbBreadCrumb;
  gdWFileDialog* pdial = new gdWFileDialog(vt, m_sRootPath, m_pData, (gdWFileView::tbFlags) nTmp);
  pdial->setCurrentPath(m_sRootPath);
  pdial->show();
 }

void gdFHtmlEditor::endDlg(gdWDialogData* pDataEmit)
 {
  if ( !pDataEmit ) return;
  gdWFileDialogData* pData = dynamic_cast<gdWFileDialogData*>(pDataEmit);
  if ( pData->result != Wt::WDialog::Accepted )
    {
     delete pData;
     return;
     //if ( pData->file.empty() )
        //m_result->setText("File selected is empty");
      //else
        //m_result->setText("File selected = " +  pData->file);
    }
  delete pData;
  return;
 }

