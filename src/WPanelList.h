// This may look like C code, but it's really -*- C++ -*-
/*
 * Copyright (C) 2008 Emweb bvba, Kessel-Lo, Belgium.
 *
 * See the LICENSE file for terms of use.
 */
#ifndef PANEL_LIST_H_
#define PANEL_LIST_H_

#include <Wt/WContainerWidget>
#include <Wt/WPanel>
#include <Wt/WSignal>

class WPanelList : public Wt::WContainerWidget {
public :
  WPanelList(Wt::WContainerWidget* parent);

  Wt::WPanel* addWidget(const Wt::WString& text, Wt::WWidget* w);
  void addPanel(Wt::WPanel* panel);
  void setAutoCollapse(bool bCollapse = true);
  //  void removePanel(Wt::WPanel *panel);

private slots :
  void selectionChanged();

public :
  Wt::Signal<void>& s_changed() {
    return s_changed_;
  };

private :
  Wt::Signal<void> s_changed_;

private :
  void onExpand(bool notUndo);

  int wasExpanded_;
  int m_bAutoCollapse;
};

#endif // PANEL_LIST_H_
