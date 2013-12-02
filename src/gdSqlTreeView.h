/*
 * Copyright (C) 2006-Today Guy Deleeuw
 *
 * See the LICENSE file for terms of use.
 */

#ifndef GD_gdSqlTreeView_H__
#define GD_gdSqlTreeView_H__

#include <Wt/WContainerWidget>
#include <Wt/WTreeView>

#include "gdSqlModel.h"
#include "gdProxyModel.h"

class gdSqlTreeView : public Wt::WTreeView
 {
  public :
               gdSqlTreeView(const std::string& pQuery, gdCoreSql* pSql, bool bDestroy = true, Wt::WContainerWidget* parent = 0);
              ~gdSqlTreeView();
   void        load();
   void        loadModel(const std::string& pQuery);
   void        loadModel();
   int         getCount();
   void        removeAllRows();
   gdSqlModel* model() const;
  
  public : // Signals
   Wt::Signal<void>& modelReLoaded();

  private slots :

  private :
   Wt::Signal<void>           modelReLoaded_;
   gdSqlModel*                m_model;
   gdProxyModel*              m_proxy;
   bool                       m_bDestroy;
 };

#endif // ifdef GD_gdSqlTreeView_H__

