/*
 * Copyright (C) 2006-Today Guy Deleeuw
 *
 * See the LICENSE file for terms of use.
 */

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/replace.hpp>

#include <Wt/WApplication>
#include <Wt/WString>

#include <gdcore/gdCore.h>

#include "gdStdModel.h"
#include "gdProxyModel.h"

using namespace Wt;

// TODO : Improve : inconsttently event checkStateChanged

gdStdModel::gdStdModel(WObject* parent)
  : WStandardItemModel(parent),
    m_bHeaderLoaded(false), m_bUseHeader(true), m_bCol0Checkable(false), m_bFirstRowReserved(false),
    m_bLastRowReserved(false), m_proxy(0), m_bOnCheckChanged(false), m_bDebug(false)
{
  setSortRole(UserRole);
}

gdStdModel::gdStdModel(int rows, int columns, WObject* parent)
  : WStandardItemModel(rows, columns, parent),
    m_bHeaderLoaded(false), m_bUseHeader(true), m_bCol0Checkable(false), m_bFirstRowReserved(false),
    m_bLastRowReserved(false), m_proxy(0), m_bOnCheckChanged(false), m_bDebug(false)
{
  setSortRole(UserRole);
}

gdStdModel::~gdStdModel()
{ }

gdStdModel::gdStdModel(const gdStdModel& smSrc)
// : WStandardItemModel(smSrc) // ne fonctionne pas
{
  if ( this == &smSrc ) return;
  if ( m_bDebug )
    wApp->log("notice") <<  "gdStdModel::gdStdModel copy constructor";
  m_strTitle            = smSrc.m_strTitle;
  m_strTitleA           = smSrc.m_strTitleA;
  m_strTitleB           = smSrc.m_strTitleB;
  m_strTitleC           = smSrc.m_strTitleC;
  m_strTitleD           = smSrc.m_strTitleD;
  m_vColumnsWidth       = smSrc.m_vColumnsWidth;
  m_vType               = smSrc.m_vType;
  m_proxy               = smSrc.m_proxy;
  m_bCol0Checkable      = smSrc.m_bCol0Checkable;
  m_bHeaderLoaded       = smSrc.m_bHeaderLoaded;
  m_bUseHeader          = smSrc.m_bUseHeader;
  m_bFirstRowReserved   = smSrc.m_bFirstRowReserved;
  m_bLastRowReserved    = smSrc.m_bLastRowReserved;
  m_bDebug              = smSrc.m_bDebug;
  m_bOnCheckChanged     = smSrc.m_bOnCheckChanged;
  // TODO : Selected items and checked items
  // TODO Ne compile plus sous Ubuntu 12.10 mais de toute manière WStandardItemModel n'a pas de copy constructor
  //  WStandardItemModel::WStandardItemModel(smSrc);
  //  const WStandardItemModel& wsim = dynamic_cast<const WStandardItemModel&>(smSrc);
  //  WStandardItemModel::WStandardItemModel(wsim);
}

void gdStdModel::setTitle(const std::string& title)
{
  m_strTitle  = title;
}

const std::string& gdStdModel::title() const
{
  return m_strTitle;
}

void gdStdModel::setTitleA(const std::string& titleA)
{
  m_strTitleA = titleA;
}

const std::string& gdStdModel::titleA() const
{
  return m_strTitleA;
}

void gdStdModel::setTitleB(const std::string& titleB)
{
  m_strTitleB = titleB;
}

const std::string& gdStdModel::titleB() const
{
  return m_strTitleB;
}

void gdStdModel::setTitleC(const std::string& titleC)
{
  m_strTitleC = titleC;
}

const std::string& gdStdModel::titleC() const
{
  return m_strTitleC;
}

void gdStdModel::setTitleD(const std::string& titleD)
{
  m_strTitleD = titleD;
}

const std::string& gdStdModel::titleD() const
{
  return m_strTitleD;
}

void gdStdModel::setProxy(gdProxyModel* proxy)
{
  m_proxy = proxy;
}

gdProxyModel* gdStdModel::proxy()
{
  return m_proxy;
}

void gdStdModel::setFirstRowReserved(bool bReserved)
{
  m_bFirstRowReserved = bReserved;
}

bool gdStdModel::isFirstRowReserved()
{
  return m_bFirstRowReserved;
}

void gdStdModel::setLastRowReserved(bool bReserved)
{
  m_bLastRowReserved = bReserved;
}

bool gdStdModel::isLastRowReserved()
{
  return m_bLastRowReserved;
}

void gdStdModel::setCol0Checkable(bool bCheckable)
{
  m_bCol0Checkable = bCheckable;
}

bool gdStdModel::isCol0Checkable()
{
  return m_bCol0Checkable;
}

void gdStdModel::useHeader(bool bUseHeader)
{
  m_bUseHeader = bUseHeader;
}

bool gdStdModel::isHeaderUsed()
{
  return m_bUseHeader;
}

void gdStdModel::loadHeader()
{
  m_bHeaderLoaded = true;
}

bool gdStdModel::isHeaderLoaded()
{
  return m_bHeaderLoaded;
}

void gdStdModel::reload()
{ }

void gdStdModel::removeAllRows()
{
  int    start = isFirstRowReserved();
  int    count = rowCount() - isFirstRowReserved() - isLastRowReserved();
  if ( rowCount() > start )
    removeRows(start, count);
}

gdStdItem* gdStdModel::itemFromIndexModel(const WModelIndex& index) const
{
  return dynamic_cast<gdStdItem*>(WStandardItemModel::itemFromIndex(index));
}

gdStdItem* gdStdModel::itemFromIndex(const WModelIndex& index) const
{
  if ( !m_proxy ) return dynamic_cast<gdStdItem*>(WStandardItemModel::itemFromIndex(index));
  WModelIndex idx = m_proxy->mapToSource(index);
  return dynamic_cast<gdStdItem*>(WStandardItemModel::itemFromIndex(idx));
}

gdStdItem* gdStdModel::realItem(int row, int column) const
{
  if ( !m_proxy ) return dynamic_cast<gdStdItem*>(WStandardItemModel::item(row, column));
  WModelIndex idxProxy  = m_proxy->index(row, column);
  WModelIndex idxSource = m_proxy->mapToSource(idxProxy);
  return dynamic_cast<gdStdItem*>(WStandardItemModel::itemFromIndex(idxSource));
}

int gdStdModel::realRowCount() const
{
  if ( !m_proxy )
    return WStandardItemModel::rowCount();
  return m_proxy->rowCount();
}

int gdStdModel::rowCountView()
{
  return realRowCount() - isFirstRowReserved() - isLastRowReserved();
}

int gdStdModel::firstRow()
{
  return isFirstRowReserved();
}

int gdStdModel::lastRow()
{
  return realRowCount() - isLastRowReserved();
}

void gdStdModel::setColumnType(int column, stdTypes type)
{
  for (int i = m_vType.size() - 1; i < column; i++)
    m_vType.push_back(type_unknown);
  m_vType[column] = type;
}

gdStdModel::stdTypes gdStdModel::columnType(int column)
{
  if ( column >= m_vType.size() ) return type_unknown;
  return m_vType[column];
}

bool gdStdModel::isColumnNumeric(int column)
{
  if ( column >= m_vType.size() ) return false;
  if ( m_vType[column] == type_int4   ||
       m_vType[column] == type_int8   ||
       m_vType[column] == type_double )
    return true;
  return false;
}

void gdStdModel::onCheckChanged(gdStdItem* item, Wt::CheckState chkst)
{
  if ( m_bDebug )
    wApp->log("notice") <<  "gdStdModel::onCheckChanged item = " << item;
  doCheckChanged(item, chkst);
  //  if ( !m_bOnCheckChanged )
  //     checkStateChanged_.emit(item, chkst);
}

void gdStdModel::doCheckChanged(gdStdItem* item, Wt::CheckState chkst)
{
  if ( m_bDebug )
    wApp->log("notice") <<  "gdStdModel::doCheckChanged item = " << item << " invisibleRootItem = " << invisibleRootItem() << " OnCheck = " << m_bOnCheckChanged;
  //  gdStdItem* pItem = m_modelTree->itemFromIndex(item->index());
  if ( !item ) return;
  gdStdItem* pItem = item;
  if ( pItem == invisibleRootItem() ) return;
  if ( m_bOnCheckChanged ) return;
  m_bOnCheckChanged = true;
  if ( m_bDebug )
    wApp->log("notice") << "gdStdModel::doCheckChanged running";
  switch (chkst) {
  case Wt::Unchecked :
    uncheckChildrens(pItem);
    if ( pItem->parent() )
      setCheckParents(pItem->parent());
    break;
  case Wt::PartiallyChecked : // Not posssible
    wApp->log("notice") <<  "gdStdModel::doCheckChanged() abnormal Wt::PartiallyChecked";
    break;
  case Wt::Checked :
    checkChildrens(pItem);
    if ( pItem->parent() )
      setCheckParents(pItem->parent());
    break;
  }
  pItem->checkItem(chkst);
  m_bOnCheckChanged = false;
  if ( m_bDebug )
    wApp->log("notice") <<  "gdCatTvAllCodes::doCheckChanged finished";
}

void gdStdModel::checkChildrens(WStandardItem* pParent)
{
  if ( m_bDebug )
    wApp->log("notice") <<  "gdStdModel::checkChildrens start";
  WStandardItem* pItem = pParent;
  if ( pItem->hasChildren() ) {
    for (int row = 0; row < pItem->rowCount(); ++row) {
      WStandardItem* wsi = pItem->child(row, 0);
      if ( !wsi ) continue;
      wsi->setCheckState(Wt::Checked);
      gdStdItem*    gdwsi = dynamic_cast<gdStdItem*>(wsi);
      if ( gdwsi )
        gdwsi->checkItem(Wt::Checked);
      checkChildrens(wsi);
    }
  }
  if ( m_bDebug )
    wApp->log("notice") <<  "gdStdModel::checkChildrens stop";
}

void gdStdModel::uncheckChildrens(WStandardItem* pParent)
{
  if ( m_bDebug )
    wApp->log("notice") << "gdStdModel::unCheckChildrens start this = " << this << " proxy = " << m_proxy << " pParent model = " << pParent->model();
  WStandardItem* pItem = pParent;
  if ( pItem->hasChildren() ) {
    int start = 0;
    int stop =  pItem->rowCount();
    if ( pItem == invisibleRootItem() ) {
      if ( isFirstRowReserved() ) ++start;
      if ( isLastRowReserved()  ) --stop;
    }
    for (int row = start; row < stop; ++row) {
      WStandardItem* wsi = pItem->child(row, 0);
      if ( !wsi ) continue;
      wsi->setCheckState(Wt::Unchecked);
      gdStdItem*    gdwsi = dynamic_cast<gdStdItem*>(wsi);
      if ( gdwsi )
        gdwsi->checkItem(Wt::Unchecked);
      uncheckChildrens(wsi);
    }
  }
  if ( m_bDebug )
    wApp->log("notice") <<  "gdStdModel::unCheckChildrens stop";
}

void gdStdModel::setCheckParents(WStandardItem* item)
{
  WStandardItem* pItem = item;
  if ( m_bDebug )
    wApp->log("notice") <<  "gdStdModel::setCheckParents start";
  if ( pItem == invisibleRootItem() ) return;
  Wt::CheckState chkst = Wt::Unchecked;
  for (int row = 0; row < pItem->rowCount(); ++row) {
    WStandardItem* wsi = pItem->child(row, 0);
    if ( wsi->checkState() == chkst ) continue;
    if ( wsi->checkState() == Wt::PartiallyChecked ) {
      chkst = Wt::PartiallyChecked;
      break;
    }
    if ( wsi->checkState() == Wt::Checked          && chkst ==  Wt::Unchecked        ) {
      if ( !row ) {
        chkst = Wt::Checked;
        continue;
      } else {
        chkst = Wt::PartiallyChecked;
        break;
      }
    }
    if ( wsi->checkState() == Wt::Unchecked        && chkst ==  Wt::Checked        ) {
      chkst =  Wt::PartiallyChecked;
      break;
    }
  }
  pItem->setCheckState(chkst);
  gdStdItem*    gdwsi = dynamic_cast<gdStdItem*>(pItem);
  if ( gdwsi )
    gdwsi->checkItem(chkst);
  if ( pItem->parent() )
    setCheckParents(pItem->parent());
  if ( m_bDebug )
    wApp->log("notice") <<  "gdStdModel::setCheckParents stopt";
}

const std::vector<gdStdItem*>& gdStdModel::vItemsSelected(const int nColumn)
{
  m_vItemsSelected.clear();
  populate(invisibleRootItem(), m_vItemsSelected, nColumn, false);
  return m_vItemsSelected;
}

const std::vector<gdStdItem*>& gdStdModel::vItemsChecked(const int nColumn)
{
  m_vItemsChecked.clear();
  populate(invisibleRootItem(), m_vItemsChecked, nColumn, true);
  return m_vItemsChecked;
}

void gdStdModel::populate(WStandardItem* rootItem, std::vector<gdStdItem*>& vResult, const int nColumn, bool bChecked)
{
  for (int iRow = 0; iRow < rootItem->rowCount(); ++iRow) {
    WStandardItem* pItemTree = rootItem->child(iRow, 0);
    if ( bChecked ) {
      gdStdItem* pItemCheck = dynamic_cast<gdStdItem*>(rootItem->child(iRow, nColumn));
      if ( pItemTree->checkState() == Wt::Checked )
        vResult.push_back(pItemCheck);
    } else {
      // TODO : Implement a bool in gdStdItem to take the view selection
    }
    if ( pItemTree->hasChildren() )
      populate(pItemTree, vResult, nColumn, bChecked);
  }
}
