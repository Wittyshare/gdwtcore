/*
 * Copyright (C) 2006-Today Guy Deleeuw
 *
 * See the LICENSE file for terms of use.
*/

#ifndef __gdWForm__
#define __gdWForm__

#include <libxml/parser.h>
#include <libxml/tree.h>

#include <Wt/WWidget>
#include <Wt/WContainerWidget>
#include <Wt/WAnchor>
#include <Wt/WGroupBox>
#include <Wt/WText>
#include <Wt/WImage>

#include <Wt/WComboBox>
#include <Wt/WCheckBox>
#include <Wt/WRadioButton>
#include <Wt/WButtonGroup>
#include <Wt/WSelectionBox>
#include <Wt/WLineEdit>
#include <Wt/WTextArea>
#include <Wt/WCalendar>

#include <Wt/WPushButton>

class gdWFormItem
 {
  public :
                     gdWFormItem(std::string& outputAttribute, Wt::WObject* pObject, bool bRequired = false);
  bool               m_bRequired;
  std::string        m_outputAttribute;
  Wt::WObject*       m_pObject;
 };




class gdWForm : public Wt::WContainerWidget
 {
  public :
                            gdWForm(const std::string& rXmlFile, Wt::WContainerWidget* parent = 0);
                            ~gdWForm();
   void                     load();
   bool                     checkRequired();
   bool			    m_bAutoLoad;
   std::vector<gdWFormItem*>   m_vObjects;

  private :
   int                      resizeImage(std::string in, std::string out, std::string size);
   bool                     loadXmlFile(const std::string& rXmlFile);
   bool                     loadWForm         (xmlNode* xmlParent, Wt::WContainerWidget* parent = 0);
   bool                     loadWObjects      (xmlNode* xmlParent, Wt::WContainerWidget* parent = 0);
   bool                     loadWClass        (xmlNode* xmlParent, Wt::WWidget* widget);

   // Static widget
   Wt::WAnchor*             loadWAnchor       (xmlNode* xmlParent, Wt::WContainerWidget* parent = 0);
   Wt::WGroupBox*           loadWGroupBox     (xmlNode* xmlParent, Wt::WContainerWidget* parent = 0);
   Wt::WText*               loadWText         (xmlNode* xmlParent, Wt::WContainerWidget* parent = 0);
   Wt::WImage*              loadWImage        (xmlNode* xmlParent, Wt::WContainerWidget* parent = 0);

   Wt::WComboBox*           loadWComboBox     (xmlNode* xmlParent, Wt::WContainerWidget* parent = 0);
   Wt::WCheckBox*           loadWCheckBox     (xmlNode* xmlParent, Wt::WContainerWidget* parent = 0);
   Wt::WButtonGroup*        loadWButtonGroup  (xmlNode* xmlParent, Wt::WContainerWidget* parent = 0);
   Wt::WRadioButton*        loadWRadioButton  (xmlNode* xmlParent, Wt::WContainerWidget* parent = 0);
   Wt::WSelectionBox*       loadWSelectionBox (xmlNode* xmlParent, Wt::WContainerWidget* parent = 0);
   Wt::WLineEdit*           loadWLineEdit     (xmlNode* xmlParent, Wt::WContainerWidget* parent = 0);
   Wt::WTextArea*           loadWTextArea     (xmlNode* xmlParent, Wt::WContainerWidget* parent = 0);
   Wt::WCalendar*           loadWCalendar     (xmlNode* xmlParent, Wt::WContainerWidget* parent = 0);

   Wt::WPushButton*         loadWPushButton   (xmlNode* xmlParent, Wt::WContainerWidget* parent = 0);

   std::string              email();
   void                     sendEmail(const std::string& rBody, std::string rEmailTo);
   std::string              buildEmailBody();
   std::string              objectValues(Wt::WObject* pObject);
   std::string              getLdapDn(std::string& email);

  public slots :
   void           doReset();
   void           doEmail();
   void           doCSV();
   void           doLdap();
   void           doClose();

  private :
   std::string                 m_sXmlFile;
   std::string                 m_action;
   std::string                 m_sRedirectPage;

   // email
   std::string                 m_sFrom;
   std::string                 m_sSmtpServer;
   std::vector<std::string>    m_vTo;
   std::string                 m_sSubject;
   std::string                 m_sRedirectText;
   bool                        m_bRedirectTextAddEmailBody;

   // csv
   // Path to the csv file, the file does exist with the correct fields name in row 1
   std::string                 m_sCsvFile;
   // Store the delimiter  : by default ;
   std::string                 m_sCsvDelimiter;
   // If m_bCsvEmailForm is true a confirmation is send by email, by default : false
   bool                        m_bCsvEmailForm;
   // If m_bCheckLdap is True the app add a column "dn" before the column "email". The column email does exist
   //  search "email" in the ldap db
   //  If the server does not responding return : CHECKLDAP ERROR : Cannot connect to the server"
   //  If the search failed return "CHECKLDAP ERROR : Cannot query ldap"
   //  If the result is multivalued, return "CHECKLDAP ERROR : More than one value"
   //  If the result = 0 value : return "CHECKLDAP : Unknown EMAIL"
   bool                        m_bCheckLdap;
   std::string                 m_sLdapServer;
   std::string                 m_sLdapBase;   // start search point
   std::string                 m_sLdapLogon;
   std::string                 m_sLdapSecret;
   bool                        m_bDebugLdap;
 };

#endif // ifdef __gdWForm__

