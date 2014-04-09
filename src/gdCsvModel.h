// This may look like C code, but it's really -*- C++ -*-
/*
 * Copyright (C) 2008 Emweb bvba, Kessel-Lo, Belgium.
 *
 * See the LICENSE file for terms of use.
 */
#ifndef gdCsvModel_H_
#define gdCsvModel_H_

/*! \file gdCsvModel.h */

#include "gdStdModel.h"

/*! \class gdCsvModel
 *  \brief A class to load a csv file into a Model
*/
class gdCsvModel : public gdStdModel {
public :
  /*! \brief Constructor.  */
  gdCsvModel();

  /*! \brief Destructor.  */
  ~gdCsvModel();

  /*! \brief Set the full path to the csv file.  */
  void        setFilePath(std::string& strFilePath);

  /*! \brief Set the field separator.  */
  void        setFieldSeparator(char sep);

  /*! \brief Load the model or reload.  */
  void        reload();

public :
  std::vector<std::string>    m_vColumnsDBNames;
  std::vector<std::string>    m_vColumnsNames;
  std::vector<int>            m_vColumnsWidth;

private :
  std::string           m_csvFilePath;
  int                   m_nMaxRows;
  char                  m_cFieldSeparator;
};

#endif // gdCsvModel_H_

