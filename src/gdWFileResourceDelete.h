#ifndef __gdWFileResourceDelete_HEADER__
#define __gdWFileResourceDelete_HEADER__

#include <vector>

#include <Wt/WFileResource>

class gdWFileResourceDelete : public Wt::WFileResource
 {
  public:
          gdWFileResourceDelete(const std::string& mimeType, const std::string& fileName, Wt::WObject* parent = 0);
   void   deleteAfterProcessed(bool bDelete);

  protected :
    virtual void              handleRequest(const Wt::Http::Request& request, Wt::Http::Response &response);

  private :
   bool                       m_bDelete;
};

#endif // __gdWFileResourceDelete_HEADER__

