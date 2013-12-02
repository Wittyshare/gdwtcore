/*
 * Copyright (C) 2008 Emweb bvba, Kessel-Lo, Belgium.
 *
 * See the LICENSE file for terms of use.
 */

#include <iostream>
#include <fstream>

#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/replace.hpp>

#include <Wt/WString>

#include "gdCsvModel.h"

using namespace Wt;

gdCsvModel::gdCsvModel()
 : gdStdModel(), m_nMaxRows(-1), m_cFieldSeparator(',')
 { }

gdCsvModel::~gdCsvModel()
 { }

void gdCsvModel::setFilePath(std::string& strFilePath)
 { m_csvFilePath = strFilePath; }

void gdCsvModel::setFieldSeparator(char sep)
 { m_cFieldSeparator = sep; }

void gdCsvModel::reload()
 {
  std::ifstream fcsv(m_csvFilePath.c_str());
  int csvRow = 0;
  while (fcsv)
   {
    std::string line;
    getline(fcsv, line);

    std::string field;
    std::vector<std::string> fields;
    bool onStr = false;
    char theCar;
    for(int nCar = 0; nCar < line.size(); nCar++)
     {
      theCar = line[nCar];
      if ( onStr )
       {
         if ( theCar == '"' ) { onStr = false; continue; }
          else                { field.push_back(theCar); continue; }
       }
      else
        if ( theCar == '"' )            { onStr = true; continue; }
     if ( theCar == m_cFieldSeparator ) { fields.push_back(field); field.clear(); continue; }
      else                              { field.push_back(theCar); continue; }
     }
    fields.push_back(field);
    field.clear();

    if ( fcsv )
      {
       for(int col = 0; col < fields.size(); col++)
        {
         if ( col >= columnCount() ) insertColumns(columnCount(), col + 1 - columnCount());

         if ( !csvRow )
           {
            m_vColumnsDBNames.push_back(fields[col]);
            int colWidth = atoi(WString::tr(m_vColumnsDBNames[col] + "_width").narrow().c_str());
            if ( !colWidth ) colWidth = 120;
            m_vColumnsWidth.push_back(colWidth);

            std::string strColName = WString::tr(m_vColumnsDBNames[col]).narrow();
            boost::algorithm::replace_all(strColName, "?", "");
            m_vColumnsNames.push_back(strColName);

            setColumnType(col, type_int4);
           }

	 if ( isHeaderUsed() && csvRow == -1 )
	    setHeaderData(col, boost::any(Wt::WString::fromUTF8(fields[col])));
	  else
           {
	    int dataRow = isHeaderUsed() ? csvRow - 1 : csvRow;

	    if ( m_nMaxRows != -1 && dataRow >= m_nMaxRows ) return;

	    if (dataRow >= rowCount())
	       insertRows(rowCount(), dataRow + 1 - rowCount());

	    boost::any data;
	    std::string s = fields[col];
	    //std::string s = Wt::WString::fromUTF8(fields[col]);

	    try // TODO : Tester si int4 ne prend pas la main sur double
              {
	       int i = boost::lexical_cast<int>(s);
	       data = boost::any(i);
               setColumnType(col, type_int4);
	      }
             catch (boost::bad_lexical_cast&)
               {
	        try
                  {
	           double d = boost::lexical_cast<double>(s);
	           data = boost::any(d);
                   setColumnType(col, type_double);
	          } catch (boost::bad_lexical_cast&)
                      {
	               data = boost::any(Wt::WString::fromUTF8(s));
                       setColumnType(col, type_text);
	              }
	       }
	    setData(dataRow, col, data, UserRole);
	    data = boost::any(Wt::WString::fromUTF8(s));
	    setData(dataRow, col, data, DisplayRole);
	  }
      }
    }

    ++csvRow;
  }
 }

