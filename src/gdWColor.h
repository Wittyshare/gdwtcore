/*
 * Copyright (C) 2006-Today Guy Deleeuw
 *
 * See the LICENSE file for terms of use.
*/

#ifndef __gdWColor_H__
#define __gdWColor_H__

/*! \file gdWColor.h */

#include <Wt/WContainerWidget>
#include <Wt/WTable>
#include <Wt/WSpinBox>

#include "gdWColorDialog.h"

/*! \class gdWColor
 *  \brief A class that render the rgb value and call a color picker
*/

class gdWColor : public Wt::WContainerWidget {
public :
  /*! \brief Constructor.

  */
  gdWColor(Wt::WContainerWidget* parent = NULL);
  gdWColor(int red, int green, int blue, int alpha, Wt::WContainerWidget* parent = NULL);
  void                  init();
  int                   colorRed();
  int                   colorGreen();
  int                   colorBlue();
  int                   colorAlpha();

protected :
  void                  updateCellColor();

public slots :
  void                  setColorRed(int red);
  void                  setColorGreen(int green);
  void                  setColorBlue(int blue);
  void                  setColorAlpha(int alpha);
  void                  doChartColor();
  void                  doEndChartColor(gdWColorDialogData* pData);

private :
  int                   m_nRed;
  int                   m_nGreen;
  int                   m_nBlue;
  int                   m_nAlpha;

  Wt::WTableCell*       m_pColorCell;
  Wt::WSpinBox*         m_spRed;
  Wt::WSpinBox*         m_spGreen;
  Wt::WSpinBox*         m_spBlue;
  Wt::WSpinBox*         m_spAlpha;
};

#endif // ifdef __gdWColor_H__

