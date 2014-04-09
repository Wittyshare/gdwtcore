/*
 * Copyright (C) 2006-Today Guy Deleeuw
 *
 * See the LICENSE file for terms of use.
 */

#ifndef GD_gdStdModel_H__
#define GD_gdStdModel_H__

#include <Wt/WStandardItemModel>
#include <Wt/WStandardItem>
#include <Wt/WString>
#include <Wt/WSignal>

#include "gdStdItem.h"

class gdProxyModel;

/*! \class gdStdModel
 *  \brief A class that superceed the Wt::WStandardModel class to allow storage of titles from report
*/
class gdStdModel : public Wt::WStandardItemModel {
public :
  /*! \brief Data type.  */
  enum stdTypes {
    type_unknown     =  0, //!< Unknown type.
    type_int4        =  1, //!< long
    type_int8        =  2, //!< long long
    type_double      =  3, //!< double
    type_text        =  4, //!< string
    type_date        =  5, //!< date
    type_end
  };

public :
  /*! \brief Constructor.  */
  gdStdModel(Wt::WObject* parent = 0);
  gdStdModel(int rows, int columns, Wt::WObject* parent = 0);

  /*! \brief Destructor.  */
  ~gdStdModel();

  /*! \brief copy constructor.  */
  gdStdModel(const gdStdModel& smSrc);

  /*! \brief set/get the main title.  */
  void                setTitle(const std::string& title);
  const std::string&  title() const;

  /*! \brief set/get the top left title.  */
  void                setTitleA(const std::string& titleA);
  const std::string&  titleA() const;

  /*! \brief set/get the top right title.  */
  void                setTitleB(const std::string& titleB);
  const std::string&  titleB() const;

  /*! \brief set/get the bottom left title.  */
  void                setTitleC(const std::string& titleC);
  const std::string&  titleC() const;

  /*! \brief set/get the bottom right title.  */
  void                setTitleD(const std::string& titleD);
  const std::string&  titleD() const;

  /*! \brief set/get proxymodel companion. */
  void                setProxy(gdProxyModel* proxy);
  gdProxyModel*       proxy();

  /*! \brief set/get first row role */
  void                setFirstRowReserved(bool bReserved = false);
  bool                isFirstRowReserved();

  /*! \brief set/get last row role */
  void                setLastRowReserved(bool bReserved = false);
  bool                isLastRowReserved();

  /*! \brief set/get if first column can be checked */
  void                setCol0Checkable(bool bCkeckable = false);
  bool                isCol0Checkable();

  /*! \brief set/get if the header is available */
  void                useHeader(bool bUseHeader = true);
  bool                isHeaderUsed();

  /*! \brief Load the header, this implementation set just the header as loaded. */
  virtual void        loadHeader();
  bool                isHeaderLoaded();

  /*! \brief Load the header and the data or reload the data only. */
  virtual void        reload();

  /*! \brief remove all rows from the model. */
  void                removeAllRows();

  /*! \brief get the item from a model. */
  gdStdItem*          realItem(int row, int column = 0) const;

  /*! \brief get the item from an index. */
  gdStdItem*          itemFromIndex(const Wt::WModelIndex& index) const;

  /*! \brief get the item from an index in the model (not the proxy). */
  gdStdItem*          itemFromIndexModel(const Wt::WModelIndex& index) const;

  /*! \brief Return the number of rows in the proxy or the model. */
  int                 realRowCount() const;

  /*! \brief Return the number of rows in the proxy or the model without first and last row reserved. */
  int                 rowCountView();

  /*! \brief Return the first row depend of the use of the first row. */
  int                 firstRow();

  /*! \brief Return the last row depend of if last row is reserverd. */
  int                 lastRow();

  /*! \brief Set/get the column data type. */
  virtual void        setColumnType(int column, stdTypes type);
  stdTypes            columnType(int column);

  /*! \brief Check if this column are numeric. */
  bool                isColumnNumeric(int column);

  /*! \brief Check/Uncheck some items. */
  void doCheckChanged(gdStdItem* item, Wt::CheckState chkst);

  /*! \brief Check all children */
  void checkChildrens(Wt::WStandardItem* parent);
  /*! \brief Uncheck all children */
  void uncheckChildrens(Wt::WStandardItem* parent);
  /*! \brief Set the check state of the parents */
  void setCheckParents(Wt::WStandardItem* item);

  /*! \brief Get a vector of column ol with selected items */
  const std::vector<gdStdItem*>& vItemsSelected(const int nColumn);
  /*! \brief Get a vector of checked items */
  const std::vector<gdStdItem*>& vItemsChecked(const int nColumn);

public slots :
  void onCheckChanged(gdStdItem* item, Wt::CheckState chkst);

public :
  /*! \brief Signal emitted when an item are checked/unchecked */
  Wt::Signal<gdStdItem*, Wt::CheckState>& checkStateChanged() {
    return checkStateChanged_;
  };

private :
  Wt::Signal<gdStdItem*, Wt::CheckState> checkStateChanged_;

public :
  std::vector<int>            m_vColumnsWidth;

private :
  void                        populate(Wt::WStandardItem* rootItem, std::vector<gdStdItem*>& vResult, const int nColumn, bool bChecked);

private :
  std::vector<stdTypes>       m_vType;

  std::string                 m_strTitle;
  std::string                 m_strTitleA;
  std::string                 m_strTitleB;
  std::string                 m_strTitleC;
  std::string                 m_strTitleD;

  gdProxyModel*               m_proxy;
  bool                        m_bCol0Checkable;
  bool                        m_bHeaderLoaded;
  bool                        m_bUseHeader;
  bool                        m_bFirstRowReserved;
  bool                        m_bLastRowReserved;
  bool                        m_bDebug;
  bool                        m_bOnCheckChanged;
  std::vector<gdStdItem*>     m_vItemsSelected;
  std::vector<gdStdItem*>     m_vItemsChecked;
};

#endif // ifdef GD_gdStdModel_H__

