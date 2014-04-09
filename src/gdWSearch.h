/*
 * Copyright (C) 2006-Today Guy Deleeuw
 *
 * See the LICENSE file for terms of use.
*/

#ifndef __gdWSearch__
#define __gdWSearch__

#include <Wt/WContainerWidget>
#include <Wt/WLineEdit>
#include <Wt/WImage>

class gdWSearch : public Wt::WContainerWidget {
public :
  gdWSearch(Wt::WContainerWidget* parent = 0);
  ~gdWSearch();
  void               setText(const std::string& text);
  const std::string& text();

public slots :
  void           doSearch();
  void           doFocussed();
  void           doBlurred();

public :
  /*! \brief Signal emitted when a search are validated */
  Wt::Signal<Wt::WString>& searchValidated();
  Wt::Signal<Wt::WString>& searchFocussed();
  Wt::Signal<Wt::WString>& searchBlurred();

private :
  Wt::Signal<Wt::WString> searchValidated_;
  Wt::Signal<Wt::WString> searchFocussed_;
  Wt::Signal<Wt::WString> searchBlurred_;

private :
  Wt::WLineEdit*     m_leSearch;
  Wt::WImage*        m_pImage;
};

#endif // ifdef __gdWSearch__

