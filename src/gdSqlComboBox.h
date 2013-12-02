/*
 * Copyright (C) 2006-Today Guy Deleeuw
 *
 * See the LICENSE file for terms of use.
 */

#ifndef GD_gdSqlComboBox_H__
#define GD_gdSqlComboBox_H__

#include <Wt/WComboBox>

#include "gdSqlModel.h"

class gdSqlComboBox : public Wt::WComboBox
 {
  public :
               gdSqlComboBox(int modelColumn, Wt::WContainerWidget* parent = 0, gdSqlModel* pModel = 0);
              ~gdSqlComboBox();
   int         getSelectionID();
   void        setSelectionID(int selID);
   gdSqlModel* model() const { return m_model; };
  
  private slots :

  private :
   gdSqlModel*                     m_model;
   gdSqlModel*                     m_ParentModel;
 };

#endif // ifdef GD_gdSqlComboBox_H__

