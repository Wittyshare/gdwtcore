/*
 * Copyright (C) 2006-Today Guy Deleeuw
 *
 * See the LICENSE file for terms of use.
*/

#include <Wt/WApplication>
#include <Wt/WContainerWidget>
#include <Wt/WText>
#include <Wt/WBreak>
#include <Wt/WImage>
#include <Wt/WTheme>

#include "dlgHourGlass.h"

using namespace Wt;

dlgHourGlass::dlgHourGlass(const WString& pLabel) : WDialog()
{
  addStyleClass("dlghourglass");
  setTitleBarEnabled(false);
  resize(140, 80);
  WContainerWidget* pRoot = new WContainerWidget();
  pRoot->setContentAlignment(Wt::AlignCenter);
  pRoot->addWidget(new WText(pLabel));
  pRoot->addWidget(new WBreak());
  WImage* pImage = new WImage();
  pImage->setImageRef(wApp->theme()->resourcesUrl() + "gdwtcore/Images/large-loading.gif");
  pRoot->addWidget(pImage);
  contents()->addWidget(pRoot);
}
