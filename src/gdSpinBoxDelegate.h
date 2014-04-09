#ifndef __gdSpinBoxDelegate__
#define __gdSpinBoxDelegate__

#include <Wt/WContainerWidget>
#include <Wt/WWidget>
#include <Wt/WAbstractItemModel>
#include <Wt/WModelIndex>
#include <Wt/WItemDelegate>
#include <Wt/WSpinBox>

// A custom item widget that contains only a Wt::WDoubleSpinBox
class gdSpinBoxCustomItem : public Wt::WContainerWidget {
public:
  gdSpinBoxCustomItem(const Wt::WModelIndex& index, long pMin, long pMax, long pStep);

  // Updates the model index
  void       setIndex(const Wt::WModelIndex& index);

  // Returns the editor
  Wt::WSpinBox* wsb() const;

private:
  Wt::WSpinBox*         m_spb;
  Wt::WModelIndex       m_idx;
  bool                  m_bOnChange;

  // Updates the model with the selected value
  void onChanged();
};


// A custom delegate that uses an SpinBox
class gdSpinBoxDelegate : public Wt::WItemDelegate {
public:
  gdSpinBoxDelegate(double pMin, long pMax, long pStep, WObject* parent = 0);
  void     setFirstRowReserved(bool bReserved);
  bool     isFirstRowReserved();

  // Creates or updates an gdSpinBoxCustomItem
  Wt::WWidget* update(Wt::WWidget* widget, const Wt::WModelIndex& index, Wt::WFlags<Wt::ViewItemRenderFlag> flags);

  // Updates the model index of the gdSpinBoxCustomItem
  void     updateModelIndex(Wt::WWidget* widget, const Wt::WModelIndex& index);

private:
  long        m_lMin;
  long        m_lMax;
  long        m_lStep;
  bool        m_bFirstRowReserved;
};

#endif // __gdSpinBoxDelegate__

