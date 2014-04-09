/*
 * Copyright (C) 2006-Today Guy Deleeuw
 *
 * See the LICENSE file for terms of use.
 */

/*! \file gdWtThread.h */

#ifndef __gdWtThread__
#define __gdWtThread__

#include <boost/filesystem/path.hpp>
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>
#include <boost/bind.hpp>

#include <Wt/WApplication>
#include <Wt/WObject>
#include <Wt/WTimer>

/*! \class gdWtThread
 *  \brief A class that attach a thread to Wt app
*/
class gdWtThread : public Wt::WObject {
public :
  /*! \brief Constructor.  */
  gdWtThread();
  /*! \brief Destructor.  */
  ~gdWtThread();

  /*! \brief specify the function called when the startJobprocessing is called. */
  void setProcessCallback(boost::function<void ()> pFunc);

  /*! \brief Process a job. */
  void startJobProcessing();

private :
  void startThread(Wt::WApplication* app);
  void stopThread();

private slots :
  void isJobTerminated();

public :
  Wt::Signal<void>& jobTerminated() {
    return jobTerminated_;
  };

private :
  Wt::Signal<void> jobTerminated_;

private :
  Wt::WTimer                       m_timer;
  boost::thread                    m_thread;
  boost::recursive_mutex           m_mutex;
  boost::condition                 m_condition;
  volatile bool                    m_stopThread;
  bool                             m_bBusy;
  boost::function<void ()>         processCallback;
  Wt::WApplication*                m_pApp;
};

#endif // __gdWtThread__

