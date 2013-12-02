/*
 * Copyright (C) 2008-Today Guy Deleeuw
 *
 * See the LICENSE file for terms of use.
*/

#ifndef __GDWSPLASHDIALOG_H__
#define __GDWSPLASHDIALOG_H__

#include <boost/bind.hpp>

#include "gdWDialog.h"

class gdWSplashDialog : public gdWDialog
 {
  public :
        gdWSplashDialog(const Wt::WString &title, gdWDialogData* refData, int nTimer);
   void buildContent(Wt::WContainerWidget* pwcMain);

  public slots :
   void endSplash();

  private :
   int               m_nTimer;
 };

#endif // ifdef __GDWSSPLASDIALOG_H__

