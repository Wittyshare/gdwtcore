/*
 * Copyright (C) 2006-Today Guy Deleeuw
 *
 * See the LICENSE file for terms of use.
 */

#include <stdio.h>

#include <iostream>

#include <Wt/WLogger>

#include "gdWtThread.h"

using namespace Wt;

gdWtThread::gdWtThread() : WObject(), m_stopThread(false), processCallback(0), m_bBusy(false), m_pApp(0)
{
  m_timer.setInterval(1000); // Toutes les secondes
  m_timer.timeout().connect(SLOT(this, gdWtThread::isJobTerminated));
  m_thread = boost::thread(boost::bind(&gdWtThread::startThread, this, WApplication::instance()));
}

gdWtThread::~gdWtThread()
{
  if ( m_pApp )
    m_pApp->log("notice") << "Destroying Thread ...";
  stopThread();
}

void gdWtThread::setProcessCallback(boost::function<void ()> pFunc)
{
  processCallback = pFunc;
}

void gdWtThread::startThread(WApplication* app)
{
  m_pApp = app;
  if ( m_pApp )
    m_pApp->log("notice") << "In gdWtThread::startThread";
  boost::recursive_mutex::scoped_lock lock(m_mutex);
  while (m_stopThread == false) {
    if ( m_pApp )
      m_pApp->log("notice") << "Waiting for some job ...";
    m_condition.wait(lock); // On attend une notification de liberation du verrouillage
    m_bBusy = true;
    if ( m_pApp )
      m_pApp->log("notice") << "Thread unlocked";
    if ( m_stopThread ) break;
    if ( m_pApp )
      m_pApp->log("notice") << "Got work! Quickly doing it...";
    lock.unlock();
    if ( m_pApp )
      m_pApp->attachThread();
    if ( processCallback )
      processCallback();
    lock.lock();
    if ( m_pApp )
      m_pApp->attachThread(false);
    if ( m_pApp )
      m_pApp->log("notice") << "Work done :-)";
    m_bBusy = false;
  }
  if ( m_pApp )
    m_pApp->log("notice") << "Thread done !!!";
  if ( m_pApp )
    m_pApp->attachThread(false);
}

void gdWtThread::stopThread()
{
  if ( m_pApp )
    m_pApp->log("notice") << "Stoping Thread ...";
  boost::recursive_mutex::scoped_lock lock(m_mutex);
  m_stopThread = true;
  lock.unlock();
  m_condition.notify_one(); // on notifie une liberation du verrouillage
  m_thread.join();
  if ( m_pApp )
    m_pApp->log("notice") << "Thread joined";
}

void gdWtThread::startJobProcessing()
{
  if ( m_pApp )
    m_pApp->log("notice") << "gdWtThread::startJobProcessing : Locking Thread ...";
  boost::recursive_mutex::scoped_lock lock(m_mutex);
  // wake up the worker thread
  m_condition.notify_one(); // on notifie une liberation du verrouillage
  if ( m_pApp )
    m_pApp->log("notice") << "gdWtThread::startJobProcessing : Start Timer";
  m_timer.start();
}

void gdWtThread::isJobTerminated()
{
  boost::recursive_mutex::scoped_lock lock(m_mutex);
  if (!m_bBusy) {
    if ( m_pApp )
      m_pApp->log("notice") << "gdWtThread::isJobTerminated : Stop timer";
    m_timer.stop();
    jobTerminated().emit();
  }
}

