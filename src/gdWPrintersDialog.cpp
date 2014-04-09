/*
 * Copyright (C) 2008-Today Guy Deleeuw
 *
 * See the LICENSE file for terms of use.
*/

#include <stdio.h>

#include <boost/algorithm/string/replace.hpp>

#include <Wt/WApplication>
#include <Wt/WTheme>

#include "gdWPrintersDialog.h"

using namespace Wt;

gdWPrintersDialogData::gdWPrintersDialogData(int selectedPrinter) : gdWDialogData(), m_selectedPrinter(selectedPrinter)
{ }



gdWPrintersDialog::gdWPrintersDialog(WString title, gdWPrintersDialogData* refData)
  : gdWDialog(title, refData), m_refData(refData)
{
  if ( WString::tr("byObjectStyleSheet").narrow() == "true" )
    wApp->useStyleSheet(wApp->theme()->resourcesUrl() + "gdwtcore/Css/gdWPrintersDialog.css");
  wApp->messageResourceBundle().use(wApp->docRoot() + wApp->resourcesUrl() + "/gdwtcore/Transl/gdWPrintersDialog");
  addStyleClass("gdWPrintersDialog");
  m_refData->saveDataCallback = boost::bind(&gdWPrintersDialog::saveData, boost::ref(*this), m_refData);
  printers();
  WContainerWidget* content = getContent();
  m_pSelBox = new WSelectionBox();
  for (int i = 0; i < m_vPrinters.size(); i++)
    m_pSelBox->addItem(m_vPrinters[i]);
  m_pSelBox->setVerticalSize(9);
  m_pSelBox->setSelectionMode(Wt::SingleSelection);
  m_pSelBox->setCurrentIndex(m_refData->m_selectedPrinter);
  content->addWidget(m_pSelBox);
  resize(250, 300);
}

gdWPrintersDialog::~gdWPrintersDialog()
{ }

void gdWPrintersDialog::printers()
{
  m_vPrinters.clear();
  FILE* ptr;
  std::string cmd = "lpstat -a | sed -e \"s/ .*$//\"";
  if ( (ptr = popen(cmd.c_str(), "r")) == NULL ) {
    fprintf(stderr, "gdWPrintersDialog::printers() : cannot popen\n");
    return;
  }
  char line[256];
  while (1) {
    if ( fgets(line, 256, ptr) == NULL ) break;
    std::string s(line);
    boost::algorithm::replace_all(s, "\n", "");
    m_vPrinters.push_back(s);
  }
  fclose(ptr);
  return;
}

void gdWPrintersDialog::saveData(gdWDialogData* pData)
{
  m_refData->m_selectedPrinter = m_pSelBox->currentIndex();
  m_refData->m_printerName     = m_pSelBox->currentText().toUTF8();
}

