/*
 * Copyright (C) 2006-Today Guy Deleeuw
 *
 * See the LICENSE file for terms of use.
*/

#include <Wt/WApplication>
#include <Wt/WTable>
#include <Wt/WCssDecorationStyle>
#include <Wt/WTheme>

#include "gdWLogon.h"

using namespace Wt;

gdWLogon::gdWLogon(std::string logonText, std::string passwdordText, WContainerWidget* parent)
 : WContainerWidget(parent), m_leLogon(0), m_lePassword(0), m_sLogonText(logonText), m_sPasswordText(passwdordText)
 {
  if ( WString::tr("byObjectStyleSheet").narrow() == "true" )
     wApp->useStyleSheet(wApp->theme()->resourcesUrl() + "gdwtcore/Css/gdWLogon.css");
  addStyleClass("gdWLogon");
 }

gdWLogon::~gdWLogon()
 { }

void gdWLogon::setTextLogon(std::string& logonText)
 { m_sLogonText = logonText; }

void gdWLogon::setTextPassword(std::string& passwordText)
 { m_sPasswordText = passwordText; }

void gdWLogon::load()
 {
  m_leLogon = new WLineEdit();
  m_leLogon->setText(m_sLogonText);
  m_leLogon->setFocus(true);
  m_leLogon->setTextSize(50);
  m_leLogon->resize(150, WLength::Auto);
  m_leLogon->enterPressed().connect(SLOT(this, gdWLogon::doLogon));

  m_lePassword = new WLineEdit();
  m_lePassword->setEchoMode(Wt::WLineEdit::Password);
  m_lePassword->setTextSize(50);
  m_lePassword->resize(150, WLength::Auto);
  m_lePassword->enterPressed().connect(SLOT(this, gdWLogon::doLogon));

  WTable* pTable = new WTable();
  pTable->elementAt(0, 0)->addWidget(new WText("User : "));
  pTable->elementAt(0, 1)->addWidget(m_leLogon);
  pTable->elementAt(1, 0)->addWidget(new WText("Password : "));
  pTable->elementAt(1, 1)->addWidget(m_lePassword);

  addWidget(pTable);

  m_wtError = new WText();
  m_wtError->decorationStyle().setForegroundColor(red);
  m_wtError->decorationStyle().setTextDecoration(WCssDecorationStyle::Blink);
  addWidget(m_wtError);

  WContainerWidget:: load();
}

void gdWLogon::setError(std::string& sError)
 { m_wtError->setText(sError); }

void gdWLogon::doLogon()
 {
  logonValidated_.emit(m_leLogon->text().toUTF8(), m_lePassword->text().toUTF8());
 }

Signal<std::string, std::string>& gdWLogon::logonValidated()
 { return logonValidated_; }

