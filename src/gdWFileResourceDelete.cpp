/*
 * Copyright (C) 2006-Today G. Deleeuw
 *
 * See the LICENSE file for terms of use.
 */
#include <stdio.h>

#include <boost/filesystem/operations.hpp>

#include <Wt/Http/Response>

#include "gdWFileResourceDelete.h"

using namespace Wt;

gdWFileResourceDelete::gdWFileResourceDelete(const std::string& mimeType, const std::string& fileName, WObject* parent)
  : WFileResource(mimeType, fileName, parent), m_bDelete(true)
{ }

void gdWFileResourceDelete::deleteAfterProcessed(bool bDelete)
{
  m_bDelete = bDelete;
}

void gdWFileResourceDelete::handleRequest(const Http::Request& request, Http::Response& response)
{
  WFileResource::handleRequest(request, response);
  // this was the last data for the request
  if ( !response.continuation() )
    if ( m_bDelete )
      unlink(fileName().c_str());
}

