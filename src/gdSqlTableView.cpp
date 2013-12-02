/*
 * Copyright (C) 2006-Today Guy Deleeuw
 *
 * See the LICENSE file for terms of use.
 */

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/replace.hpp>

#include <Wt/WItemDelegate>

#include "gdSqlTableView.h"

using namespace Wt;

gdSqlTableView::gdSqlTableView(const std::string& pQuery, gdCoreSql* pSql, bool bDestroy, WContainerWidget* parent)
 : WTableView(parent), m_model(0), m_proxy(0), m_bDestroy(bDestroy)
 {
  m_model = new gdSqlModel();
  m_model->setSql(pSql, bDestroy);
  m_model->setQuery(pQuery);
 }

gdSqlTableView::~gdSqlTableView()
 {
  if ( !m_bDestroy ) return;
  delete m_model;
  m_model = 0;
 }

void gdSqlTableView::load()
 {
  setAlternatingRowColors(true);
  setSelectionMode(SingleSelection);
  if ( m_model->isFirstRowReserved() )
    {
     m_proxy = new gdProxyModel(this);
     m_proxy->setSourceModel(m_model);
     m_model->setProxy(m_proxy);
     setModel(m_proxy);
     setEditTriggers(Wt::WAbstractItemView::SingleClicked);
     setEditOptions(Wt::WAbstractItemView::SingleEditor);
     WItemDelegate *delegate = new WItemDelegate(this);
     delegate->setTextFormat("%s");
     setItemDelegate(delegate);
    }
   else
     setModel(m_model);

  loadModel();

  for(int col = 0; col < m_model->columnCount(); col++)
   {
    setHeaderAlignment(col, AlignLeft);
    if ( m_model->isColumnNumeric(col))
       setColumnAlignment(col, AlignRight);
     else
       setColumnAlignment(col, AlignLeft);
    setSortingEnabled(col, true );
    setColumnWidth(col, m_model->m_vColumnsWidth[col]);
   }

  WTableView::load();
 }

void gdSqlTableView::loadModel(const std::string& pQuery)
 {
  m_model->setQuery(pQuery);
  loadModel();
 }

void gdSqlTableView::loadModel()
 {
  m_model->reload();
  modelReLoaded_.emit();
 }

int gdSqlTableView::getCount()
 { return m_model->invisibleRootItem()->rowCount(); }

void gdSqlTableView::removeAllRows()
 { m_model->removeAllRows(); }

gdSqlModel* gdSqlTableView::model() const
 { return m_model; }

Signal<void>& gdSqlTableView::modelReLoaded()
 { return modelReLoaded_; }

