#ifndef __gdFileView__
#define __gdFileView__

#include <boost/filesystem/path.hpp>

#include <Wt/WStandardItemModel>
#include <Wt/WTreeView>
#include <Wt/WMenu>
#include <Wt/WMessageBox>

#include "gdToolbar.h"

namespace gdwtcore {
enum viewType {
  typeNone               =  0,
  typeOpen               =  1,
  typeAppend             =  2,
  typeSave               =  3,
  typeSaveAs             =  4,
  typeSaveChecked        =  5,
  typeSaveAsChecked      =  6,
  typeImport             =  7,
  typeLink               =  8,
  typeWebLink            =  9,
  typeWebDownload        = 10
};
}

/*
 tbBreadCrumb : Ajoute une barre de progression dans la profondeur des repertoires.
                Si absent un '..' (parent directory) est ajoute en debut d'arbre
                   sauf pour l'expand d'un arbre (skipParentDir de loadtree)

 tbUpRoot     : Autorise de remonter au dessus du path initial (dangereux car l'utilisateur peut remonter jusqu'a le vrai root.
              : Si absent le root = path initial.
*/
class gdWFileView {
public :
  enum tbFlags {
    tbDefault      = 0x00,
    tbUseToolbar   = 0x01,
    tbBreadCrumb   = 0x02,
    tbUpRoot       = 0x04,
    tbShowHidden   = 0x08,
    tbUseLeftTree  = 0x10,
    tbUseDummy     = 0x20
  };

  enum signalType {
    none            = 0,
    selected        = 1,
    doubleClick     = 2
  };

  enum skipMode { noSkipParent = 0, skipParent = 1 };

public:
  gdWFileView(gdwtcore::viewType dlgType, const boost::filesystem::path& rootPath, tbFlags flags = tbDefault, Wt::WContainerWidget* parent = 0);
  ~gdWFileView();
  void            init(const boost::filesystem::path& bcPath);
  bool            treeTraverse(Wt::WStandardItem* rootItem, const std::vector<std::string*>& vPath, int nLevel);
  void            treeMatchPath(const boost::filesystem::path& curPath);
  void            setBreadCrumb(const boost::filesystem::path& startPath);
  bool            loadTree(Wt::WStandardItem* currentItem, const boost::filesystem::path& currentPath, skipMode skipParentDir = noSkipParent);
  bool            loadFolder(Wt::WStandardItem* currentItem, const boost::filesystem::path& newPath, bool bEdit = false);
  bool            loadFile(Wt::WStandardItem* currentItem, const boost::filesystem::path& currentPath);
  void            setCurrentPath(const std::string& currentPath);
  std::string     selectedDirectory();
  std::string     selectedFile();
  std::string     selectedPath();
  std::string     currentFolderPath();
  void            makeToolbar();

public :
  Wt::Signal<signalType, std::string>& fileSelected() {
    return fileSelected_;
  };

private :
  Wt::Signal<signalType, std::string> fileSelected_;

public slots:
  void onViewDblClick(Wt::WModelIndex idx, Wt::WMouseEvent mouseEvent);
  void onTreeSelectionChanged();
  void onTreeMouseUp(Wt::WModelIndex idx, Wt::WMouseEvent mouseEvent);
  void onTreeExpanded(Wt::WModelIndex idx);
  void onViewSelectionChanged();
  void onViewExpanded(Wt::WModelIndex idx);
  void onBreadCrumbSelected(Wt::WMenuItem* pMenuItem);
  void doRename();
  void doConfirmDelete();
  void doDelete(Wt::StandardButton button);
  void doNewFolder();
  void onDirectoryNameChanged(Wt::WStandardItem* pItem);

private:
  class breadCrump {
  public :
    std::string                   title;
    std::string                   path;
  };
  gdwtcore::viewType              m_viewType;
  const boost::filesystem::path   m_fsRootPath;
  tbFlags                         m_tbFlags;
  gdToolbar*                      m_pTb;
  gdToolbarItem*                  m_tbiDelete;
  gdToolbarItem*                  m_tbiNewFolder;
  gdToolbarItem*                  m_tbiRename;
  Wt::WContainerWidget*           m_contMenu;
  Wt::WMenu*                      m_pMenu;
  Wt::WStandardItemModel*         m_pModelTree;
  Wt::WStandardItemModel*         m_pModelView;
  Wt::WTreeView*                  m_pTVTree;
  Wt::WTreeView*                  m_pTVView;
  std::string                     m_selectedFile;
  std::string                     m_selectedDirectory;
  std::vector<breadCrump*>        m_vBreadCrumb;
  bool                            m_bOnDataChanged;
  Wt::WMessageBox*                m_pMb;
  bool                            m_bOnRename;
};

#endif // __gdFileView__

