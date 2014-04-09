/*
 * Copyright (C) 2006-Today Guy Deleeuw
 *
 * See the LICENSE file for terms of use.
*/

#ifndef __gdContainerBar_H__
#define __gdContainerBar_H__

/*! \file gdContainerBar.h */

#include <Wt/WContainerWidget>
#include <Wt/WPanel>
#include "Wt/WAbstractItemView"

#include "gdToolbar.h"

/*! \class gdContainerBar
 *  \brief A class that superceed a Wt::WContainerWidget with a gdToolBar and a WAbstractItemView
*/

class gdContainerBar : public Wt::WContainerWidget {
public :
  /*! \brief Constructor.

  */
  gdContainerBar(Wt::WAbstractItemView* view = 0, Wt::WContainerWidget* parent = NULL);
  void                      setPanel(Wt::WPanel* pPanel);
  Wt::WPanel*               panel();
  void                      setView(Wt::WAbstractItemView* view);
  void                      init();
  Wt::WAbstractItemView*    view();
  void                      setFirstRow(long fisrtRow);
  std::vector<Wt::WString>  getUniqueValues(int column);
  void                      setTitle(std::string title);
  void                      hideCheckBox(bool bHide = false);
  void                      hideRowsCount(bool bHide = false);
  void                      hideReload(bool bHide = false);
  void                      hidePdf(bool bHide = false);
  void                      hideDelete(bool bHide = true);
  gdToolbarItem*            addItem(std::string imgPath, std::string label, std::string tooltip);

public slots :
  void      doPdf();
  void      doModelRowCountChanged();
  void      doDelete();
  void      doCheck();

public : // Signals
  Wt::Signal<long>& rowDeleted()    {
    return rowDeleted_;
  }

private :
  Wt::Signal<long>           rowDeleted_;

protected :
  void      makeToolbar();

protected :
  Wt::WPanel*                     m_pPanel;
  Wt::WAbstractItemView*          m_pView;
  gdToolbar*                      m_pToolBar;
  std::string                     m_title;
  Wt::WCheckBox*                  m_pWCb;
  gdToolbarItem*                  m_tbiCheckBox;
  gdToolbarItem*                  m_tbiCountRows;
  gdToolbarItem*                  m_tbiCountRowsTitle;
  gdToolbarItem*                  m_tbiReload;
  gdToolbarItem*                  m_tbiPdf;
  gdToolbarItem*                  m_tbiDelete;
  long                            m_l1row;
};

#endif // ifdef __gdContainerBar_H__

