/*
 * Copyright (C) 2006-Today Guy Deleeuw
 *
 * See the LICENSE file for terms of use.
 */

#ifndef _gdSqlSuggestionPopup_H__
#define _gdSqlSuggestionPopup_H__

#include <Wt/WContainerWidget>
#include <Wt/WLineEdit>
#include <Wt/WSuggestionPopup>
#include <Wt/WSortFilterProxyModel>
#include <Wt/WSignal>

#include "gdSqlModel.h"

class gdSqlSuggestionPopup : public Wt::WContainerWidget
 {
  public :
   gdSqlSuggestionPopup(gdCoreSql* refSql, std::string strQuery, int column, Wt::WContainerWidget* parent = 0);
  
  public :
   void        loadModel();
   void        setQuery(const std::string& query);
   int         getSelectionID();
   void        setSelectionID(int selID);
   void        setCurrentIndex(int index);
   int         currentIndex();
   gdSqlModel* model();
   std::string value();

  private :
   void filter(const Wt::WString& strFilter);

  private slots :
   void doChanged(int id, Wt::WFormWidget* search);

  public :
   Wt::Signal<int>& s_changed() { return s_changed_; };

  private :
   Wt::Signal<int> s_changed_;

  private :
   int                           m_nIndex;
   int                           m_nColumn;
   std::string                   m_strQuery;
   gdSqlModel*                   m_pSqlModel;
   Wt::WSortFilterProxyModel*    m_pFilterModel;
   Wt::WLineEdit*                m_leSearch;
   Wt::WSuggestionPopup*         m_pSuggestionPopup;
 };

#endif // ifdef _gdSqlSuggestionPopup_H__

