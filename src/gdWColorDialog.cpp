/*
 * Copyright (C) 2008-Today Guy Deleeuw
 *
 * See the LICENSE file for terms of use.
*/

#include <stdio.h>

#include <boost/algorithm/string/replace.hpp>

#include <Wt/WApplication>
#include <Wt/WBreak>
#include <Wt/WTheme>

#include "gdWColorDialog.h"

using namespace Wt;

gdWColorDialogData::gdWColorDialogData(WColor color) : gdWDialogData(), m_color(color)
{
}



gdWColorDialog::gdWColorDialog(WString title, gdWColorDialogData* refData)
  : gdWDialog(title, refData), m_refData(refData), m_pColorTable(0)
{
  if ( WString::tr("byObjectStyleSheet").narrow() == "true" )
    wApp->useStyleSheet(wApp->theme()->resourcesUrl() + "gdwtcore/Css/gdWColorDialog.css");
  wApp->messageResourceBundle().use(wApp->docRoot() + wApp->resourcesUrl() + "/gdwtcore/Transl/gdWColorDialog");
  addStyleClass("gdWColorDialog");
  m_refData->saveDataCallback = boost::bind(&gdWColorDialog::saveData, boost::ref(*this), m_refData);
  WContainerWidget* content = getContent();
  int row    = 0;
  int column = 0;
  m_pColorTable = new WTable(content);
  m_pColorTable->addStyleClass("gdWColorTable");
  for (int red = 255; red > -1; red -= 51) {
    for (int green = 255, column = 0; green > -1; green -= 51) {
      for (int blue = 255; blue > -1; blue -= 51) {
        addColor(m_pColorTable->elementAt(row, column), WColor(red, green, blue));
        column++;
      }
      if ( green == 153 ) {
        row++;
        column = 0;
      }
    }
    row++;
  }
  content->addWidget(m_pColorTable);
  content->addWidget(new WBreak());
  m_pText = new WText();
  setColor(m_refData->m_color);
  content->addWidget(m_pText);
  resize(400, 450);
}

gdWColorDialog::~gdWColorDialog()
{
}

void gdWColorDialog::addColor(WTableCell* cell, const WColor& color)
{
  cell->decorationStyle().setBackgroundColor(color);
  cell->resize(20, 20);
  char    toolTip[64];
  sprintf(toolTip, "RGB(%3d, %3d, %3d)", color.red(), color.green(), color.blue());
  cell->setToolTip(toolTip);
  const WColor* const javaColor = &color;
  cell->clicked().connect(boost::bind(&gdWColorDialog::setColor, boost::ref(*this), *javaColor));
}

void gdWColorDialog::setColor(const WColor& color)
{
  m_refData->m_color = color;
  char    selection[64];
  sprintf(selection, "RGB(%3d, %3d, %3d)", color.red(), color.green(), color.blue());
  m_pText->setText(selection);
}

void gdWColorDialog::saveData(gdWDialogData* pData)
{
  //  fprintf(stderr, "RGB(%3d, %3d, %3d)\n", m_refData->m_color.red(), m_refData->m_color.green(), m_refData->m_color.blue());
}

