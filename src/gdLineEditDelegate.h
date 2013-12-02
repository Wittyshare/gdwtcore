#ifndef __gdLineEditDelegate__
#define __gdLineEditDelegate__

#include <Wt/WContainerWidget>
#include <Wt/WWidget>
#include <Wt/WAbstractItemModel>
#include <Wt/WItemDelegate>
#include <Wt/WModelIndex>
#include <Wt/WLineEdit>

// A custom em widget that contains only a Wt::WLineEdit
class gdLineEditCustomItem : public Wt::WContainerWidget
 {
  public:
                       gdLineEditCustomItem(const Wt::WModelIndex& index, bool bFirstRowReserved = false);

    // Updat the model index
    void               setIndex(const Wt::WModelIndex& index);

    // Retur the editor
    Wt::WLineEdit*     edit() const;

  private slots :
    // Updat the model with the edited value
    void               onEdit();

  private:
   Wt::WLineEdit*           edit_;
   Wt::WModelIndex          index_;
   bool                     m_bFirstRowReserved;
 };

// A custom delegate that uses an gdLineEditCustomItem
class gdLineEditDelegate : public Wt::WItemDelegate
 {
  public:
            gdLineEditDelegate(Wt::WObject *parent = 0);
   void     setFirstRowReserved(bool bReserved);
   bool     isFirstRowReserved();

   // Creates or updates an gdLineEditCustomItem
   Wt::WWidget* update(Wt::WWidget *widget, const Wt::WModelIndex& index, Wt::WFlags<Wt::ViewItemRenderFlag> flags);

  // Updates the model index of the gdLineEditCustomItem
  void updateModelIndex(Wt::WWidget *widget, const Wt::WModelIndex& index);

  private:
   bool                    m_bFirstRowReserved;
 };

#endif // __gdLineEditDelegate__

