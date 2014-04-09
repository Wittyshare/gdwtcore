
#include <boost/filesystem.hpp>

#include <iostream>
#include <dlfcn.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

#include <Wt/WApplication>
#include <Wt/WDateTime>
#include <Wt/WStandardItem>

#include <gdcore/gdCore.h>

#include "gdMeetingView.h"

// Asterisk
#include <gdcore/bdAsterisk/MeetMeList.h>
#include <gdcore/bdAsterisk/MeetMeListAnswer.h>
#include <gdcore/bdAsterisk/MeetMeMuteUnMute.h>

using namespace Wt;

gdMeetingView::gdMeetingView(WContainerWidget* parent)
  : WContainerWidget(parent), m_lRowHeight(0), m_bHideHeader(false), m_lPort(5038), m_sServer("localhost"), m_lConferenceID(0),
    m_modelConferences(0), m_tvConferences(0), m_currentConference(0), m_dDelay(15000), m_manager(0)
{
  wApp->messageResourceBundle().use(wApp->docRoot() + wApp->resourcesUrl() + "/gdwtcore/Transl/WAsterisk/WMeetingView");
  addStyleClass("gdMeetingView");
  wApp->log("notice") <<  "gdMeetingView::gdMeetingView : start";
}

gdMeetingView::~gdMeetingView()
{
  if ( m_manager ) delete m_manager;
  m_manager = 0;
  wApp->log("notice") <<  "gdMeetingView::~gdMeetingView : dtor";
}

void gdMeetingView::load()
{
  wApp->log("notice") <<  "gdMeetingView::load : start";
  WContainerWidget::load();
  wApp->log("notice") <<  "gdMeetingView::load : end base classe";
  // List all active conferences
  m_modelConferences = new WStandardItemModel(0, 6);
  m_modelConferences->setHeaderData(0, WString::tr("gdMeetingView-id"));
  m_modelConferences->setHeaderData(1, WString::tr("gdMeetingView-count"));
  m_modelConferences->setHeaderData(2, WString::tr("gdMeetingView-marked"));
  m_modelConferences->setHeaderData(3, WString::tr("gdMeetingView-activity"));
  m_modelConferences->setHeaderData(4, WString::tr("gdMeetingView-creation"));
  m_modelConferences->setHeaderData(5, WString::tr("gdMeetingView-locked"));
  m_modelConferences->setSortRole(UserRole);
  m_tvConferences = new WTableView(this);
  m_tvConferences->setModel(m_modelConferences);
  m_tvConferences->setColumnResizeEnabled(true);
  m_tvConferences->setSortingEnabled(true);
  m_tvConferences->setAlternatingRowColors(true);
  m_tvConferences->setSelectionMode(ExtendedSelection);
  m_tvConferences->setColumnAlignment(1, AlignRight);
  m_tvConferences->setColumnAlignment(3, AlignRight);
  if ( m_lRowHeight > 0 )
    m_tvConferences->setRowHeight(m_lRowHeight);
  m_tvConferences->setColumnWidth(0, 180);
  m_tvConferences->setColumnWidth(1, 300);
  if ( m_bHideHeader )
    m_tvConferences->setHeaderHeight(0);
  std::string  columnsToHide = m_sHideColumns;
  std::vector<std::string> sVectColumns;
  gd_core_string2vector(sVectColumns, columnsToHide.c_str(), ",");
  for (int iCol = 0; iCol < sVectColumns.size(); ++iCol) {
    int col = asNumber(sVectColumns[iCol]);
    m_tvConferences->setColumnHidden(col, true);
  }
  wApp->log("notice") <<  "gdMeetingView::load : start handler";
  m_manager = new ConnectionHandler(m_sServer, (int) m_lPort);
  wApp->log("notice") <<  "gdMeetingView::load : end handler, start connect " << m_sUser;
  if ( m_manager->connect(m_sUser, m_sPassword) != 0 ) {
    wApp->log("ERROR") <<  "gdMeetingView::load : cannot connect to AMI interface !!!";
    return;
  }
  wApp->log("notice") <<  "gdMeetingView::load : connected to AMI interface";
  doLoadConferences();
  m_pTimerConference.setInterval(m_dDelay);
  m_pTimerConference.timeout().connect(SLOT(this, gdMeetingView::doLoadConferences));
  m_pTimerConference.start();
  m_tvConferences->selectionChanged().connect(SLOT(this, gdMeetingView::onConferenceChanged));
}

void gdMeetingView::setRowHeight(long rowHeight)
{
  m_lRowHeight = rowHeight;
}

void gdMeetingView::hideHeader(bool bHideHeader)
{
  m_bHideHeader = bHideHeader;
}

void gdMeetingView::hideColumns(const std::string& columnsToHide)
{
  m_sHideColumns = columnsToHide;
}

void gdMeetingView::setServer(const std::string& sServer)
{
  m_sServer = sServer;
}

void gdMeetingView::setPort(long lPort)
{
  m_lPort = lPort;
}

void gdMeetingView::setRootNumber(const std::string& sRootNumber)
{
  m_sRootNumber = sRootNumber;
}

void gdMeetingView::setUser(const std::string& sUser)
{
  m_sUser = sUser;
}

void gdMeetingView::setPassword(const std::string& sPassword)
{
  m_sPassword = sPassword;
}

void gdMeetingView::setConferenceID(long lConf)
{
  m_lConferenceID = lConf;
}

void gdMeetingView::doLoadConferences()
{
  // If m_lConferenceID != 0 then, a conference number is forced
  if ( m_lConferenceID ) return;
  m_modelConferences->removeRows(0, m_modelConferences->rowCount());
  ConferenceList       confList;
  ConferenceListAnswer confAns;
  m_manager->sendAndWaitForAnswer(confList, confAns);
  wApp->log("notice") << "There are " << confAns.getNbConnected() << " active conference(s)";
  std::vector<Conference*> vConf = confAns.getConnectedExt();
  for (int i = 0; i < vConf.size(); ++i) {
    wApp->log("notice") << "Id : " << vConf[i]->id() << " participants : " << vConf[i]->partiesCount() << " marked : " << vConf[i]->marked()
                        << " activity : " <<  vConf[i]->activity() << " creation : " <<  vConf[i]->creation() << " locked : " <<  vConf[i]->locked();
    std::vector<WStandardItem*> pRow;
    WStandardItem* pCol0 = new WStandardItem(vConf[i]->id());
    long conferenceID = asNumber(vConf[i]->id());
    pCol0->setData(conferenceID, UserRole);
    pRow.push_back(pCol0);
    WStandardItem* pCol1 = new WStandardItem(asString(vConf[i]->partiesCount()));
    pCol1->setData(vConf[i]->partiesCount(), UserRole);
    pRow.push_back(pCol1);
    WStandardItem* pCol2 = new WStandardItem(vConf[i]->marked());
    pCol2->setData(vConf[i]->marked(), UserRole);
    pRow.push_back(pCol2);
    WStandardItem* pCol3 = new WStandardItem(vConf[i]->activity());
    pCol3->setData(vConf[i]->activity(), UserRole);
    pRow.push_back(pCol3);
    WStandardItem* pCol4 = new WStandardItem(vConf[i]->creation());
    pCol4->setData(vConf[i]->creation(), UserRole);
    pRow.push_back(pCol4);
    WStandardItem* pCol5 = new WStandardItem(vConf[i]->locked());
    pCol5->setData(vConf[i]->locked(), UserRole);
    pRow.push_back(pCol5);
    m_modelConferences->appendRow(pRow);
  }
}

void gdMeetingView::onConferenceChanged()
{
  wApp->log("notice") <<  "gdMeetingView::onConferenceChanged() : start ...";
  WModelIndexSet pSet = m_tvConferences->selectedIndexes();
  WModelIndexSet::iterator it = pSet.begin();
  if ( pSet.empty() ) return;
  WModelIndex    idx = *it;
  long conferenceID = asNumber(m_modelConferences->item(idx.row(), 0)->data(UserRole));
  wApp->log("notice") <<  "gdMeetingView::onConferenceChanged() : idx row = " << idx.row() << " conferenceID " << conferenceID;
}

