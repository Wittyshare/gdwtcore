#ifndef _gdViewToPdf_HEADER
#define _gdViewToPdf_HEADER

#include "hpdf.h"

#include <gdcore/gdPdf.h>

#include <Wt/WAbstractItemView>

class gdViewToPdf
 {
  public:
   gdViewToPdf(const char* pdfFile, Wt::WAbstractItemView* pTV, bool bFull, long firstRow = 0);
   ~gdViewToPdf();
   void setPageProperties(gdStPdf* pPdf, HPDF_Page page);
   void pageOptimize(gdStPdf* pPdf, HPDF_Page page, HPDF_PageDirection direction);
   HPDF_Page pageInit(gdStPdf* pPdf, HPDF_Outline outline);
   void pageClose(gdStPdf* pPdf, HPDF_Page page, int curPage, int curSubPage, int maxPages, HPDF_Outline outline);
   void logStruct(gdStPdf* pPdf);
   void printPdfPage(int curPage, int maxPages, HPDF_Outline outline);

  public :
   Wt::WAbstractItemView*         m_pTV;
   gdStPdf*                       m_pPdf;
   long                           m_l1row;
 };

#endif // _gdViewToPdf_HEADER
