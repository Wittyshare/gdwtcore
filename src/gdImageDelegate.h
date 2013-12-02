#ifndef __gdImageDelegate__
#define __gdImageDelegate__

#include <Wt/WContainerWidget>
#include <Wt/WWidget>
#include <Wt/WAbstractItemModel>
#include <Wt/WModelIndex>
#include <Wt/WItemDelegate>
#include <Wt/WImage>

// A custom item widget that contains only a WImage
class gdImageCustomItem : public Wt::WContainerWidget
 {
  public:
                       gdImageCustomItem(const Wt::WModelIndex& index, std::string& imageRef, bool bFirstRowReserved);

   // Updates the model index
   void                setIndex(const Wt::WModelIndex& index);

   // Returns the editor
   Wt::WImage*         image() const;

  private slots :
    // Updates the model with the edited value
    void onClicked();

  private:
    Wt::WImage*              image_;
    Wt::WModelIndex          index_;
    bool                     m_bFirstRowReserved;
 };

// A custom delegate that uses an gdImageCustomItem
class gdImageDelegate : public Wt::WItemDelegate
 {
  public:
            gdImageDelegate(std::string imageRef, Wt::WObject *parent = 0);
   void     setFirstRowReserved(bool bReserved);
   bool     isFirstRowReserved();

   // Creates or updates an gdImageCustomItem
   Wt::WWidget* update(Wt::WWidget *widget, const Wt::WModelIndex& index, Wt::WFlags<Wt::ViewItemRenderFlag> flags);

   // Updates the model index of the gdImageCustomItem
   void     updateModelIndex(Wt::WWidget *widget, const Wt::WModelIndex& index);

  private:
   std::string             imageRef_;
   bool                    m_bFirstRowReserved;
 };

#endif // __gdImageDelegate__

