/*
 * Copyright (C) 2006-Today Guy Deleeuw
 *
 * See the LICENSE file for terms of use.
 */

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/replace.hpp>

#include <Wt/WApplication>

#include "gdStdItem.h"

using namespace Wt;
// ========== gdStdItem ===============

gdStdItem::gdStdItem() : WStandardItem(), m_bChecked(false), m_bDebug(false)
{ }

gdStdItem::gdStdItem(const WString& text) : WStandardItem(text), m_bChecked(false), m_bDebug(false)
{ }

void gdStdItem::setDebugging(bool bDebug)
{
  m_bDebug = bDebug;
}

bool gdStdItem::debugging()
{
  return m_bDebug;
}

void gdStdItem::setData(const boost::any& data, int role)
{
  if ( role == Wt::CheckStateRole ) {
    if ( m_bDebug )
      wApp->log("notice") <<  "gdStdItem::setData = " << text();
    Wt::CheckState retState = Wt::Unchecked;
    if ( isTristate() )
      retState = boost::any_cast<Wt::CheckState>(data);
    else {
      bool chkst = boost::any_cast<bool>(data);
      if ( chkst ) retState = Wt::Checked;
    }
    WStandardItem::setData(data, role);
    checkStateChanged_.emit(this, retState);
    return;
  }
  WStandardItem::setData(data, role);
}

void gdStdItem::checkItem(Wt::CheckState state)
{
  if ( m_bDebug )
    wApp->log("notice") <<  "gdStdItem::checkItem = " << text();
  gdStdItem*       pDim = 0;
  try {
    pDim = boost::any_cast<gdStdItem*>(this->data());
  } catch (boost::bad_any_cast& error) {}
  if ( pDim )
    if ( state == Wt::Unchecked ) pDim->m_bChecked = false;
    else                         pDim->m_bChecked = true;
}

bool gdStdItem::isItemChecked()
{
  return m_bChecked;
}

gdStdItem* gdStdItem::parent() const
{
  return dynamic_cast<gdStdItem*>(WStandardItem::parent());
}

