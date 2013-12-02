/*
 * Copyright (C) 2006-Today Guy Deleeuw
 *
 * See the LICENSE file for terms of use.
*/

#ifndef __gdSqlContainerBar_H__
#define __gdSqlContainerBar_H__

/*! \file gdSqlContainerBar.h */

#include <Wt/WContainerWidget>
#include <Wt/WPanel>
#include <Wt/WTreeView>

#include "gdToolbar.h"
#include "gdContainerBar.h"
#include "gdSqlTreeView.h"
#include "gdSqlTableView.h"

class gdColumnSelection
 {
  public :
   int                       columnIndex;
   std::vector<std::string>  strSelection;
   std::string               strCondition;
 };

/*! \class gdSqlContainerBar
*/

class gdSqlContainerBar : public gdContainerBar
 {
  public :
  /*! \brief Constructor.

  */
                   gdSqlContainerBar(Wt::WContainerWidget* parent = NULL);
                   gdSqlContainerBar(gdSqlTableView* tableview, std::vector<int> *vColSelection = 0, Wt::WContainerWidget* parent = NULL);
                   gdSqlContainerBar(gdSqlTreeView*  treeview,  std::vector<int> *vColSelection = 0, Wt::WContainerWidget* parent = NULL);

   void            setView(gdSqlTreeView*  treeview,  std::vector<int> *vColSelection = 0);
   void            setView(gdSqlTableView* tableview, std::vector<int> *vColSelection = 0);
   void            initView(std::vector<int> *vColSelection = 0);
   gdSqlTreeView*  treeView();
   gdSqlTableView* tableView();
   gdSqlModel*     model();
   void            addTVSelectRows(std::vector<int> *vColSelection);
   void            setTable(std::string tableName);// for rowRemoved from model

  public slots :
   void      doModelReloaded();
   void      doUpdateSelection(Wt::WStandardItem* pItem);
   void      doReload();
   void      doRowRemoved(long row);
   void      doModelRowCountChanged();

  private :
   gdSqlTreeView*                  m_pSqlTreeView;
   gdSqlTableView*                 m_pSqlTableView;
   Wt::WAbstractItemView*          m_pView;
   gdSqlModel*                     m_pSqlModel;
   Wt::WStandardItemModel*         m_pModelSelectedRows;
   Wt::WTreeView*                  m_pTVSelectRows;
   std::string                     m_strQueryCondition;
   std::vector<gdColumnSelection*> m_vColSel;
   std::string                     m_strTableName;
 };

#endif // ifdef __gdSqlContainerBar_H__


