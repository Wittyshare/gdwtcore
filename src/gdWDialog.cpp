/*
 * Copyright (C) 2008-Today Guy Deleeuw
 *
 * See the LICENSE file for terms of use.
*/

#include <stdio.h>

#include <Wt/WPushButton>

#include <Wt/WApplication>
#include <Wt/WEnvironment>
#include <Wt/WTheme>

#include "gdWDialog.h"

using namespace Wt;

gdWDialogData::gdWDialogData() : result(Wt::WDialog::Accepted), callerCallback(0), saveDataCallback(0)
 { }


gdWDialog::gdWDialog(const WString &title, gdWDialogData* refData, int nUseButtons, Wt::Orientation orientation)
 : WDialog(title), m_refData(refData), m_nUseButtons(nUseButtons), m_wcContent(0), m_wcBottom(0), m_imgIcon(0), m_blButtons(0)
 {
  if ( WString::tr("byObjectStyleSheet").narrow() == "true" )
     wApp->useStyleSheet(wApp->theme()->resourcesUrl() + "gdwtcore/Css/gdWDialog.css");
  wApp->messageResourceBundle().use(wApp->docRoot() + wApp->resourcesUrl() + "/gdwtcore/Transl/gdWDialog");
  addStyleClass("gdWDialog");

  bool useOwnTB = true;
  if ( useOwnTB )
    {
     // Title bar management
     WHBoxLayout* hboxTitle = new WHBoxLayout();
     m_imgIcon = new WImage();
     hboxTitle->addWidget(m_imgIcon, 0);
   
     m_wtTitle = new WText(title);
     hboxTitle->addWidget(m_wtTitle, 1);
   
     WImage* image = new WImage(wApp->theme()->resourcesUrl() + "gdwtcore/Icons/cancel.png");
     image->clicked().connect(SLOT(this, gdWDialog::onRejected));
     hboxTitle->addWidget(image, 0);
   
     WContainerWidget* pTitleBar = titleBar();
     pTitleBar->setLayout(hboxTitle);
    }
   else
    {
     setTitleBarEnabled(true);
     setWindowTitle(title);
    }

  double  buttonWidth  = 100.00; // TODO : Normalement 0.0 mais width().value retourne 0 sur un button et transferer ca dans 1 fonction computeButtonWidth
  if ( orientation == Wt::Vertical ) m_blButtons = new WVBoxLayout();
   else                              m_blButtons = new WHBoxLayout();
  if ( m_nUseButtons == buttonOkCancel || m_nUseButtons == buttonCancel || m_nUseButtons == buttonAll )
    {
     WPushButton* bCancel = new WPushButton(WString::tr("gdwdialog_btn-cancel"));
     bCancel->clicked().connect(SLOT(this, gdWDialog::onRejected));
     m_blButtons->addWidget(bCancel, 0);
     if ( bCancel->width().value() > buttonWidth )
        buttonWidth  = bCancel->width().value();
     bCancel->setWidth(buttonWidth);
    }
  if ( m_nUseButtons == buttonOkCancel || m_nUseButtons == buttonOk || m_nUseButtons == buttonAll )
    {
     WPushButton* bOk = new WPushButton(WString::tr("gdwdialog_btn-ok"));
     bOk->clicked().connect(SLOT(this, gdWDialog::onAccepted));
     m_blButtons->addWidget(bOk, 0);
     if ( bOk->width().value() > buttonWidth )
        buttonWidth  = bOk->width().value();
     bOk->setWidth(buttonWidth);
    }

  m_wcBottom  = new WContainerWidget();

  WHBoxLayout* hbox = new WHBoxLayout();
  hbox->addWidget(m_wcBottom, 1);
  hbox->addLayout(m_blButtons, 0);

  WContainerWidget* contBottom = new WContainerWidget();
  contBottom->setLayout(hbox);

  m_wcContent             = new WContainerWidget();
//  m_wcContent->resize(WLength(100, WLength::Percentage), WLength(100, WLength::Percentage));
  m_wcContent->setOverflow(WContainerWidget::OverflowAuto);

  WVBoxLayout*      vbox = new WVBoxLayout();
  vbox->addWidget(m_wcContent, 1);
  vbox->addWidget(contBottom,  0);

  vbox->setContentsMargins(1, 0, 1, 0);
  contents()->setLayout(vbox);

  finished().connect(SLOT(this, gdWDialog::onFinished));

  setResizable(true);

  resize("75%", "95%");
 }

gdWDialog::~gdWDialog()
 { }

void gdWDialog::addButton(WPushButton* pButton)
 {
  if ( m_blButtons )
     m_blButtons->addWidget(pButton, 0);
 }

void gdWDialog::onAccepted()
 {
  if ( m_refData )
    {
     m_refData->result = result();
     if ( m_refData->saveDataCallback )
       m_refData->saveDataCallback(m_refData);
    }
  WDialog::accept();
 }

void gdWDialog::onRejected()
 {
  if ( m_refData )
     m_refData->result = result();
  WDialog::reject();
 }

void gdWDialog::onFinished(DialogCode dlgRes)
 {
  if ( m_refData )
    {
     m_refData->result = result();
     if ( m_refData->callerCallback )
        m_refData->callerCallback(m_refData);
    }
  delete this;
 }

void gdWDialog::setWindowTitle(const WString& title)
 {
  if ( m_imgIcon )
     m_wtTitle->setText(title);
   else
    WDialog::setWindowTitle(title);
 }

const WString gdWDialog::windowTitle() const
 {
  if ( m_imgIcon )
     return m_wtTitle->text();
   else
     return WDialog::windowTitle();
 }

void gdWDialog::setWindowIcon(const std::string& urlIcon)
 {
  if ( m_imgIcon )
     m_imgIcon->setImageRef(urlIcon);
 }

