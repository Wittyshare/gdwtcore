#include "gdTextImageDelegate.h"

using namespace Wt;

gdTextImageCustomItem::gdTextImageCustomItem(const WModelIndex& index, std::string& imageRef, bool bFirstRowReserved)
 : index_(index), m_bFirstRowReserved(bFirstRowReserved)
 {
  image_ = new WImage(imageRef, this);
  image_->clicked().connect(SLOT(this, gdTextImageCustomItem::onClicked));

  setAttributeValue("style", "line-height: 1em;"); // Otherwise the Image overflows the cell
  setSelectable(true);                             // WTreeView by default disables selection
 }

void gdTextImageCustomItem::setIndex(const WModelIndex& index)
 { index_ = index; }

WImage*gdTextImageCustomItem::image() const
 { return image_; }

void gdTextImageCustomItem::onClicked()
 {
  WAbstractItemModel* model = const_cast<WAbstractItemModel *>(index_.model());
  if ( m_bFirstRowReserved )
     if ( index_.row() == 0 )
        return;
// TODO : remplacer par un event
  model->removeRows(index_.row(), 1);
 }




gdTextImageDelegate::gdTextImageDelegate(std::string imageRef, WObject *parent)
 : WItemDelegate(parent), imageRef_(imageRef), m_bFirstRowReserved(false)
 { }

void gdTextImageDelegate::setFirstRowReserved(bool bReserved)
 { m_bFirstRowReserved = bReserved; }

bool gdTextImageDelegate::isFirstRowReserved()
 { return m_bFirstRowReserved; }

WWidget* gdTextImageDelegate::update(WWidget *widget, const WModelIndex& index, WFlags<ViewItemRenderFlag> flags)
 {
  gdTextImageCustomItem *item;

  if (widget) {
     if ( isFirstRowReserved() )
        if ( index.row() == 0 )
           return widget; // On empeche la recherche sur cette colonne
     item = dynamic_cast<gdTextImageCustomItem *>(widget);
    } else {
     if ( isFirstRowReserved() )
        if ( index.row() == 0 )
           return WItemDelegate::update(widget, index, flags);
     item = new gdTextImageCustomItem(index, imageRef_, m_bFirstRowReserved);
     widget = item;
    }

  return widget;
 }

void gdTextImageDelegate::updateModelIndex(WWidget *widget, const WModelIndex& index)
 {
  gdTextImageCustomItem *item = dynamic_cast<gdTextImageCustomItem *>(widget);
  item->setIndex(index);
  WItemDelegate::updateModelIndex(widget, index);
 }

