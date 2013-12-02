/*
 * Copyright (C) 2006-Today Guy Deleeuw
 *
 * See the LICENSE file for terms of use.
 */

/*! \file gdProxyModel.h */

#ifndef GD_gdProxyModel_H__
#define GD_gdProxyModel_H__

#include <Wt/WObject>
#include <Wt/WSortFilterProxyModel>

#include <gdcore/gdCoreSql.h>
#include "gdStdModel.h"

/*!
    \class gdProxyModel
    \brief Render all items matching a specific patter
*/
class gdProxyModel : public Wt::WSortFilterProxyModel
 {
  public :
   /*! \brief Constructor.  */
                        gdProxyModel(Wt::WObject* parent);
   /*! \brief Destructor.  */
                       ~gdProxyModel();
   /*! \brief Set/get the source model.  */
   virtual void         setSourceModel(gdStdModel* pModel);
   gdStdModel*          sourceModel() const;

   /*! \brief Set/get the default filter.  */
   void                 setDefaultFilter(const std::string& filter);
   std::string          defaultFilter() const;

   /*! \brief * Specifies how items with children will be treated if true : all else
                 false : only 'expanded' items in the tree.
                CAUTION: Do not 'dummy entry' these dummy entry are often used to increase performance when creating
                 the tree and are replaced by the real items when expand */
   void                 setAllChildsFiltering(const bool bAllChildsFiltering);
   bool                 allChildsFiltering() const;
  
  public slots :
    void itemChanged(Wt::WStandardItem* item);

  protected :
   virtual bool         filterAcceptRow(int sourceRow, const Wt::WModelIndex &sourceParent) const;
   virtual bool         lessThan(const Wt::WModelIndex& lhs, const Wt::WModelIndex& rhs) const;

  private :
   gdStdModel*          m_pSourceModel;
   std::string          m_defaultFilter;
   bool                 m_bOp;
   WObject*             m_parent;
   bool                 m_bAllChildsFiltering;
 };

#endif // ifdef GD_gdProxyModel_H__

