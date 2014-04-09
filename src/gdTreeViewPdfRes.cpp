/*
 * Copyright (C) 2006-Today G. Deleeuw
 *
 * See the LICENSE file for terms of use.
 */
#include <stdio.h>

#include <boost/filesystem/operations.hpp>

#include <Wt/Http/Response>

#include "gdViewToPdf.h"
#include "gdApplication.h"

#include "gdTreeViewPdfRes.h"

using namespace Wt;

gdTVPdfResource::gdTVPdfResource(WObject* parent) : WFileResource("application/pdf", "/tmp/none.pdf", parent), m_pTV(0), m_l1row(0)
{
  suggestFileName("tv.pdf");
}

void gdTVPdfResource::handleRequest(const Http::Request& request, Http::Response& response)
{
  // is not a continuation for more data of a previous request
  if ( !request.continuation() ) {
    if ( !m_pTV ) return;
    std::string strPdf = gdWApp->getUserTmpFile("pdf");
    fprintf(stderr, "construction du wtree2pdf\n");
    gdViewToPdf*  cPdf = new gdViewToPdf(strPdf.c_str(), m_pTV, 0, m_l1row);
    if ( !cPdf->m_pPdf ) return;
    fprintf(stderr, "Impression de la page\n");
    cPdf->printPdfPage(1, 1, 0);
    delete cPdf;
    setFileName(strPdf);
  }
  WFileResource::handleRequest(request, response);
  // this was the last data for the request
  if ( !response.continuation() )
    unlink(fileName().c_str());
}

