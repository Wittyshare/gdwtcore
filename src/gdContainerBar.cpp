/*
 * Copyright (C) 2006-Today Guy Deleeuw
 *
 * See the LICENSE file for terms of use.
*/
#include <stdio.h>

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/replace.hpp>

#include <Wt/WApplication>
#include <Wt/WVBoxLayout>
#include <Wt/WAnchor>
#include <Wt/WStandardItemModel>
#include <Wt/WStandardItem>
#include <Wt/WTheme>

#include "gdViewToPdf.h"
#include "gdContainerBar.h"
#include "gdComboBoxDelegate.h"
#include "gdTreeViewPdfRes.h"

using namespace Wt;

gdContainerBar::gdContainerBar(WAbstractItemView* view, WContainerWidget* parent)
 : WContainerWidget(parent), m_tbiCountRows(0), m_pPanel(0), m_pToolBar(0), m_l1row(0)
 {
  wApp->messageResourceBundle().use(wApp->docRoot() + wApp->resourcesUrl() + "/gdwtcore/Transl/gdContainerBar");

  addStyleClass("gdcontbar");
  if ( view )
    {
     setView(view);
     init();
    }
 }

void gdContainerBar::setPanel(WPanel* pPanel)
 { m_pPanel = pPanel; }

WPanel* gdContainerBar::panel()
 { return m_pPanel; }

void gdContainerBar::setView(WAbstractItemView* view)
 { m_pView = view; }

void gdContainerBar::init()
 {
  makeToolbar();

  WVBoxLayout*       vbox = new WVBoxLayout();

  if ( !m_pPanel || !m_pPanel->titleBar() )
     vbox->addWidget(m_pToolBar,  0);
//   else
//     m_pPanel->titleBarWidget()->addWidget(m_pToolBar);

  vbox->addWidget(m_pView, 1);

  resize(WLength(100, WLength::Percentage), WLength(100, WLength::Percentage));
  setOverflow(WContainerWidget::OverflowHidden);
  setLayout(vbox);

  view()->model()->rowsInserted().connect(SLOT(this, gdContainerBar::doModelRowCountChanged));
  view()->model()->rowsRemoved() .connect(SLOT(this, gdContainerBar::doModelRowCountChanged));
 }

WAbstractItemView* gdContainerBar::view()
 { return m_pView; }

std::vector<WString> gdContainerBar::getUniqueValues(int column)
 {
  std::vector<WString> vStr;
  vStr.push_back(WString::tr("gdcontbar_all"));
  for(int row = 0; row < m_pView->model()->rowCount(); row++)
   {
    WString  curStr = asString(m_pView->model()->data(row, column));
    bool     bMatch = false;
    for(int iStr = 0; iStr < vStr.size(); iStr++)
      if ( vStr[iStr] == curStr )
        {
         bMatch = true;
         break;
        }
    if ( !bMatch ) vStr.push_back(curStr);
   }
  return vStr;
 }

void gdContainerBar::setTitle(std::string title)
 { m_title = title; }

void gdContainerBar::hideCheckBox(bool bHide)
 { m_tbiCheckBox->setHidden(bHide); }

void gdContainerBar::hideRowsCount(bool bHide)
 {
  m_tbiCountRows->setHidden(bHide);
  m_tbiCountRowsTitle->setHidden(bHide);
 }

void gdContainerBar::hideReload(bool bHide)
 { m_tbiReload->setHidden(bHide); }

void gdContainerBar::hidePdf(bool bHide)
 { m_tbiPdf->setHidden(bHide); }

void gdContainerBar::hideDelete(bool bHide)
 { m_tbiDelete->setHidden(bHide); }

gdToolbarItem* gdContainerBar::addItem(std::string imgPath, std::string label, std::string tooltip)
 {
  if ( !m_pPanel )
     return m_pToolBar->addToolbarItem(imgPath, label, tooltip);
   else
    {
     gdToolbarItem* pTbi       = gdToolbar::newToolbarItem(imgPath, label, tooltip);
     m_pPanel->titleBarWidget()->insertWidget(m_pPanel->titleBarWidget()->count() - 1, pTbi);
     return pTbi;
    }
 }

void gdContainerBar::makeToolbar()
 {
  if ( !m_pPanel )
     m_pToolBar = new gdToolbar();

  m_pWCb = new WCheckBox();
  m_pWCb->checked()  .connect(SLOT(this, gdContainerBar::doCheck));
  m_pWCb->unChecked().connect(SLOT(this, gdContainerBar::doCheck));
  if ( !m_pPanel )
     m_tbiCheckBox       = m_pToolBar->addCheckBox(m_pWCb);
   else
    {
     m_tbiCheckBox       = gdToolbar::newCheckBox(m_pWCb);
     m_pPanel->titleBarWidget()->insertWidget(m_pPanel->titleBarWidget()->count() - 1, m_tbiCheckBox);
    }
  hideCheckBox(true);

  if ( !m_pPanel )
     m_tbiCountRows      = m_pToolBar->addToolbarItem("", "", WString::tr("gdcontbar_tbi_rowcount").toUTF8());
   else
    {
     m_tbiCountRows      = gdToolbar::newToolbarItem("", "", WString::tr("gdcontbar_tbi_rowcount").toUTF8());
     m_pPanel->titleBarWidget()->insertWidget(m_pPanel->titleBarWidget()->count() - 1, m_tbiCountRows);
    }
  if ( !m_pPanel )
     m_tbiCountRowsTitle = m_pToolBar->addToolbarItem("",  WString::tr("gdcontbar_tbi_rowtitle").toUTF8(), "");
   else
    {
     m_tbiCountRowsTitle = gdToolbar::newToolbarItem("",  WString::tr("gdcontbar_tbi_rowtitle").toUTF8(), "");
     m_pPanel->titleBarWidget()->insertWidget(m_pPanel->titleBarWidget()->count() - 1, m_tbiCountRowsTitle);
    }
  hideRowsCount(false);

  if ( !m_pPanel )
     m_tbiReload         = m_pToolBar->addToolbarItem(wApp->theme()->resourcesUrl() + "gdwtcore/Icons/Reload.png", "", WString::tr("gdcontbar_tbi_reload").toUTF8());
   else
    {
     m_tbiReload = gdToolbar::newToolbarItem(wApp->theme()->resourcesUrl() + "gdwtcore/Icons/Reload.png",  "", WString::tr("gdcontbar_tbi_reload").toUTF8());
     m_pPanel->titleBarWidget()->insertWidget(m_pPanel->titleBarWidget()->count() - 1, m_tbiReload);
    }
  hideReload(true);

  if ( !m_pPanel )
     m_tbiPdf            = m_pToolBar->addToolbarItem(wApp->theme()->resourcesUrl() + "gdwtcore/Icons/Pdf.png", "", WString::tr("gdcontbar_tbi_pdf").toUTF8());
   else
    {
     m_tbiPdf = gdToolbar::newToolbarItem(wApp->theme()->resourcesUrl() + "gdwtcore/Icons/Pdf.png", "", WString::tr("gdcontbar_tbi_pdf").toUTF8());
     m_pPanel->titleBarWidget()->insertWidget(m_pPanel->titleBarWidget()->count() - 1, m_tbiPdf);
    }
  m_tbiPdf->clicked().connect(SLOT(this, gdContainerBar::doPdf));
  hidePdf(false);

  if ( !m_pPanel )
     m_tbiDelete         = m_pToolBar->addToolbarItem(wApp->theme()->resourcesUrl() + "gdwtcore/Icons/Delete.png", "", WString::tr("gdcontbar_tbi_delete").toUTF8());
   else
    {
     m_tbiDelete = gdToolbar::newToolbarItem(wApp->theme()->resourcesUrl() + "gdwtcore/Icons/Delete.png", "",  WString::tr("gdcontbar_tbi_delete").toUTF8());
     m_pPanel->titleBarWidget()->insertWidget(m_pPanel->titleBarWidget()->count() - 1, m_tbiDelete);
    }
  m_tbiDelete->clicked().connect(SLOT(this, gdContainerBar::doDelete));
  hideDelete(true);
 }

void gdContainerBar::doCheck()
 {
  WStandardItemModel*  model = dynamic_cast<WStandardItemModel*>(view()->model());
  for(int row = 0; row < model->rowCount(); row++)
   {
    WStandardItem *wsi = model->invisibleRootItem()->child(row, 0);
    wsi->setChecked(m_pWCb->isChecked());
   }
 }

void gdContainerBar::doPdf()
 {
  gdTVPdfResource* pdfRes = new gdTVPdfResource(this);
  pdfRes->setFirstRow(m_l1row);
  pdfRes->setView(m_pView);
  wApp->redirect(pdfRes->url());
 }

/*
   Pour utiliser cette fonction il faut :
    - activer le bouton 'delete' via hideDelete(false);
    - connecter le signal rowsAboutToBeRemoved() du model
*/

void gdContainerBar::doDelete()
 {
  WModelIndexSet pSet = m_pView->selectedIndexes();
  if ( pSet.empty() ) return;

// TODO : Dialog confirm

  for(WModelIndexSet::iterator it = pSet.begin(); it!= pSet.end(); ++it)
   {
    WModelIndex    pIdx = *it;
    rowDeleted_.emit(pIdx.row());
    m_pView->model()->removeRows(pIdx.row(), 1);
   }
 }

void gdContainerBar::doModelRowCountChanged()
 {
  m_tbiCountRows->setText("0");
  if ( m_pView )
     if ( m_pView->model() )
        m_tbiCountRows->setText(boost::lexical_cast<std::string>(m_pView->model()->rowCount()));
 }

void gdContainerBar::setFirstRow(long firstRow)
 { m_l1row = firstRow; }
