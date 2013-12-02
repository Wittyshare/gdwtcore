/*
 * Copyright (C) 2006-Today Guy Deleeuw
 *
 * See the LICENSE file for terms of use.
*/

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/replace.hpp>

//#include <vmime/vmime.hpp>
//#include <vmime/platforms/posix/posixHandler.hpp>

#include <Wt/WApplication>
#include <Wt/WLogger>
#include <Wt/WLabel>
#include <Wt/WBreak>
#include <Wt/WDate>
#include <Wt/WMessageBox>
#include <Wt/WTimer>
#include <Wt/WTheme>
#include <Wt/Mail/Client>
#include <Wt/Mail/Message>

#include <gdcore/gdCore.h>
#include <gdcore/gdLdap.h>
#include <gdcore/gdXml.h>
#include <gdcore/gdImage.h>

#include "gdWForm.h"

using namespace Wt;

gdWFormItem::gdWFormItem(std::string& outputAttribute, WObject* pObject, bool bRequired)
 : m_outputAttribute(outputAttribute), m_pObject(pObject), m_bRequired(bRequired)
 { }






gdWForm::gdWForm(const std::string& rXmlFile, WContainerWidget* parent)
 : WContainerWidget(parent), m_sXmlFile(rXmlFile), m_bRedirectTextAddEmailBody(false), m_bCheckLdap(false), m_bDebugLdap(false), m_bCsvEmailForm(false)
 {
  if ( WString::tr("byObjectStyleSheet").narrow() == "true" )
     wApp->useStyleSheet(wApp->theme()->resourcesUrl() + "gdwtcore/Css/gdWForm.css");
  addStyleClass("gdWForm");
 }

gdWForm::~gdWForm()
 { }

void gdWForm::load()
 {
  m_vObjects.clear();
  loadXmlFile(m_sXmlFile);
  WContainerWidget::load();
  resize(WLength(100, WLength::Percentage), WLength(100, WLength::Percentage));
  setOverflow(WContainerWidget::OverflowAuto);
 }

bool gdWForm::loadXmlFile(const std::string& rXmlFile)
 {
  // Prepare the XML document
  xmlDoc  *doc_xml      = NULL;
  xmlNode *root_element = NULL;

  LIBXML_TEST_VERSION

  /* parse the file and get the DOM */
  doc_xml = xmlReadFile(rXmlFile.c_str(), NULL, 0);
  if ( doc_xml == NULL )
    {
     wApp->log("notice") << "gdWForm::loadXmlFile doc_xml = NULL " << rXmlFile; 
     return false;
    }

  /* Get the root element node */
  root_element = xmlDocGetRootElement(doc_xml);
  if ( !root_element )
    {
     wApp->log("notice") << "gdWForm::loadXmlFile no root element " << rXmlFile; 
     xmlFreeDoc(doc_xml);
     return false;
    }

  // If the name = WtForm, start building the form.
  if ( !strcmp((char*) root_element->name, "WForm") )
     loadWForm(root_element, this);

  xmlFreeDoc(doc_xml);
  doc_xml = NULL;

  xmlCleanupParser();

  return true;
 }

bool gdWForm::loadWForm(xmlNode* xmlParent, WContainerWidget* parent)
 {
  for(xmlNode* cur_node = xmlParent->children; cur_node; cur_node = cur_node->next)
   {
    if ( !strcmp((char*) cur_node->name, "WObjects") )
       loadWObjects(cur_node, parent);
   }
  return true;
 }

bool gdWForm::loadWObjects(xmlNode* xmlParent, WContainerWidget* parent)
 {
  for(xmlNode* cur_node = xmlParent->children; cur_node; cur_node = cur_node->next)
   {
    // Static objects
    if ( !strcmp((char*) cur_node->name, "WUseStyleSheet") ) {
       wApp->useStyleSheet(getXmlStr(cur_node, ""));
       continue;
    }
    if ( !strcmp((char*) cur_node->name, "WAnchor") ) {
       loadWAnchor(cur_node, parent);
       continue;
    }
    if ( !strcmp((char*) cur_node->name, "WGroupBox") ) {
       loadWGroupBox(cur_node, parent);
       continue;
    }
    if ( !strcmp((char*) cur_node->name, "WBreak") ) {
       parent->addWidget(new WBreak());
       continue;
    }
    if ( !strcmp((char*) cur_node->name, "WText") ) {
       loadWText(cur_node, parent);
       continue;
    }
    if ( !strcmp((char*) cur_node->name, "WImage") ) {
       loadWImage(cur_node, parent);
       continue;
    }

    // Objects that can be modified
    if ( !strcmp((char*) cur_node->name, "WComboBox") ) {
       loadWComboBox(cur_node, parent);
       continue;
    }
    if ( !strcmp((char*) cur_node->name, "WCheckBox") ) {
       loadWCheckBox(cur_node, parent);
       continue;
    }
    if ( !strcmp((char*) cur_node->name, "WButtonGroup") ) {
       loadWButtonGroup(cur_node, parent);
       continue;
    }
    if ( !strcmp((char*) cur_node->name, "WSelectionBox") ) {
       loadWSelectionBox(cur_node, parent);
       continue;
    }
    if ( !strcmp((char*) cur_node->name, "WLineEdit") ) {
       loadWLineEdit(cur_node, parent);
       continue;
    }
    if ( !strcmp((char*) cur_node->name, "WTextArea") ) {
       loadWTextArea(cur_node, parent);
       continue;
    }
    if ( !strcmp((char*) cur_node->name, "WCalendar") ) {
       loadWCalendar(cur_node, parent);
       continue;
    }
/*
 * Un button radio doit etre dans un WButtonGroup
    if ( !strcmp((char*) cur_node->name, "WRadioButton") ) {
       loadWRadioButton(cur_node, parent);
       continue;
    }
*/
    if ( !strcmp((char*) cur_node->name, "WPushButton") ) {
       loadWPushButton(cur_node, parent);
       continue;
    }
   }
  return true;
 }

bool gdWForm::loadWClass(xmlNode* xmlParent, WWidget* widget)
 {
  std::string sClass       = getXmlStringProp(xmlParent, "class", "");
  if ( sClass.size() ) 
     widget->addStyleClass(sClass);
 }

WAnchor* gdWForm::loadWAnchor(xmlNode* xmlParent, WContainerWidget* parent)
 {
  std::string link         = getXmlStringProp(xmlParent, "link", "");
  std::string imgPath      = getXmlStringProp(xmlParent, "url", "");
  std::string target       = getXmlStringProp(xmlParent, "target", "");
  std::string text         = getXmlStr(xmlParent, "");

  WAnchor* pA = new WAnchor();
  loadWClass(xmlParent, pA);
  pA->setLink(link);
//  pA->setImage(imgPath);
  if ( target == "TargetSelf" )
     pA->setTarget(Wt::TargetSelf);
  if ( target == "TargetThisWindow" )
     pA->setTarget(Wt::TargetThisWindow);
  if ( target == "TargetNewWindow" )
     pA->setTarget(Wt::TargetNewWindow);
  pA->setText(WString::fromUTF8(text));

  for(xmlNode* cur_node = xmlParent->children; cur_node; cur_node = cur_node->next)
   {
    if ( !strcmp((char*) cur_node->name, "WImage") ) {
       pA->setImage(loadWImage(cur_node));
       continue;
      }
   }

  if ( parent )
     parent->addWidget(pA);
  return pA;
 }

WGroupBox* gdWForm::loadWGroupBox(xmlNode* xmlParent, WContainerWidget* parent)
 {
  std::string title        = getXmlStringProp(xmlParent, "title", "");

  WGroupBox* gb = new WGroupBox();
  loadWClass(xmlParent, gb);
  gb->setTitle(WString::fromUTF8(title));

  for(xmlNode* cur_node = xmlParent->children; cur_node; cur_node = cur_node->next)
   {
    if ( !strcmp((char*) cur_node->name, "WObjects") )
       loadWObjects(cur_node, gb);
   }
  if ( parent )
     parent->addWidget(gb);
  return gb;
 }

WText* gdWForm::loadWText(xmlNode* xmlParent, WContainerWidget* parent)
 {
  // Le <![CDATA[MonWtext]] est enlevé par libxml2 il ne reste que MonWtext
  // exemple : <WText><![CDATA[<h1> Hello world !</h1>]]></WText>
  WText*      pText        = new WText();
  bool        bWordWrap    = getXmlBoolProp  (xmlParent, "wordWrap", pText->wordWrap());
  std::string text         = getXmlStr(xmlParent, "");

  loadWClass(xmlParent, pText);
  pText->setText(WString::fromUTF8(text));
  pText->setWordWrap(bWordWrap);

  if ( parent )
     parent->addWidget(pText);
  return pText;
 }

WImage* gdWForm::loadWImage(xmlNode* xmlParent, WContainerWidget* parent)
 {
  std::string url          = getXmlStringProp(xmlParent, "url", "");
  std::string altText      = getXmlStringProp(xmlParent, "alt", "");
  std::string size         = getXmlStringProp(xmlParent, "resize", "");
  std::string urlSized     = url;

  if ( size.size() > 0 ) {
     boost::algorithm::replace_all(urlSized, ".png", "_resize.png");
     urlSized = "/users/guest/tmp/img.png";
     int status = gdImage_resizeImage(wApp->docRoot() + url, wApp->docRoot() + urlSized, size);
     wApp->log("notice") << "gdWForm::loadWImage : resize url = " << url << " urlSized = " << urlSized;
     if ( status ) {
        wApp->log("notice") << "gdWForm::loadWImage : cannot resize to urlSized = " << urlSized << " status = " << status;
        urlSized = url;
       }
    }
  WImage*     pImg = new WImage();
  loadWClass(xmlParent, pImg);
  pImg->setImageRef(urlSized);
  pImg->setAlternateText(WString::fromUTF8(altText));

  if ( parent )
     parent->addWidget(pImg);
  return pImg;
 }




WComboBox* gdWForm::loadWComboBox(xmlNode* xmlParent, WContainerWidget* parent)
 {
  int         defaultIndex = getXmlIntProp   (xmlParent, "defaultIndex", -1);
  bool        labelBuddy   = getXmlBoolProp  (xmlParent, "labelBuddy", false);
  std::string label        = getXmlStringProp(xmlParent, "label", "");
  std::string outputAtt    = getXmlStringProp(xmlParent, "outputAttribute", label.c_str());

  WComboBox* pCbb = new WComboBox();
  loadWClass(xmlParent, pCbb);

  WLabel* pLabel = new WLabel(WString::fromUTF8(label), parent);
  pLabel->setWordWrap(true);
  loadWClass(xmlParent, pLabel);

  for(xmlNode* cur_node = xmlParent->children; cur_node; cur_node = cur_node->next)
   {
    if ( !strcmp((char*) cur_node->name, "item") ) {
       pCbb->addItem(WString::fromUTF8(getXmlStr(cur_node, "")));
       continue;
      }
   }
  pCbb->setCurrentIndex(defaultIndex); // -1 = no selection does not work
  if ( labelBuddy ) pLabel->setBuddy(pCbb);

  m_vObjects.push_back(new gdWFormItem(outputAtt, pCbb));
  if ( parent )
     parent->addWidget(pCbb);
  return pCbb;
 }

WCheckBox* gdWForm::loadWCheckBox(xmlNode* xmlParent, WContainerWidget* parent)
 {
  bool        bState       = getXmlBoolProp  (xmlParent, "checked", false);
  bool        labelBuddy   = getXmlBoolProp  (xmlParent, "labelBuddy", false);
  std::string label        = getXmlStringProp(xmlParent, "label", "");
  std::string outputAtt    = getXmlStringProp(xmlParent, "outputAttribute", label.c_str());
  std::string text         = getXmlStr(xmlParent, "");

  WCheckBox* pCB = new WCheckBox();
  loadWClass(xmlParent, pCB);

  WLabel* pLabel = new WLabel(WString::fromUTF8(label), parent);
  pLabel->setWordWrap(true);
  loadWClass(xmlParent, pLabel);

  pCB->setChecked(bState);
  pCB->setText(WString::fromUTF8(text));
  if ( labelBuddy ) pLabel->setBuddy(pCB);

  m_vObjects.push_back(new gdWFormItem(outputAtt, pCB));
  if ( parent )
     parent->addWidget(pCB);
//  if ( parent )
//     parent->addWidget(pLabel);
  return pCB;
 }

WButtonGroup* gdWForm::loadWButtonGroup(xmlNode* xmlParent, WContainerWidget* parent)
 {
  int nbRb                   = 0;
  int           defaultIndex = getXmlIntProp   (xmlParent, "defaultIndex", nbRb);
  std::string   outputAtt    = getXmlStringProp(xmlParent, "outputAttribute", "");
  WButtonGroup* bg           = new WButtonGroup(parent);

  for(xmlNode* cur_node = xmlParent->children; cur_node; cur_node = cur_node->next)
   {
    if ( !strcmp((char*) cur_node->name, "WBreak") ) {
       parent->addWidget(new WBreak());
       continue;
    }
    if ( !strcmp((char*) cur_node->name, "WRadioButton") ) {
       WRadioButton* pRB = loadWRadioButton(cur_node, parent);
       bg->addButton(pRB, nbRb++);
       continue;
    }
   }
  bg->setCheckedButton(bg->button(defaultIndex));
  m_vObjects.push_back(new gdWFormItem(outputAtt, bg));
  return bg;
 }

WRadioButton* gdWForm::loadWRadioButton(xmlNode* xmlParent, WContainerWidget* parent)
 {
// C'est WButtonGroup que l'on ajoute dans le vecteur d'objets
  std::string text         = getXmlStr(xmlParent, "");

  WRadioButton* pRB = new WRadioButton(parent);
  loadWClass(xmlParent, pRB);

  pRB->setText(text);

  return pRB;
 }

WSelectionBox* gdWForm::loadWSelectionBox(xmlNode* xmlParent, WContainerWidget* parent)
 {
  int         vertSize     = getXmlIntProp   (xmlParent, "verticalSize", 3);
  int         defaultIndex = getXmlIntProp   (xmlParent, "defaultIndex", -1);
  bool        labelBuddy   = getXmlBoolProp  (xmlParent, "labelBuddy", false);
  std::string selMode      = getXmlStringProp(xmlParent, "selectionMode", "SingleSelection");
  std::string label        = getXmlStringProp(xmlParent, "label", "");
  std::string outputAtt    = getXmlStringProp(xmlParent, "outputAttribute", label.c_str());

  WSelectionBox* pSB = new WSelectionBox();
  loadWClass(xmlParent, pSB);

  WLabel* pLabel = new WLabel(WString::fromUTF8(label), parent);
  pLabel->setWordWrap(true);
  loadWClass(xmlParent, pLabel);

  for(xmlNode* cur_node = xmlParent->children; cur_node; cur_node = cur_node->next)
   {
    if ( !strcmp((char*) cur_node->name, "item") ) {
       pSB->addItem(WString::fromUTF8(getXmlStr(cur_node, "")));
       continue;
      }
   }
  if ( selMode == "NoSelection" ) {
     pSB->setSelectionMode(Wt::NoSelection);
  }
  if ( selMode == "SingleSelection" ) {
     pSB->setSelectionMode(Wt::SingleSelection);
     pSB->setCurrentIndex(defaultIndex);
  }
  if ( selMode == "ExtendedSelection" ) {
     // TODO : add multi values in defaultIndex
     pSB->setSelectionMode(Wt::ExtendedSelection);
     std::set<int> mySet;
     mySet.insert(defaultIndex);
     pSB->setSelectedIndexes(mySet);
  }

  if ( labelBuddy ) pLabel->setBuddy(pSB);

  m_vObjects.push_back(new gdWFormItem(outputAtt, pSB));
  if ( parent )
     parent->addWidget(pSB);
  return pSB;
 }

WLineEdit* gdWForm::loadWLineEdit(xmlNode* xmlParent, WContainerWidget* parent)
 {
  int         textSize     = getXmlIntProp   (xmlParent, "textSize", 30);
  int         maxLength    = getXmlIntProp   (xmlParent, "maxLength", 30);
  bool        labelBuddy   = getXmlBoolProp  (xmlParent, "labelBuddy", false);
  bool        bRequired    = getXmlBoolProp  (xmlParent, "required", false);
  bool        bHidden      = getXmlBoolProp  (xmlParent, "hidden", false);
  std::string sEchoMode    = getXmlStringProp(xmlParent, "echoMode", "Normal");
  std::string label        = getXmlStringProp(xmlParent, "label", "");
  std::string outputAtt    = getXmlStringProp(xmlParent, "outputAttribute", label.c_str());
  std::string text         = getXmlStr(xmlParent, "");

  WLineEdit* pLE = new WLineEdit();
  loadWClass(xmlParent, pLE);
  pLE->setTextSize(textSize);
  pLE->setMaxLength(maxLength);
  if ( sEchoMode == "Password" ) pLE->setEchoMode(Wt::WLineEdit::Password);
   else                          pLE->setEchoMode(Wt::WLineEdit::Normal);
  pLE->setText(WString::fromUTF8(text));
  pLE->setHidden(bHidden);

  WLabel* pLabel = new WLabel(WString::fromUTF8(label), parent);
  pLabel->setHidden(bHidden);
  pLabel->setWordWrap(true);
  loadWClass(xmlParent, pLabel);
  if ( labelBuddy ) pLabel->setBuddy(pLE);

  m_vObjects.push_back(new gdWFormItem(outputAtt, pLE, bRequired));
  if ( parent )
     parent->addWidget(pLE);
  return pLE;
 }

WTextArea* gdWForm::loadWTextArea(xmlNode* xmlParent, WContainerWidget* parent)
 {
  int         nbCols       = getXmlIntProp   (xmlParent, "columns", 20);
  int         nbRows       = getXmlIntProp   (xmlParent, "rows", 5);
  bool        labelBuddy   = getXmlBoolProp  (xmlParent, "labelBuddy", false);
  bool        bRequired    = getXmlBoolProp  (xmlParent, "required", false);
  std::string label        = getXmlStringProp(xmlParent, "label", "");
  std::string outputAtt    = getXmlStringProp(xmlParent, "outputAttribute", label.c_str());
  std::string text         = getXmlStr(xmlParent, "");

  WTextArea* pTA = new WTextArea();
  loadWClass(xmlParent, pTA);
  pTA->setColumns(nbCols);
  pTA->setRows(nbRows);
  pTA->setText(WString::fromUTF8(text));

  WLabel* pLabel = new WLabel(WString::fromUTF8(label), parent);
  pLabel->setWordWrap(true);
  loadWClass(xmlParent, pLabel);
  if ( labelBuddy ) pLabel->setBuddy(pTA);

  m_vObjects.push_back(new gdWFormItem(outputAtt, pTA, bRequired));
  if ( parent )
     parent->addWidget(pTA);
  return pTA;
 }

WCalendar* gdWForm::loadWCalendar(xmlNode* xmlParent, WContainerWidget* parent)
 {
  bool        labelBuddy   = getXmlBoolProp  (xmlParent, "labelBuddy", false);
  std::string label        = getXmlStringProp(xmlParent, "label", "");
  std::string outputAtt    = getXmlStringProp(xmlParent, "outputAttribute", label.c_str());
  std::string text         = getXmlStr(xmlParent, "");

  WCalendar* pCal = new WCalendar();
  loadWClass(xmlParent, pCal);

  if ( text.size() )
     pCal->select(WDate::fromString(text));
   else
     pCal->select(WDate::currentServerDate());

  WLabel* pLabel = new WLabel(WString::fromUTF8(label), parent);
  pLabel->setWordWrap(true);
  loadWClass(xmlParent, pLabel);
//  if ( labelBuddy ) pLabel->setBuddy(pCal);

  m_vObjects.push_back(new gdWFormItem(outputAtt, pCal));
  if ( parent )
     parent->addWidget(pCal);
  return pCal;
 }




// TODO : store by button
WPushButton* gdWForm::loadWPushButton(xmlNode* xmlParent, WContainerWidget* parent)
 {
  std::string text              = getXmlStr(xmlParent, "");
  std::string action            = getXmlStringProp(xmlParent, "action", "");

  if ( action == "email" || action == "csv") {
     m_sRedirectPage               = getXmlStringProp(xmlParent, "redirectPage", "/");
     m_bRedirectTextAddEmailBody   = getXmlBoolProp  (xmlParent, "redirectTextAddEmailBody", false);
     m_sRedirectText               = getXmlStringProp(xmlParent, "redirectText", "");
    }

  WPushButton* pBut = new WPushButton();
  if ( parent )
     parent->addWidget(pBut);
  
  loadWClass(xmlParent, pBut);

  for(xmlNode* cur_node = xmlParent->children; cur_node; cur_node = cur_node->next)
   {
    if ( !strcmp((char*) cur_node->name, "WButtonText") ) {
       WText* pText = loadWText(cur_node, 0);
       text         = pText->text().toUTF8();
       delete pText;
       continue;
      }
    if ( !strcmp((char*) cur_node->name, "WText") ) {
       WText* pText = loadWText(cur_node, 0);
       m_sRedirectText = pText->text().toUTF8();
       delete pText;
       continue;
      }
   }

  pBut->setText(text);

  if ( action == "reset" ) {
     pBut->clicked().connect(SLOT(this, gdWForm::doReset));
     wApp->log("notice") << "gdWForm::loadWPushButton : Action is reset ";
     return pBut;
    }

  if ( action == "email" )
     wApp->log("notice") << "gdWForm::loadWPushButton : Action is email ";

  // The csv file exist and is accessible by the web server and each column = the order of each object declared.
  if ( action == "csv" ) {
     m_action                      = action;
     m_sCsvFile                    = getXmlStringProp(xmlParent, "csvFile", "");
     m_sCsvDelimiter               = getXmlStringProp(xmlParent, "csvSeparator", ";");
     m_bCsvEmailForm                = getXmlBoolProp  (xmlParent, "csvEmailForm", false);
     wApp->log("notice") << "gdWForm::loadWPushButton : Action is csv ";
    }

  // Email
  if ( (action == "csv" && m_bCsvEmailForm) || action == "email" ) {
     m_sSmtpServer                 = getXmlStringProp(xmlParent, "smtpServer", "smtp.example.net");
     m_sFrom                       = getXmlStringProp(xmlParent, "from", "");
     m_sSubject                    = getXmlStringProp(xmlParent, "subject", "");
     std::string sTo               = getXmlStringProp(xmlParent, "to", "");
     boost::algorithm::replace_all(sTo, " ", "");
     boost::split(m_vTo, sTo, boost::is_any_of(","));
     for(int cpt = 0; cpt < m_vTo.size(); cpt++) {
        boost::algorithm::replace_all(m_vTo[cpt], ",", "");
      }
     //  If checkLdap is true and an email exist in the form I check the ldap db for a known email.
     m_bCheckLdap                  = getXmlBoolProp  (xmlParent, "checkLdap", false);
     m_bDebugLdap                  = getXmlBoolProp  (xmlParent, "debugLdap", false);
     m_sLdapServer                 = getXmlStringProp(xmlParent, "ldapServer", "ldap.exemple.net");
     m_sLdapBase                   = getXmlStringProp(xmlParent, "ldapBase", "dc=exemple,dc=net");
    }

  if ( action == "ldap" ) {
     m_action                      = action;
     m_bDebugLdap                  = getXmlBoolProp  (xmlParent, "debugLdap", false);
     m_sLdapServer                 = getXmlStringProp(xmlParent, "ldapServer", "ldap.exemple.net");
     m_sLdapBase                   = getXmlStringProp(xmlParent, "ldapBase", "dc=exemple,dc=net");
     m_sLdapLogon                  = getXmlStringProp(xmlParent, "ldapLogon", "cn=rootdn,dc=example,dc=net");
     m_sLdapSecret                 = getXmlStringProp(xmlParent, "ldapSecret", "aGoodPassword");
     wApp->log("notice") << "gdWForm::loadWPushButton : Action is ldap ";
    }

  if ( action == "email" )
     pBut->clicked().connect(SLOT(this, gdWForm::doEmail));

  if ( action == "csv" )
     pBut->clicked().connect(SLOT(this, gdWForm::doCSV));

  if ( action == "ldap" )
     pBut->clicked().connect(SLOT(this, gdWForm::doLdap));

  return pBut;
 }

void gdWForm::doReset()
 {
  clear();
  load();
 }

void gdWForm::doEmail()
 {
  if ( !checkRequired() ) return;

  // Important : build the body before the clear
  WText* wtBody = 0;
  if ( m_bRedirectTextAddEmailBody ) 
     wtBody = new WText(WString(buildEmailBody(), Wt::UTF8));
  clear();
  addWidget(new WText(m_sRedirectText));
  if ( m_bRedirectTextAddEmailBody ) 
     addWidget(wtBody);
  std::string message  = m_sRedirectText + wtBody->text().toUTF8();
  sendEmail(message, "");
 }

std::string gdWForm::email()
 {
  std::string val;
  for(int cpt = 0; cpt < m_vObjects.size(); cpt++) {
     if ( m_vObjects[cpt]->m_outputAttribute == "mail" ) {
        val = objectValues(m_vObjects[cpt]->m_pObject);
        break;
       }
   }
  return val;
 }

void gdWForm::sendEmail(const std::string& rBody, std::string rEmailTo)
{
  //wApp->log("notice") << "gdWForm::sendEmail : " << rBody << " To " << rEmailTo;
  wApp->log("notice") << "gdWForm::sendEmail :  To " << rEmailTo;

  Mail::Message message;
  message.setFrom(Mail::Mailbox(m_sFrom));

  if ( rEmailTo.size() > 0 )
    message.addRecipient(Mail::To, Mail::Mailbox(rEmailTo));
  for(int cpt = 0; cpt < m_vTo.size(); cpt++) {
    message.addRecipient(Mail::To, Mail::Mailbox(m_vTo[cpt]));
  }
  message.setSubject(m_sSubject);
  message.setBody("No html renderer in your mail client ");
  message.addHtmlBody (rBody);
  Mail::Client client;
  client.connect(m_sSmtpServer);
  client.send(message);

  /*
  vmime::platformDependant::setHandler<vmime::platforms::posix::posixHandler>();

  try
   {
    vmime::messageBuilder mb;
    
    // Fill in the basic fields
    mb.setExpeditor(vmime::mailbox(m_sFrom));
    
    vmime::addressList to;
    if ( rEmailTo.size() > 0 )
       to.appendAddress(vmime::create <vmime::mailbox>(rEmailTo));
    for(int cpt = 0; cpt < m_vTo.size(); cpt++) {
       to.appendAddress(vmime::create <vmime::mailbox>(m_vTo[cpt]));
     }
    mb.setRecipients(to);
    
    mb.setSubject(vmime::text(m_sSubject));
    
    // Message body
    mb.constructTextPart(vmime::mediaType(vmime::mediaTypes::TEXT, vmime::mediaTypes::TEXT_HTML));
    vmime::ref<vmime::htmlTextPart> textPart = mb.getTextPart().dynamicCast<vmime::htmlTextPart>();
//    textPart->setCharset(vmime::charsets::ISO8859_15);
    textPart->setCharset(vmime::charsets::UTF_8);
    textPart->setText(vmime::create<vmime::stringContentHandler>(rBody));
//    textPart->setText(vmime::create<vmime::stringContentHandler>
//                      ("<h1>Hello world !</h1>")
//                     );
    textPart->setPlainText(vmime::create<vmime::stringContentHandler>
                      ("No html renderer in your mail client ?")
                     );

//    mb.getTextPart()->setText(vmime::create <vmime::stringContentHandler>(
//                        "I'm writing this short text to test message construction " \
//                        "using the vmime::messageBuilder component."));
//
    
    // Construction
    vmime::ref<vmime::message> msg = mb.construct();
    
    // Raw text generation
    if ( 0 ) {
       std::cout << "Generated message:" << std::endl;
       std::cout << "==================" << std::endl;

       vmime::utility::outputStreamAdapter out(std::cout);
       msg->generate(out);
       std::cout << std::endl;
       std::cout << "==================" << std::endl;
      }

    vmime::ref<vmime::net::session> sess = vmime::create<vmime::net::session>();
    vmime::utility::url url(m_sSmtpServer);
    vmime::ref<vmime::net::transport> tr = sess->getTransport(url);
    tr->connect();
    tr->send(msg);
    tr->disconnect();
   }
  // VMime exception
  catch (vmime::exception& e) {
   std::cout << "vmime::exception: " << e.what() << std::endl;
   throw;
  }
  // Standard exception
  catch (std::exception& e) {
   std::cout << "std::exception: " << e.what() << std::endl;
   //throw;
   }
   */
 }

bool gdWForm::checkRequired()
 {
  bool bOk = true;
  for(int cpt = 0; cpt < m_vObjects.size(); cpt++) {
     if ( !m_vObjects[cpt]->m_bRequired ) continue;
     if ( objectValues(m_vObjects[cpt]->m_pObject).size() > 0 ) continue;
     bOk = false;
     WFormWidget* pW = dynamic_cast<WFormWidget*>(m_vObjects[cpt]->m_pObject);
     pW->setFocus();
     WMessageBox::show("Form incomplete", "Please complete the required fields", Ok);
     pW->setFocus();
     break;
  }
  return bOk;
 }

std::string gdWForm::buildEmailBody()
 {
  std::string sBody;

  sBody += "<table class=\"WForm-table\" style=\"border-collapse: collapse; font-family: Arial, Helvetica, sans-serif\">\n";

  sBody += " <thead style=\"background-color: #06C; color: #FFF;\">\n";
  sBody += "  <tr style=\"text-align: left;\">\n";
  sBody += "   <th style=\"width: 8em;\">Attribute</th>\n";
  sBody += "   <th>Values</th>\n";
  sBody += "  </tr>\n";
  sBody += " </thead>\n";

  sBody += " <tbody>\n";
  for(int cpt = 0; cpt < m_vObjects.size(); cpt++) {
     sBody += "  <tr style=\"border-width: 1px 0; border-style: solid; border-color: #06C;\">\n";
     sBody += "   <td>" + m_vObjects[cpt]->m_outputAttribute + "</td>\n";
     sBody += "   <td>" + gdcore_string2xhtml(objectValues(m_vObjects[cpt]->m_pObject)) + "</td>\n";
     sBody += "  </tr>\n";
   }
  sBody += " </tbody>\n";
  sBody += "</table>\n";
  return sBody;
 }

std::string gdWForm::objectValues(WObject* pObject)
 {
  std::string sValue;

  if ( !pObject ) return sValue;

  if ( typeid(*pObject) == typeid(WComboBox) ) {
     WComboBox* pCombo = dynamic_cast<WComboBox*>(pObject);
     sValue = asString(pCombo->currentIndex()).toUTF8()
            + " : "
            + pCombo->currentText().toUTF8();
     return sValue;
  }
  if ( typeid(*pObject) == typeid(WCheckBox) ) {
     WCheckBox* pCB = dynamic_cast<WCheckBox*>(pObject);
     sValue = asString(pCB->isChecked()).toUTF8();
     return sValue;
  }
  if ( typeid(*pObject) == typeid(WButtonGroup) ) {
     WButtonGroup* pBG = dynamic_cast<WButtonGroup*>(pObject);
     sValue = asString(pBG->selectedButtonIndex()).toUTF8();
     return sValue;
  }
  if ( typeid(*pObject) == typeid(WRadioButton) ) {
     WRadioButton* pRB = dynamic_cast<WRadioButton*>(pObject);
     sValue = asString(pRB->isChecked()).toUTF8();
     return sValue;
  }
  if ( typeid(*pObject) == typeid(WSelectionBox) ) {
     WSelectionBox* pSB = dynamic_cast<WSelectionBox*>(pObject);
     if ( pSB->selectionMode() == Wt::NoSelection || pSB->selectionMode() == Wt::SingleSelection ) {
        sValue = asString(pSB->currentIndex()).toUTF8()
               + " : "
               + pSB->currentText().toUTF8();
        return sValue;
     }
     // ExtendedSelection
     const std::set<int>& mySel = pSB->selectedIndexes();
     std::set<int>::const_iterator it;
     int  idx = 0;
     for(it = mySel.begin(); it != mySel.end(); ++it, ++idx) {
        if ( idx ) sValue += "; ";
        sValue += asString(*it).toUTF8()
               + " : "
               + pSB->itemText(*it).toUTF8();
     }
     return sValue;
  }
  if ( typeid(*pObject) == typeid(WLineEdit) ) {
     WLineEdit* pLE = dynamic_cast<WLineEdit*>(pObject);
//     TODO : que faire ?
//     if ( pLE->echoMode() == Wt::WLineEdit::Password )
//        sValue = "*****";
//      else
        sValue = pLE->text().toUTF8();
     return sValue;
  }
  if ( typeid(*pObject) == typeid(WTextArea) ) {
     if ( m_action == "csv" )
        return "gdWForm Error WTextArea is not supported with csv format !!!";
     WTextArea* pTA = dynamic_cast<WTextArea*>(pObject);
     sValue = pTA->text().toUTF8();
     return sValue;
  }
  if ( typeid(*pObject) == typeid(WCalendar) ) {
     WCalendar* pCal = dynamic_cast<WCalendar*>(pObject);
     const std::set<WDate>& rDate = pCal->selection();
     std::set<WDate>::const_iterator it;
     int  idx = 0;
     for(it = rDate.begin(); it != rDate.end(); ++it, ++idx) {
        if ( idx ) sValue += "; ";
        sValue += (*it).toString("yyyy-MM-dd").narrow();
     }
     return sValue;
  }

  return sValue;
 }

void gdWForm::doCSV()
 {
  wApp->log("notice") << "gdWForm::doCSV";
  if ( !checkRequired() ) return;

  wApp->log("notice") << "gdWForm::doCSV : try to open ..." << m_sCsvFile.c_str();
  // Open m_sCsvFile
  FILE* hCsv = fopen(m_sCsvFile.c_str(), "a");
  if ( !hCsv ) {
      wApp->log("notice") << "gdWForm::doCSV : cannot open " << m_sCsvFile.c_str();
      return;
     }

  wApp->log("notice") << "gdWForm::doCSV : " << m_sCsvFile.c_str() << " correctly opened";
  std::string sCsvLine;
  std::string currentEmail;
  for(int cpt = 0; cpt < m_vObjects.size(); cpt++) {
     if ( cpt ) sCsvLine += m_sCsvDelimiter;
     if ( m_vObjects[cpt]->m_outputAttribute == "mail" ) {
        if ( m_bCheckLdap ) { // check ldap email and add the dn if exist
           currentEmail = objectValues(m_vObjects[cpt]->m_pObject);
           std::string sDn = getLdapDn(currentEmail);
           sCsvLine += "\"" + sDn + "\"" + m_sCsvDelimiter;
          }
        }
     sCsvLine += "\"" + objectValues(m_vObjects[cpt]->m_pObject) + "\"";
   }
  sCsvLine += "\n";
  fwrite(sCsvLine.c_str(), sCsvLine.size(), 1, hCsv);

  // Close m_sCsvFile
  fclose(hCsv);

  // Important : build the body before the clear
  WText* wtBody = new WText(WString("", Wt::UTF8));
  wApp->log("notice") << "gdWForm::doCSV : buildEmailBody";
  if ( m_bRedirectTextAddEmailBody ) {
     wtBody->setText(WString(buildEmailBody(), Wt::UTF8));
     // Not valid : XSS: discarding invalid attribute: href: javascript:window.print()
     //body += "<a href=\"javascript:window.print()\">print</a>";
  }
  wApp->log("notice") << "gdWForm::doCSV : clear widget";
  clear();
  wApp->log("notice") << "gdWForm::doCSV : end clear widget";
  addWidget(new WText(m_sRedirectText));
  wApp->log("notice") << "gdWForm::doCSV : end add text";
  if ( m_bRedirectTextAddEmailBody ) 
     addWidget(wtBody);

  if ( m_bCsvEmailForm ) {
//     std::string curEmail = email();
     std::string message  = m_sRedirectText + wtBody->text().toUTF8();
     wApp->log("notice") << "gdWForm::doCSV : sendEmail to " << currentEmail;
     sendEmail(message, currentEmail);
    }

  wApp->log("notice") << "gdWForm::doCSV : end";

  // Close the app after 60 000 milisecond (1 minute)
  WTimer::singleShot(60000, SLOT(this, gdWForm::doClose));
 }


void gdWForm::doClose()
 {
  // TODO : add a redirect field in the action button
  wApp->log("notice") << "gdWForm::doClose";
  if ( m_sRedirectPage.size() > 0 ) {
     wApp->redirect(m_sRedirectPage);
     wApp->log("notice") << "gdWForm::doClose redirect to : " << m_sRedirectPage;
    }
  wApp->quit();
 }

std::string gdWForm::getLdapDn(std::string& email)
 {
  gdCLdapServer cLdapServer;
  cLdapServer.strServer      = m_sLdapServer;

  if ( cLdapServer.TryServer() )
      wApp->log("notice") << "gdWForm::getLdapDn : Try server : " << m_sLdapServer << " for email : " << email << " OK";
   else {
      wApp->log("notice") << "gdWForm::getLdapDn : Try server : " << m_sLdapServer << " for email : " << email << " ERROR";
      return "CHECKLDAP ERROR : Cannot connect to the server";
    }

  gdCLdap             cLdap;
  cLdap.setDebugging(m_bDebugLdap);

  gdCLdapAttributes   cWitchAttrs;
  gdCLdapFilter       cFilter;
  gdCLdapEntries      cEntries;

  cFilter.pServer = &cLdapServer;
  cFilter.strInitialSearchBase = m_sLdapBase;
  cFilter.nScope        = LDAP_SCOPE_SUB;
  cFilter.strFilter     = "(&(mail=" + email + "))";
  cWitchAttrs.push_back(new gdCLdapAttribute("mail"));
  if ( !cLdap.GetInfo(cFilter, cWitchAttrs, cEntries) ) {
     wApp->log("notice") << "gdWForm::getLdapDn : Query LDAP : " << m_sLdapServer << " ERROR";
     return "CHECKLDAP ERROR : Cannot query ldap";
    }

  if ( !cEntries.size() ) {
     wApp->log("notice") << "gdWForm::getLdapDn : Query LDAP : " << m_sLdapServer << " ERROR Unknown EMAIL";
     return "CHECKLDAP ERROR : Unknown EMAIL";
    }

  if ( cEntries.size() > 1 ) {
     wApp->log("notice") << "gdWForm::getLdapDn : Query LDAP : " << m_sLdapServer << " ERROR  More than one value";
     return "CHECKLDAP ERROR : More than one value";
    }

  wApp->log("notice") << "gdWForm::getLdapDn : Query LDAP : " << email << " match as " <<  cEntries[0]->Dn;
  return  cEntries[0]->Dn;
 }

void gdWForm::doLdap()
 {
  wApp->log("notice") << "gdWForm::doLdap()";
  if ( !checkRequired() ) return;

  gdCLdapServer cLdapServer;
  cLdapServer.strServer      = m_sLdapServer;
  cLdapServer.strDisplayName = m_sLdapServer;
  cLdapServer.strLogon       = m_sLdapLogon;
  cLdapServer.strPassword    = m_sLdapSecret;

  gdCLdapEntry       entry;
  gdCLdapAttribute*  newAttr01 = new gdCLdapAttribute("objectClass"); newAttr01->addValue("EurIndividualPerson");
  newAttr01->bNewAttribute = true;
  entry.Attrs.push_back(newAttr01);

  std::string sGivenName;
  std::string sSn;
  std::string sCn;
  for(int cpt = 0; cpt < m_vObjects.size(); cpt++) {
    if ( m_vObjects[cpt]->m_outputAttribute == "sn" )        sSn        = objectValues(m_vObjects[cpt]->m_pObject);
    if ( m_vObjects[cpt]->m_outputAttribute == "givenName" ) sGivenName = objectValues(m_vObjects[cpt]->m_pObject);
    gdCLdapAttribute*  newAttr = new gdCLdapAttribute(m_vObjects[cpt]->m_outputAttribute); newAttr->addValue(objectValues(m_vObjects[cpt]->m_pObject));
    newAttr->bNewAttribute = true;
    entry.Attrs.push_back(newAttr);
   }

  gdCLdapAttribute*  newAttr = new gdCLdapAttribute("cn"); newAttr->addValue(sSn + " " + sGivenName);
  newAttr->bNewAttribute = true;
  entry.Attrs.push_back(newAttr);

  entry.Dn = "cn=" + sSn + " " + sGivenName + "," + m_sLdapBase;

  if ( !cLdapServer.UpdateEntry(entry, true) )
     fprintf(stderr, "cannot update ldap entry\n");
   else
     fprintf(stderr, "Update ldap entry done !\n");

  clear();
  addWidget(new WText(m_sRedirectText));
 }

