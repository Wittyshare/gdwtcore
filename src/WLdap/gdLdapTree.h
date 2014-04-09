/*
 * Copyright (C) 2006-Today Guy Deleeuw
 *
 * See the LICENSE file for terms of use.
*/

/*! \file gdLdapTree.h */

#ifndef __gdLdapTree__
#define __gdLdapTree__

#include <Wt/WContainerWidget>
#include <Wt/WTreeView>

#include <gdcore/gdLdap.h>

#include "../gdStdModel.h"
#include "../gdStdItem.h"
#include "../gdProxyModel.h"

using namespace Wt;

/*!
    \class gdLdapTree
    \brief Render all items catalogue with a specific type (table, product, ...)
*/
class gdLdapTree : public WTreeView {
public :
  /*! \brief Constructor.  */
  gdLdapTree(WContainerWidget* parent = 0);
  /*! \brief Destructor.  */
  ~gdLdapTree();

  /*! \brief Set/get the server name */
  void                        setServerName(const std::string& sName);
  const std::string&          serverName();

  /*! \brief Set/get the server display name */
  void                        setServerDisplayName(const std::string& sDisplayName);
  const std::string&          serverDisplayName();

  /*! \brief Set/get the filter */
  void                        setFilter(const std::string& sFilter);
  const std::string&          filter();

  /*! \brief Set/get the scope */
  void                        setScope(const int nScope);
  const int                   scope();

  /*! \brief Set/get the debugging mode. */
  void                        setDebugging(bool bDebug);
  const bool                  debugging();

  /*! \brief Set/get tree with checkbox. */
  void                        setCheckAllowed(int nCheckType);
  int                         checkAllowed();

  /*! \brief Set/get the method to add children when an item has subordonates true : add a dummy entry, false add all real children */
  void                        setDummy(const bool bDummy);
  const bool                  dummy();

  /*! \brief add a base, if empty this class retrieve all naming contexts */
  void                        addNamingContext(const std::string& namingContext, bool bClear = false);

  /*! \brief See the base implementation */
  virtual void  load();

  /*! \brief Build the model */
  void                        buildModel();
  gdStdModel*                 model();
  /*! \brief Load the ldap result */
  void                        populateEntry(WStandardItem* pRootItemCol00);
  /*! \brief Load the an ldap entry */
  void                        addEntry(WStandardItem* pRootItem, const std::string& rdn, const std::string& dn, bool bHasChildren = false);

  /*! \brief Make the view */
  void                         buildView();
  /*! \brief Return the width of the tree */
  WLength                      treeWidth();

  void                         checkDN(const std::string& rDN);

private :
  void                         checkDN(WStandardItem* pRootItem, const std::string& rDN, Wt::CheckState rCheck);

public slots :
  void                         onTreeExpanded(Wt::WModelIndex idx);

private :
  std::string                          m_sServerName;          //!< Ldap server name ex. ldap.example.com
  std::string                          m_sServerDisplayName;   //!< Ldap server display name ex. 'Ldap on Example server'
  std::string                          m_sFilter;              //!< Ldap filter ex. "(&(objectClass=*))" (default), "(&(objectClass=person))", ...
  int                                  m_nScope;               //!< Ldap search scope LDAP_SCOPE_SUB, LDAP_SCOPE_ONE (default), ...
  gdCLdapServer                        m_cLdapServer;
  gdCLdap                              m_cLdap;
  std::vector<std::string>             m_vSearchBases;         //!< Naming contexts || search base
  gdStdModel*                          m_modelTree;            //!< The model.
  gdProxyModel*                        m_pProxy;               //!< The filter model for first row as search area.
  int                                  m_nCheckAllowed;
  bool                                 m_bDebug;
  bool                                 m_bDummy;
};

#endif // ifdef __gdLdapTree__

