/*
 * Copyright (C) 2006-Today Guy Deleeuw
 *
 * See the LICENSE file for terms of use.
 */

#ifndef GD_APPLICATION__
#define GD_APPLICATION__

/*! \file gdApplication.h */

#include <Wt/WApplication>
#include <Wt/WText>

/*! \class gdApplication
 *  \brief A class that superceed the WApplication class to increase the number of functions.
*/
class gdApplication : public Wt::WApplication
 {
  public :
  /*! \brief Constructor.  */
   gdApplication(const Wt::WEnvironment& env);

  /*! \brief Destructor.  */
   ~gdApplication();

  /*! \brief Get the client ip : */
   std::string            getClientIP();

  /*! \brief Get the server port : 80, 443, ect... .  */
   std::string            getServerPort();

  /*! \brief Get the server host url : http://192.168.3.192:80 by example.  */
   std::string            getServerHostUrl(bool bWithPort = false);

  /*! \brief Get an environment parameter.  */
   std::string            getParameter(const std::string& param, const std::string& defaultVal = "");

  /*! \brief Get the user ID, passed to the server environment or a special variable by default return guest.  */
   std::string            getUserID();

  /*! \brief Get the user rootPath see the app_config.xml file, by default : /var/www/users.  */
   std::string            getUserRootPath();

  /*! \brief Get the rootPath for the application example : /var/www/users/guest/gdQuery2/ eventually force to another app module like gdReport.  */
   std::string            getUserRootAppPath(const std::string& forceApp = "", bool bUseSlash = true);

  /*! \brief Get the rootPath for the application example : /var/www/users/guest/gdQuery2/ specify if the / are added to the end of the string */
   std::string            getUserRootAppPath(bool bUseSlash = true) { return getUserRootAppPath("", bUseSlash); };

  /*! \brief Get the cfgPath for the user example : /var/www/users/guest/Cfg. */
   std::string            getUserCfgPath();

  /*! \brief Get the cfgFile for the user example : /var/www/users/guest/Cfg/gdQuery2. To modify the file, remember add .xml at the end of the string returned  */
   std::string            getUserCfgFile();

  /*! \brief Get the tmpPath directory for the user example : /var/www/users/guest/tmp/.  */
   std::string            getUserTmpPath();

  /*! \brief Get a tmp file for the user example : /var/www/users/guest/tmp/tmp0001.  */
   std::string            getUserTmpFile(const std::string& defaultExtension = "tmp");

  /*! \brief Convert a tmp file to a URI file for the user example : /var/www/users/guest/tmp/tmp0001.pdf to http://localhost/users/guest/tmp/temp1.pdf */
   std::string            converttUserTmpFileToURIFile(const std::string& tmpPath);

  /*! \brief Facility function to access the application object.  */
   static gdApplication*  gdinstance() { return (gdApplication*) instance(); };

  /*! \brief Check if the application run on the local network  */
   bool                   isOnInternalNet() { return m_bRemoteUser; };

  /*! \brief Check if the application run on the extranet  */
   bool                   isOnExtranet()    { return m_bUserID; };

  private :
   std::string        m_strUserID;    // The cgiValue accessed only one time
   bool               m_bRemoteUser;  // si RemoteUser est present cela signifie que l'on tourne sur un serveur configure pour
                                      //  accepter les utilisateurs locaux.
   bool               m_bUserID;      // si userID est present cela signifie que l'on tourne sur le serveur extranet
 };

/*! \brief Define a shortcut to the application instance */
#define gdWApp gdApplication::gdinstance()

#endif // GD_APPLICATION__
