/*
 * Copyright (C) 2006-Today Guy Deleeuw
 *
 * See the LICENSE file for terms of use.
 */

#include <boost/algorithm/string/replace.hpp>

#include <Wt/WApplication>
#include <Wt/WTheme>

#include <gdcore/gdImage.h>

#include "gdToolbar.h"

using namespace Wt;

gdToolbarItem::gdToolbarItem(gdToolbar* pParent, bool asAnchor, const int overDelay)
  : WContainerWidget(pParent),
    m_asAnchor(asAnchor), m_pIcon(0), m_pButton(0), m_pStr(0), m_bInPanel(false), m_pImage(0), m_anchor(0), m_pPopupMenu(0), m_nOverDelay(overDelay), m_nOutDelay(0), m_pLayout(0)
{
  if ( 0 ) {
    m_pLayout = new WHBoxLayout();
    setLayout(m_pLayout);
  }
  if ( WString::tr("byObjectStyleSheet").narrow() == "true" )
    wApp->useStyleSheet(wApp->theme()->resourcesUrl() + "gdwtcore/Css/gdToolbar.css");
  if ( parent() )
    addStyleClass("gdToolbarItem");
  else {
    addStyleClass("gdPanelItem");
    m_bInPanel = true;
  }
  if ( !m_pLayout )
    setInline(true);
  if ( m_asAnchor ) {
    m_anchor = new WAnchor();
    if ( m_nOverDelay )
      m_anchor->setMouseOverDelay(m_nOverDelay);
    m_anchor->setTarget(Wt::TargetNewWindow);
    if ( m_pLayout ) m_pLayout->addWidget(m_anchor);
    else            addWidget(m_anchor);
  }
}

void gdToolbarItem::setOverDelay(int overDelay)
{
  m_nOverDelay = overDelay;
}

void gdToolbarItem::setOutDelay(int outDelay)
{
  m_nOutDelay = outDelay;
}

void gdToolbarItem::setIcon(std::string iconPath)
{
  //  boost::algorithm::replace_first(iconPath, "/var/www", "");
  if ( m_asAnchor ) {
    if ( !m_pImage ) {
      //wApp->log("notice") << "gdToolbarItem::setIcon anchor iconPath = " << iconPath << " and docroot = " << wApp->appRoot();
      gdImageProperties imgProp = gdImage_size(wApp->docRoot() + iconPath);
      //        wApp->log("notice") << "width = " << imgProp.width << " height = " << imgProp.height;
      m_pImage = new WImage();
      m_pImage->setWidth(WLength(imgProp.width, WLength::Pixel));
      m_pImage->setHeight(WLength(imgProp.height, WLength::Pixel));
      // setMouseOverDelay deja positionne sur m_anchor, a mon avis ces deux lignes ne sont pas nécessaires. a tester
      //        if ( m_nOverDelay )
      //           m_pImage->setMouseOverDelay(m_nOverDelay);
      m_pImage->setImageRef(iconPath);
      if ( m_anchor ) m_anchor->setImage(m_pImage);
      if ( !m_bInPanel ) m_pImage->addStyleClass("gdToolbarItemIcon");
      else              m_pImage->addStyleClass("gdPanelItemIcon");
    }
  } else {
    if ( !m_pIcon ) {
      //wApp->log("notice") << "gdToolbarItem::setIcon icon iconPath = " << iconPath << " and docroot = " << wApp->appRoot();
      gdImageProperties imgProp = gdImage_size(wApp->docRoot() + iconPath);
      //        wApp->log("notice") << "width = " << imgProp.width << " height = " << imgProp.height;
      m_pIcon = new WImage();
      if ( m_nOverDelay )
        m_pIcon->setMouseOverDelay(m_nOverDelay);
      m_pIcon->setWidth(WLength(imgProp.width, WLength::Pixel));
      m_pIcon->setHeight(WLength(imgProp.height, WLength::Pixel));
      m_pIcon->setImageRef(iconPath);
      if ( !m_bInPanel ) m_pIcon->addStyleClass("gdToolbarItemIcon");
      else              m_pIcon->addStyleClass("gdPanelItemIcon");
      m_pIcon->doubleClicked().connect(SLOT(this, gdToolbarItem::onDoubleClicked));
      m_pIcon->clicked().connect(SLOT(this, gdToolbarItem::onClicked));
      if ( m_nOverDelay )
        m_pIcon->mouseWentOver().connect(SLOT(this, gdToolbarItem::onMouseWentOver));
      // Ne marche pas : si > 0 donne un flicker dans les menus
      if ( m_nOutDelay )
        m_pIcon->mouseWentOut().connect(SLOT(this, gdToolbarItem::onMouseWentOut));
      if ( m_pLayout ) m_pLayout->addWidget(m_pIcon);
      else                       addWidget(m_pIcon);
    }
  }
}

void gdToolbarItem::setText(std::string label)
{
  if ( !m_asAnchor ) {
    if ( !m_pButton ) {
      m_pButton = new WPushButton();
      if ( m_nOverDelay )
        m_pButton->setMouseOverDelay(m_nOverDelay);
      if ( !m_bInPanel )
        m_pButton->addStyleClass("gdToolbarItemText");
      else
        m_pButton->addStyleClass("gdPanelItemText");
      m_pButton->doubleClicked().connect(SLOT(this, gdToolbarItem::onDoubleClicked));
      m_pButton->clicked().connect(SLOT(this, gdToolbarItem::onClicked));
      if ( m_nOverDelay )
        m_pButton->mouseWentOver().connect(SLOT(this, gdToolbarItem::onMouseWentOver));
      // Ne marche pas : si > 0 donne un flicker dans les menus
      if ( m_nOutDelay )
        m_pButton->mouseWentOut().connect(SLOT(this, gdToolbarItem::onMouseWentOut));
      if ( m_pLayout) m_pLayout->addWidget(m_pButton);
      else                      addWidget(m_pButton);
      if ( m_pStr ) {
        delete m_pStr;
        m_pStr = 0;
      }
    }
    m_pButton->setText(label);
  } else {
    if ( m_anchor ) m_anchor->setText(label);
  }
}

const WString& gdToolbarItem::text()
{
  if ( m_asAnchor && m_anchor )
    return m_anchor->text();
  else {
    if ( m_pButton )
      return m_pButton->text();
    else {
      if ( !m_pStr )
        m_pStr = new WString("");
      return *m_pStr;
    }
  }
}

WImage* gdToolbarItem::icon()
{
  return m_pIcon;
}

void gdToolbarItem::setUrl(const std::string& url)
{
  m_url = url;
}

const std::string& gdToolbarItem::url() const
{
  return m_url;
}

void gdToolbarItem::setPopupMenu(WPopupMenu* pPopupMenu)
{
  m_pPopupMenu = pPopupMenu;
}

WPopupMenu* gdToolbarItem::popupMenu()
{
  return m_pPopupMenu;
}

void gdToolbarItem::onClicked(const WMouseEvent& e)
{
  clicked().emit(this, e);
}

void gdToolbarItem::onDoubleClicked(const WMouseEvent& e)
{
  doubleClicked().emit(this, e);
}

void gdToolbarItem::onMouseWentOver(const WMouseEvent& e)
{
  if ( m_pPopupMenu )
    if ( !m_pPopupMenu->isHidden() )
      m_pPopupMenu->setHidden(true);
  mouseWentOver().emit(this, e);
}

void gdToolbarItem::onMouseWentOut(const WMouseEvent& e)
{
  if ( m_pPopupMenu )
    if ( !m_pPopupMenu->isHidden() )
      m_pPopupMenu->setHidden(true);
  mouseWentOut().emit(this, e);
}








gdToolbar::gdToolbar(WContainerWidget* pParent, bool bNoRecurse) : WContainerWidget(pParent), m_bNoRecurse(bNoRecurse)
{
  if ( WString::tr("byObjectStyleSheet").narrow() == "true" )
    wApp->useStyleSheet(wApp->theme()->resourcesUrl() + "gdwtcore/Css/gdToolbar.css");
  if ( m_bNoRecurse ) return;
  addStyleClass("gdToolbar");
  //  resize(WLength(100, WLength::Percentage), WLength(20));
  leftToolbar  = new gdToolbar(0, 1);
  leftToolbar->addStyleClass("gdToolbarLeft");
  rightToolbar = new gdToolbar(0, 1);
  rightToolbar->addStyleClass("gdToolbarRight");
  WHBoxLayout* hBox = new WHBoxLayout();
  //  hBox->setSpacing(0);
  //  hBox->setContentsMargins(2, 0, 2, 0);
  hBox->addWidget(leftToolbar,  1, AlignLeft);
  hBox->addWidget(rightToolbar, 0, AlignRight);
  setLayout(hBox);
}

gdToolbarItem* gdToolbar::addCheckBox(WCheckBox* pWCb, const int overDelay)
{
  gdToolbarItem* toolbarItem = new gdToolbarItem(leftToolbar, false, overDelay);
  toolbarItem->addWidget(pWCb);
  return toolbarItem;
}

gdToolbarItem* gdToolbar::newCheckBox(WCheckBox* pWCb, const int overDelay)
{
  gdToolbarItem* toolbarItem = new gdToolbarItem(0, false, overDelay);
  toolbarItem->addWidget(pWCb);
  return toolbarItem;
}

gdToolbarItem* gdToolbar::newToolbarItem(std::string imgPath, std::string label, std::string tooltip, const int overDelay)
{
  gdToolbarItem* toolbarItem = new gdToolbarItem(0, false, overDelay);
  buildToolbarItem(toolbarItem, imgPath, label, tooltip);
  return toolbarItem;
}

gdToolbarItem* gdToolbar::addToolbarItem(std::string imgPath, std::string label, std::string tooltip, const int overDelay)
{
  gdToolbarItem* toolbarItem = new gdToolbarItem(leftToolbar, false, overDelay);
  buildToolbarItem(toolbarItem, imgPath, label, tooltip);
  return toolbarItem;
}

gdToolbarItem* gdToolbar::addToolbarAnchor(std::string imgPath, std::string label, std::string tooltip, const int overDelay)
{
  gdToolbarItem* toolbarItem = new gdToolbarItem(leftToolbar, true, overDelay);
  buildToolbarItem(toolbarItem, imgPath, label, tooltip);
  return toolbarItem;
}

gdToolbarItem* gdToolbar::addRightToolbarItem(std::string imgPath, std::string label, std::string tooltip, const int overDelay)
{
  gdToolbarItem* toolbarItem = new gdToolbarItem(rightToolbar, false, overDelay);
  buildToolbarItem(toolbarItem, imgPath, label, tooltip);
  return toolbarItem;
}

void gdToolbar::buildToolbarItem(gdToolbarItem* toolbarItem, std::string imgPath, std::string label, std::string tooltip)
{
  toolbarItem->setToolTip(tooltip);
  if ( imgPath != "" )
    toolbarItem->setIcon(imgPath);
  if ( label != "" )
    toolbarItem->setText(label);
}

