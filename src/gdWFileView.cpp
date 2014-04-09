/*
 * Copyright (C) 2006-Today G. Deleeuw
 *
 * See the LICENSE file for terms of use.
 */

// TODO :
//        - terminer rename
//        - toolbar button UP
//        - ajouter root au tree left
//        - quand tree left width change : resize column0 du treeview
//	  - Terminer Drag & Drop
//        - Si d&d pas d'icones si dans une WDialog
//        - si pas de container pas de Hscrollbar quand resize du panel
//        - quand Vscrollbar plus de button sort

#include <stdio.h>

#include <boost/algorithm/string/replace.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/exception.hpp>

#include <iostream>
#include <time.h>

#include <Wt/WApplication>
#include <Wt/WHBoxLayout>
#include <Wt/WVBoxLayout>
#include <Wt/WStandardItem>
#include <Wt/WText>
#include <Wt/WStackedWidget>
#include <Wt/WItemDelegate>
#include <Wt/WTheme>

#include <gdcore/gdCore.h>

#include "gdWFileView.h"

using namespace Wt;
using namespace gdwtcore;

gdWFileView::gdWFileView(viewType nViewType, const boost::filesystem::path& rootPath, tbFlags flags, WContainerWidget* parent)
  : m_viewType(nViewType), m_fsRootPath(rootPath), m_tbFlags(flags), m_pModelView(0), m_contMenu(0), m_pTb(0), m_bOnDataChanged(false), m_pMenu(0), m_bOnRename(false)
{
  if ( m_tbFlags == tbDefault ) {
    int nTmp = tbDefault;
    //     nTmp |= tbUseToolbar;
    //     nTmp |= tbBreadCrumb;
    m_tbFlags = (tbFlags) nTmp;
  }
  if ( WString::tr("byObjectStyleSheet").narrow() == "true" )
    wApp->useStyleSheet(wApp->theme()->resourcesUrl() + "gdwtcore/Css/gdWFileView.css");
  wApp->messageResourceBundle().use(wApp->docRoot() + wApp->resourcesUrl() + "/gdwtcore/Transl/gdWFileView");
  parent->addStyleClass("gdwfileview");
  parent->resize(WLength(100, WLength::Percentage), WLength(100, WLength::Percentage));
  parent->setOverflow(WContainerWidget::OverflowHidden);
  m_contMenu = new WContainerWidget();
  m_contMenu->addStyleClass("gdbreadcrumb");
  m_pModelTree = new WStandardItemModel(0, 1);
  m_pModelTree->setHeaderData(0, WString::tr("gdWFileView-Name"));
  m_pModelTree->setSortRole(UserRole);
  m_pTVTree = new WTreeView();
  //  m_pTVTree->setLayoutSizeAware(true);
  m_pTVTree->setModel(m_pModelTree);
  m_pTVTree->setColumnResizeEnabled(true);
  m_pTVTree->setSortingEnabled(true);
  m_pTVTree->setSelectionMode(SingleSelection);
  m_pTVTree->selectionChanged().connect(SLOT(this, gdWFileView::onTreeSelectionChanged));
  //  m_pTVTree->setDropsEnabled(true);
  m_pTVTree->expanded().connect(SLOT(this, gdWFileView::onTreeExpanded));
  m_pTVTree->mouseWentUp().connect(SLOT(this, gdWFileView::onTreeMouseUp));
  double dblWidth = (parent->width().value() * 30) / 100;
  //  m_pTVTree->resize(WLength(dblWidth, parent->width().unit()), WLength::Auto);
  //  m_pTVTree->resize(WLength::Auto, 550);
  //  m_pTVTree->resize(WLength::Auto, WLength::Auto);
  //  m_pTVTree->resize(WLength(30, WLength::Percentage), WLength(100, WLength::Percentage));
  m_pModelView = new WStandardItemModel(0, 3);
  m_pModelView->setHeaderData(0, WString::tr("gdWFileView-Name"));
  m_pModelView->setHeaderData(1, WString::tr("gdWFileView-Size"));
  m_pModelView->setHeaderData(2, WString::tr("gdWFileView-Date"));
  m_pModelView->setSortRole(UserRole);
  m_pModelView->itemChanged().connect(SLOT(this, gdWFileView::onDirectoryNameChanged));
  //TODO : implement the load method for the treeView and treeTree
  boost::filesystem::path   fsRealRoot;
  if ( (m_tbFlags & tbUpRoot) ) fsRealRoot = "/";
  else                         fsRealRoot = m_fsRootPath.string();
  if ( m_tbFlags & tbUseLeftTree )
    loadTree(m_pModelTree->invisibleRootItem(), fsRealRoot, noSkipParent);
  m_pTVView = new WTreeView();
  m_pTVView->setModel(m_pModelView);
  m_pTVView->setColumnResizeEnabled(true);
  m_pTVView->setSortingEnabled(true);
  m_pTVView->setAlternatingRowColors(true);
  //  m_pTVView->setRowHeight(14);
  m_pTVView->setSelectionMode(ExtendedSelection);
  m_pTVView->setColumnAlignment(1, AlignRight);
  m_pTVView->doubleClicked().connect(SLOT(this, gdWFileView::onViewDblClick));
  m_pTVView->selectionChanged().connect(SLOT(this, gdWFileView::onViewSelectionChanged));
  m_pTVView->expanded().connect(SLOT(this, gdWFileView::onViewExpanded));
  //  m_pTVView->setDragEnabled(true);
  m_pTVView->setEditTriggers(WAbstractItemView::SingleClicked);
  WItemDelegate* delegate = new WItemDelegate(parent);
  delegate->setTextFormat("%s");
  m_pTVView->setItemDelegate(delegate);
  init(m_fsRootPath);
  WHBoxLayout* hbox = new WHBoxLayout();
  if ( m_tbFlags & tbUseLeftTree ) {
    hbox->addWidget(m_pTVTree, 1);
    hbox->setResizable(0, true, WLength(33, WLength::Percentage));
  }
  hbox->addWidget(m_pTVView, 2);
  WVBoxLayout* vbox = new WVBoxLayout();
  if ( m_tbFlags & tbUseToolbar ) {
    makeToolbar();
    vbox->addWidget(m_pTb, 0);
  }
  vbox->addWidget(m_contMenu, 0);
  vbox->addLayout(hbox, 1);
  parent->setLayout(vbox);
}

gdWFileView::~gdWFileView()
{
  if ( m_pModelTree ) {
    delete m_pModelTree;
    m_pModelTree = 0;
  }
  if ( m_pModelView ) {
    delete m_pModelView;
    m_pModelView = 0;
  }
}

void gdWFileView::init(const boost::filesystem::path& startPath)
{
  m_pModelView->removeRows(0, m_pModelView->rowCount());
  if ( m_tbFlags & tbUseLeftTree )
    treeMatchPath(startPath);
  else if ( !loadTree(m_pModelView->invisibleRootItem(), startPath, noSkipParent) ) return;;
  if ( m_tbFlags & tbUseLeftTree )
    m_pModelTree->sort(0);
  m_pModelView->sort(0);
  setBreadCrumb(startPath);
  m_selectedDirectory = startPath.string();
}

bool gdWFileView::treeTraverse(WStandardItem* rootItem, const std::vector<std::string*>& vPath, int nLevel)
{
  if ( nLevel >= vPath.size() ) return false;
  for (int nItem = 0; nItem < rootItem->rowCount(); nItem++) {
    WStandardItem* tmpItem = rootItem->child(nItem, 0);
    std::string str = tmpItem->text().toUTF8();
    if ( *(vPath[nLevel]) == str ) {
      if ( nLevel == vPath.size() - 1 ) {
        m_pTVTree->select(tmpItem->index());
        m_pTVTree->scrollTo(tmpItem->index());
        return true;
      } else {
        m_pTVTree->expand(tmpItem->index());
        onTreeExpanded(tmpItem->index());
        return treeTraverse(tmpItem, vPath, ++nLevel);
      }
      break;
    }
  }
  return false;
}

void gdWFileView::treeMatchPath(const boost::filesystem::path& curPath)
{
  boost::filesystem::path     myPath(curPath);
  boost::filesystem::path     rootPath;
  std::vector<std::string*>   vPath;
  if ( (m_tbFlags & tbUpRoot) ) rootPath = "/";
  else                         rootPath = m_fsRootPath.string();
  //  vPath.push_back(new std::string(WString::tr("gdWFileView-root").toUTF8()));
  if ( myPath != rootPath )
    while (myPath.has_parent_path() ) {
      std::string*  pStr = new std::string(myPath.filename().string());
      vPath.insert(vPath.begin() + 0, pStr);
      myPath = myPath.parent_path();
      if ( myPath == rootPath ) break;
    }
  if ( treeTraverse(m_pModelTree->invisibleRootItem(), vPath, 0) ) return;
  loadTree(m_pModelView->invisibleRootItem(), rootPath, noSkipParent);
  return;
  WModelIndexSet  pSet  = m_pTVTree->selectedIndexes();
  if ( pSet.size() )
    m_pTVTree->scrollTo(*pSet.begin());
  //  WModelIndexSet  pSet;
  //  m_pTVTree->setSelectedIndexes(pSet);
}

void gdWFileView::setBreadCrumb(const boost::filesystem::path& bcPath)
{
  m_contMenu->clear();
  m_pMenu = 0;
  m_vBreadCrumb.clear();
  breadCrump* bc = new breadCrump();
  bc->title      = ""; // the root slash ?
  m_vBreadCrumb.push_back(bc);
  bc = new breadCrump();
  bc->title      = WString::tr("gdWFileView-root").toUTF8();
  if ( (m_tbFlags & tbUpRoot) )
    bc->path       = "/";
  else
    bc->path       = m_fsRootPath.string();
  m_vBreadCrumb.push_back(bc);
  boost::filesystem::path myPath(bcPath);
  if ( myPath != bc->path )
    while (myPath.has_parent_path() ) {
      breadCrump* bc2 = new breadCrump();
      bc2->title = myPath.filename().string();
      bc2->path  = myPath.string();
      m_vBreadCrumb.insert(m_vBreadCrumb.begin() + 2, bc2);
      bc2 = new breadCrump();
      bc2->title = " / ";
      m_vBreadCrumb.insert(m_vBreadCrumb.begin() + 2, bc2); // the / separator
      myPath = myPath.parent_path();
      if ( myPath == bc->path ) break;
    }
  if ( !(m_tbFlags & tbBreadCrumb) ) return;
  Wt::WStackedWidget*   contents = new Wt::WStackedWidget();
  m_pMenu  = new Wt::WMenu(contents, Wt::Horizontal);
  m_pMenu->itemSelected().connect(SLOT(this, gdWFileView::onBreadCrumbSelected));
  m_contMenu->addWidget(m_pMenu);
  for (int ibc = 0; ibc < m_vBreadCrumb.size(); ibc++) {
    WMenuItem* pItem =  new WMenuItem(m_vBreadCrumb[ibc]->title, new WText(""));
    m_pMenu->addItem(pItem);
  }
}

void gdWFileView::onBreadCrumbSelected(WMenuItem* pMenuItem)
{
  if ( m_vBreadCrumb[m_pMenu->currentIndex()]->path.empty() ) return;
  init(m_vBreadCrumb[m_pMenu->currentIndex()]->path);
}


/* loadTree et loadFile utilise le path complet dans data UserRole, prefixe 1 = directory, 2 = file
     ceci afin de conserver les repertoires groupes lors d'un tri colonne.
*/
bool gdWFileView::loadTree(WStandardItem* currentItem, const boost::filesystem::path& currentPath, skipMode skipParentDir)
{
  try {
    if ( boost::filesystem::is_directory(currentPath) ) {
      boost::filesystem::path   fsRealRoot;
      if ( (m_tbFlags & tbUpRoot) ) fsRealRoot = "/";
      else                         fsRealRoot = m_fsRootPath.string();
      if ( !skipParentDir && currentPath != fsRealRoot && !((m_tbFlags & tbBreadCrumb) || (m_tbFlags & tbUseLeftTree)) ) {
        std::vector<WStandardItem*> pRow;
        boost::filesystem::path newPath = currentPath.parent_path();
        WStandardItem*  newItem = new WStandardItem("..");
        newItem->setData("1" + newPath.string(), UserRole); // le path complet
        newItem->setIcon(wApp->theme()->resourcesUrl() + "gdwtcore/Icons/folder.png");
        pRow.push_back(newItem);
        pRow.push_back(new WStandardItem());
        pRow.push_back(new WStandardItem());
        currentItem->appendRow(newItem);
      }
      boost::filesystem::directory_iterator end_itr;
      for (boost::filesystem::directory_iterator itr_dir(currentPath); itr_dir != end_itr; ++itr_dir) {
        if ( !(m_tbFlags & tbShowHidden) )
          if ( gdcore_isPathHidden(itr_dir->path()) ) continue;
        bool bDir = false;
        // TODO : Ajouter une option pour ne pas suivre les liens symbolique.
        if ( gdcore_isPathSymLink(itr_dir->path()) )
          bDir = true;
        // Si symlink alors considere comme directory
        bDir = boost::filesystem::is_directory(itr_dir->status());
        if ( bDir ) {
          boost::filesystem::path newPath = itr_dir->path();
          if ( !loadFolder(currentItem, newPath) ) return false;
        } else {
          if ( currentItem->model() == m_pModelTree ) continue;
          boost::filesystem::path newPath = itr_dir->path();
          if ( !loadFile(currentItem, newPath) ) return false;
        }
      }
    }
  } catch (boost::filesystem::filesystem_error& e) {
    std::cerr << e.what() << std::endl;
    return false;
  }
  return true;
}

bool gdWFileView::loadFolder(WStandardItem* currentItem, const boost::filesystem::path& newPath, bool bEdit)
{
  WStandardItem*  newItem = new WStandardItem(WString::fromUTF8(newPath.filename().string())); // le nom.extension du fichier
  newItem->setData("1" + newPath.string(), UserRole); // le path complet
  newItem->setIcon(wApp->theme()->resourcesUrl() + "gdwtcore/Icons/folder.png");
  if ( currentItem->model() == m_pModelTree ) {
    //     newItem->setFlags(newItem->flags() | ItemIsDropEnabled);
    currentItem->appendRow(newItem);
  } else {
    std::vector<WStandardItem*> pRow;
    pRow.push_back(newItem);
    pRow.push_back(new WStandardItem());
    pRow.push_back(new WStandardItem());
    currentItem->appendRow(pRow);
    if ( bEdit ) {
      m_pModelView->sort(0);
      newItem->setFlags(ItemIsSelectable | ItemIsEditable);
      WModelIndexSet  pSet;
      m_pTVView->setSelectedIndexes(pSet);
      m_pTVView->edit(newItem->index());
    }
  }
  if ( currentItem->model() == m_pModelTree )
    newItem->appendRow(new WStandardItem());
  else {
    std::vector<WStandardItem*> pRowChild;
    if ( m_tbFlags & tbUseLeftTree ) return true;
    pRowChild.push_back(new WStandardItem());
    pRowChild.push_back(new WStandardItem());
    pRowChild.push_back(new WStandardItem());
    newItem->appendRow(pRowChild);
  }
  return true;
}

bool gdWFileView::loadFile(WStandardItem* currentItem, const boost::filesystem::path& currentPath)
{
  long fsize           = boost::filesystem::file_size(currentPath);
  std::string  strSize = boost::lexical_cast<std::string>(fsize);
  std::string  strDate;
  std::time_t  t       = boost::filesystem::last_write_time(currentPath);
  struct tm    ttm;
  char         c[100];
  localtime_r(&t, &ttm);
  strftime(c, 100, "%Y-%m-%d", &ttm);
  strDate = c;
  std::vector<WStandardItem*> pRow;
  WStandardItem*  newItem = new WStandardItem(WString::fromUTF8(currentPath.filename().string())); // le nom.extension du fichier
  //  newItem->setData("2" + currentPath.parent_path().string(), UserRole); // le path complet du parent
  newItem->setData("2" + currentPath.string(), UserRole); // le path complet du parent
  //  newItem->setFlags(newItem->flags() | ItemIsDragEnabled);
  // TODO : Deplacer les resources dans /usr/local/share/Wt/resources/
  newItem->setIcon(wApp->theme()->resourcesUrl() + "gdwtcore/Icons/file.png");
  pRow.push_back(newItem);
  WStandardItem* colSize = new WStandardItem(strSize);
  colSize->setData(fsize, UserRole);
  pRow.push_back(colSize);
  WStandardItem* colDate = new WStandardItem(strDate);
  colDate->setData(strDate, UserRole);
  pRow.push_back(colDate);
  currentItem->appendRow(pRow);
  return true;
}

void gdWFileView::onViewDblClick(WModelIndex idx, WMouseEvent mouseEvent)
{
  WModelIndex    idxcol0;
  if ( idx.column() != 0 ) {
    if ( idx.parent().isValid() )
      idxcol0 = idx.parent().child(idx.row(), 0);
    else
      idxcol0 = idx.child(idx.row(), 0);
  } else
    idxcol0 = idx;
  if ( !idxcol0.isValid() ) return;
  std::string str = asString(idxcol0.data(UserRole)).toUTF8();
  char type = str[0];
  str.erase(0, 1);
  //fprintf(stderr, "onDblclick - Type = %c, Selected path = %s\n", type, str.c_str());
  if ( type == '1' ) {
    init(str);
  } else {
    m_selectedFile = str;
    fileSelected().emit(doubleClick, m_selectedFile);
  }
}

void gdWFileView::onTreeSelectionChanged()
{
  WModelIndexSet pSet = m_pTVTree->selectedIndexes();
  WModelIndexSet::iterator it = pSet.begin();
  if ( pSet.empty() ) return;
  WModelIndex    idx = *it;
  std::string str = asString(idx.data(UserRole)).toUTF8();
  str.erase(0, 1);
  m_pModelView->invisibleRootItem()->removeRows(0, m_pModelView->invisibleRootItem()->rowCount());
  if ( !loadTree(m_pModelView->invisibleRootItem(), str, noSkipParent) ) return;;
  m_pModelView->sort(0);
  setBreadCrumb(str);
  m_selectedDirectory = str;
  m_selectedFile.clear();
  fileSelected().emit(none, m_selectedFile);
}

void gdWFileView::onTreeMouseUp(WModelIndex idx, WMouseEvent mouseEvent)
{
  if ( idx.column() != 0 ) return;
}

void gdWFileView::onTreeExpanded(WModelIndex idx)
{
  if ( idx.column() != 0 ) return;
  std::string str = asString(idx.data(UserRole)).toUTF8();
  str.erase(0, 1);
  WStandardItem* pItem = m_pModelTree->itemFromIndex(idx);
  pItem->removeRows(0, pItem->rowCount());
  if ( !loadTree(pItem, str, skipParent) ) return;
  m_pModelTree->sort(0);
}


/* Attention : le signal est envoyé deux fois si doubleclick ! */
void gdWFileView::onViewSelectionChanged()
{
  WModelIndexSet pSet = m_pTVView->selectedIndexes();
  WModelIndexSet::iterator it = pSet.begin();
  if ( pSet.empty() ) return;
  WModelIndex    idx = *it;
  std::string str = asString(idx.data(UserRole)).toUTF8();
  char type = str[0];
  str.erase(0, 1);
  //fprintf(stderr, "onViewSelectionChanged - Type = %c, Selected path = %s\n", type, str.c_str());
  if ( type == '2' ) {
    //if ( m_selectedFile == str ) return; // When dblclick : the signal are emitted 2 times if this test are removed
    m_selectedFile = str;
    fileSelected().emit(selected, m_selectedFile);
  } else {
    m_selectedFile.clear();
    m_selectedDirectory = str;
    fileSelected().emit(none, m_selectedFile);
  }
}


void gdWFileView::onViewExpanded(WModelIndex idx)
{
  if ( idx.column() != 0 ) return;
  std::string str = asString(idx.data(UserRole)).toUTF8();
  char type = str[0];
  str.erase(0, 1);
  //fprintf(stderr, "onViewExpanded - Type = %c, Selected path = %s\n", type, str.c_str());
  WStandardItem* pItem = m_pModelView->itemFromIndex(idx);
  if ( type == '1' ) {
    pItem->removeRows(0, pItem->rowCount());
    if ( !loadTree(pItem, str, skipParent) ) return;
    m_pModelView->sort(0);
  }
}

void gdWFileView::setCurrentPath(const std::string& currentPath)
{
  init(currentPath);
}

std::string gdWFileView::selectedDirectory()
{
  return m_selectedDirectory;
}

std::string gdWFileView::selectedFile()
{
  return m_selectedFile;
}

std::string gdWFileView::selectedPath()
{
  if ( gdcore_isPathFile(m_selectedFile) ) return m_selectedFile;
  else                                    return m_selectedDirectory;
}

std::string gdWFileView::currentFolderPath()
{
  if ( m_vBreadCrumb.size() ) return m_vBreadCrumb[m_vBreadCrumb.size() - 1]->path;
  else                       return m_fsRootPath.string();
}

void gdWFileView::makeToolbar()
{
  m_pTb = new gdToolbar();
  m_tbiNewFolder = m_pTb->addToolbarItem(wApp->theme()->resourcesUrl() + "gdwtcore/Icons/folder_new.png", WString::tr("gdWFileView-newfolder").toUTF8(), WString::tr("gdWFileView-newfolder-tooltips").toUTF8());
  m_tbiNewFolder->clicked().connect(SLOT(this, gdWFileView::doNewFolder));
  m_tbiDelete = m_pTb->addToolbarItem(wApp->theme()->resourcesUrl() + "gdwtcore/Icons/Delete.png", WString::tr("gdWFileView-delete").toUTF8(), WString::tr("gdWFileView-delete-tooltips").toUTF8());
  m_tbiDelete->clicked().connect(SLOT(this, gdWFileView::doConfirmDelete));
  m_tbiRename = m_pTb->addToolbarItem(wApp->theme()->resourcesUrl() + "gdwtcore/Icons/rename.png", WString::tr("gdWFileView-rename").toUTF8(), WString::tr("gdWFileView-rename-tooltips").toUTF8());
  m_tbiRename->clicked().connect(SLOT(this, gdWFileView::doRename));
}

void gdWFileView::doRename()
{
  WModelIndexSet pSet = m_pTVView->selectedIndexes();
  WModelIndexSet::iterator it = pSet.begin();
  if ( pSet.empty() ) return;
  WModelIndex    idx = *it;
  WStandardItem* pItem = m_pModelView->itemFromIndex(m_pModelView->index(idx.row(), 0, m_pModelView->parent(idx)));
  pItem->setFlags(ItemIsSelectable | ItemIsEditable);
  WModelIndexSet  pSet2;
  m_bOnRename = true;
  m_pTVView->setSelectedIndexes(pSet2);
  m_pTVView->edit(pItem->index());
}

void gdWFileView::doConfirmDelete()
{
  WModelIndexSet pSet = m_pTVView->selectedIndexes();
  WModelIndexSet::iterator it = pSet.begin();
  if ( pSet.empty() ) return;
  WModelIndex    idx = *it;
  std::string str = asString(idx.data(UserRole)).toUTF8();
  str.erase(0, 1);
  boost::algorithm::replace_all(str, m_fsRootPath.string(), "");
  m_pMb = new WMessageBox("Confirm", "Remove " + str + " ?", Wt::Question, Wt::Ok | Wt::Cancel);
  m_pMb->buttonClicked().connect(SLOT(this, gdWFileView::doDelete));
  m_pMb->show();
}

void gdWFileView::doDelete(Wt::StandardButton button)
{
  delete m_pMb;
  if ( button != Wt::Ok ) return;
  WModelIndexSet pSet = m_pTVView->selectedIndexes();
  WModelIndexSet::iterator it = pSet.begin();
  if ( pSet.empty() ) return;
  WModelIndex    idx = *it;
  std::string str = asString(idx.data(UserRole)).toUTF8();
  char type = str[0];
  str.erase(0, 1);
  try {
    boost::filesystem::remove_all(str);
  } catch (boost::filesystem::filesystem_error& e) {
    std::cerr << e.what() << std::endl;
    return;
  }
  fprintf(stderr, "delete item = %s\n", str.c_str());
  m_pModelView->removeRow(idx.row(), idx.parent());
  m_pModelView->sort(0);
  boost::filesystem::path newPath = str;
  m_selectedDirectory = newPath.parent_path().string();
  if ( m_tbFlags& tbUseLeftTree && type == '1' ) {
    treeMatchPath(str);
    pSet = m_pTVTree->selectedIndexes();
    if ( !pSet.empty() ) {
      it = pSet.begin();
      WModelIndex    idx = *it;
      WStandardItem* treeItem = m_pModelTree->itemFromIndex(idx);
      treeItem->parent()->removeRow(treeItem->row());
      treeMatchPath(newPath.parent_path().string());
      setBreadCrumb(newPath.parent_path().string());
    }
  }
}

void gdWFileView::doNewFolder()
{
  boost::filesystem::path newPath = currentFolderPath() + "/NewFolder";
  loadFolder(m_pModelView->invisibleRootItem(), newPath, true);
}

void gdWFileView::onDirectoryNameChanged(WStandardItem* pItem)
{
  if ( pItem->index().column() != 0 ) return;
  if ( m_bOnDataChanged ) return;
  m_bOnDataChanged = true;
  std::string newName = asString(pItem->data(EditRole)).toUTF8();
  std::string oldPath = asString(pItem->data(UserRole)).toUTF8();
  char type = oldPath[0];
  oldPath.erase(0, 1);
  pItem->setFlags(ItemIsSelectable);
  std::string newPath = currentFolderPath() + "/" + newName;
  pItem->setData(type + newPath, UserRole); // le path complet
  m_bOnDataChanged = false;
  bool bOnRename = m_bOnRename;
  m_bOnRename = false;
  if ( !bOnRename ) {
    fprintf(stderr, "create new directory = %s\n", newPath.c_str());
    try {
      boost::filesystem::create_directory(newPath);
    } catch (boost::filesystem::filesystem_error& e) {
      std::cerr << e.what() << std::endl;
      return;
    }
  } else {
    fprintf(stderr, "Rename item = %s in %s\n", oldPath.c_str(), newPath.c_str());
    try {
      boost::filesystem::rename(oldPath, newPath);
    } catch (boost::filesystem::filesystem_error& e) {
      std::cerr << e.what() << std::endl;
      return;
    }
  }
  if ( type != '1' ) return;
  WModelIndexSet pSet = m_pTVTree->selectedIndexes();
  WModelIndexSet::iterator it = pSet.begin();
  WStandardItem* treeItem = m_pModelTree->invisibleRootItem();
  if ( !pSet.empty() ) {
    WModelIndex    idx = *it;
    WStandardItem* treeItem = m_pModelTree->itemFromIndex(idx);
  }
  if ( !bOnRename )
    loadFolder(treeItem, newPath);
  else {
    oldPath = "1" + oldPath;
    for (int iChild = 0; iChild < treeItem->rowCount(); iChild++) {
      std::string tvPath = asString(treeItem->child(iChild, 0)->data(UserRole)).toUTF8();
      if ( tvPath == oldPath ) {
        treeItem->child(iChild, 0)->setText(newName);
        treeItem->setData(type + newPath, UserRole); // le path complet
      }
    }
  }
  treeItem->sortChildren(0, Wt::AscendingOrder);
}

