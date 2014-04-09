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

#include <Wt/WString>
#include <Wt/WStandardItemModel>

#include "gdViewToPdf.h"

using namespace Wt;

const bool useOldLabel = true;

gdViewToPdf::gdViewToPdf(const char* pdfFile, WAbstractItemView* pTV, bool bFull, long firstRow) : m_pTV(pTV), m_pPdf(0), m_l1row(firstRow)
{
  m_pPdf = gdPdf_openPdf(pdfFile, (int) bFull);
}

gdViewToPdf::~gdViewToPdf()
{
  if ( m_pPdf ) {
    gdPdf_savePdf(m_pPdf);
    gdPdf_closePdf(m_pPdf);
  }
}

// Je pourrais utiliser cette fonction, mais elle retourne la largeur nécessaire
//   pour afficher le label, pas une repartirion sur la hauteur
//    float hWidth = HPDF_Page_TextWidth(page, TitlesCols_[iCol].narrow().c_str());
void gdViewToPdf::setPageProperties(gdStPdf* pPdf, HPDF_Page page)
{
  if ( !pPdf ) return;
  pageOptimize(pPdf, page, HPDF_PAGE_PORTRAIT);
  int    nbColsSheets1 = pPdf->pageProperties.nbColumnsSheets;
  int    nbRowsSheets1 = pPdf->pageProperties.nbRowsSheets;
  // Si une seule page colonne et une seule page ligne  en mode portrait : OK
  if ( (nbColsSheets1 == 1) && (nbRowsSheets1 == 1) ) {
    pPdf->pageProperties.nOptimized = 1;
    return;
  }
  pageOptimize(pPdf, page, HPDF_PAGE_LANDSCAPE);
  int    nbColsSheets2 = pPdf->pageProperties.nbColumnsSheets;
  int    nbRowsSheets2 = pPdf->pageProperties.nbRowsSheets;
  // Si une seule page colonne et une seule page ligne  en mode paysage : OK
  if ( (nbColsSheets1 == 1) && (nbRowsSheets2 == 1) ) {
    pPdf->pageProperties.nOptimized = 1;
    return;
  }
  /*
  // Si le nombre de pages en portrait et en paysage est identique alors passage en portrait
    if ( (nbColsSheets1 == nbColsSheets2) && (nbRowsSheets1 == nbRowsSheets2) )
      {
       pageOptimize(pPdf, page, HPDF_PAGE_PORTRAIT);
       pPdf->pageProperties.nOptimized = 1;
       return;
      }

  // Si le nombre de pages en portrait est moindre ou egale qu'en paysage alors passage en portrait
    if ( (nbColsSheets1 * nbRowsSheets1) <= (nbColsSheets2 * nbRowsSheets2) )
      {
       pageOptimize(pPdf, page, HPDF_PAGE_PORTRAIT);
       pPdf->pageProperties.nOptimized = 1;
       return;
      }
  */
  // On reste en paysage.
  pPdf->pageProperties.nOptimized                = 1;
}

void gdViewToPdf::pageOptimize(gdStPdf* pPdf, HPDF_Page page, HPDF_PageDirection direction)
{
  HPDF_Page_SetSize(page, HPDF_PAGE_SIZE_A4, direction);
  gdPdf_clearPageProperties(pPdf);
  pPdf->pageProperties.nOptimized                = 0;
  pPdf->pageProperties.pageOrientation           = direction;
  pPdf->pageProperties.em2Point                  = 0.40; // TODO pixel2point =1/20 soit 0.05
  pPdf->pageProperties.maxRowsInHeader           = 3;
  pPdf->pageProperties.maxRowsInRowLabel         = 2;
  pPdf->pageProperties.fontTitlesSize            = 12;
  pPdf->pageProperties.fontHeaderSize            = 6.25;
  pPdf->pageProperties.fontRowLabelSize          = 6.25;
  pPdf->pageProperties.fontRowQteSize            = 6.25;
  pPdf->pageProperties.pageWidth                 = HPDF_Page_GetWidth(page);
  pPdf->pageProperties.pageHeight                = HPDF_Page_GetHeight(page);
  pPdf->pageProperties.pageTopMargin             = 15;
  pPdf->pageProperties.pageLeftMargin            = 25;
  pPdf->pageProperties.pageRightMargin           = 10;
  pPdf->pageProperties.pageBottomMargin          = 10;
  pPdf->pageProperties.pageTitlesHeight          = 12;
  pPdf->pageProperties.pageHeightAfterTitles     = 20;
  pPdf->pageProperties.pageNbRows                =  m_pTV->model()->rowCount() - m_l1row;
  pPdf->pageProperties.pageNbColumns             =  m_pTV->model()->columnCount();
  pPdf->pageProperties.headerHeightOfRow         =  9;
  pPdf->pageProperties.headerColsWidth           = (int*) malloc(sizeof(int) * pPdf->pageProperties.pageNbColumns);
  pPdf->pageProperties.headerMaxNbRows           =  1;
  pPdf->pageProperties.headerNbRowsOfEachColumn  = (int*) malloc(sizeof(int) * pPdf->pageProperties.pageNbColumns);
  pPdf->pageProperties.nbRowsOfLabel             =  1;
  pPdf->pageProperties.nbRowsOfRowLabel          = (int*) malloc(sizeof(int) * pPdf->pageProperties.pageNbRows);
  pPdf->pageProperties.footerHeight              =  0;
  pPdf->pageProperties.nbColumnsSheets           =  0;
  pPdf->pageProperties.nbRowsSheets              =  0;
  pPdf->pageProperties.rootSheetColumns          =  0;
  pPdf->pageProperties.rootSheetRows             =  0;
  // Transfert des largeurs colonnes.
  for (int iCol = 0; iCol < pPdf->pageProperties.pageNbColumns; iCol++)
    pPdf->pageProperties.headerColsWidth[iCol] = m_pTV->columnWidth(iCol).value();
  // Preparation des fonts pour les titres et initialisation de la valeur pageTitlesHeight
  gdPdf_setFontTitle(pPdf, page);
  // Preparation des fonts pour le header et initialisation de la valeur headerHeightOfRow
  gdPdf_setFontColTitle(pPdf, page);
  HPDF_Page_SetTextLeading(page, pPdf->pageProperties.headerHeightOfRow);
  //  pPdf->pageProperties.headerHeightOfRow = 0;
  gdPdf_setFontColTitle(pPdf, page);
  // Le footer aura la même taille qu'une ligne du header.
  pPdf->pageProperties.footerHeight = pPdf->pageProperties.headerHeightOfRow;
  // Calcul du nombre de lignes des titres colonnes
  //  (il sera le même sur tous les tableaux).
  //  et calcul reduction du libelle si il n'entre pas dans la largeur par default.
  for (int iCol = 0; iCol < pPdf->pageProperties.pageNbColumns; iCol++) {
    int         nbRowsTitles     = 1;
    WString     strHeader        = asString(m_pTV->model()->headerData(iCol));
    while (1) {
      if ( gdPdf_printColTitle(pPdf, page, 0, iCol, strHeader.toUTF8().c_str(), nbRowsTitles, nbRowsTitles, 1, 0) ) break;
      if ( nbRowsTitles <= pPdf->pageProperties.maxRowsInHeader ) {
        nbRowsTitles++;
        continue;
      }
      break; // Si on desire laisser la librairie pdf se débrouiller : activer le break
      // Si les pPdf->pageProperties.maxRowsInHeader lignes sont atteintes on supprime les caracteres espaces avant et apres le label
      std::string  Str(strHeader.toUTF8());
      // on enleve les espaces avant :
      std::string Str2 = Str.substr(Str.find_first_not_of(' '));
      // on enleve les espaces apres :
      Str = Str2;
      size_t pos = Str.find_last_not_of(' ');
      Str.erase(pos + 1, Str.length());
      nbRowsTitles = 1;
      //  et si cela ne suffit pas on supprime les caracteres fin de chaines 1 a 1;
      while (1) {
        //std::cerr << "j'essaie :" << Str << std::endl;
        if ( gdPdf_printColTitle(pPdf, page, 0, iCol, Str.c_str(), nbRowsTitles, nbRowsTitles, 1, 0) ) {
          m_pTV->model()->setHeaderData(iCol, Str);
          strHeader = Str;
          break;
        };
        if ( nbRowsTitles <= pPdf->pageProperties.maxRowsInHeader ) {
          nbRowsTitles++;
          continue;
        }
        // TODO : Le PDF et WAbstractItemView de Koen ne prennent pas en compte la reorganisation du label si il peut entrer sur + lignes
        Str.erase(Str.length() - 1, 1);
      }
      // Maintenant on verifie qu'il n'y a pas trop de lignes
      for (int i = 1; i < nbRowsTitles; i++) {
        if ( gdPdf_printColTitle(pPdf, page, 0, iCol, strHeader.toUTF8().c_str(), nbRowsTitles, i, 1, 0) ) {
          nbRowsTitles = i;
          break;
        };
      }
      break;
    }
    if ( nbRowsTitles > pPdf->pageProperties.headerMaxNbRows ) pPdf->pageProperties.headerMaxNbRows = nbRowsTitles;
    pPdf->pageProperties.headerNbRowsOfEachColumn[iCol] = nbRowsTitles;
  }
  // Calcul par decoupage.
  if ( useOldLabel ) {
    pPdf->pageProperties.headerMaxNbRows = 1;
    for (int iCol = 0; iCol < pPdf->pageProperties.pageNbColumns; iCol++) {
      std::string strHeader        = asString(m_pTV->model()->headerData(iCol)).toUTF8();
      int nSz = strHeader.length() / pPdf->pageProperties.headerColsWidth[iCol];
      if ( strHeader.length() % pPdf->pageProperties.headerColsWidth[iCol] ) nSz++;
      if ( !nSz ) nSz = 1;
      pPdf->pageProperties.headerNbRowsOfEachColumn[iCol] = nSz;
      if ( nSz > pPdf->pageProperties.headerMaxNbRows ) pPdf->pageProperties.headerMaxNbRows = nSz;
    }
  }
  // Calcul du nombre de lignes par label ligne.
  //  (il sera le même sur tous les tableaux).
  //  et calcul reduction du libelle si il n'entre pas dans la largeur par default.
  gdPdf_setFontRowTitle(pPdf, page);
  for (int iRow = 0; iRow < pPdf->pageProperties.pageNbRows; iRow++) {
    int         nbRowsLabel     = 1;
    //    if ( m_pTV->column1Fixed() ) continue;
    std::string strHeader       = asString(m_pTV->model()->data(iRow + m_l1row, 0)).toUTF8();
    while (1) {
      int align = 0;
      if ( m_pTV->columnAlignment(0) == Wt::AlignRight ) align = 1;
      if ( gdPdf_printRowTitle(pPdf, page, iRow, iRow, strHeader.c_str(), nbRowsLabel, 1, 0, pPdf->pageProperties.pageWidth, align) ) break;
      if ( nbRowsLabel <= pPdf->pageProperties.maxRowsInRowLabel ) {
        nbRowsLabel++;
        continue;
      }
      // Si les pPdf->pageProperties.maxRowsInRowLabel lignes sont atteintes on supprime les caracteres espaces avant et apres le label
      std::string  Str(strHeader);
      // on enleve les espaces avant :
      std::string Str2 = Str.substr(Str.find_first_not_of(' '));
      // on enleve les espaces apres :
      Str = Str2;
      size_t pos = Str.find_last_not_of(' ');
      Str.erase(pos + 1, Str.length());
      nbRowsLabel = 1;
      //  et si cela ne suffit pas on supprime les caracteres fin de chaines 1 a 1;
      while (1) {
        //std::cerr << "j'essaie :" << Str << std::endl;
        if ( gdPdf_printRowTitle(pPdf, page, iRow, iRow, Str.c_str(), nbRowsLabel, 1, 0, pPdf->pageProperties.pageWidth, align) ) {
          m_pTV->model()->setData(iRow + m_l1row, 0, Str);
          strHeader = Str;
          break;
        };
        if ( nbRowsLabel <= pPdf->pageProperties.maxRowsInRowLabel ) {
          nbRowsLabel++;
          continue;
        }
        Str.erase(Str.length() - 1, 1);
      }
      // Maintenant on verifie qu'il n'y a pas trop de lignes
      for (int i = 1; i < nbRowsLabel; i++) {
        if ( gdPdf_printRowTitle(pPdf, page, iRow, iRow, strHeader.c_str(), i, 1, 0, pPdf->pageProperties.pageWidth, align) ) {
          nbRowsLabel = i;
          break;
        };
      }
      break;
    }
    if ( nbRowsLabel > pPdf->pageProperties.nbRowsOfLabel ) pPdf->pageProperties.nbRowsOfLabel = nbRowsLabel;
    pPdf->pageProperties.nbRowsOfRowLabel[iRow] = nbRowsLabel;
  }
  /* START COLUMNS COMPUTING */
  // Calcul de la largeur maxi
  float baseWidth = pPdf->pageProperties.pageLeftMargin + (m_pTV->columnWidth(0).value() * pPdf->pageProperties.em2Point) + pPdf->pageProperties.pageRightMargin;
  float columnsWidth = 0.0;
  for (int iCol = 1; iCol < m_pTV->model()->columnCount(); iCol++)
    columnsWidth += (m_pTV->columnWidth(iCol).value() * pPdf->pageProperties.em2Point);
  float widthNeeded = baseWidth + columnsWidth;
  // Calcul du nombre de tableaux necessaires pour afficher toutes les colonnes
  pPdf->pageProperties.nbColumnsSheets    = 1;
  ST_GDPDF_COLS_SHEET*  curColsSheet      = gdPdf_getNewColsSheet();
  pPdf->pageProperties.rootSheetColumns   = curColsSheet;
  curColsSheet->colStart                  = 1;
  curColsSheet->colEnd                    = curColsSheet->colStart;
  while (1) {
    curColsSheet->sheetWidth = baseWidth;
    for (int iCol = curColsSheet->colStart; iCol < m_pTV->model()->columnCount(); iCol++) {
      if ( (curColsSheet->sheetWidth + (m_pTV->columnWidth(iCol).value() * pPdf->pageProperties.em2Point)) > pPdf->pageProperties.pageWidth ) break;
      curColsSheet->colEnd++;
      curColsSheet->sheetWidth += (m_pTV->columnWidth(iCol).value() * pPdf->pageProperties.em2Point);
    }
    curColsSheet->sheetWidth -= (pPdf->pageProperties.pageLeftMargin + pPdf->pageProperties.pageRightMargin);
    curColsSheet->nbColumns            = curColsSheet->colEnd - curColsSheet->colStart;
    if ( curColsSheet->colEnd == m_pTV->model()->columnCount() ) break;
    curColsSheet->nextSheet            = gdPdf_getNewColsSheet();
    curColsSheet->nextSheet->prevSheet = curColsSheet;
    curColsSheet                       = curColsSheet->nextSheet;
    curColsSheet->colStart             = curColsSheet->prevSheet->colEnd;
    curColsSheet->colEnd               = curColsSheet->colStart;
    pPdf->pageProperties.nbColumnsSheets++;
  }
  /* START ROWS COMPUTING */
  // Calcul de la hauteur maxi
  float headerHeight = pPdf->pageProperties.pageTopMargin + (pPdf->pageProperties.pageTitlesHeight * 2);
  float footerHeight = pPdf->pageProperties.footerHeight  + pPdf->pageProperties.pageBottomMargin;
  float rowsHeight   = (pPdf->pageProperties.headerHeightOfRow * pPdf->pageProperties.headerMaxNbRows);
  for (int iRow = 0; iRow < pPdf->pageProperties.pageNbRows; iRow++)
    rowsHeight += (pPdf->pageProperties.headerHeightOfRow * pPdf->pageProperties.nbRowsOfRowLabel[iRow]);
  float heightNeeded          = headerHeight + rowsHeight + footerHeight;
  float sheetHeightAvailable  = pPdf->pageProperties.pageHeight - (headerHeight + footerHeight);
  // Calcul du nombre de tableaux necessaires pour afficher toutes les lignes
  pPdf->pageProperties.nbRowsSheets       = 1;
  ST_GDPDF_ROWS_SHEET*  curRowsSheet      = gdPdf_getNewRowsSheet();
  pPdf->pageProperties.rootSheetRows      = curRowsSheet;
  curRowsSheet->rowStart                  = 0;
  curRowsSheet->rowEnd                    = curRowsSheet->rowStart;
  while (1) {
    curRowsSheet->sheetHeight = pPdf->pageProperties.pageHeightAfterTitles + (pPdf->pageProperties.headerHeightOfRow * pPdf->pageProperties.headerMaxNbRows);
    for (int iRow = curRowsSheet->rowStart; iRow < pPdf->pageProperties.pageNbRows; iRow++) {
      if ( (curRowsSheet->sheetHeight + (pPdf->pageProperties.headerHeightOfRow * pPdf->pageProperties.nbRowsOfRowLabel[iRow])) > sheetHeightAvailable) break;
      curRowsSheet->rowEnd++;
      curRowsSheet->sheetHeight += (pPdf->pageProperties.headerHeightOfRow * pPdf->pageProperties.nbRowsOfRowLabel[iRow]);
    }
    curRowsSheet->nbRows               = curRowsSheet->rowEnd - curRowsSheet->rowStart;
    if ( curRowsSheet->rowEnd == pPdf->pageProperties.pageNbRows ) break;
    curRowsSheet->nextSheet            = gdPdf_getNewRowsSheet();
    curRowsSheet->nextSheet->prevSheet = curRowsSheet;
    curRowsSheet                       = curRowsSheet->nextSheet;
    curRowsSheet->rowStart             = curRowsSheet->prevSheet->rowEnd;
    curRowsSheet->rowEnd               = curRowsSheet->rowStart;
    pPdf->pageProperties.nbRowsSheets++;
  }
}

HPDF_Page gdViewToPdf::pageInit(gdStPdf* pPdf, HPDF_Outline outline)
{
  HPDF_Page page = HPDF_AddPage(pPdf->pdfHandle);
  HPDF_Page_SetTextLeading(page, pPdf->pageProperties.headerHeightOfRow);
  if ( !pPdf->pageProperties.nOptimized ) {
    setPageProperties(pPdf, page);
    HPDF_Destination dst = HPDF_Page_CreateDestination(page);
    HPDF_Destination_SetXYZ(dst, 0, HPDF_Page_GetHeight(page), 1);
    HPDF_Destination_SetFit(dst);
    HPDF_Outline_SetDestination(outline, dst);
    //  page = HPDF_AddPage(pPdf->pdfHandle);
  }
  HPDF_Page_SetSize(page, HPDF_PAGE_SIZE_A4,  pPdf->pageProperties.pageOrientation);
  pPdf->pageProperties.startWidth  = 0;
  pPdf->pageProperties.startHeight = 0;
  gdPdf_setFontTitle(pPdf, page);
  // TODO : print a title
  return page;
}

void gdViewToPdf::pageClose(gdStPdf* pPdf, HPDF_Page page, int curPage, int curSubPage, int maxPages, HPDF_Outline outline)
{
  char            buf[128];
  sprintf(buf, "page %d-%d of %d.", curPage, curSubPage, maxPages);
  gdPdf_print_date(pPdf, page);
  gdPdf_print_pageNumber(pPdf, page, buf);
  HPDF_Destination dst = HPDF_Page_CreateDestination(page);
  HPDF_Destination_SetXYZ(dst, 0, HPDF_Page_GetHeight(page), 1);
  HPDF_Destination_SetFit(dst);
  HPDF_Outline_SetDestination(outline, dst);
}

void gdViewToPdf::logStruct(gdStPdf* pPdf)
{
  const int LOGSTRUCT = 1;
  std::string err;
  if ( !LOGSTRUCT ) return;
  err += " page em2Point = ";
  err += boost::lexical_cast<std::string>(pPdf->pageProperties.em2Point);
  err += " page nOptimized = ";
  err += boost::lexical_cast<std::string>(pPdf->pageProperties.nOptimized);
  err += ", page pageOrientation = ";
  err += boost::lexical_cast<std::string>(pPdf->pageProperties.pageOrientation);
  err += ", pageWidth = ";
  err += boost::lexical_cast<std::string>(pPdf->pageProperties.pageWidth);
  err += ", pageHeight = ";
  err += boost::lexical_cast<std::string>(pPdf->pageProperties.pageHeight);
  err += ", pageTopMargin = ";
  err += boost::lexical_cast<std::string>(pPdf->pageProperties.pageTopMargin);
  err += ", pageLeftMargin = ";
  err += boost::lexical_cast<std::string>(pPdf->pageProperties.pageLeftMargin);
  err += ", pageRightMargin = ";
  err += boost::lexical_cast<std::string>(pPdf->pageProperties.pageRightMargin);
  err += ", pageBottomMargin = ";
  err += boost::lexical_cast<std::string>(pPdf->pageProperties.pageBottomMargin);
  err += ", pageNbRows = ";
  err += boost::lexical_cast<std::string>(pPdf->pageProperties.pageNbRows);
  err += ", pageNbColumns = ";
  err += boost::lexical_cast<std::string>(pPdf->pageProperties.pageNbColumns);
  err += ", headerHeightOfRow = ";
  err += boost::lexical_cast<std::string>(pPdf->pageProperties.headerHeightOfRow);
  err += "\n headerColsWidth : ";
  for (int i = 0; i < pPdf->pageProperties.pageNbColumns; i++) {
    if ( i ) err += ", ";
    err += boost::lexical_cast<std::string>(pPdf->pageProperties.headerColsWidth[i]);
  }
  err += "\n headerMaxNbRows = ";
  err += boost::lexical_cast<std::string>(pPdf->pageProperties.headerMaxNbRows);
  err += "\n headerNbRowsOfEachColumn : ";
  for (int i = 0; i < pPdf->pageProperties.pageNbColumns; i++) {
    if ( i ) err += ", ";
    err += boost::lexical_cast<std::string>(pPdf->pageProperties.headerNbRowsOfEachColumn[i]);
  }
  err += "\n nbRowsOfLabel = ";
  err += boost::lexical_cast<std::string>(pPdf->pageProperties.nbRowsOfLabel);
  err += "\n nbRowsOfRowLabel : ";
  for (int i = 0; i < pPdf->pageProperties.pageNbRows; i++) {
    if ( i ) err += ", ";
    err += boost::lexical_cast<std::string>(pPdf->pageProperties.nbRowsOfRowLabel[i]);
  }
  err += "\n footerHeight = ";
  err += boost::lexical_cast<std::string>(pPdf->pageProperties.footerHeight);
  err += ", nbColumnsSheets = ";
  err += boost::lexical_cast<std::string>(pPdf->pageProperties.nbColumnsSheets);
  err += ", nbRowsSheets = ";
  err += boost::lexical_cast<std::string>(pPdf->pageProperties.nbRowsSheets);
  std::cerr << err << std::endl;
  ST_GDPDF_ROWS_SHEET* curRowsSheet = pPdf->pageProperties.rootSheetRows;
  while (curRowsSheet) {
    err = " curRowSheet height=";
    err += boost::lexical_cast<std::string>(curRowsSheet->sheetHeight);
    err += ", nbRowss=";
    err += boost::lexical_cast<std::string>(curRowsSheet->nbRows);
    err += ", rowStart=";
    err += boost::lexical_cast<std::string>(curRowsSheet->rowStart);
    err += ", rowEnd=";
    err += boost::lexical_cast<std::string>(curRowsSheet->rowEnd);
    err += ", pageBreak=";
    err += boost::lexical_cast<std::string>(curRowsSheet->pageBreak);
    err += ".";
    std::cerr << err << std::endl;
    curRowsSheet = curRowsSheet->nextSheet;
  }
  ST_GDPDF_COLS_SHEET* curColsSheet = pPdf->pageProperties.rootSheetColumns;
  while (curColsSheet) {
    err = " curColsSheet width=";
    err += boost::lexical_cast<std::string>(curColsSheet->sheetWidth);
    err += ", nbCols=";
    err += boost::lexical_cast<std::string>(curColsSheet->nbColumns);
    err += ", colStart=";
    err += boost::lexical_cast<std::string>(curColsSheet->colStart);
    err += ", colEnd=";
    err += boost::lexical_cast<std::string>(curColsSheet->colEnd);
    err += ", pageBreak=";
    err += boost::lexical_cast<std::string>(curColsSheet->pageBreak);
    err += ".";
    std::cerr << err << std::endl;
    curColsSheet = curColsSheet->nextSheet;
  }
}

void gdViewToPdf::printPdfPage(int curPage, int maxPages, HPDF_Outline outline)
{
  gdStPdf* pPdf = m_pPdf;
  if ( !pPdf ) return;
  HPDF_Page page         = pageInit(pPdf, outline);
  bool      bReleasePage = true;
  logStruct(pPdf);
  ST_GDPDF_ROWS_SHEET* curRowsSheet = pPdf->pageProperties.rootSheetRows;
  int subPage = 1;
  while (curRowsSheet) {
    ST_GDPDF_COLS_SHEET* curColsSheet = pPdf->pageProperties.rootSheetColumns;
    while (curColsSheet) {
      if ( !bReleasePage ) {
        page = pageInit(pPdf, outline);
        bReleasePage = true;
      }
      gdPdf_setFontColTitle(pPdf, page);
      std::string strHeader = asString(m_pTV->model()->headerData(0)).toUTF8();
      if ( useOldLabel ) {
        for (int nLabel = 0; nLabel < pPdf->pageProperties.headerNbRowsOfEachColumn[0]; nLabel++) {
          std::string target;
          target.assign(strHeader, (nLabel * pPdf->pageProperties.headerColsWidth[0]), pPdf->pageProperties.headerColsWidth[0]);
          gdPdf_printColTitleByRow(pPdf, page, 0, 0, nLabel, target.c_str(), 0);
        }
      } else
        gdPdf_printColTitle(pPdf, page, 0, 0, strHeader.c_str(), pPdf->pageProperties.headerMaxNbRows, pPdf->pageProperties.headerNbRowsOfEachColumn[0], 0, 0);
      for (int iCol = curColsSheet->colStart; iCol < curColsSheet->colEnd; iCol++) {
        std::string strHeader = asString(m_pTV->model()->headerData(iCol)).toUTF8();
        if ( useOldLabel ) {
          for (int nLabel = 0; nLabel < pPdf->pageProperties.headerNbRowsOfEachColumn[iCol]; nLabel++) {
            std::string target;
            target.assign(strHeader, (nLabel * pPdf->pageProperties.headerColsWidth[iCol]), pPdf->pageProperties.headerColsWidth[iCol]);
            gdPdf_printColTitleByRow(pPdf, page, curColsSheet->colStart, iCol, nLabel, target.c_str(), 0);
          }
        } else
          gdPdf_printColTitle(pPdf, page, curColsSheet->colStart, iCol, strHeader.c_str(), pPdf->pageProperties.headerMaxNbRows,
                              pPdf->pageProperties.headerNbRowsOfEachColumn[iCol], 0, 0);
      }
      int align = 0;
      if ( m_pTV->columnAlignment(0) == Wt::AlignRight ) align = 1;
      for (int iRow = curRowsSheet->rowStart; iRow < curRowsSheet->rowEnd; iRow++) {
        gdPdf_setFontRowTitle(pPdf, page);
        std::string strHeader = asString(m_pTV->model()->data(iRow + m_l1row, 0)).toUTF8();
        gdPdf_printRowTitle(pPdf, page, curRowsSheet->rowStart, iRow, strHeader.c_str(), pPdf->pageProperties.nbRowsOfRowLabel[iRow], 0,
                            0, curColsSheet->sheetWidth, align);
        gdPdf_setFontQte(pPdf, page);
        for (int iCol = curColsSheet->colStart; iCol < curColsSheet->colEnd; iCol++) {
          std::string strData = asString(m_pTV->model()->data(iRow + m_l1row, iCol)).toUTF8();
          int align = 0;
          if ( m_pTV->columnAlignment(iCol) == Wt::AlignRight ) align = 1;
          if ( !gdPdf_printQte(pPdf, page, curRowsSheet->rowStart, iRow, curColsSheet->colStart, iCol, strData.c_str(), 0, align) )
            gdPdf_printQte(pPdf, page, curRowsSheet->rowStart, iRow, curColsSheet->colStart, iCol, "###", 0, align);
        }
      }
      pPdf->pageProperties.startHeight += curRowsSheet->sheetHeight;
      int pageBreakCol = curColsSheet->pageBreak;
      if ( pPdf->pageProperties.startHeight > (pPdf->pageProperties.pageHeight - (pPdf->pageProperties.startHeight + pPdf->pageProperties.pageTopMargin
           + (pPdf->pageProperties.pageTitlesHeight * 2) + pPdf->pageProperties.footerHeight + pPdf->pageProperties.pageBottomMargin)) )
        pageBreakCol = 1;
      if ( pageBreakCol ) {
        pageClose(pPdf, page, curPage, subPage++, maxPages, outline);
        bReleasePage = false;
        pageBreakCol = 0;
      }
      curColsSheet = curColsSheet->nextSheet;
    }
    pPdf->pageProperties.startHeight = 0;
    curColsSheet = pPdf->pageProperties.rootSheetColumns;
    float width = 0.0;
    while (curColsSheet) {
      if ( curColsSheet->sheetWidth > width ) width = curColsSheet->sheetWidth;
      curColsSheet = curColsSheet->nextSheet;
    }
    pPdf->pageProperties.startWidth = (width + pPdf->pageProperties.pageLeftMargin);
    int pageBreakRow = curRowsSheet->pageBreak;
    curRowsSheet = curRowsSheet->nextSheet;
    if ( pageBreakRow ) {
      pageClose(pPdf, page, curPage, subPage++, maxPages, outline);
      bReleasePage = false;
      pPdf->pageProperties.startWidth = 0;
      pageBreakRow = 0;
    }
  }
  if ( bReleasePage ) {
    pageClose(pPdf, page, curPage, subPage++, maxPages, outline);
  }
}

