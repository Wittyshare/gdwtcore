/*
 * Copyright (C) 2006-Today Guy Deleeuw
 *
 * See the LICENSE file for terms of use.
*/

#ifndef __gdWLogon__
#define __gdWLogon__

#include <Wt/WContainerWidget>
#include <Wt/WLineEdit>
#include <Wt/WText>

class gdWLogon : public Wt::WContainerWidget {
public :
  gdWLogon(std::string logonText = std::string(), std::string passwdordText = std::string(), Wt::WContainerWidget* parent = 0);
  ~gdWLogon();
  void           setTextLogon(std::string& logonText);
  void           setTextPassword(std::string& passwdordText);
  void           load();
  void           setError(std::string& sError);

public slots :
  void           doLogon();

public :
  /*! \brief Signal emitted when a logon are validated */
  Wt::Signal<std::string, std::string>& logonValidated();

private :
  Wt::Signal<std::string, std::string> logonValidated_;

private :
  std::string                                                        m_sLogonText;
  std::string                                                        m_sPasswordText;
  Wt::WLineEdit*                                                     m_leLogon;
  Wt::WLineEdit*                                                     m_lePassword;
  Wt::WText*                                                         m_wtError;
};

#endif // ifdef __gdWLogon__

