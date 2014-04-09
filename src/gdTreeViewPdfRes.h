#ifndef __gdTVPdfResource__HEADER__
#define __gdTVPdfResource__HEADER__

#include <string>

#include <Wt/WFileResource>
#include <Wt/WAbstractItemView>

class gdTVPdfResource : public Wt::WFileResource {
public:
  gdTVPdfResource(Wt::WObject* parent = 0);
  void  setFirstRow(long firstRow)      {
    m_l1row = firstRow;
  };
  void  setView(Wt::WAbstractItemView* pTV) {
    m_pTV = pTV;
  };

protected :
  virtual void              handleRequest(const Wt::Http::Request& request, Wt::Http::Response& response);

private :
  Wt::WAbstractItemView*                  m_pTV;
  long                                    m_l1row;
};



#endif // __gdTVPdfResource__HEADER__
