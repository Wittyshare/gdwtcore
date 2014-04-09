/*
 * Copyright (C) 2006-Today Guy Deleeuw
 *
 * See the LICENSE file for terms of use.
*/

#ifndef DLG_HOURGLASS_H__
#define DLG_HOURGLASS_H__

/*! \file gdApplication.h */

#include <Wt/WDialog>

/*! \class dlgHourGlass
 *  \brief A dialog widget that show a hour glass
*/
class dlgHourGlass : public Wt::WDialog {
public :
  /*! \brief Constructor.

  */
  dlgHourGlass(const Wt::WString& pLabel = Wt::WString());

};

#endif // ifdef DLG_HOURGLASS_H__
