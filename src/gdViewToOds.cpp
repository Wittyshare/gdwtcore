/*
 * Copyright (C) 2006-Today G. Deleeuw
 *
 * See the LICENSE file for terms of use.
 */
#include <stdio.h>

#include <iostream>

#include <boost/algorithm/string/replace.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/exception.hpp>
#include <boost/lexical_cast.hpp>

#include <Wt/WApplication>
#include <Wt/WString>
#include <Wt/WStandardItemModel>

#include <gdcore/gdCore.h>

#include "gdViewToOds.h"

using namespace Wt;

gdViewToOds::gdViewToOds(const char* odsFile, const WAbstractItemView* pTV) : m_pTV(pTV), m_strXmlFile(odsFile), m_strOdsFile(odsFile), m_pOdsHandle(0)
 {
  boost::algorithm::replace_all(m_strXmlFile, ".ods", ".odsxml");
 }

gdViewToOds::~gdViewToOds()
 {
  if ( m_pOdsHandle )
    finalize();
  m_pOdsHandle = 0;
 }

void gdViewToOds::initialize()
 {
  std::string strHFile = wApp->docRoot() + wApp->resourcesUrl() + "/gddbcore/gdods/odsHead2.xml";
  const char* pHeaderFile = strHFile.c_str();

// Reservation memoire pour contenir le header ods.
  long szHead = boost::filesystem::file_size(pHeaderFile);
  char*     pMem = (char*) malloc(szHead + 1);
  if ( !pMem ) return;
  pMem[szHead] = 0;

// Chargement en memoire du fichier contenant le header ods
  FILE*                   hHeader;
  hHeader = fopen(pHeaderFile, "r");
  if ( !hHeader ) return;
  size_t ret = fread(pMem, 1, szHead, hHeader);
  fclose(hHeader);

// Ouverture de fichier ods
  m_pOdsHandle = fopen(m_strXmlFile.c_str(), "w");
  if ( !m_pOdsHandle ) {
     fprintf(stderr, "Cannot open m_pOdsHandle : %s\n", m_strXmlFile.c_str());
     return;
    }

// Ecriture du header ods
  fprintf(m_pOdsHandle, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n\n");
  fprintf(m_pOdsHandle, "%s\n", pMem);
  free(pMem);

// Construction du document
  initDocument();
 }

void gdViewToOds::finalize()
 {
  if ( !m_pOdsHandle ) return;
  finishDocument();

  fclose(m_pOdsHandle);
  m_pOdsHandle = 0;

  std::string      cmd("/usr/local/lassie/binsh/gdBuildODs.sh ");
  cmd += m_strXmlFile;
  cmd += " ";
  cmd += m_strOdsFile;

  int err = system(cmd.c_str());
  boost::filesystem::remove(m_strXmlFile);

  if ( err )
    fprintf(stderr, "Cannot execute command : %s\n", (cmd.c_str()));
 }

void gdViewToOds::initDocument()
 {
  if ( !m_pOdsHandle ) return;

  initStyles();

  fprintf(m_pOdsHandle, "  <office:body>\n");
  fprintf(m_pOdsHandle, "    <office:spreadsheet>\n");
  fprintf(m_pOdsHandle, "      <table:calculation-settings table:case-sensitive=\"false\" table:automatic-find-labels=\"false\" table:use-regular-expressions=\"false\"/>\n");

  initTab("Sheet 1");
  finishTab();
 }

void gdViewToOds::finishDocument()
 {
  if ( !m_pOdsHandle ) return;

  fprintf(m_pOdsHandle, "    </office:spreadsheet>\n");
  fprintf(m_pOdsHandle, "  </office:body>\n");
  fprintf(m_pOdsHandle, "</office:document-content>\n");
 }

void gdViewToOds::initStyles()
 {
  if ( !m_pOdsHandle ) return;

  fprintf(m_pOdsHandle, "  <office:automatic-styles>\n");

  // Style du tab
  fprintf(m_pOdsHandle, "    <style:style style:name=\"tab1\" style:family=\"table\" style:master-page-name=\"Default\">\n");
  fprintf(m_pOdsHandle, "      <style:table-properties table:display=\"true\" style:writing-mode=\"lr-tb\"/>\n");
  fprintf(m_pOdsHandle, "    </style:style>\n");

  // Style ligne 0 (header)
  double height = (m_pTV->headerHeight().value() / 118.1) * 6;
  fprintf(m_pOdsHandle, "    <style:style style:name=\"roh1\" style:family=\"table-row\">\n");
  fprintf(m_pOdsHandle, "      <style:table-row-properties style:row-height=\"%fcm\" fo:break-before=\"auto\" style:use-optimal-row-height=\"false\"/>\n", height);
  fprintf(m_pOdsHandle, "    </style:style>\n");

  // Style ligne x
  height = (m_pTV->rowHeight().value() / 118.1) * 6;
  fprintf(m_pOdsHandle, "    <style:style style:name=\"rox\" style:family=\"table-row\">\n");
  fprintf(m_pOdsHandle, "      <style:table-row-properties style:row-height=\"%fcm\" fo:break-before=\"auto\" style:use-optimal-row-height=\"true\"/>\n", height);
  fprintf(m_pOdsHandle, "    </style:style>\n");

  for(int iColumn = 0; iColumn <  m_pTV->model()->columnCount(); iColumn++)
   {
    // Valeur en pixel / 118.1 = Valeur en Cm : petit décalage de 0,1 a 0,2
    double width = (m_pTV->columnWidth(iColumn).value() / 118.1) * 3;
    fprintf(m_pOdsHandle, "    <style:style style:name=\"co%03d\" style:family=\"table-column\">\n", iColumn);
    fprintf(m_pOdsHandle, "      <style:table-column-properties fo:break-before=\"auto\" style:column-width=\"%fcm\"/>\n", width);
    fprintf(m_pOdsHandle, "    </style:style>\n");
   }

  // Style Cellule du header TODO : voir couleur de fond #000080 et couleur texte #ffffff
  fprintf(m_pOdsHandle, "    <style:style style:name=\"ce1\" style:family=\"table-cell\" style:parent-style-name=\"Default\">\n");
  fprintf(m_pOdsHandle, "      <style:table-cell-properties style:glyph-orientation-vertical=\"0\" fo:background-color=\"#000080\" style:cell-protect=\"protected\" style:print-content=\"true\" style:diagonal-bl-tr=\"none\" style:diagonal-tl-br=\"none\" style:text-align-source=\"fix\" style:repeat-content=\"false\" fo:wrap-option=\"no-wrap\" fo:border=\"none\" style:direction=\"ltr\" style:rotation-angle=\"0\" style:rotation-align=\"none\" style:shrink-to-fit=\"false\" style:vertical-align=\"middle\"/>\n");
  fprintf(m_pOdsHandle, "      <style:paragraph-properties fo:text-align=\"center\" fo:margin-left=\"0cm\" style:writing-mode=\"page\"/>\n");
  fprintf(m_pOdsHandle, "      <style:text-properties fo:color=\"#ffffff\" style:text-outline=\"false\" style:text-line-through-style=\"none\" style:font-name=\"Arial\" fo:font-size=\"9pt\" fo:font-style=\"normal\" fo:text-shadow=\"none\" style:text-underline-style=\"none\" fo:font-weight=\"bold\" style:font-size-asian=\"9pt\" style:font-style-asian=\"normal\" style:font-weight-asian=\"bold\" style:font-name-complex=\"Arial\" style:font-size-complex=\"9pt\" style:font-style-complex=\"normal\" style:font-weight-complex=\"bold\"/>\n");
  fprintf(m_pOdsHandle, "    </style:style>\n");

  fprintf(m_pOdsHandle, "  </office:automatic-styles>\n");
 }

void gdViewToOds::initTab(const std::string& tabLabel)
 {
  if ( !m_pOdsHandle ) return;
  fprintf(m_pOdsHandle, "     <table:table table:name=\"%s\" table:style-name=\"tab1\" table:print=\"false\">\n",
                        gdcore_string2xhtml(tabLabel).c_str());
  fprintf(m_pOdsHandle, "       <office:forms form:automatic-focus=\"false\" form:apply-design-mode=\"false\"/>\n");
  initSheet();
 }

void gdViewToOds::finishTab()
 {
  if ( !m_pOdsHandle ) return;
  finishSheet();
  fprintf(m_pOdsHandle, "     </table:table>\n");
 }

void gdViewToOds::initSheet()
 {
  if ( !m_pOdsHandle ) return;

 // Impression des titres colonnes
  buildHeader();

  // Impression des lignes du tableau.
  buildRows();
 }

void gdViewToOds::finishSheet()
 {
  if ( !m_pOdsHandle ) return;
 }

void gdViewToOds::buildHeader()
 {
  if ( !m_pOdsHandle ) return;

  // Columns styles
  for(int iHeader = 0; iHeader < m_pTV->model()->columnCount(); iHeader++)
    fprintf(m_pOdsHandle, "       <table:table-column table:style-name=\"co%03d\" table:default-cell-style-name=\"Default\"/>\n", iHeader);
    //fprintf(m_pOdsHandle, "       <table:table-column table:style-name=\"co%03d\" table:number-columns-repeated=\"%d\" table:default-cell-style-name=\"Default\"/>\n", iHeader, 1);

  // Row 0
  fprintf(m_pOdsHandle, "       <table:table-row table:style-name=\"roh1\">\n");
 
  // Column 0
  WString hd(asString(m_pTV->model()->headerData(0)));
  fprintf(m_pOdsHandle, "         <table:table-cell table:style-name=\"ce1\"  office:value-type=\"string\"><text:p>%s</text:p></table:table-cell>\n",
                        hd.toUTF8().c_str());
  for(int iHeader = 1; iHeader < m_pTV->model()->columnCount(); iHeader++)
   {
    hd = asString(m_pTV->model()->headerData(iHeader));
    fprintf(m_pOdsHandle, "         <table:table-cell table:style-name=\"ce1\"  office:value-type=\"string\"><text:p>%s</text:p></table:table-cell>\n",
                          hd.toUTF8().c_str());
   }
  // End Row 0
  fprintf(m_pOdsHandle,   "       </table:table-row>\n");
 }

void gdViewToOds::buildRows()
 {
  if ( !m_pOdsHandle ) return;

  for(int iRow = 0; iRow < m_pTV->model()->rowCount(); iRow++)
   {
    // Ouverture de la ligne
    fprintf(m_pOdsHandle,   "       <table:table-row table:style-name=\"rox\">\n");
    // Impression de la ligne
    buildRow(iRow);
    // Fermeture de la ligne
    fprintf(m_pOdsHandle,   "       </table:table-row>\n");
   }
 }

void gdViewToOds::buildRow(int iRow)
 {
  if ( !m_pOdsHandle ) return;

  for(int iColumn = 0; iColumn <  m_pTV->model()->columnCount(); iColumn++)
   {
    std::string str(asString(m_pTV->model()->data(iRow, iColumn)).toUTF8());
    double      dbl(asNumber(m_pTV->model()->data(iRow, iColumn)));
    boost::any boostData = m_pTV->model()->data(iRow, iColumn);
    if ( boostData.type() == typeid(double) )
       fprintf(m_pOdsHandle, "         <table:table-cell office:value-type=\"float\" office:value=\"%f\"><text:p>%s</text:p></table:table-cell>\n",
                dbl, str.c_str());
     else
      {
       if ( boostData.type() == typeid(bool) || boostData.type() == typeid(int) || boostData.type() == typeid(long) )
          fprintf(m_pOdsHandle, "         <table:table-cell office:value-type=\"long\" office:value=\"%ld\"><text:p>%s</text:p></table:table-cell>\n",
                   (long) dbl, str.c_str());
        else
          if ( boostData.type() == typeid(std::string) || boostData.type() == typeid(WString) )
            {
             fprintf(m_pOdsHandle, "         <table:table-cell office:value-type=\"string\" office:value=\"%s\"><text:p>%s</text:p></table:table-cell>\n",
                      gdcore_string2xhtml(str).c_str(), gdcore_string2xhtml(str).c_str());
            }
           else
             fprintf(m_pOdsHandle, "         <table:table-cell office:value-type=\"string\"><text:p> </text:p></table:table-cell>\n");
      }
   }
 }

