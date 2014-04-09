#include "gdImageDelegate.h"

using namespace Wt;

gdImageCustomItem::gdImageCustomItem(const WModelIndex& index, std::string& imageRef, bool bFirstRowReserved)
  : index_(index), m_bFirstRowReserved(bFirstRowReserved)
{
  image_ = new WImage(imageRef, this);
  image_->clicked().connect(SLOT(this, gdImageCustomItem::onClicked));
  setAttributeValue("style", "line-height: 1em;"); // Otherwise the Image overflows the cell
  setSelectable(true);                             // WTreeView by default disables selection
}

void gdImageCustomItem::setIndex(const WModelIndex& index)
{
  index_ = index;
}

WImage* gdImageCustomItem::image() const
{
  return image_;
}

void gdImageCustomItem::onClicked()
{
  WAbstractItemModel* model = const_cast<WAbstractItemModel*>(index_.model());
  if ( m_bFirstRowReserved )
    if ( index_.row() == 0 )
      return;
  // TODO : remplacer par un event
  model->removeRows(index_.row(), 1);
}




gdImageDelegate::gdImageDelegate(std::string imageRef, WObject* parent)
  : WItemDelegate(parent), imageRef_(imageRef), m_bFirstRowReserved(false)
{ }

void gdImageDelegate::setFirstRowReserved(bool bReserved)
{
  m_bFirstRowReserved = bReserved;
}

bool gdImageDelegate::isFirstRowReserved()
{
  return m_bFirstRowReserved;
}

WWidget* gdImageDelegate::update(WWidget* widget, const WModelIndex& index, WFlags<ViewItemRenderFlag> flags)
{
  gdImageCustomItem* item;
  if (widget) {
    if ( isFirstRowReserved() )
      if ( index.row() == 0 )
        return widget; // On empeche la recherche sur cette colonne
    item = dynamic_cast<gdImageCustomItem*>(widget);
  } else {
    if ( isFirstRowReserved() )
      if ( index.row() == 0 )
        return WItemDelegate::update(widget, index, flags);
    item = new gdImageCustomItem(index, imageRef_, m_bFirstRowReserved);
    widget = item;
  }
  return widget;
}

void gdImageDelegate::updateModelIndex(WWidget* widget, const WModelIndex& index)
{
  gdImageCustomItem* item = dynamic_cast<gdImageCustomItem*>(widget);
  item->setIndex(index);
  WItemDelegate::updateModelIndex(widget, index);
}

