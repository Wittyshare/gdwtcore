/*
 * Copyright (C) 2008-Today Guy Deleeuw
 *
 * See the LICENSE file for terms of use.
*/

/*! \file gdWPrintersDialog.h */

#ifndef __gdWPrintersDialog__
#define __gdWPrintersDialog__

#include <Wt/WSelectionBox>

#include "gdWDialog.h"

/*! \class gdWPrintersDialogData
*/
class gdWPrintersDialogData : public gdWDialogData {
public :
  gdWPrintersDialogData(int selectedPrinters = -1);

public :
  int               m_selectedPrinter;
  std::string       m_printerName;
};

/*! \class gdWPrintersDialog
 *  \brief Dialog to allow users to select a printer
*/

class gdWPrintersDialog : public gdWDialog {
public :
  gdWPrintersDialog(Wt::WString title, gdWPrintersDialogData* refData = 0);
  ~gdWPrintersDialog();
  /*! \brief Load the printers list.  */
  void         printers();

  void         saveData(gdWDialogData* pData = 0);

private :
  gdWPrintersDialogData*          m_refData;
  Wt::WSelectionBox*              m_pSelBox;
  std::vector<std::string>        m_vPrinters;
};

#endif // ifdef __gdWPrintersDialog__

