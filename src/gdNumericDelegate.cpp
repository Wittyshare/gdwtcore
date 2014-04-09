#include <stdio.h>

#include <boost/lexical_cast.hpp>

#include <Wt/WItemDelegate>

#include <gdcore/gdCore.h>

#include "gdNumericDelegate.h"

using namespace Wt;

gdNumericCustomItem::gdNumericCustomItem(const WModelIndex& index, double pMin, double pMax, int precision)
  : index_(index), m_nPrecision(precision)
{
  edit_ = new WLineEdit(this);
  if ( pMin || pMax ) {
    WDoubleValidator* pDoubleVal = new WDoubleValidator(pMin, pMax);
    edit_->setValidator(pDoubleVal);
  }
  edit_->setAttributeValue("style", "text-align: right;"); // right alignment
  edit_->changed().connect(SLOT(this, gdNumericCustomItem::onEdit));
  setAttributeValue("style", "line-height: 1em;"); // Otherwise the line edit overflows the cell
  setSelectable(true);                             // WTreeView by default disables selection
}

void gdNumericCustomItem::setIndex(const WModelIndex& index)
{
  index_ = index;
}

WLineEdit* gdNumericCustomItem::edit() const
{
  return edit_;
}

void gdNumericCustomItem::onEdit()
{
  WAbstractItemModel* model = const_cast<WAbstractItemModel*>(index_.model());
  double theDouble = asNumber(edit_->text());
  model->setData(index_, gdcore_double2string(theDouble, m_nPrecision));
  model->setData(index_, theDouble, UserRole);
}




gdNumericDelegate::gdNumericDelegate(double pMin, double pMax, WObject* parent)
  : WItemDelegate(parent), m_dblMin(pMin), m_dblMax(pMax), m_nPrecision(2), m_bFirstRowReserved(false)
{ }

void gdNumericDelegate::setPrecision(int precision)
{
  m_nPrecision = precision;
}

void gdNumericDelegate::setFirstRowReserved(bool bReserved)
{
  m_bFirstRowReserved = bReserved;
}

bool gdNumericDelegate::isFirstRowReserved()
{
  return m_bFirstRowReserved;
}

WWidget* gdNumericDelegate::update(WWidget* widget, const WModelIndex& index, WFlags<ViewItemRenderFlag> flags)
{
  gdNumericCustomItem* item;
  if ( widget ) {
    if ( isFirstRowReserved() )
      if ( index.row() == 0 ) {
        return WItemDelegate::update(widget, index, flags);
      }
    item = dynamic_cast<gdNumericCustomItem*>(widget);
  } else {
    if ( isFirstRowReserved() )
      if ( index.row() == 0 ) {
        return WItemDelegate::update(widget, index, flags);
      }
    item = new gdNumericCustomItem(index, m_dblMin, m_dblMax, m_nPrecision);
    widget = item;
    item->edit()->resize(WLength( 95, WLength::Percentage), WLength(100, WLength::Percentage));
  }
  double theDouble = asNumber(index.data(UserRole));
  item->edit()->setText(gdcore_double2string(theDouble, m_nPrecision));
  return widget;
}

void gdNumericDelegate::updateModelIndex(WWidget* widget, const WModelIndex& index)
{
  if ( isFirstRowReserved() )
    if ( index.row() == 0 ) {
      WItemDelegate* pItem = dynamic_cast<WItemDelegate*>(widget);
      return pItem->updateModelIndex(widget, index);
    }
  gdNumericCustomItem* item = dynamic_cast<gdNumericCustomItem*>(widget);
  item->setIndex(index);
  WItemDelegate::updateModelIndex(widget, index);
}


