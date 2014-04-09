#ifndef __gdWTextDelegate__
#define __gdWTextDelegate__

#include <Wt/WContainerWidget>
#include <Wt/WWidget>
#include <Wt/WAbstractItemModel>
#include <Wt/WModelIndex>
#include <Wt/WItemDelegate>
#include <Wt/WText>
#include <Wt/WDoubleValidator>

// A custom item widget that contains only a Wt::WText
class gdWTextCustomItem : public Wt::WContainerWidget {
public:
  gdWTextCustomItem(const Wt::WModelIndex& index);

  // Updates the model index
  void       setIndex(const Wt::WModelIndex& index);

  // Returns the editor
  Wt::WText*     text() const;

private:
  Wt::WText*      text_;
  Wt::WModelIndex index_;

  // Updates the model with the edited value
  void onText();
};


// A custom delegate that uses an gdWTextCustomItem
class gdWTextDelegate : public Wt::WItemDelegate {
public:
  gdWTextDelegate(WObject* parent = 0);
  void   setFirstRowReserved(bool bReserved);
  bool   isFirstRowReserved();
  void   setRowHeight(double rowHeight);

  // Creates or updates an gdWTextCustomItem
  Wt::WWidget* update(Wt::WWidget* widget, const Wt::WModelIndex& index, Wt::WFlags<Wt::ViewItemRenderFlag> flags);

  // Updates the model index of the gdWTextCustomItem
  void updateModelIndex(Wt::WWidget* widget, const Wt::WModelIndex& index);

private:
  bool        m_bFirstRowReserved;
  double      m_rowHeight;
};

#endif // __gdWTextDelegate__

