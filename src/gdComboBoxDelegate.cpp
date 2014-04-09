#include <stdio.h>

#include <Wt/WStandardItemModel>
#include <Wt/WStandardItem>

#include "gdComboBoxDelegate.h"

using namespace Wt;

gdComboBoxCustomItem::gdComboBoxCustomItem(const WModelIndex& index, bool bFirstRowReserved)
  : m_idx(index), m_cbb(0), m_bFirstRowReserved(bFirstRowReserved), m_bBusy(false)
{
  m_cbb = new WComboBox(this);
  setAttributeValue("style", "line-height: 1em;"); // Otherwise the line edit overflows the cell
  setSelectable(true);                             // WTreeView by default disables selection
}

void gdComboBoxCustomItem::loadItems(std::vector<std::string>& labels)
{
  WStandardItemModel*   model = new WStandardItemModel(0, 2, this);
  if ( m_bFirstRowReserved )
    if ( m_idx.row() == 0 ) {
      long lKid = 0;
      std::vector<WStandardItem*> pRow;
      WStandardItem* pItem = new WStandardItem(WString());
      pItem->setData(lKid, UserRole);
      pRow.push_back(pItem);
      pItem = new WStandardItem(WString("All"));
      pItem->setData(WString("All"), UserRole);
      pRow.push_back(pItem);
      model->invisibleRootItem()->appendRow(pRow);
    }
  for (long lRow = 0; lRow < labels.size(); ++lRow) {
    long lKid = lRow;
    ++lKid; // Un Id commence toujours a 1
    WString wstr = WString::fromUTF8(labels[lRow]);
    std::vector<WStandardItem*> pRow;
    WStandardItem* pItem = new WStandardItem();
    pItem->setData(lKid, UserRole);
    pItem->setData(lKid, DisplayRole);
    pRow.push_back(pItem);
    pItem = new WStandardItem(wstr);
    pItem->setData(wstr, UserRole);
    pRow.push_back(pItem);
    model->invisibleRootItem()->appendRow(pRow);
  }
  m_cbb->setModel(model);
  m_cbb->setModelColumn(1);
  m_cbb->activated().connect(SLOT(this, gdComboBoxCustomItem::onChanged));
}

void gdComboBoxCustomItem::setIndex(const WModelIndex& index)
{
  m_idx = index;
}

WModelIndex& gdComboBoxCustomItem::index()
{
  return m_idx;
}

WComboBox* gdComboBoxCustomItem::wcb() const
{
  return m_cbb;
}

void gdComboBoxCustomItem::updateCurrentIndex()
{
  /*
    WString    valModel = asString(m_idx.model()->data(m_idx.row(), m_idx.column(), Wt::UserRole));
    for(int idx = 0; idx < wcb()->model()->rowCount(); ++idx) {
       WString    valCbb = asString(wcb()->model()->data(idx, 1, Wt::UserRole));
       if ( valCbb == valModel ) {
          wcb()->setCurrentIndex(idx); // un id commence toujours a 1
          break;
         }
     }
  */
  int idx       = asNumber(m_idx.model()->data(m_idx.row(), m_idx.column(), UserRole));
  wcb()->setCurrentIndex(idx);
}

void gdComboBoxCustomItem::onChanged()
{
  WAbstractItemModel*  model = const_cast<WAbstractItemModel*>(m_idx.model());
  int idx2 = m_cbb->currentIndex();
  if ( m_bFirstRowReserved && m_idx.row() == 0 ) {
    WString str;
    if ( idx2 )
      str = boost::lexical_cast<std::string>(idx2 - 1);
    model->setData(m_idx, str,  DisplayRole);
  } else
    model->setData(m_idx, idx2, DisplayRole);
  model->setData(m_idx, idx2, UserRole);
}




gdComboBoxDelegate::gdComboBoxDelegate(std::vector<std::string> labels, WObject* parent)
  : WItemDelegate(parent), m_labels(labels), m_bFirstRowReserved(false)
{ }

void gdComboBoxDelegate::setFirstRowReserved(bool bReserved)
{
  m_bFirstRowReserved = bReserved;
}

bool gdComboBoxDelegate::isFirstRowReserved()
{
  return m_bFirstRowReserved;
}

WWidget* gdComboBoxDelegate::update(WWidget* widget, const WModelIndex& index, WFlags<ViewItemRenderFlag> flags)
{
  gdComboBoxCustomItem*   item;
  if (widget) {
    item = dynamic_cast<gdComboBoxCustomItem*>(widget);
  } else {
    item = new gdComboBoxCustomItem(index, isFirstRowReserved());
    item->loadItems(m_labels);
    widget = item;
    item->wcb()->resize(WLength( 95, WLength::Percentage), WLength(100, WLength::Percentage));
  }
  item->updateCurrentIndex();
  return widget;
}

void gdComboBoxDelegate::updateModelIndex(WWidget* widget, const WModelIndex& index)
{
  gdComboBoxCustomItem*    item = dynamic_cast<gdComboBoxCustomItem*>(widget);
  if ( isFirstRowReserved() )
    if ( index.row() == 0 )
      return WItemDelegate::updateModelIndex(widget, index);
  item->setIndex(index);
  WItemDelegate::updateModelIndex(widget, index);
}

void gdComboBoxDelegate::setEditState(WWidget* widget, const boost::any& value) const
{
  gdComboBoxCustomItem*    item = dynamic_cast<gdComboBoxCustomItem*>(widget);
  if ( m_bFirstRowReserved )
    if ( item->index().row() == 0 ) return;
  WItemDelegate::setEditState(widget, value);
}

boost::any gdComboBoxDelegate::editState(WWidget* widget) const
{
  gdComboBoxCustomItem*    item = dynamic_cast<gdComboBoxCustomItem*>(widget);
  if ( m_bFirstRowReserved )
    if ( item->index().row() != 0 )
      return WItemDelegate::editState(widget);
  return item->wcb()->currentIndex();
}

