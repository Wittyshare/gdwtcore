 /*         Author: Guy Deleeuw : guy.deleeuw@gmail.com */

#ifndef __gdMeetingView_H__
#define __gdMeetingView_H__

#include <Wt/WContainerWidget>
#include <Wt/WTableView>
#include <Wt/WStandardItemModel>
#include <Wt/WTimer>

#include <gdcore/bdAsterisk/ConnectionHandler.h>

class gdMeetingView : public Wt::WContainerWidget {
  public :
                                    gdMeetingView(Wt::WContainerWidget* parent = 0);
                                    ~gdMeetingView();
   void                             load();

   void                             setRowHeight(long rowHeight);
   void                             hideHeader(bool bHideHeader);
   void                             hideColumns(const std::string& columnsToHide);
   void                             setServer(const std::string& sServer);
   void                             setPort(long lPort);
   void                             setRootNumber(const std::string& sRootNumber);
   void                             setUser(const std::string& sUser);
   void                             setPassword(const std::string& sPassword);
   void                             setConferenceID(long lConf);

  public slots:
   void                             onConferenceChanged();
   void                             doLoadConferences();

  private :
   long                             m_lRowHeight;
   bool                             m_bHideHeader;
   std::string                      m_sHideColumns;
   std::string                      m_sServer;
   long                             m_lPort;
   std::string                      m_sRootNumber;
   std::string                      m_sUser;
   std::string                      m_sPassword;
   long                             m_lConferenceID;
   Wt::WStandardItemModel*          m_modelConferences;
   Wt::WTableView*                  m_tvConferences;
   long                             m_currentConference;
   Wt::WTimer                       m_pTimerConference;
   double                           m_dDelay;
   ConnectionHandler*               m_manager;
 };

#endif 

