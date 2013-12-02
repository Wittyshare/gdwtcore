#ifndef __gDoubledSpinBoxDelegate__
#define __gDoubledSpinBoxDelegate__

#include <Wt/WContainerWidget>
#include <Wt/WWidget>
#include <Wt/WAbstractItemModel>
#include <Wt/WModelIndex>
#include <Wt/WItemDelegate>
#include <Wt/WDoubleSpinBox>

// A custom item widget that contains only a WDoubleSpinBox
class gdDoubleSpinBoxCustomItem : public Wt::WContainerWidget
 {
  public:
              gdDoubleSpinBoxCustomItem(const Wt::WModelIndex& index, double pMin, double pMax, double pStep);

   // Updates the model index
   void       setIndex(const Wt::WModelIndex& index);

   // Returns the editor
   Wt::WDoubleSpinBox* wsb() const;

  private:
   Wt::WDoubleSpinBox*   m_spb;
   Wt::WModelIndex       m_idx;
   bool                  m_bOnChange;

   // Updates the model with the selected value
   void onChanged();
};


// A custom delegate that uses an SpinBox
class gDoubledSpinBoxDelegate : public Wt::WItemDelegate
 {
  public:
            gDoubledSpinBoxDelegate(double pMin, double pMax, double pStep, Wt::WObject* parent = 0);
   void     setFirstRowReserved(bool bReserved);
   bool     isFirstRowReserved();

   // Creates or updates an gdDoubleSpinBoxCustomItem
   Wt::WWidget* update(Wt::WWidget* widget, const Wt::WModelIndex& index, Wt::WFlags<Wt::ViewItemRenderFlag> flags);

   // Updates the model index of the gdDoubleSpinBoxCustomItem
   void     updateModelIndex(Wt::WWidget* widget, const Wt::WModelIndex& index);

  private:
   double      m_dblMin;
   double      m_dblMax;
   double      m_dblStep;
   bool        m_bFirstRowReserved;
};

#endif // __gDoubledSpinBoxDelegate__

