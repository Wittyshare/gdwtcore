/*
 * Copyright (C) 2006-Today Guy Deleeuw
 *
 * See the LICENSE file for terms of use.
*/

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/replace.hpp>

#include <Wt/WVBoxLayout>
#include <Wt/WAnchor>

#include <gdcore/gdCoreSql.h>

#include "gdSqlContainerBar.h"
#include "gdComboBoxDelegate.h"
#include "gdTreeViewPdfRes.h"

using namespace Wt;

gdSqlContainerBar::gdSqlContainerBar(WContainerWidget* parent)
  : gdContainerBar(0, parent), m_pTVSelectRows(0), m_pSqlTreeView(0), m_pSqlTableView(0), m_pSqlModel(0)
{
  addStyleClass("gdsqlcontbar");
}

gdSqlContainerBar::gdSqlContainerBar(gdSqlTreeView* treeview, std::vector<int> *vColSelection, WContainerWidget* parent)
  : gdContainerBar(0, parent), m_pTVSelectRows(0), m_pSqlTreeView(treeview), m_pSqlTableView(0), m_pSqlModel(0)
{
  addStyleClass("gdsqlcontbar");
  if ( treeview )
    setView(treeview, vColSelection);
}

gdSqlContainerBar::gdSqlContainerBar(gdSqlTableView* tableview, std::vector<int> *vColSelection, WContainerWidget* parent)
  : gdContainerBar(0, parent), m_pTVSelectRows(0), m_pSqlTableView(tableview), m_pSqlTreeView(0), m_pSqlModel(0)
{
  addStyleClass("gdsqlcontbar");
  if ( tableview )
    setView(tableview, vColSelection);
}

void gdSqlContainerBar::setView(gdSqlTreeView* treeview, std::vector<int> *vColSelection)
{
  if ( m_pSqlTableView ) delete m_pSqlTableView;
  m_pSqlTableView = 0;
  if ( m_pSqlTreeView  && m_pSqlTreeView != treeview ) delete m_pSqlTreeView;
  m_pSqlTreeView = treeview;
  m_pView        = treeview;
  m_pSqlModel    = treeview->model();
  m_pSqlTreeView->modelReLoaded().connect(SLOT(this, gdSqlContainerBar::doModelReloaded));
  initView(vColSelection);
}

void gdSqlContainerBar::setView(gdSqlTableView* tableview, std::vector<int> *vColSelection)
{
  if ( m_pSqlTreeView ) delete m_pSqlTreeView;
  m_pSqlTreeView = 0;
  if ( m_pSqlTableView && m_pSqlTableView != tableview ) delete m_pSqlTableView;
  m_pSqlTableView = tableview;
  m_pView         = tableview;
  m_pSqlModel     = tableview->model();
  m_pSqlTableView->modelReLoaded().connect(SLOT(this, gdSqlContainerBar::doModelReloaded));
  initView(vColSelection);
}

void gdSqlContainerBar::initView(std::vector<int> *vColSelection)
{
  gdContainerBar::setFirstRow((long) model()->isFirstRowReserved());
  gdContainerBar::setView(m_pView);
  makeToolbar();
  if ( model()->isCol0Checkable() )
    hideCheckBox(false);
  hideReload(false);
  m_tbiReload->clicked().connect(SLOT(this, gdSqlContainerBar::doReload));
  addTVSelectRows(vColSelection);
  WVBoxLayout*       vbox = new WVBoxLayout();
  if ( !panel() || !panel()->titleBar() )
    vbox->addWidget(m_pToolBar,  0);
  if ( m_pTVSelectRows )
    vbox->addWidget(m_pTVSelectRows, 0);
  vbox->addWidget(m_pView, 1);
  resize(WLength(100, WLength::Percentage), WLength(100, WLength::Percentage));
  setOverflow(WContainerWidget::OverflowHidden);
  setLayout(vbox);
  model()->rowsInserted().connect(SLOT(this, gdSqlContainerBar::doModelRowCountChanged));
  model()->rowsRemoved() .connect(SLOT(this, gdSqlContainerBar::doModelRowCountChanged));
  rowDeleted().connect(SLOT(this, gdSqlContainerBar::doRowRemoved));
}

gdSqlTreeView* gdSqlContainerBar::treeView()
{
  return m_pSqlTreeView;
}

gdSqlTableView* gdSqlContainerBar::tableView()
{
  return m_pSqlTableView;
}

gdSqlModel* gdSqlContainerBar::model()
{
  return m_pSqlModel;
}

void gdSqlContainerBar::setTable(std::string tableName)
{
  m_strTableName = tableName;
}

/*
   =============================================================================================
   Pour utiliser cette fonctionnalité :
   - Les noms de champs de db doivent etre complet exemple : table$field et pas AS field
      Tenir compte du $ qui est remplacé par "." gdSqlModel load header
   =============================================================================================
*/
void gdSqlContainerBar::addTVSelectRows(std::vector<int> *vColSelection)
{
  if ( !vColSelection )         return;
  if ( !vColSelection->size() ) return;
  // On force la mise en page du model
  doReload();
  // On recupere le query de base et on charge le model
  m_strQueryCondition = model()->query();
  // On force le chargement du model
  if ( m_pSqlTreeView )
    m_pSqlTreeView->load();
  else if ( m_pSqlTableView )
    m_pSqlTableView->load();
  for (int sel = 0; sel < vColSelection->size(); sel++ ) {
    gdColumnSelection*  pcs = new gdColumnSelection();
    pcs->columnIndex           =  vColSelection->at(sel);
    std::vector<WString> vWStr = getUniqueValues(pcs->columnIndex);
    for (int iStr = 0; iStr < vWStr.size(); iStr++)
      pcs->strSelection.push_back(vWStr[iStr].toUTF8());
    //       pcs->strSelection.push_back(vWStr[iStr].narrow()); // le model a deja de l'utf8 donc pas from utf8 ?
    std::sort(pcs->strSelection.begin() + 1, pcs->strSelection.end());
    m_vColSel.push_back(pcs);
  }
  m_pModelSelectedRows = new WStandardItemModel(1, model()->columnCount());
  for (int col = 0; col < model()->columnCount(); col++) {
    WStandardItem* pItem = new WStandardItem();
    for (int sel = 0; sel < m_vColSel.size(); sel++ )
      if ( m_vColSel[sel]->columnIndex == col ) {
        std::string val = "0";
        pItem->setText(val);
        pItem->setData(0, UserRole);
      }
    m_pModelSelectedRows->setItem(0, col, pItem);
  }
  m_pModelSelectedRows->itemChanged().connect(SLOT(this, gdSqlContainerBar::doUpdateSelection));
  m_pTVSelectRows = new WTreeView();
  m_pTVSelectRows->setAlternatingRowColors(false);
  //  m_pTVSelectRows->setRootIsDecorated(false);
  m_pTVSelectRows->setSelectionMode(NoSelection);
  m_pTVSelectRows->setModel(m_pModelSelectedRows);
  for (int col = 0; col < model()->columnCount(); col++) {
    m_pModelSelectedRows->setHeaderData(col, model()->headerData(col));
    m_pTVSelectRows->setColumnWidth(col, m_pView->columnWidth(col));
  }
  for (int sel = 0; sel < m_vColSel.size(); sel++ ) {
    gdComboBoxDelegate* pDeleg = new gdComboBoxDelegate(m_vColSel[sel]->strSelection);
    m_pTVSelectRows->setItemDelegateForColumn(m_vColSel[sel]->columnIndex, pDeleg);
  }
  m_pTVSelectRows->setHeaderHeight(0);
  m_pTVSelectRows->resize(WLength::Auto, 20);
}

void gdSqlContainerBar::doUpdateSelection(WStandardItem* pItem)
{
  int         id           = asNumber(pItem->data(UserRole));
  for (int sel = 0; sel < m_vColSel.size(); sel++ ) {
    if ( m_vColSel[sel]->columnIndex != pItem->column() ) continue;
    m_vColSel[sel]->strCondition.clear();
    if ( id > 0 ) // 0 = All
      if ( model()->isColumnNumeric(m_vColSel[sel]->columnIndex) )
        m_vColSel[sel]->strCondition = " AND " + model()->getHeaderDB(m_vColSel[sel]->columnIndex).toUTF8()
                                       + " = " +  m_vColSel[sel]->strSelection[id];
      else
        m_vColSel[sel]->strCondition = " AND " + model()->getHeaderDB(m_vColSel[sel]->columnIndex).toUTF8()
                                       + " = '" +  m_vColSel[sel]->strSelection[id] + "' ";
    break;
  }
  std::string strCondition;
  for (int sel = 0; sel < m_vColSel.size(); sel++ )
    strCondition +=  m_vColSel[sel]->strCondition;
  std::string strQuery     = m_strQueryCondition;
  boost::algorithm::replace_all(strQuery, "--  $condition", strCondition);
  model()->setQuery(strQuery);
  doReload();
}

void gdSqlContainerBar::doReload()
{
  if ( m_pSqlTreeView )
    m_pSqlTreeView->loadModel();
  else if ( m_pSqlTableView )
    m_pSqlTableView->loadModel();
}

void gdSqlContainerBar::doModelReloaded()
{
  m_pWCb->setChecked(false);
  doModelRowCountChanged();
}

void gdSqlContainerBar::doRowRemoved(long row)
{
  if ( row >= model()->rowCount() ) return;
  if ( m_strTableName.empty() ) return;
  if ( m_strTableName.size() == 0 ) return;
  long k_id = asNumber(model()->data(row, 0, UserRole));
  std::string strQuery = "delete from ";
  strQuery += m_strTableName;
  strQuery += " where k_id = ";
  strQuery += boost::lexical_cast<std::string>(k_id);
  strQuery += ";";
  fprintf(stderr, "Start query : %s\n", strQuery.c_str());
  gdCoreSql*   oSql = model()->m_oSql;
  int retSql = oSql->execQuery(strQuery.c_str());
  if ( !retSql )
    fprintf(stderr, "Cannot execute : %s\n", strQuery.c_str());
  oSql->releaseQuery();
}

void gdSqlContainerBar::doModelRowCountChanged()
{
  m_tbiCountRows->setText("0");
  if ( !m_pView )     return;
  if ( !m_pSqlModel ) return;
  m_tbiCountRows->setText(boost::lexical_cast<std::string>(m_pSqlModel->rowCountView()));
}

