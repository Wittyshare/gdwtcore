/*
 * Copyright (C) 2006-Today Guy Deleeuw
 *
 * See the LICENSE file for terms of use.
 */

#include <boost/lexical_cast.hpp>
//#include <boost/algorithm/string/replace.hpp>

#include <Wt/WApplication>
#include <Wt/WRegExp>
#include <Wt/WTreeView>

#include <gdcore/gdCore.h>

#include "gdProxyModel.h"

using namespace Wt;

gdProxyModel::gdProxyModel(WObject* parent)
 : WSortFilterProxyModel(parent), m_parent(parent), m_pSourceModel(0), m_defaultFilter(".*"), m_bOp(false)
 { }

gdProxyModel::~gdProxyModel()
 { wApp->log("notice") <<  "gdProxyModel::~gdProxyModel()"; }

void gdProxyModel::setSourceModel(gdStdModel* pModel)
 {
  m_pSourceModel = pModel;
  WSortFilterProxyModel::setSourceModel(pModel);
  setFilterRegExp(m_defaultFilter);
  setSortRole(UserRole);
  setDynamicSortFilter(true);
  sourceModel()->itemChanged().connect(SLOT(this, gdProxyModel::itemChanged));
 }

gdStdModel* gdProxyModel::sourceModel() const
 { return m_pSourceModel; }

void gdProxyModel::setDefaultFilter(const std::string& filter)
 { m_defaultFilter = filter; }

std::string gdProxyModel::defaultFilter() const
 { return m_defaultFilter; }

void gdProxyModel::setAllChildsFiltering(const bool bAllChildsFiltering)
 { m_bAllChildsFiltering = bAllChildsFiltering; }

bool gdProxyModel::allChildsFiltering() const
 { return m_bAllChildsFiltering; }

void gdProxyModel::itemChanged(WStandardItem* item)
 {
  if ( !sourceModel()->isFirstRowReserved() ) return;
  if ( m_bOp ) return;
  WModelIndex idx = item->index();
  if ( idx.row()    != 0 ) return;
  m_bOp = true;
  setFilterRegExp(m_defaultFilter);
  m_bOp = false;
 }

bool gdProxyModel::filterAcceptRow(int sourceRow, const WModelIndex &sourceParent) const
 {
  WStandardItem* pItem = sourceModel()->itemFromIndexModel(sourceParent);
  if ( !pItem ) {
//     wApp->log("notice") << "gdProxyModel::filterAcceptRow() pItem is NULL it is normal ? : sourceRow = "
//                         << sourceRow << " invisible item = " << sourceModel()->invisibleRootItem();
     pItem =  sourceModel()->invisibleRootItem();
  }
  if ( sourceModel()->isFirstRowReserved() && pItem == sourceModel()->invisibleRootItem() )
     if ( sourceRow == 0 ) return true;
  if ( sourceModel()->isLastRowReserved() && pItem == sourceModel()->invisibleRootItem() )
     if ( sourceRow == sourceModel()->rowCount() - 1 ) return true;

  bool               bAccept = true;
  WRegExp            regex;
  for(unsigned nColumn = 0; nColumn < sourceModel()->columnCount(); ++nColumn) {
    WStandardItem* pISearch = sourceModel()->item(0, nColumn);
    if ( pISearch->flags() & Wt::ItemIsEditable ) {
       WString filter = pISearch->text();
       if ( !filter.empty() ) regex.setPattern(".*" + filter + ".*", filterFlags());
        else                  regex.setPattern(".*", filterFlags());
       WStandardItem* pItem2 = pItem->child(sourceRow, nColumn);
       // Filtrage des enfants : colonne 0 uniquement, si il y a des enfants et si le parent est de type WTreeView
       if ( !nColumn && pItem2->hasChildren() && dynamic_cast<WTreeView*>(m_parent) ) {
             WTreeView* wtv = dynamic_cast<WTreeView*>(m_parent);
             bool bExpanded = false;
             // Ne peut pas fonctionner car la view a le proxy comme modèle
//             if ( wtv ) {
//                WModelIndex pIdx = mapFromSource(pItem2->index());
//                bExpanded = wtv->isExpanded(pIdx);
//               }
             for(int nChild = 0; nChild < pItem2->rowCount(); ++nChild) {
               // Si pas de filtrage complet et que l'element n'est pas expand on ne fait rien.
               if ( !allChildsFiltering() && !bExpanded ) continue;
               WStandardItem* pCh = pItem2->child(nChild, nColumn);
               if ( filterAcceptRow(pCh->index().row(), pItem2->index()) )
                  return true;
             }
          }
       WString wsLabel = pItem2->text();
       bAccept = regex.exactMatch(wsLabel);
       if ( !bAccept ) break;
      }
   }
  return bAccept;
 }

bool gdProxyModel::lessThan(const WModelIndex& lhs, const WModelIndex& rhs) const
 {
//  fprintf(stderr, "lhs.row() = %d, rhs.row() = %d, order = %d\n", lhs.row(), rhs.row(), sortOrder());
  if ( sortOrder() == Wt::AscendingOrder ) {
     if ( sourceModel()->isFirstRowReserved() )
        if ( rhs.row() == 0 )                             return false; // First row stay on row 0 (for regex pattern)
     if ( sourceModel()->isLastRowReserved() )
        if ( lhs.row() == sourceModel()->rowCount() - 1 ) return false; // Last  row stay on last (for sum)  
    } else {
     if ( sourceModel()->isFirstRowReserved() )
        if ( lhs.row() == 0 )                             return false; // First row stay on row 0 (for regex pattern)
     if ( sourceModel()->isLastRowReserved() )
        if ( rhs.row() == sourceModel()->rowCount() - 1 ) return false; // Last  row stay on last (for column sum)  
    }
  return WSortFilterProxyModel::lessThan(lhs, rhs);
 }

