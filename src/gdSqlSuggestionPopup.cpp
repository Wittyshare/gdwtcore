/*
 * Copyright (C) 2006-Today Guy Deleeuw
 *
 * See the LICENSE file for terms of use.
 */

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/replace.hpp>

#include <Wt/WApplication>

#include "gdSqlSuggestionPopup.h"

using namespace Wt;

gdSqlSuggestionPopup::gdSqlSuggestionPopup(gdCoreSql* refSql, std::string strQuery, int column, WContainerWidget* parent)
 : WContainerWidget(parent), m_strQuery(strQuery), m_pSqlModel(0), m_nIndex(0), m_nColumn(column)
 {
  wApp->styleSheet().addRule(".highlight", "color: red");

  m_pSqlModel = new gdSqlModel();
  m_pSqlModel->setSql(refSql, true);
  loadModel();

  m_pFilterModel = new WSortFilterProxyModel();
  m_pFilterModel->setSourceModel(m_pSqlModel);
  m_pFilterModel->setFilterKeyColumn(column);
  m_pFilterModel->setFilterRole(DisplayRole);
  m_pFilterModel->setFilterFlags(MatchCaseInsensitive);

  m_leSearch = new WLineEdit(this);
  m_leSearch->setTextSize(60);

  WSuggestionPopup::Options optSpCode = { "<span class=\"highlight\">", "</span>", 0, "", " ", "" };

  m_pSuggestionPopup = new WSuggestionPopup(WSuggestionPopup::generateMatcherJS(optSpCode), WSuggestionPopup::generateReplacerJS(optSpCode), this);
  m_pSuggestionPopup->setModel(m_pFilterModel);
  m_pSuggestionPopup->setModelColumn(column);
  m_pSuggestionPopup->forEdit(m_leSearch, WSuggestionPopup::DropDownIcon | WSuggestionPopup::Editing);
  m_pSuggestionPopup->setFilterLength(1);
  m_pSuggestionPopup->filterModel().connect(this, &gdSqlSuggestionPopup::filter);
  m_pSuggestionPopup->setMinimumSize(400,  25);
  m_pSuggestionPopup->setMaximumSize(400, 250);
//  m_pSuggestionPopup->setGlobalPopup(true);
  m_pSuggestionPopup->activated().connect(SLOT(this, gdSqlSuggestionPopup::doChanged));
 }

void gdSqlSuggestionPopup::filter(const WString& strFilter)
 {
  if ( strFilter.empty() )
     m_pFilterModel->setFilterRegExp(".*");
   else
     m_pFilterModel->setFilterRegExp("^.*" + strFilter + ".*");
 }

void gdSqlSuggestionPopup::doChanged(int index, WFormWidget* search)
 {
  if ( index >= 0 )
    {
     WModelIndex idxFilter = m_pFilterModel->index(index, 0);
     WModelIndex idxModel  = m_pFilterModel->mapToSource(idxFilter);
     m_nIndex = idxModel.row();
    }
   else m_nIndex = index;
  s_changed_.emit(m_nIndex);
 }

gdSqlModel* gdSqlSuggestionPopup::model()
 {
  return m_pSqlModel;
 }

std::string gdSqlSuggestionPopup::value()
 {
  return m_leSearch->text().toUTF8();
 }

void gdSqlSuggestionPopup::setCurrentIndex(int index)
 {
  m_nIndex = index;
  m_pSuggestionPopup->setDefaultIndex(m_nIndex);
  WString wstr;
  if ( index < 0 || index >= m_pSqlModel->rowCount() )
     {
      m_pFilterModel->setFilterRegExp("");
      wstr = "";
     }
   else
      wstr = asString(m_pSqlModel->item(m_nIndex, m_nColumn)->data());
  m_leSearch->setText(wstr);
 }

int gdSqlSuggestionPopup::currentIndex()
 {
  return m_nIndex;
 }

void gdSqlSuggestionPopup::setQuery(const std::string& query)
 {
  m_strQuery = query;
 }

void gdSqlSuggestionPopup::loadModel()
 {
  m_pSqlModel->setQuery(m_strQuery);
  m_pSqlModel->reload();
 }

int gdSqlSuggestionPopup::getSelectionID()
 {
  WStandardItem*          rootTree = m_pSqlModel->invisibleRootItem();
  int                     curIdx   = currentIndex();
  if ( curIdx == -1 ) return 0;
  boost::any              idxID    = rootTree->child(curIdx, 0)->data(UserRole);
  long                    retval   = boost::any_cast<long>(idxID);
   
  return retval;
 }

void gdSqlSuggestionPopup::setSelectionID(int selID)
 {
  WStandardItem*          rootTree = m_pSqlModel->invisibleRootItem();
  for(int nCount = 0; nCount < rootTree->rowCount(); nCount++)
   {
    boost::any              idxID    = rootTree->child(nCount, 0)->data(UserRole);
    long                    k_id     = boost::any_cast<long>(idxID);
    if ( k_id != selID ) continue;
    setCurrentIndex(nCount);
    break;
   }
 }

