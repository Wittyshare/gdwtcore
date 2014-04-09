/*
 * Copyright (C) 2006-Today Guy Deleeuw
 *
 * See the LICENSE file for terms of use.
 */

#ifndef GD_gdStdItem_H__
#define GD_gdStdItem_H__

#include <Wt/WStandardItem>
#include <Wt/WString>
#include <Wt/WSignal>

/*!
    \class gdStdItem
    \brief Reimplement Wt::WStandardItem to handle checked() event.
*/
class gdStdItem : public Wt::WStandardItem {
public :
  /*! \brief Constructor */
  gdStdItem();
  gdStdItem(const Wt::WString& text);

  /*! \brief handle the setData */
  void                   setData(const boost::any& data, int role = Wt::UserRole);

  /*! \brief set debugging */
  void                   setDebugging(bool bDebug);
  bool                   debugging();

  /*! \brief check an item */
  void                   checkItem(Wt::CheckState state);
  bool                   isItemChecked();

  /*! \brief get the parent */
  gdStdItem*             parent() const;

public :
  /*! \brief Signal emitted when an item are checked/unchecked */
  Wt::Signal<gdStdItem*, Wt::CheckState>& checkStateChanged() {
    return checkStateChanged_;
  };

private :
  Wt::Signal<gdStdItem*, Wt::CheckState> checkStateChanged_;

private :
  bool                            m_bChecked;
  bool                            m_bDebug;
};

#endif // ifdef GD_gdStdItem_H__

