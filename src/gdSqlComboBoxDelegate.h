#ifndef __gdSqlComboBoxDelegateDelegate__
#define __gdSqlComboBoxDelegateDelegate__

#include <Wt/WContainerWidget>
#include <Wt/WWidget>
#include <Wt/WComboBox>
#include <Wt/WAbstractItemModel>
#include <Wt/WStandardItemModel>
#include <Wt/WModelIndex>
#include <Wt/WItemDelegate>

#include <gdcore/gdCoreSql.h>

#include "gdSqlModel.h"

// A custom item widget that contains only a Wt::WComboBox
class gdSqlComboBoxCustomItem : public Wt::WContainerWidget {
public :
  // Contructor
  gdSqlComboBoxCustomItem(int colModel, const Wt::WModelIndex& index, Wt::WStandardItemModel* model, bool bFirstRowReserved = false);

  // Load the rows.
  virtual void       loadItems();

  // Updates the model index
  void           setIndex(const Wt::WModelIndex& index);

  // get the idex of the current object
  Wt::WModelIndex&   index();

  // Returns the editor
  Wt::WComboBox*     wcb() const;

  // Activate the debugging mode
  void           setDebugging(bool bDebug);

private :
  // Updates the model with the selected value
  void           onChanged();

private :
  Wt::WComboBox*          m_cbb;
  Wt::WModelIndex         m_idx;
  int                     m_colModel;
  Wt::WStandardItemModel* m_pCbbDelModel;
  bool                    m_bFirstRowReserved;
  bool                    m_bDebug;
};




// A custom delegate that use a ComboBox
class gdSqlComboBoxDelegate : public Wt::WItemDelegate {
public :
  // Constructor
  gdSqlComboBoxDelegate(gdCoreSql* pSql, std::string strQuery, int colModel, Wt::WObject* parent = 0);
  void     setFirstRowReserved(bool bReserved);
  bool     isFirstRowReserved();
  // Destructor
  ~gdSqlComboBoxDelegate();
  // Creates or updates a gdSqlComboBoxCustomItem
  Wt::WWidget* update(Wt::WWidget* widget, const Wt::WModelIndex& index, Wt::WFlags<Wt::ViewItemRenderFlag> flags);
  // Updates the model index of the gdSqlComboBoxCustomItem
  void     updateModelIndex(Wt::WWidget* widget, const Wt::WModelIndex& index);

  // Set the edit state
  virtual void setEditState(Wt::WWidget* widget, const boost::any& value) const;

  // Request the value of the editor.
  virtual boost::any editState(Wt::WWidget* widget) const;

  // Activate the debugging mode
  void           setDebugging(bool bDebug);


private :
  gdCoreSql*               m_pSql;
  std::string              m_query;
  int                      m_colModel;
  gdSqlModel*              m_pModel;
  bool                     m_bFirstRowReserved;
  bool                     m_bDebug;
};

#endif // __gdSqlComboBoxDelegateDelegate__

