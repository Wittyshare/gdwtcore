/*
 * Copyright (C) 2008 Emweb bvba, Kessel-Lo, Belgium.
 *
 * See the LICENSE file for terms of use.
 */

#include "WPanelList.h"

using namespace Wt;

WPanelList::WPanelList(Wt::WContainerWidget *parent)
  : Wt::WContainerWidget(parent), wasExpanded_(0), m_bAutoCollapse(true)
{ }

WPanel *WPanelList::addWidget(const Wt::WString& text, Wt::WWidget *w)
{
  WPanel *p = new WPanel();
  p->setTitle(text);
  p->setCentralWidget(w);

  addPanel(p);

  return p;
}

void WPanelList::addPanel(Wt::WPanel *panel)
{
  panel->setCollapsible(true);
  panel->collapse();

  panel->expandedSS().connect(SLOT(this, WPanelList::onExpand));
  panel->expanded().connect(SLOT(this, WPanelList::selectionChanged));
  panel->collapsed().connect(SLOT(this, WPanelList::selectionChanged));

  WContainerWidget::addWidget(panel);
}

void WPanelList::setAutoCollapse(bool bAutoCollapse)
 { m_bAutoCollapse = bAutoCollapse; }

void WPanelList::onExpand(bool notUndo)
{
  WPanel *panel = dynamic_cast<WPanel*>(sender());

  if (notUndo) {
    wasExpanded_ = -1;

    for (unsigned i = 0; i < children().size(); ++i) {
      WPanel *p = dynamic_cast<WPanel*>(children()[i]);
      if (p != panel) {
	if (!p->isCollapsed())
	  wasExpanded_ = i;
        if ( m_bAutoCollapse )
	   p->collapse();
      }
    }
  } else {
    if (wasExpanded_ != -1) {
      WPanel *p = dynamic_cast<WPanel *>(children()[wasExpanded_]);
      p->expand();
    }
  }
}

void WPanelList::selectionChanged()
 { s_changed_.emit(); }
