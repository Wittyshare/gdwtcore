/*
 * Copyright (C) 2008-Today Guy Deleeuw
 *
 * See the LICENSE file for terms of use.
*/

/*! \file gdWDialog.h */

#ifndef __GDWDIALOG_H__
#define __GDWDIALOG_H__

#include <boost/bind.hpp>

#include <Wt/WContainerWidget>
#include <Wt/WHBoxLayout>
#include <Wt/WVBoxLayout>
#include <Wt/WDialog>
#include <Wt/WText>
#include <Wt/WImage>

/*! \class gdWDialogData
 *  \brief Cette classe est necessaire pour gdWDialog
 Elle contient le resultat de accept et reject ainsi deux callback :
   saveData appele si WT::WDialog::accept() est declenche, si ce pointeur est non null la fonction callback est appellee.
   caller   si ce pointeur est non NULL, cette fonction est appellée.
*/
class gdWDialogData {
public :
  gdWDialogData();
  virtual ~gdWDialogData() {}; // Pour rendre la classe polymorphic

public :
  Wt::WDialog::DialogCode                         result;
  boost::function<void (gdWDialogData* refData)>  callerCallback;
  boost::function<void (gdWDialogData* refData)>  saveDataCallback;
};

/*! \class gdWDialog
 *  \brief Cette classe est derivee de Wt::WDialog et permet de s'auto gerer.
           Attention, l'objet gdWDialog est automatiquement detruit
*/

class gdWDialog : public Wt::WDialog {
public :
  /*! \brief button enumerator. */
  enum useButton {
    buttonNone      = 0x00,
    buttonOkCancel  = 0x01, // By default
    buttonOk        = 0x02,
    buttonCancel    = 0x03,
    buttonAll       = 0xFF
  };

public :
  /*! \brief Constructor.
  */
  gdWDialog(const Wt::WString& title, gdWDialogData* refData, int nUseButtons = buttonOkCancel, Wt::Orientation orientation = Wt::Vertical);
  /*! \brief destructor.
  */
  ~gdWDialog();

  /*! \brief get the content widget
  */
  Wt::WContainerWidget* getContent() {
    return m_wcContent;
  };
  /*! \brief add a button.
  */
  void              addButton(Wt::WPushButton* pButton);
  /*! \brief get the bottom container.
  */
  Wt::WContainerWidget* getBottom()  {
    return m_wcBottom;
  };

  /*! \brief Set the dialog title
  */
  void setWindowTitle(const Wt::WString& title);

  /*! \brief Get the dialog title
  */
  const Wt::WString windowTitle() const;

  /*! \brief Set the dialog title icon
  */
  void setWindowIcon(const std::string& urlIcon);

public slots :
public slots :
  void onAccepted();
  void onRejected();
  void onFinished(DialogCode dlgRes);

private :
  gdWDialogData*            m_refData;
  int                       m_nUseButtons;
  Wt::WContainerWidget*     m_wcContent;
  Wt::WContainerWidget*     m_wcBottom;
  Wt::WBoxLayout*           m_blButtons;
  Wt::WImage*               m_imgIcon;
  Wt::WText*                m_wtTitle;
};

#endif // ifdef __GDWDIALOG_H__

