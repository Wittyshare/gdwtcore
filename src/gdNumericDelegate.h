#ifndef __gdNumericDelegate__
#define __gdNumericDelegate__

#include <Wt/WContainerWidget>
#include <Wt/WWidget>
#include <Wt/WAbstractItemModel>
#include <Wt/WModelIndex>
#include <Wt/WItemDelegate>
#include <Wt/WLineEdit>
#include <Wt/WDoubleValidator>

// A custom item widget that contains only a Wt::WLineEdit
class gdNumericCustomItem : public Wt::WContainerWidget
 {
  public:
              gdNumericCustomItem(const Wt::WModelIndex& index, double pMin, double pMax, int precision);

   // Updates the model index
   void       setIndex(const Wt::WModelIndex& index);

   // Returns the editor
   Wt::WLineEdit* edit() const;

  private:
   Wt::WLineEdit  *edit_;
   Wt::WModelIndex index_;
   int             m_nPrecision;

   // Updates the model with the edited value
   void onEdit();
 };


// A custom delegate that uses an gdNumericCustomItem
class gdNumericDelegate : public Wt::WItemDelegate
 {
  public:
          gdNumericDelegate(double pMin, double pMax, WObject *parent = 0);
   void   setPrecision(int precision);
   void   setFirstRowReserved(bool bReserved);
   bool   isFirstRowReserved();

  // Creates or updates an gdNumericCustomItem
  Wt::WWidget* update(Wt::WWidget *widget, const Wt::WModelIndex& index, Wt::WFlags<Wt::ViewItemRenderFlag> flags);

  // Updates the model index of the gdNumericCustomItem
  void updateModelIndex(Wt::WWidget *widget, const Wt::WModelIndex& index);

  private:
   double      m_dblMin;
   double      m_dblMax;
   int         m_nPrecision;
   bool        m_bFirstRowReserved;
 };

#endif // __gdNumericDelegate__

