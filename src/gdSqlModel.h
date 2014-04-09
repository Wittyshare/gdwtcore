/*
 * Copyright (C) 2006-Today Guy Deleeuw
 *
 * See the LICENSE file for terms of use.
 */

#ifndef GD_gdSqlModel_H__
#define GD_gdSqlModel_H__

#include <Wt/WStandardItemModel>
#include <Wt/WStandardItem>
#include <Wt/WString>

#include <gdcore/gdCoreSql.h>
#include "gdStdModel.h"

/*!
    \class gdStandardItemSql
    \brief Reimplement gdStdItem
*/
class gdStandardItemSql : public gdStdItem {
public :
  gdStandardItemSql(const Wt::WString& text);
};






/*!
    \class gdSqlModel
    \brief Reimplement gdStdModel
*/
class gdSqlModel : public gdStdModel {
public :
  gdSqlModel();
  ~gdSqlModel();
  void                            setSql(gdCoreSql* refSql, bool bDestroy = false);
  void                            setQuery(const std::string& pQuery);
  std::string                     query();
  void                            loadHeader();
  void                            reload();
  Wt::WString                     getHeaderDB(int column);
  int                             columnNameIndex(const Wt::WString& name);
  void                            setColumnType(int column, gdCoreSql::sqlTypes type);

public :
  std::vector<Wt::WString>        m_vColumnsDBNames;
  std::vector<Wt::WString>        m_vColumnsNames;
  gdCoreSql*                      m_oSql;

private :
  std::string                     m_pQuery;
  bool                            m_bDestroySql;
};

#endif // ifdef GD_gdSqlModel_H__

