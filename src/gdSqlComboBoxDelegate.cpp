#include <Wt/WApplication>
#include <Wt/WLogger>

#include "gdSqlComboBoxDelegate.h"

using namespace Wt;

gdSqlComboBoxCustomItem::gdSqlComboBoxCustomItem(int colModel, const WModelIndex& index, WStandardItemModel* model, bool bFirstRowReserved)
 : m_idx(index), m_cbb(0), m_colModel(colModel), m_pCbbDelModel(model), m_bDebug(false)
 {
  m_cbb = new WComboBox(this);

  setAttributeValue("style", "line-height: 1em;"); // Otherwise the combobox overflows the cell
  setSelectable(true);                             // WTreeView by default disables selection
 }

void gdSqlComboBoxCustomItem::loadItems()
 {
  for(int row = 0; row < m_pCbbDelModel->rowCount(); row++)
    m_cbb->addItem(asString(m_pCbbDelModel->item(row, m_colModel)->data()));
  long         k_id     = asNumber(m_idx.data(UserRole));

  for(long row = 0; row < m_pCbbDelModel->rowCount(); row++)
   {
    long delk_id =asNumber(m_pCbbDelModel->data(row, 0, UserRole));
    if ( delk_id != k_id ) continue;
    m_cbb->setCurrentIndex(row);
    if ( m_bDebug )
       wApp->log("notice") << " gdSqlComboBoxCustomItem::loadItems Initial index : " << row << " and m_pCbbDelModel = " << m_pCbbDelModel;
    break;
   }
  m_cbb->activated().connect(SLOT(this, gdSqlComboBoxCustomItem::onChanged));
 }

// Updates the model index
void gdSqlComboBoxCustomItem::setIndex(const WModelIndex& index)
 { m_idx = index; }

WModelIndex& gdSqlComboBoxCustomItem::index()
 { return m_idx; }

// Returns the editor
WComboBox* gdSqlComboBoxCustomItem::wcb() const
 { return m_cbb; }

// Activate the debugging
void gdSqlComboBoxCustomItem::setDebugging(bool bDebug)
 { m_bDebug = bDebug; }

// Updates the model with the selected value
void gdSqlComboBoxCustomItem::onChanged()
 {
  long selId = m_cbb->currentIndex();
  if ( m_bDebug )
     wApp->log("notice") << " gdSqlComboBoxCustomItem::onChanged selId : " << selId;
  WAbstractItemModel*  model = const_cast<WAbstractItemModel*>(m_idx.model());
  long k_id = asNumber(m_pCbbDelModel->data(selId, 0, UserRole));
  if ( m_bDebug )
     wApp->log("notice") << " gdSqlComboBoxCustomItem::onChanged k_id : " << k_id;
  model->setData(m_idx, k_id, UserRole);
 }






gdSqlComboBoxDelegate::gdSqlComboBoxDelegate(gdCoreSql* pSql, std::string strQuery, int colModel, WObject* parent)
 : WItemDelegate(parent), m_pSql(pSql), m_query(strQuery), m_colModel(colModel), m_bFirstRowReserved(false), m_bDebug(false)
 {
  m_pModel = new gdSqlModel();
  m_pModel->setSql(pSql);
  m_pModel->setQuery(m_query);
  m_pModel->reload();
 }

gdSqlComboBoxDelegate::~gdSqlComboBoxDelegate()
 {
  delete m_pModel;
  m_pModel = 0;
 }

void gdSqlComboBoxDelegate::setFirstRowReserved(bool bReserved)
 { m_bFirstRowReserved = bReserved; }

bool gdSqlComboBoxDelegate::isFirstRowReserved()
 { return m_bFirstRowReserved; }

// Activate the debugging
void gdSqlComboBoxDelegate::setDebugging(bool bDebug)
 { m_bDebug = bDebug; }

// Creates or updates an gdSqlComboBoxCustomItem
WWidget* gdSqlComboBoxDelegate::update(WWidget* widget, const WModelIndex& index, WFlags<ViewItemRenderFlag> flags)
 {
  gdSqlComboBoxCustomItem*   item;

  if ( widget )
    {
     if ( m_bDebug )
        wApp->log("notice") << " gdSqlComboBoxDelegate::update existing item : " << this;
     item = dynamic_cast<gdSqlComboBoxCustomItem*>(widget);
    }
   else
    {
     if ( m_bDebug )
        wApp->log("notice") << " gdSqlComboBoxDelegate::update new item : " << this << " and index.model = " << index.model();
     item = new gdSqlComboBoxCustomItem(m_colModel, index, m_pModel);
     item->setDebugging(m_bDebug);
     item->loadItems();
     widget = item;
     item->wcb()->resize(WLength( 95, WLength::Percentage), WLength(100, WLength::Percentage));
    }

  return widget;
 }

// Updates the model index of the gdSqlComboBoxCustomItem
void gdSqlComboBoxDelegate::updateModelIndex(WWidget* widget, const WModelIndex& index)
 {
  if ( isFirstRowReserved() )
     if ( index.row() == 0 ) {
        if ( m_bDebug )
           wApp->log("notice") << " gdSqlComboBoxDelegate::updateModelIndex firstRow reserved and row = 0 ";
        WItemDelegate* pItem = dynamic_cast<WItemDelegate*>(widget);
        return pItem->updateModelIndex(widget, index);
       }
  if ( m_bDebug )
     wApp->log("notice") << " gdSqlComboBoxDelegate::updateModelIndex row =  " << index.row();
  gdSqlComboBoxCustomItem*    item = dynamic_cast<gdSqlComboBoxCustomItem*>(widget);
  item->setIndex(index);
  WItemDelegate::updateModelIndex(widget, index);
 }

void gdSqlComboBoxDelegate::setEditState(WWidget* widget, const boost::any &value) const
 {
  gdSqlComboBoxCustomItem*    item = dynamic_cast<gdSqlComboBoxCustomItem*>(widget);
  if ( m_bFirstRowReserved )
     if ( item->index().row() == 0 ) {
        if ( m_bDebug )
           wApp->log("notice") << " gdSqlComboBoxDelegate::setEditState firstRow reserved and row = 0 ";
        return;
       }
  if ( m_bDebug )
     wApp->log("notice") << " gdSqlComboBoxDelegate::setEditState call base implementation";
  WItemDelegate::setEditState(widget, value);
 }

boost::any gdSqlComboBoxDelegate::editState(WWidget* widget) const
 {
  gdSqlComboBoxCustomItem*    item = dynamic_cast<gdSqlComboBoxCustomItem*>(widget);
  if ( m_bFirstRowReserved )
     if ( item->index().row() != 0 ) {
        if ( m_bDebug )
           wApp->log("notice") << " gdSqlComboBoxDelegate::editState  firstRow reserved and row != 0";
        return WItemDelegate::editState(widget);
       }
  if ( m_bDebug )
     wApp->log("notice") << " gdSqlComboBoxDelegate::editState call currentIndex";
  return item->wcb()->currentIndex();
 }

