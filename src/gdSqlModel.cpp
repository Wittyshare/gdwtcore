/*
 * Copyright (C) 2006-Today Guy Deleeuw
 *
 * See the LICENSE file for terms of use.
 */

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/replace.hpp>

#include <Wt/WString>

#include <gdcore/gdCore.h>

#include "gdSqlModel.h"

using namespace Wt;
// ========== gdStandardItemSql ===============

gdStandardItemSql::gdStandardItemSql(const WString& text) : gdStdItem(text)
{ }




gdSqlModel::gdSqlModel()
  : gdStdModel(), m_oSql(0), m_bDestroySql(false)
{ }

gdSqlModel::~gdSqlModel()
{
  if ( m_bDestroySql )
    if ( m_oSql )
      delete m_oSql;
  m_bDestroySql = false;
  m_oSql = 0;
}

void gdSqlModel::setSql(gdCoreSql* refSql, bool bDestroy)
{
  m_oSql = refSql;
  m_bDestroySql = bDestroy;
}

void gdSqlModel::setQuery(const std::string& pQuery)
{
  m_pQuery = pQuery;
}

std::string gdSqlModel::query()
{
  return m_pQuery;
}

void gdSqlModel::loadHeader()
{
  m_vColumnsDBNames.clear();
  m_vColumnsNames.clear();
  m_vColumnsWidth.clear();
  if ( !m_oSql ) return;
  if ( !m_oSql->getColumnCount() ) return;
  insertColumns(0, m_oSql->getColumnCount());
  for (int col = 0; col < columnCount(); col++) {
    std::string colHeader = m_oSql->getColumnName(col);
    boost::algorithm::replace_all(colHeader, "$", ".");
    m_vColumnsDBNames.push_back(WString::fromUTF8(colHeader));
    int colWidth = atoi(WString::tr(m_vColumnsDBNames[col].toUTF8() + "_width").narrow().c_str());
    if ( !colWidth ) colWidth = 120;
    m_vColumnsWidth.push_back(colWidth);
    std::string strColName = WString::tr(m_vColumnsDBNames[col].toUTF8()).toUTF8();
    boost::algorithm::replace_all(strColName, "?", "");
    m_vColumnsNames.push_back(WString::fromUTF8(strColName));
    setColumnType(col, m_oSql->getColumnType(col));
    //   fprintf(stderr, "col = %d, colType = %ld colWidth = %d, colDBName = %s, colName = %s\n",
    //                    col, columnType(col), m_vColumnsWidth[col], m_vColumnsDBNames[col].c_str(), m_vColumnsNames[col].c_str());
    if ( isHeaderUsed() )
      setHeaderData(col, Wt::Horizontal, m_vColumnsNames[col]);
  }
  // Ajout d'une ligne ou chaque colonne contient un WString qui servira de filtre de recherche
  if ( isFirstRowReserved() ) {
    std::vector<WStandardItem*> pRow;
    for (int col = 0; col < columnCount(); col++) {
      gdStandardItemSql* pItem = new gdStandardItemSql(WString());
      pItem->setFlags(ItemIsSelectable | ItemIsEditable);
      pRow.push_back(pItem);
    }
    invisibleRootItem()->appendRow(pRow);
  }
  gdStdModel::loadHeader();
}

void gdSqlModel::reload()
{
  removeAllRows();
  if ( !m_oSql ) return;
  //  m_oSql->setDebugging(true);
  int retSql = m_oSql->execQuery(m_pQuery.c_str());
  if ( retSql ) {
    if ( !isHeaderLoaded() )
      loadHeader();
    WStandardItem* rootTree = invisibleRootItem();
    for (int row = 0; row < m_oSql->getRowCount(); row++) {
      std::vector<WStandardItem*> pRow;
      for (int col = 0; col < columnCount(); col++) {
        gdStandardItemSql* pItem = new gdStandardItemSql(WString());
        if ( !col && isCol0Checkable() ) {
          pItem->setCheckable(true);
          pItem->checkStateChanged().connect(SLOT(this, gdSqlModel::onCheckChanged));
        }
        switch (columnType(col)) {
        case type_int4 :
        case type_int8 :
          pItem->setData(m_oSql->getValueAsLong(row, col), UserRole);
          break;
        case type_double :
          pItem->setData(m_oSql->getValueAsDouble(row, col), UserRole);
          break;
        case type_text :
        case type_date :
        default :
          pItem->setData(WString::fromUTF8(m_oSql->getValueAsStr(row, col)), UserRole);
        }
        pItem->setText(WString::fromUTF8(m_oSql->getValueAsStr(row, col)));
        pRow.push_back(pItem);
      }
      rootTree->appendRow(pRow);
    }
    m_oSql->releaseQuery();
  }
}

WString gdSqlModel::getHeaderDB(int column)
{
  if ( column < m_vColumnsDBNames.size() )
    return m_vColumnsDBNames[column];
  else
    return "";
}

int gdSqlModel::columnNameIndex(const WString& name)
{
  for (int idx = 0; idx < m_vColumnsDBNames.size(); idx++) {
    if ( name == m_vColumnsDBNames[idx] ) return idx;
  }
  return -1;
}

void gdSqlModel::setColumnType(int column, gdCoreSql::sqlTypes type)
{
  if ( type == gdCoreSql::sqlType_int8    ) return gdStdModel::setColumnType(column, type_int8);
  if ( type == gdCoreSql::sqlType_int4    ) return gdStdModel::setColumnType(column, type_int4);
  if ( type == gdCoreSql::sqlType_text    ) return gdStdModel::setColumnType(column, type_text);
  if ( type == gdCoreSql::sqlType_date    ) return gdStdModel::setColumnType(column, type_date);
  if ( type == gdCoreSql::sqlType_numeric ) return gdStdModel::setColumnType(column, type_double);
  return gdStdModel::setColumnType(column, type_unknown);
}

