#ifndef _gdViewToOds_HEADER
#define _gdViewToOds_HEADER

#include <Wt/WAbstractItemView>

class gdViewToOds {
public:
  gdViewToOds(const char* odsFile, const Wt::WAbstractItemView* pTV);
  ~gdViewToOds();
  void initialize();
  void finalize();
  void initDocument();
  void finishDocument();
  void initStyles();
  void initTab(const std::string& tabLabel);
  void finishTab();
  void initSheet();
  void finishSheet();
  void buildHeader();
  void buildRows();
  void buildRow(int iRow);

public :
  const Wt::WAbstractItemView*   m_pTV;
  std::string                    m_strXmlFile;
  std::string                    m_strOdsFile;
  FILE*                          m_pOdsHandle;
};

#endif // _gdViewToOds_HEADER
