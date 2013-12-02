/*
 * Copyright (C) 2006-Today Guy Deleeuw
 *
 * See the LICENSE file for terms of use.
 */

#ifndef GD_gdToolbar_H__
#define GD_gdToolbar_H__

#include <Wt/WContainerWidget>
#include <Wt/WHBoxLayout>
#include <Wt/WEvent>
#include <Wt/WString>
#include <Wt/WPushButton>
#include <Wt/WAnchor>
#include <Wt/WCheckBox>
#include <Wt/WImage>
#include <Wt/WPopupMenu>

class gdToolbar;

class gdToolbarItem : public Wt::WContainerWidget
 {
  public :
                      gdToolbarItem(gdToolbar* pParent = 0, bool asAnchor = false, const int overDelay = 0);
   Wt::WPushButton*   button() { return m_pButton; };
   Wt::WAnchor*       anchor() { return m_anchor; };
   void               setOverDelay(int overDelay);
   void               setOutDelay(int outDelay);
   void               setIcon(std::string iconPath);
   Wt::WImage*        icon();
   void               setText(std::string label);
   const Wt::WString& text();
   void               setUrl(const std::string& url);
   const std::string& url() const;
   void               setPopupMenu(Wt::WPopupMenu* pPopupMenu);
   Wt::WPopupMenu*    popupMenu();

  public : // Signals
   Wt::Signal<gdToolbarItem*, Wt::WMouseEvent>& clicked()       { return clicked_; }
   Wt::Signal<gdToolbarItem*, Wt::WMouseEvent>& doubleClicked() { return doubleClicked_; }
   Wt::Signal<gdToolbarItem*, Wt::WMouseEvent>& mouseWentOver() { return mouseWentOver_; }
   Wt::Signal<gdToolbarItem*, Wt::WMouseEvent>& mouseWentOut()  { return mouseWentOut_; }

  public slots :
   void onClicked(const Wt::WMouseEvent& e);
   void onDoubleClicked(const Wt::WMouseEvent& e);
   void onMouseWentOver(const Wt::WMouseEvent& e);
   void onMouseWentOut(const Wt::WMouseEvent& e);

  private :
   Wt::Signal<gdToolbarItem*, Wt::WMouseEvent> clicked_;
   Wt::Signal<gdToolbarItem*, Wt::WMouseEvent> doubleClicked_;
   Wt::Signal<gdToolbarItem*, Wt::WMouseEvent> mouseWentOver_;
   Wt::Signal<gdToolbarItem*, Wt::WMouseEvent> mouseWentOut_;
   bool                                        m_asAnchor;
   int                                         m_nOverDelay;
   int                                         m_nOutDelay;
   Wt::WHBoxLayout*                            m_pLayout;
   Wt::WImage*                                 m_pIcon;
   Wt::WPushButton*                            m_pButton;
   Wt::WString*                                m_pStr;
   Wt::WAnchor*                                m_anchor;
   Wt::WImage*                                 m_pImage;
   bool                                        m_bInPanel;
   std::string                                 m_url;
   Wt::WPopupMenu*                             m_pPopupMenu;
 };










class gdToolbar : public Wt::WContainerWidget
 {
  public :
                         gdToolbar(Wt::WContainerWidget* pParent = 0, bool noRecurse = false);
          gdToolbarItem* addCheckBox(Wt::WCheckBox* pWCb, const int overDelay = 0);
   static gdToolbarItem* newCheckBox(Wt::WCheckBox* pWCb, const int overDelay = 0);
   static gdToolbarItem* newToolbarItem(std::string imgPath, std::string label, std::string tooltip, const int overDelay = 0);
          gdToolbarItem* addToolbarItem(std::string imgPath, std::string label, std::string tooltip, const int overDelay = 0);
          gdToolbarItem* addToolbarAnchor(std::string imgPath, std::string label, std::string tooltip, const int overDelay = 0);
          gdToolbarItem* addRightToolbarItem(std::string imgPath, std::string label, std::string tooltip, const int overDelay = 0);

  private :
   static void           buildToolbarItem(gdToolbarItem* toolbarItem, std::string imgPath, std::string label, std::string tooltip);

  private :
   bool                 m_bNoRecurse;
   gdToolbar*           leftToolbar;
   gdToolbar*           rightToolbar;
 };

#endif // ifdef GD_gdToolbar_H__

