/*
 * Copyright (C) 2006-Today Guy Deleeuw
 *
 * See the LICENSE file for terms of use.
*/
#include <stdio.h>

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/replace.hpp>

#include <Wt/WApplication>
#include <Wt/WText>
#include <Wt/WTheme>

#include "gdWColor.h"

using namespace Wt;

gdWColor::gdWColor(Wt::WContainerWidget* parent)
  : Wt::WContainerWidget(parent), m_nRed(0), m_nGreen(0), m_nBlue(0), m_nAlpha(0)
{
  init();
}

gdWColor::gdWColor(int red, int green, int blue, int alpha, Wt::WContainerWidget* parent)
  : Wt::WContainerWidget(parent), m_nRed(red), m_nGreen(green), m_nBlue(blue), m_nAlpha(alpha)
{
  init();
}

void gdWColor::init()
{
  // TODO : Deplacer les resources dans /usr/local/share/Wt/resources/
  if ( WString::tr("byObjectStyleSheet").narrow() == "true" )
    wApp->useStyleSheet(wApp->theme()->resourcesUrl() + "gdwtcore/Css/gdWColor.css");
  wApp->messageResourceBundle().use(wApp->docRoot() + wApp->resourcesUrl() + "/gdwtcore/Transl/gdWColor");
  addStyleClass("gdwcolor");
  Wt::WTable*           pTable = new Wt::WTable(this);
  pTable->elementAt(0, 1)->addWidget(new Wt::WText(Wt::WString::tr("gdwcolor_red")));
  pTable->elementAt(0, 3)->addWidget(new Wt::WText(Wt::WString::tr("gdwcolor_green")));
  pTable->elementAt(0, 5)->addWidget(new Wt::WText(Wt::WString::tr("gdwcolor_blue")));
  pTable->elementAt(0, 7)->addWidget(new Wt::WText(Wt::WString::tr("gdwcolor_alpha")));
  pTable->elementAt(0, 9)->addWidget(new Wt::WText(Wt::WString::tr("gdwcolor_dialog")));
  pTable->elementAt(1, 0)->addWidget(new Wt::WText(Wt::WString::tr("gdwcolor_title")));
  m_spRed = new Wt::WSpinBox();
  m_spRed->setRange(  0, 255);
  m_spRed->setValue(m_nRed);
  m_spRed->valueChanged().connect(SLOT(this, gdWColor::setColorRed));
  m_spRed->setTextSize(3);
  pTable->elementAt(1, 1)->addWidget(m_spRed);
  m_spGreen = new Wt::WSpinBox();
  m_spGreen->setRange(  0, 255);
  m_spGreen->setValue(m_nGreen);
  m_spGreen->valueChanged().connect(SLOT(this, gdWColor::setColorGreen));
  m_spGreen->setTextSize(3);
  pTable->elementAt(1, 3)->addWidget(m_spGreen);
  m_spBlue = new Wt::WSpinBox();
  m_spBlue->setRange(  0, 255);
  m_spBlue->setValue(m_nBlue);
  m_spBlue->valueChanged().connect(SLOT(this, gdWColor::setColorBlue));
  m_spBlue->setTextSize(3);
  pTable->elementAt(1, 5)->addWidget(m_spBlue);
  m_spAlpha = new Wt::WSpinBox();
  m_spAlpha->setRange(  0, 255);
  m_spAlpha->setValue(m_nAlpha);
  m_spAlpha->valueChanged().connect(SLOT(this, gdWColor::setColorAlpha));
  m_spAlpha->setTextSize(3);
  pTable->elementAt(1, 7)->addWidget(m_spAlpha);
  m_pColorCell = pTable->elementAt(1, 9);
  Wt::WColor color(m_spRed->value(), m_spGreen->value(), m_spBlue->value(), m_spAlpha->value());
  m_pColorCell->decorationStyle().setBackgroundColor(color);
  m_pColorCell->clicked().connect(SLOT(this, gdWColor::doChartColor));
  m_pColorCell->resize(60, 30);
}

void gdWColor::setColorRed(int red)
{
  m_nRed = red;
  m_spRed->setValue(red);
  updateCellColor();
}

void gdWColor::setColorGreen(int green)
{
  m_nGreen = green;
  m_spGreen->setValue(green);
  updateCellColor();
}

void gdWColor::setColorBlue(int blue)
{
  m_nBlue = blue;
  m_spBlue->setValue(blue);
  updateCellColor();
}

void gdWColor::setColorAlpha(int alpha)
{
  m_nAlpha = alpha;
  m_spAlpha->setValue(alpha);
  updateCellColor();
}

int gdWColor::colorRed()
{
  return m_nRed;
}

int gdWColor::colorGreen()
{
  return m_nGreen;
}

int gdWColor::colorBlue()
{
  return m_nBlue;
}

int gdWColor::colorAlpha()
{
  return m_nAlpha;
}

void gdWColor::doChartColor()
{
  Wt::WColor color(m_spRed->value(), m_spGreen->value(), m_spBlue->value(), m_spAlpha->value());
  gdWColorDialogData* pData = new gdWColorDialogData(color);
  pData->callerCallback = boost::bind(&gdWColor::doEndChartColor, boost::ref(*this), pData);
  gdWColorDialog* pdial = new gdWColorDialog("Select a color", pData);
  pdial->show();
}

void gdWColor::doEndChartColor(gdWColorDialogData* pData)
{
  if ( !pData ) return;
  setColorRed(pData->m_color.red());
  setColorGreen(pData->m_color.green());
  setColorBlue(pData->m_color.blue());
  setColorAlpha(pData->m_color.alpha());
  delete pData;
}

void gdWColor::updateCellColor()
{
  WColor color(m_spRed->value(), m_spGreen->value(), m_spBlue->value(), m_spAlpha->value());
  m_pColorCell->decorationStyle().setBackgroundColor(color);
}

