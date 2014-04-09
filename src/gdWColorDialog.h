/*
 * Copyright (C) 2008-Today Guy Deleeuw
 *
 * See the LICENSE file for terms of use.
*/

/*! \file gdWColorDialog.h */

#ifndef __gdWColorDialog__
#define __gdWColorDialog__

#include <Wt/WTable>
#include <Wt/WTableCell>
#include <Wt/WColor>
#include <Wt/WText>

#include "gdWDialog.h"

/*! \class gdWColorDialogData
*/
class gdWColorDialogData : public gdWDialogData {
public :
  gdWColorDialogData(Wt::WColor color = Wt::black);

public :
  Wt::WColor      m_color;
};

/*! \class gdWColorDialog
 *  \brief Dialog to allow users to select a rgd color
*/

class gdWColorDialog : public gdWDialog {
public :
  gdWColorDialog(Wt::WString title, gdWColorDialogData* refData = 0);
  ~gdWColorDialog();
  void         addColor(Wt::WTableCell* cell, const Wt::WColor& color);
  void         setColor(const Wt::WColor& color);
  void         saveData(gdWDialogData* pData = 0);

private :
  gdWColorDialogData*             m_refData;
  Wt::WTable*                     m_pColorTable;
  Wt::WText*                      m_pText;
};

#endif // ifdef __gdWColorDialog__

