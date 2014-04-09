#ifndef __gdComboBoxDelegate__
#define __gdComboBoxDelegate__

#include <Wt/WContainerWidget>
#include <Wt/WWidget>
#include <Wt/WAbstractItemModel>
#include <Wt/WModelIndex>
#include <Wt/WItemDelegate>
#include <Wt/WComboBox>

// A custom item widget that contains only a WComboBox
class gdComboBoxCustomItem : public Wt::WContainerWidget {
public:
  /*! \brief Constructor.

  */
  gdComboBoxCustomItem(const Wt::WModelIndex& index, bool bFirstRowReserved = false);

  // Load the rows.
  virtual void       loadItems(std::vector<std::string>& labels);

  // Updates the model index
  void               setIndex(const Wt::WModelIndex& index);

  // get the idex of the current object
  Wt::WModelIndex&   index();

  // Returns the combo box object
  Wt::WComboBox*     wcb() const;

  // Update the current index of the combo box
  void               updateCurrentIndex();

private slots :
  // Updates the model with the selected value
  void               onChanged();

private :
  Wt::WComboBox*           m_cbb;
  Wt::WModelIndex          m_idx;
  bool                     m_bFirstRowReserved;
  bool                     m_bBusy;
};


// A custom delegate that uses an ComboBox
class gdComboBoxDelegate : public Wt::WItemDelegate {
public:
  gdComboBoxDelegate(std::vector<std::string> labels, Wt::WObject* parent = 0);
  void     setFirstRowReserved(bool bReserved);
  bool     isFirstRowReserved();

  // Creates or updates an gdComboBoxCustomItem
  Wt::WWidget* update(Wt::WWidget* widget, const Wt::WModelIndex& index, Wt::WFlags<Wt::ViewItemRenderFlag> flags);

  // Updates the model index of the gdComboBoxCustomItem
  void     updateModelIndex(Wt::WWidget* widget, const Wt::WModelIndex& index);

  // Set the edit state
  virtual void setEditState(Wt::WWidget* widget, const boost::any& value) const;

  // Request the value of the editor.
  virtual boost::any editState(Wt::WWidget* widget) const;

private:
  std::vector<std::string> m_labels;
  bool                     m_bFirstRowReserved;
};

#endif //  __gdComboBoxDelegate__

