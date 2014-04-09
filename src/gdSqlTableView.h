/*
 * Copyright (C) 2006-Today Guy Deleeuw
 *
 * See the LICENSE file for terms of use.
 */

#ifndef GD_gdSqlTableView_H__
#define GD_gdSqlTableView_H__

#include <Wt/WContainerWidget>
#include <Wt/WTableView>

#include "gdSqlModel.h"
#include "gdProxyModel.h"

class gdSqlTableView : public Wt::WTableView {
public :
  gdSqlTableView(const std::string& pQuery, gdCoreSql* pSql, bool bDestroy = true, Wt::WContainerWidget* parent = 0);
  ~gdSqlTableView();
  void        load();
  void        loadModel(const std::string& pQuery);
  void        loadModel();
  int         getCount();
  void        removeAllRows();
  gdSqlModel* model() const;

public : // Signals
  Wt::Signal<void>& modelReLoaded();

private :
  Wt::Signal<void>           modelReLoaded_;
  gdSqlModel*                m_model;
  gdProxyModel*              m_proxy;
  bool                       m_bDestroy;
};

#endif // ifdef GD_gdSqlTableView_H__

