/*
 * Copyright (C) 2008-Today Guy Deleeuw
 *
 * See the LICENSE file for terms of use.
*/

#include <Wt/WImage>
#include <Wt/WTimer>
#include <Wt/WText>

#include "gdWSplashDialog.h"

using namespace Wt;

gdWSplashDialog::gdWSplashDialog(const WString &title, gdWDialogData* refData, int nTimer)
 : gdWDialog(title, refData, nTimer ? gdWDialog::buttonNone : gdWDialog::buttonOkCancel), m_nTimer(nTimer)
 {
  setTitleBarEnabled(false);
  resize(450, 400);
  buildContent(getContent());
 }

void gdWSplashDialog::buildContent(WContainerWidget* pwcMain)
 {
  WVBoxLayout*      hBox1 = new WVBoxLayout();

  WText*  pAppName = new WText(WString::tr("appName"));
  hBox1->addWidget(pAppName, 0, AlignCenter);

  WImage* pImage = new WImage();
  pImage->setImageRef(WString::tr("appLogo").narrow().c_str());
  pImage->resize(200, 200);
  hBox1->addWidget(pImage, 1, AlignCenter);

  WText*  pAppDesc = new WText(WString::tr("appDesc"));
  hBox1->addWidget(pAppDesc, 0, AlignCenter);

  WHBoxLayout*      hBox2 = new WHBoxLayout();

  WText*  pAppAuthor = new WText(WString::tr("appAuthor"));
  hBox2->addWidget(pAppAuthor);

  WText*  pAppVersion = new WText(WString::tr("appVersion"));
  hBox2->addWidget(pAppVersion, 0, AlignRight);

  WContainerWidget* wcBottom = new WContainerWidget();
  hBox1->addWidget(wcBottom);
  wcBottom->setLayout(hBox2);

  pwcMain->setLayout(hBox1);
  pwcMain->setOverflow(WContainerWidget::OverflowHidden);

  if ( m_nTimer )
     WTimer::singleShot(m_nTimer, SLOT(this, gdWSplashDialog::endSplash));
 }

void gdWSplashDialog::endSplash()
 {
  onFinished(Wt::WDialog::Accepted);
 }
