/*
 * Copyright (C) 2006-Today Guy Deleeuw
 *
 * See the LICENSE file for terms of use.
*/

#include <Wt/WApplication>
#include <Wt/WHBoxLayout>
#include <Wt/WTheme>

#include <gdcore/gdImage.h>

#include "gdWSearch.h"

using namespace Wt;

gdWSearch::gdWSearch(WContainerWidget* parent)
  : WContainerWidget(parent), m_leSearch(0), m_pImage(0)
{
  if ( WString::tr("byObjectStyleSheet").narrow() == "true" )
    wApp->useStyleSheet(wApp->theme()->resourcesUrl() + "gdwtcore/Css/gdWSearch.css");
  addStyleClass("gdWSearch");
  m_leSearch = new WLineEdit();
  m_leSearch->setTextSize(50);
  //  m_leSearch->resize(150, WLength::Auto);
  m_leSearch->enterPressed().connect(SLOT(this, gdWSearch::doSearch));
  m_leSearch->focussed().connect(SLOT(this, gdWSearch::doFocussed));
  m_leSearch->blurred().connect(SLOT(this, gdWSearch::doBlurred));
  m_pImage = new WImage();
  m_pImage->setImageRef(wApp->theme()->resourcesUrl() + "gdwtcore/Icons/Loupe.png");
  gdImageProperties imgProp = gdImage_size(wApp->docRoot() + m_pImage->imageRef());
  m_pImage->setWidth(WLength(imgProp.width, WLength::Pixel));
  m_pImage->setHeight(WLength(imgProp.height, WLength::Pixel));
  m_pImage->clicked().connect(SLOT(this, gdWSearch::doSearch));
  if ( 0 ) {
    WHBoxLayout* hbox = new WHBoxLayout();
    hbox->addWidget(m_leSearch, 1);
    hbox->addWidget(m_pImage, 0);
    setLayout(hbox);
  } else {
    addWidget(m_leSearch);
    //     addWidget(m_pImage);
  }
}

gdWSearch::~gdWSearch()
{ }

void gdWSearch::setText(const std::string& text)
{
  m_leSearch->setText(text);
}

const std::string& gdWSearch::text()
{
  m_leSearch->text().toUTF8();
}

Signal<Wt::WString>& gdWSearch::searchValidated()
{
  return searchValidated_;
}

void gdWSearch::doSearch()
{
  searchValidated_.emit(m_leSearch->text());
}

Signal<Wt::WString>& gdWSearch::searchFocussed()
{
  return searchFocussed_;
}

void gdWSearch::doFocussed()
{
  searchFocussed_.emit(m_leSearch->text());
}

Signal<Wt::WString>& gdWSearch::searchBlurred()
{
  return searchBlurred_;
}

void gdWSearch::doBlurred()
{
  //  if ( m_pImage && m_pImage->hasFocus() ) return;
  searchBlurred_.emit(m_leSearch->text());
}

