/*
 * Copyright (C) 2006-Today Guy Deleeuw
 *
 * See the LICENSE file for terms of use.
*/

#include <boost/lexical_cast.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/exception.hpp>
#include <boost/algorithm/string/replace.hpp>

#include <Wt/WApplication>
#include <Wt/WItemDelegate>
#include <Wt/WTheme>

#include <gdcore/gdCore.h>

#include "gdLdapTree.h"

// TODO : Sort with UTF8
//        Set an option to render items with children first then render items base.
//        Manage x servers with y filters
//        Allow possibility to set a own naming context (or start base).

gdLdapTree::gdLdapTree(WContainerWidget* parent)
  : WTreeView(parent),
    m_modelTree(0), m_pProxy(0), m_sFilter("(&(objectClass=*))"), m_nScope(LDAP_SCOPE_ONE), m_nCheckAllowed(true), m_bDebug(false), m_bDummy(true)
{
  // Chargement du fichier de resources .xml
  //  std::string str = wApp->theme()->resourcesUrl() + "gdwtcore/Css/WLdap/gdLdapTree.css";
  //  wApp->log("notice") <<  "gdLdapTree::gdLdapTree themes resourcesUrl = " << str;
  if ( WString::tr("byObjectStyleSheet").narrow() == "true" )
    wApp->useStyleSheet(wApp->theme()->resourcesUrl() + "gdwtcore/Css/WLdap/gdLdapTree.css");
  //     wApp->useStyleSheet(wApp->theme()->resourcesUrl() + "gdwtcore/Css/WLdap/gdLdapTree.css");
  addStyleClass("gdLdapTree");
  wApp->messageResourceBundle().use(wApp->docRoot() + wApp->resourcesUrl() + "/gdwtcore/Transl/WLdap/gdLdapTree");
}

gdLdapTree::~gdLdapTree()
{ }

void  gdLdapTree::setServerName(const std::string& sName)
{
  m_sServerName = sName;
  if ( m_modelTree )
    buildModel();
}

const std::string& gdLdapTree::serverName()
{
  return m_sServerName;
}

void  gdLdapTree::setServerDisplayName(const std::string& sDisplayName)
{
  m_sServerDisplayName = sDisplayName;
}

const std::string& gdLdapTree::serverDisplayName()
{
  return m_sServerDisplayName;
}

void  gdLdapTree::setFilter(const std::string& sFilter)
{
  m_sFilter = sFilter;
  //  if ( m_modelTree )
  //     buildModel();
}

const std::string& gdLdapTree::filter()
{
  return m_sFilter;
}

void  gdLdapTree::setScope(const int nScope)
{
  m_nScope = nScope;
  //  if ( m_modelTree )
  //     buildModel();
}

const int gdLdapTree::scope()
{
  return m_nScope;
}

void  gdLdapTree::setDebugging(bool bDebug)
{
  m_bDebug = bDebug;
}

const bool gdLdapTree::debugging()
{
  return m_bDebug;
}

void gdLdapTree::setCheckAllowed(int nCheckType)
{
  m_nCheckAllowed = nCheckType;
  if ( m_modelTree )
    buildModel();
}

int gdLdapTree::checkAllowed()
{
  return m_nCheckAllowed;
}

void  gdLdapTree::setDummy(const bool bDummy)
{
  m_bDummy = bDummy;
  if ( m_modelTree )
    buildModel();
}

const bool gdLdapTree::dummy()
{
  return m_bDummy;
}

void gdLdapTree::addNamingContext(const std::string& namingContext, bool bClear)
{
  if ( bClear )
    m_vSearchBases.clear();
  m_vSearchBases.push_back(namingContext);
}

void gdLdapTree::load()
{
  if ( loaded() ) return;
  if ( m_bDebug )
    wApp->log("notice") <<  "gdLdapTree::load";
  buildView();
  WTreeView::load();
}

void gdLdapTree::buildModel()
{
  if ( m_bDebug )
    wApp->log("notice") <<  "gdLdapTree::buildModel";
  m_cLdap.setManageDSAIT(true); // -M
  m_cLdapServer.strServer      =  m_sServerName;
  m_cLdapServer.strDisplayName =  m_sServerDisplayName;
  wApp->log("notice") <<  "gdLdapTree::buildModel() try bind server " << m_cLdapServer.strServer << " ...";
  if ( m_cLdapServer.Connect() )
    wApp->log("notice") << "gdLdapTree::buildModel() ldap server connection OK";
  else {
    wApp->log("ERROR")  << "gdLdapTree::buildModel() ldap server connection NOT OK";
    return;
  }
  // On retire tous les naming contexts (base)
  if ( m_vSearchBases.size() == 0 ) {
    gdCLdapEntries      cEntries;
    if ( m_cLdapServer.GetNamingContexts(cEntries) ) {
      int nbItems = cEntries[0]->Attrs[0]->Values.size();
      for ( int i = 0; i < nbItems; i++) {
        m_vSearchBases.push_back(cEntries[0]->Attrs[0]->Values[i]->ToStr());
        if ( m_bDebug )
          wApp->log("notice") <<  "gdLdapTree::buildModel() naming context : " << m_vSearchBases[i];
      }
    }
  }
  gdStdModel*             pModel = m_modelTree;
  gdProxyModel*           pProxy = m_pProxy;
  m_modelTree                    = 0;
  m_pProxy                       = 0;
  int                     nbColumns = 2;
  m_modelTree = new gdStdModel(0, nbColumns, this);
  m_modelTree->setHeaderData(0, Wt::Horizontal, WString("rdn"));
  m_modelTree->setHeaderData(1, Wt::Horizontal, WString("dn"));
  m_modelTree->setSortRole(Wt::UserRole);
  m_modelTree->setCol0Checkable(true);
  m_modelTree->setFirstRowReserved(true);
  if ( m_modelTree->isFirstRowReserved() ) {
    std::vector<WStandardItem*> pRow;
    for (int iItem = 0; iItem < nbColumns; ++iItem) {
      gdStdItem* pItem = new gdStdItem(WString());
      pItem->setFlags(ItemIsSelectable | ItemIsEditable);
      pRow.push_back(pItem);
    }
    m_modelTree->invisibleRootItem()->appendRow(pRow);
  }
  if ( m_modelTree->isFirstRowReserved() ) {
    m_pProxy = new gdProxyModel(this);
    m_pProxy->setSourceModel(m_modelTree);
    m_pProxy->setAllChildsFiltering(true); // Attention ne marche pas si le tree utilise des 'dummy entry'
    m_modelTree->setProxy(m_pProxy);
    setModel(m_pProxy);
  } else {
    m_pProxy = 0;
    setModel(m_modelTree);
  }
  // Ajout des naming context
  for ( int i = 0; i < m_vSearchBases.size(); ++i) {
    addEntry(m_modelTree->invisibleRootItem(), m_vSearchBases[i], m_vSearchBases[i], true);
  }
  m_modelTree->sort(0);
  if ( pProxy ) delete pProxy;
  if ( pModel ) delete pModel;
}

gdStdModel* gdLdapTree::model()
{
  return m_modelTree;
}

void gdLdapTree::populateEntry(WStandardItem* pRootItemCol00)
{
  WStandardItem* pRootItemCol01 = pRootItemCol00->parent()->child(pRootItemCol00->row(), 1);
  std::string    searchBase     = asString(pRootItemCol01->data(Wt::UserRole)).toUTF8();
  if ( m_bDebug )
    wApp->log("notice") <<  "gdLdapTree::populateEntry() search all childs of " << searchBase;
  gdCLdapFilter       cFilter;
  gdCLdapAttributes   cWitchAttrs;
  gdCLdapEntries      cEntries;
  cFilter.pServer                 = &m_cLdapServer;
  cFilter.strInitialSearchBase    = searchBase;
  cFilter.nScope                  = m_nScope;
  cFilter.strFilter               = m_sFilter;
  // TODO : export to an external conf file
  cWitchAttrs.push_back(new gdCLdapAttribute("rdn"));
  cWitchAttrs.push_back(new gdCLdapAttribute("dn"));
  cWitchAttrs.push_back(new gdCLdapAttribute("hasSubordinates"));
  if ( !m_cLdap.GetInfo(cFilter, cWitchAttrs, cEntries) ) {
    wApp->log("notice") <<  "gdLdapTree::populateEntry() cannot query ldap !!!";
    return;
  }
  pRootItemCol00->removeRows(0, pRootItemCol00->rowCount());
  for (int nEntry = 0; nEntry < cEntries.size(); ++nEntry) {
    std::string rdn;
    std::string dn;
    bool        bHasChildren = false;
    for (int nAttr = 0; nAttr < cEntries[nEntry]->Attrs.size(); ++nAttr) {
      if ( cEntries[nEntry]->Attrs[nAttr]->Attr == "rdn" )              rdn = cEntries[nEntry]->Attrs[nAttr]->Values[0]->ToStr();
      if ( cEntries[nEntry]->Attrs[nAttr]->Attr == "dn" )               dn  = cEntries[nEntry]->Attrs[nAttr]->Values[0]->ToStr();
      if ( cEntries[nEntry]->Attrs[nAttr]->Attr == "hasSubordinates" ) {
        if ( cEntries[nEntry]->Attrs[nAttr]->Values[0]->ToStr() == "TRUE" )
          bHasChildren = true;
      }
    }
    addEntry(pRootItemCol00, rdn, dn, bHasChildren);
  }
  pRootItemCol00->sortChildren(1, Wt::AscendingOrder); // 0 crash
  //  m_modelTree->sort(0); // crash aussi
}

void gdLdapTree::addEntry(WStandardItem* pRootItem, const std::string& rdn, const std::string& dn, bool bHasChildren)
{
  if ( m_bDebug )
    wApp->log("notice") <<  "gdLdapTree::addEntry() dn is " << dn << " hasChild = " << bHasChildren;
  std::vector<WStandardItem*> rowItems;
  gdStdItem* labelItem   = new gdStdItem(WString::fromUTF8(rdn));
  labelItem->setData(rdn, Wt::UserRole);
  labelItem->setToolTip(WString::fromUTF8(dn));
  if ( checkAllowed() ) {
    labelItem->setCheckable(true);
    labelItem->setTristate(true);
    labelItem->setCheckState(Wt::Unchecked);
    labelItem->checkStateChanged().connect(SLOT(m_modelTree, gdStdModel::onCheckChanged));
  }
  if ( bHasChildren )
    labelItem->setIcon(wApp->theme()->resourcesUrl() + "gdwtcore/Icons/folder.png");
  else
    labelItem->setIcon(wApp->theme()->resourcesUrl() + "gdwtcore/Icons/file.png");
  rowItems.push_back(labelItem);
  gdStdItem* dnItem   = new gdStdItem(WString::fromUTF8(dn));
  dnItem->setData(dn, Wt::UserRole);
  rowItems.push_back(dnItem);
  pRootItem->appendRow(rowItems);
  if ( bHasChildren )
    if ( dummy() ) {
      std::string str("-");
      addEntry(labelItem, str, str, false); // dummy entry to allow a drop/down button on the item
    } else {
      populateEntry(labelItem);
    }
}

void gdLdapTree::buildView()
{
  if ( m_bDebug )
    wApp->log("notice") <<  "gdLdapTree::buildView";
  if ( !m_modelTree )
    buildModel();
  if ( !m_modelTree ) return;
  setAlternatingRowColors(true);
  setSelectionMode(SingleSelection);
  setHeaderAlignment(0, Wt::AlignLeft);
  setHeaderAlignment(1, Wt::AlignLeft);
  setColumnAlignment(0, Wt::AlignLeft);
  setColumnAlignment(1, Wt::AlignLeft);
  setSortingEnabled(0, true);
  setSortingEnabled(1, true);
  setColumnWidth(0, 400);
  setColumnWidth(1,  80);
  setColumnHidden(0, false);
  setColumnHidden(1, true);
  // Pour le proxy
  setEditTriggers(Wt::WAbstractItemView::SingleClicked);
  setEditOptions(Wt::WAbstractItemView::SingleEditor);
  WItemDelegate* delegate = new WItemDelegate(this);
  delegate->setTextFormat("%s");
  setItemDelegate(delegate);
  expanded().connect(SLOT(this, gdLdapTree::onTreeExpanded));
  resize(treeWidth(), WLength::Auto);
}

WLength gdLdapTree::treeWidth()
{
  double dblWidth = 10.0; // scrollbar
  for (int nCol = 0; nCol < m_modelTree->columnCount(); ++nCol) {
    if ( isColumnHidden(nCol) ) continue;
    if ( isSortingEnabled(nCol) ) dblWidth += 10.0;
    dblWidth += columnWidth(nCol).value();
  }
  return dblWidth;
}

void gdLdapTree::onTreeExpanded(WModelIndex idx)
{
  if ( m_bDebug )
    wApp->log("notice") <<  "gdLdapTree::onTreeExpanded";
  if ( idx.column() != 0 ) return;
  if ( !m_bDummy ) return;
  gdStdItem* pItem = m_modelTree->itemFromIndex(idx);
  populateEntry(pItem);
}

void gdLdapTree::checkDN(const std::string& rDN)
{
  checkDN(m_modelTree->invisibleRootItem(), rDN, Wt::Checked);
}

void gdLdapTree::checkDN(WStandardItem* pRootItem, const std::string& rDN,  Wt::CheckState rCheck)
{
  int start = 0;
  int stop  = pRootItem->rowCount();
  if ( pRootItem == m_modelTree->invisibleRootItem() ) {
    if ( m_modelTree->isFirstRowReserved() ) ++start;
    if ( m_modelTree->isLastRowReserved() )  --stop;
  }
  for (int iRow = start; iRow < stop; ++iRow) {
    if ( asString(pRootItem->child(iRow, 1)->data(Wt::UserRole)).toUTF8() == rDN ) {
      if ( m_bDebug )
        wApp->log("notice") <<  "gdLdapTree::checkDN() dn is " << asString(pRootItem->child(iRow, 1)->data(Wt::UserRole)).toUTF8();
      pRootItem->child(iRow, 0)->setCheckState(rCheck);
    }
    if ( pRootItem->child(iRow, 0)->hasChildren() )
      checkDN(pRootItem->child(iRow, 0), rDN, rCheck);
  }
}

