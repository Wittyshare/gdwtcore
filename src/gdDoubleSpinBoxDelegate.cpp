#include <stdio.h>

#include <Wt/WHBoxLayout>

#include <gdcore/gdCore.h>

#include "gdDoubleSpinBoxDelegate.h"

using namespace Wt;

gdDoubleSpinBoxCustomItem::gdDoubleSpinBoxCustomItem(const WModelIndex& index, double pMin, double pMax, double pStep)
  : m_idx(index), m_spb(0), m_bOnChange(false)
{
  m_spb = new WDoubleSpinBox(this);
  m_spb->setMinimum(pMin);
  m_spb->setMaximum(pMax);
  m_spb->setSingleStep(pStep);
  m_spb->setDecimals(0);
  m_spb->setAttributeValue("style", "text-align: right;"); // right alignment
  m_spb->valueChanged().connect(SLOT(this, gdDoubleSpinBoxCustomItem::onChanged));
  setLayout(new WHBoxLayout());
  layout()->setContentsMargins(1, 1, 1, 1);
  layout()->addWidget(m_spb);
  setSelectable(true);                             // WTreeView by default disables selection
}

void gdDoubleSpinBoxCustomItem::setIndex(const WModelIndex& index)
{
  m_idx = index;
}

WDoubleSpinBox* gdDoubleSpinBoxCustomItem::wsb() const
{
  return m_spb;
}

void gdDoubleSpinBoxCustomItem::onChanged()
{
  if ( m_bOnChange ) return;
  m_bOnChange = true;
  WAbstractItemModel*  model = const_cast<WAbstractItemModel*>(m_idx.model());
  double val = m_spb->value();
  model->setData(m_idx, gdcore_double2string(val, m_spb->decimals()));
  model->setData(m_idx, val, UserRole);
  m_bOnChange = false;
}




gDoubledSpinBoxDelegate::gDoubledSpinBoxDelegate(double pMin, double pMax, double pStep, WObject* parent)
  : WItemDelegate(parent), m_dblMin(pMin), m_dblMax(pMax), m_dblStep(pStep), m_bFirstRowReserved(false)
{ }

void gDoubledSpinBoxDelegate::setFirstRowReserved(bool bReserved)
{
  m_bFirstRowReserved = bReserved;
}

bool gDoubledSpinBoxDelegate::isFirstRowReserved()
{
  return m_bFirstRowReserved;
}

WWidget* gDoubledSpinBoxDelegate::update(WWidget* widget, const WModelIndex& index, WFlags<ViewItemRenderFlag> flags)
{
  gdDoubleSpinBoxCustomItem*   item;
  if ( widget ) {
    if ( isFirstRowReserved() )
      if ( index.row() == 0 ) {
        return WItemDelegate::update(widget, index, flags);
      }
    item = dynamic_cast<gdDoubleSpinBoxCustomItem*>(widget);
  } else {
    if ( isFirstRowReserved() )
      if ( index.row() == 0 ) {
        return WItemDelegate::update(widget, index, flags);
      }
    item = new gdDoubleSpinBoxCustomItem(index, m_dblMin, m_dblMax, m_dblStep);
    widget = item;
    item->wsb()->resize(WLength( 95, WLength::Percentage), WLength(100, WLength::Percentage));
  }
  double val     = asNumber(index.data(UserRole));
  item->wsb()->setValue(val);
  return widget;
}

void gDoubledSpinBoxDelegate::updateModelIndex(WWidget* widget, const WModelIndex& index)
{
  if ( isFirstRowReserved() )
    if ( index.row() == 0 ) {
      WItemDelegate* pItem = dynamic_cast<WItemDelegate*>(widget);
      return pItem->updateModelIndex(widget, index);
    }
  gdDoubleSpinBoxCustomItem*    item = dynamic_cast<gdDoubleSpinBoxCustomItem*>(widget);
  item->setIndex(index);
  WItemDelegate::updateModelIndex(widget, index);
}

