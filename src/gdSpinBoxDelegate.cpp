#include <stdio.h>

#include <Wt/WHBoxLayout>

#include <gdcore/gdCore.h>

#include "gdSpinBoxDelegate.h"

using namespace Wt;

gdSpinBoxCustomItem::gdSpinBoxCustomItem(const WModelIndex& index, long pMin, long pMax, long pStep)
                    : m_idx(index), m_spb(0), m_bOnChange(false)
 {
  m_spb = new WSpinBox(this);
  m_spb->setMinimum(pMin);
  m_spb->setMaximum(pMax);
  m_spb->setSingleStep(pStep);
  m_spb->setAttributeValue("style", "text-align: right;"); // right alignment
  m_spb->valueChanged().connect(SLOT(this, gdSpinBoxCustomItem::onChanged));

  setLayout(new WHBoxLayout());
  layout()->setContentsMargins(1, 1, 1, 1);
  layout()->addWidget(m_spb);

  setSelectable(true);                             // WTreeView by default disables selection
 }

void gdSpinBoxCustomItem::setIndex(const WModelIndex& index)
 { m_idx = index; }

WSpinBox* gdSpinBoxCustomItem::wsb() const
 { return m_spb; }

void gdSpinBoxCustomItem::onChanged()
 {
  if ( m_bOnChange ) return;
  m_bOnChange = true;
  WAbstractItemModel*  model = const_cast<WAbstractItemModel*>(m_idx.model());
  
  long   val = m_spb->value();
  model->setData(m_idx, val, DisplayRole);
  model->setData(m_idx, val, UserRole);
  m_bOnChange = false;
 }




gdSpinBoxDelegate::gdSpinBoxDelegate(double pMin, long pMax, long pStep, WObject* parent)
                  : WItemDelegate(parent), m_lMin(pMin), m_lMax(pMax), m_lStep(pStep), m_bFirstRowReserved(false)
 { }

void gdSpinBoxDelegate::setFirstRowReserved(bool bReserved)
 { m_bFirstRowReserved = bReserved; }

bool gdSpinBoxDelegate::isFirstRowReserved()
 { return m_bFirstRowReserved; }

WWidget* gdSpinBoxDelegate::update(WWidget* widget, const WModelIndex& index, WFlags<ViewItemRenderFlag> flags)
 {
  gdSpinBoxCustomItem*   item;
  
  if ( widget )
    {
     if ( isFirstRowReserved() )
        if ( index.row() == 0 ) {
           return WItemDelegate::update(widget, index, flags);
          }
  	item = dynamic_cast<gdSpinBoxCustomItem*>(widget);
    }
  else
    {
     if ( isFirstRowReserved() )
        if ( index.row() == 0 ) {
           return WItemDelegate::update(widget, index, flags);
       }
     item = new gdSpinBoxCustomItem(index, m_lMin, m_lMax, m_lStep);
     widget = item;
     item->wsb()->resize(WLength( 95, WLength::Percentage), WLength(100, WLength::Percentage));
    }
  
  long   val     = asNumber(index.data(UserRole));
  item->wsb()->setValue(val);
  
  return widget;
 }

void gdSpinBoxDelegate::updateModelIndex(WWidget* widget, const WModelIndex& index)
 {
  if ( isFirstRowReserved() )
     if ( index.row() == 0 ) {
        WItemDelegate* pItem = dynamic_cast<WItemDelegate*>(widget);
        return pItem->updateModelIndex(widget, index);
       }
  gdSpinBoxCustomItem*    item = dynamic_cast<gdSpinBoxCustomItem*>(widget);
  item->setIndex(index);
  WItemDelegate::updateModelIndex(widget, index);
 }

