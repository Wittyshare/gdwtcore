/*
 * Copyright (C) 2006-Today Guy Deleeuw
 *
 * See the LICENSE file for terms of use.
 */

#include <stdio.h>
#include <iostream>

#include <boost/algorithm/string/replace.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/exception.hpp>
#include <boost/lexical_cast.hpp>
#include "boost/date_time/gregorian/gregorian.hpp"
#include "boost/date_time/posix_time/posix_time.hpp"

#include <Wt/WEnvironment>
#include <Wt/WLogger>

#include "gdApplication.h"

using namespace Wt;

gdApplication::gdApplication(const WEnvironment& env)
 : WApplication(env), m_bRemoteUser(false), m_bUserID(false)
 { }

gdApplication::~gdApplication()
 {
  std::cout << ("notice") << "Destroy application for userID : " << getUserID();
  // Le programmeur ne doit pas appeler lui-même les destructeurs des classes mères, le langage s'en charge. 
 }

std::string gdApplication::getParameter(const std::string& param, const std::string& defaultVal)
 {
  std::string  strVal("");
  try
   {
    const std::string* strVal2 = wApp->environment().getParameter(param);
    if ( strVal2 )
       strVal = *strVal2;
     else
       strVal =  defaultVal;
   }
  catch(std::exception& e)
   {
    strVal = e.what();
    std::string errorToCompare = "missing argument: " + param;
    if ( strVal == errorToCompare )
       strVal =  defaultVal;
     else
       strVal = "unknown error !";
   }

  return strVal;
 }

std::string gdApplication::getClientIP()
 {
  std::string retValue = environment().getCgiValue("Remote_Addr");
  return retValue;
 }

std::string gdApplication::getServerPort()
 {
  std::string retValue = environment().getCgiValue("SERVER_PORT");
  return retValue;
 }

std::string gdApplication::getServerHostUrl(bool bWithPort)
 {
  std::string retValue = wApp->environment().urlScheme();
  retValue += "://";
  retValue += environment().hostName();
  if ( bWithPort )
    {
     retValue += ":";
     retValue += getServerPort();
    }

  return retValue;
 }

std::string gdApplication::getUserID()
 {
  if ( m_strUserID != "" ) return m_strUserID;
  // si userID existe on est dans l'extranet (userID est fixé a 'user_extranet_id' par la conf apache de l'extranet)
  m_strUserID = getParameter(WString::tr("userID").narrow(), "guest");
  if ( m_strUserID == "guest" ) {
     // si REMOTE_USER existe on est dans le reseau local (auth apache)
     m_strUserID = environment().getCgiValue("REMOTE_USER");
     if ( m_strUserID == "" ) {
        // sais pas ou on est : mon pc de test ?
        m_strUserID = "guest";
       }
      else m_bRemoteUser = true;
    }
   else m_bUserID = true;
  wApp->log("notice") << "Initializing application for userID : " << m_strUserID;
  return m_strUserID;
 }

std::string gdApplication::getUserRootPath()
 {
  std::string  strUserRootPath(WString::tr("rootPath").narrow());
  if ( strUserRootPath == "" || strUserRootPath == "??rootPath??" ) strUserRootPath = "/var/www/users";
  strUserRootPath += "/";

  std::string  strUser = getUserID();
  strUser += "/";
  strUserRootPath += strUser;

  if ( !boost::filesystem::exists(strUserRootPath) )
    {
     if ( !boost::filesystem::create_directory(strUserRootPath) )
       {
        wApp->log("notice") << "Cannot create directory" << strUserRootPath;
       }
     std::string strTmp = strUserRootPath + "tmp";
     if ( !boost::filesystem::create_directory(strTmp) )
       {
        wApp->log("notice") << "Cannot create directory" << strTmp;
       }
    }
  return strUserRootPath;
 }

std::string gdApplication::getUserRootAppPath(const std::string& forceApp, bool bUseSlash)
 {
  std::string  strUserRootAppPath(getUserRootPath());
  if ( forceApp == "" )
     strUserRootAppPath += WString::tr("useAppName").narrow();
   else
     strUserRootAppPath += forceApp;
  if ( bUseSlash )
     strUserRootAppPath += "/";

  if ( !boost::filesystem::exists(strUserRootAppPath) ) {
     if ( !boost::filesystem::create_directory(strUserRootAppPath) )
        wApp->log("notice") << "Cannot create directory" << strUserRootAppPath;
    }
  return strUserRootAppPath;
 }

std::string gdApplication::getUserCfgPath()
 {
  std::string  strUserCfgPath(getUserRootPath());
  strUserCfgPath += "Cfg";
  if ( !boost::filesystem::exists(strUserCfgPath) )
     if ( !boost::filesystem::create_directory(strUserCfgPath) )
        wApp->log("notice") << "Cannot create directory" << strUserCfgPath;

  return strUserCfgPath;
 }

std::string gdApplication::getUserCfgFile()
 {
  std::string  strUserCfgFile(getUserCfgPath());
  strUserCfgFile += "/";
  strUserCfgFile += WString::tr("useAppName").narrow();
  return strUserCfgFile;
 }

std::string gdApplication::getUserTmpPath()
 {
  std::string  strUserTmpPath(getUserRootPath());
  strUserTmpPath += "tmp/";

  return strUserTmpPath;
 }

std::string gdApplication::getUserTmpFile(const std::string& defaultExtension)
 { return (getUserTmpPath() + "temp" + boost::lexical_cast<std::string>(clock()) + "." + defaultExtension); }

std::string gdApplication::converttUserTmpFileToURIFile(const std::string& tmpPath)
 {
  std::string  strSysPath(WString::tr("mainPath").narrow());

  std::string  strHost(getServerHostUrl(false));

  std::string retPath = tmpPath;
  boost::algorithm::replace_all(retPath, strSysPath, strHost);
  return retPath;
 }

