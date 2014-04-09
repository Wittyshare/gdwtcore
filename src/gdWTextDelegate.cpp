#include <stdio.h>

#include <boost/lexical_cast.hpp>

#include <Wt/WItemDelegate>

#include <gdcore/gdCore.h>

#include "gdWTextDelegate.h"

using namespace Wt;

gdWTextCustomItem::gdWTextCustomItem(const WModelIndex& index)
  : index_(index)
{
  text_ = new WText(this);
  text_->setVerticalAlignment(Wt::AlignMiddle);
  text_->setWordWrap(true);
  setVerticalAlignment(Wt::AlignMiddle);
  //  setAttributeValue("style", "line-height: 1em;"); // Otherwise the line edit overflows the cell
  setSelectable(true);                             // WTreeView by default disables selection
}

void gdWTextCustomItem::setIndex(const WModelIndex& index)
{
  index_ = index;
}

Wt::WText* gdWTextCustomItem::text() const
{
  return text_;
}





gdWTextDelegate::gdWTextDelegate(WObject* parent)
  : WItemDelegate(parent), m_bFirstRowReserved(false), m_rowHeight(38)
{ }

void gdWTextDelegate::setFirstRowReserved(bool bReserved)
{
  m_bFirstRowReserved = bReserved;
}

bool gdWTextDelegate::isFirstRowReserved()
{
  return m_bFirstRowReserved;
}

void gdWTextDelegate::setRowHeight(double rowHeight)
{
  m_rowHeight = rowHeight;
}

WWidget* gdWTextDelegate::update(WWidget* widget, const WModelIndex& index, WFlags<ViewItemRenderFlag> flags)
{
  gdWTextCustomItem* item;
  if ( widget ) {
    if ( isFirstRowReserved() )
      if ( index.row() == 0 ) {
        return WItemDelegate::update(widget, index, flags);
      }
    item = dynamic_cast<gdWTextCustomItem*>(widget);
  } else {
    if ( isFirstRowReserved() )
      if ( index.row() == 0 ) {
        return WItemDelegate::update(widget, index, flags);
      }
    item = new gdWTextCustomItem(index);
    //     item->setHeight(m_rowHeight - 12);
    item->text()->setHeight(m_rowHeight);
    //     item->resize(WLength(100, WLength::Percentage), WLength(100, WLength::Percentage));
    //     item->text()->resize(WLength(100, WLength::Percentage), WLength(80, WLength::Percentage));
    widget = item;
  }
  Wt::WString  myText = asString(index.data(UserRole));
  item->text()->setText(myText);
  return widget;
}

void gdWTextDelegate::updateModelIndex(WWidget* widget, const WModelIndex& index)
{
  if ( isFirstRowReserved() )
    if ( index.row() == 0 ) {
      WItemDelegate* pItem = dynamic_cast<WItemDelegate*>(widget);
      return pItem->updateModelIndex(widget, index);
    }
  gdWTextCustomItem* item = dynamic_cast<gdWTextCustomItem*>(widget);
  item->setIndex(index);
  WItemDelegate::updateModelIndex(widget, index);
}


