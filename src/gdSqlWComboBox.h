/*
 * Copyright (C) 2006-Today Guy Deleeuw
 *
 * See the LICENSE file for terms of use.
 */

#ifndef GD_gdSqlWComboBox_H__
#define GD_gdSqlWComboBox_H__

#include <Wt/WComboBox>

#include "gdSqlModel.h"

class gdSqlWComboBox : public Wt::WComboBox
 {
  public :
               gdSqlWComboBox(int modelColumn, Wt::WContainerWidget* parent = 0);
   int         getSelectionID();
   void        setSelectionID(int selID);
   gdSqlModel* model() const { return m_model; };
  
  private slots :

  private :
   gdSqlModel*                     m_model;
 };

#endif // ifdef GD_gdSqlWComboBox_H__

