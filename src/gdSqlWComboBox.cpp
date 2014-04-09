/*
 * Copyright (C) 2006-Today Guy Deleeuw
 *
 * See the LICENSE file for terms of use.
 */

#include <boost/lexical_cast.hpp>

#include "gdSqlWComboBox.h"

using namespace Wt;

gdSqlWComboBox::gdSqlWComboBox(int modelColumn, WContainerWidget* parent): WComboBox(parent), m_model(0)
{
  m_model = new gdSqlModel();
  setModel(m_model);
  setModelColumn(modelColumn);
}

int gdSqlWComboBox::getSelectionID()
{
  WStandardItem*          rootTree = m_model->invisibleRootItem();
  int                     curIdx   = currentIndex();
  boost::any              idxID    = rootTree->child(curIdx, 0)->data(UserRole);
  long                    retval = boost::any_cast<long>(idxID);
  return retval;
}

void gdSqlWComboBox::setSelectionID(int selID)
{
  WStandardItem*          rootTree = m_model->invisibleRootItem();
  for (int nCount = 0; nCount < rootTree->rowCount(); nCount++) {
    boost::any              idxID    = rootTree->child(nCount, 0)->data(UserRole);
    long                    k_id     = boost::any_cast<long>(idxID);
    if ( k_id != selID ) continue;
    setCurrentIndex(nCount);
    break;
  }
}

