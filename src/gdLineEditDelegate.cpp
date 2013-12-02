#include "gdLineEditDelegate.h"

using namespace Wt;

gdLineEditCustomItem::gdLineEditCustomItem(const WModelIndex& index, bool bFirstRowReserved)
 : index_(index), m_bFirstRowReserved(bFirstRowReserved)
 {
  edit_ = new WLineEdit(this);
  edit_->changed().connect(SLOT(this, gdLineEditCustomItem::onEdit));

  setAttributeValue("style", "line-height: 1em;"); // Otherwise the line edit overflows the cell
  setSelectable(true);                             // WTreeView by default disables selection
 }

// Updates the model index
void gdLineEditCustomItem::setIndex(const WModelIndex& index)
 { index_ = index; }

// Returns the editor
WLineEdit* gdLineEditCustomItem::edit() const
 { return edit_; }

// Updates the model with the edited value
void gdLineEditCustomItem::onEdit()
 {
  WAbstractItemModel* model = const_cast<WAbstractItemModel *>(index_.model());

  if ( m_bFirstRowReserved )
     if ( index_.row() == 0 )
        return;

  model->setData(index_, edit_->text(), DisplayRole);
  model->setData(index_, edit_->text(), UserRole);
 }


gdLineEditDelegate::gdLineEditDelegate(WObject *parent)
 : WItemDelegate(parent), m_bFirstRowReserved(false)
 { }

void gdLineEditDelegate::setFirstRowReserved(bool bReserved)
 { m_bFirstRowReserved = bReserved; }

bool gdLineEditDelegate::isFirstRowReserved()
 { return m_bFirstRowReserved; }

WWidget* gdLineEditDelegate::update(WWidget *widget, const WModelIndex& index, WFlags<ViewItemRenderFlag> flags)
 {
  gdLineEditCustomItem *item;

  if (widget) {
     if ( isFirstRowReserved() )
        if ( index.row() == 0 )
           return WItemDelegate::update(widget, index, flags);
     item = dynamic_cast<gdLineEditCustomItem *>(widget);
  } else {
     if ( isFirstRowReserved() )
        if ( index.row() == 0 )
           return WItemDelegate::update(widget, index, flags);
     item = new gdLineEditCustomItem(index);
     widget = item;
     item->edit()->resize(WLength( 95, WLength::Percentage), WLength(100, WLength::Percentage));
  }

  boost::any text = index.data(DisplayRole);
  item->edit()->setText(asString(text));

  return widget;
 }

// Updates the model index of the gdLineEditCustomItem
void gdLineEditDelegate::updateModelIndex(WWidget *widget, const WModelIndex& index)
 {
  if ( isFirstRowReserved() )
     if ( index.row() == 0 ) {
        WItemDelegate* pItem = dynamic_cast<WItemDelegate*>(widget);
        return pItem->updateModelIndex(widget, index);
       }
  gdLineEditCustomItem *item = dynamic_cast<gdLineEditCustomItem *>(widget);
  item->setIndex(index);
  WItemDelegate::updateModelIndex(widget, index);
 }

