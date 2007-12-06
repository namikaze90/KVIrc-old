//=============================================================================
//
//   File : kvi_thread.cpp
//   Creation date : Tue Jul 6 1999 16:04:45 CEST by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 1999-2005 Szymon Stefanek (pragma at kvirc dot net)
//
//   This program is FREE software. You can redistribute it and/or
//   modify it under the terms of the GNU General Public License
//   as published by the Free Software Foundation; either version 2
//   of the License, or (at your opinion) any later version.
//
//   This program is distributed in the HOPE that it will be USEFUL,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//   See the GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program. If not, write to the Free Software Foundation,
//   Inc. ,59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//
//=============================================================================

#define __KVILIB__


#ifndef _GNU_SOURCE
	#define _GNU_SOURCE
#endif

#include "kvi_thread.h"

#include <errno.h>


#include "kvi_string.h"
#include "kvi_settings.h"
#include "kvi_error.h"


#include <QApplication>


static KviThreadManager * g_pThreadManager = 0;

void KviThreadManager::globalInit()
{
	g_pThreadManager = new KviThreadManager();
}

void KviThreadManager::globalDestroy()
{
	delete g_pThreadManager;
    g_pThreadManager = 0;
}

KviThreadManager::KviThreadManager()
: QObject()
{
	if(g_pThreadManager)debug("Hey...what are ya doing ?");

	m_pMutex = new QMutex();
	m_pThreadList = new QList<KviThread*>;
	m_iWaitingThreads = 0;

}


KviThreadManager::~KviThreadManager()
{
	m_pMutex->lock();
	// Terminate all the slaves
	foreach(KviThread *t,*m_pThreadList)
	{
		m_pMutex->unlock();
		delete t;
		m_pMutex->lock();
	}

	// there are no more child threads
	// thus no more slave events are sent.
	// Disable the socket notifier, we no longer need it

	// we're no longer in this world
	g_pThreadManager = 0;

	m_pMutex->unlock();

	// finish the cleanup
	delete m_pMutex;
	m_pMutex = 0;
	delete m_pThreadList;
	m_pThreadList = 0;

	// byez :)
}


void KviThreadManager::registerSlaveThread(KviThread *t)
{
	m_pMutex->lock();
	m_pThreadList->append(t);
	m_pMutex->unlock();
}

void KviThreadManager::unregisterSlaveThread(KviThread *t)
{
	m_pMutex->lock();
	m_pThreadList->removeAll(t);
	m_pMutex->unlock();
}

void KviThreadManager::postSlaveEvent(QObject *o,QEvent *e)
{
	QApplication::postEvent(o,e); // we believe this to be thread-safe
}


void KviThreadManager::threadEnteredWaitState()
{
	m_pMutex->lock();
	m_iWaitingThreads++;
	m_pMutex->unlock();
}

void KviThreadManager::threadLeftWaitState()
{
	m_pMutex->lock();
	m_iWaitingThreads--;
	if(m_iWaitingThreads < 0)
	{
		debug("Ops.. got a negative number of waiting threads ?");
		m_iWaitingThreads = 0;
	}
	m_pMutex->unlock();
}

KviThread::KviThread()
{
	g_pThreadManager->registerSlaveThread(this);
	m_pRunningMutex = new QMutex();
}

KviThread::~KviThread()
{
//	debug(">> KviThread::~KviThread() : (this = %d)",this);
	wait();
	delete m_pRunningMutex;
	g_pThreadManager->unregisterSlaveThread(this);
//	debug("<< KviThread::~KviThread() : (this = %d)",this);
}


void KviThread::usleep(unsigned long usec)
{
	QThread::usleep(usec);
}

void KviThread::msleep(unsigned long msec)
{
	QThread::msleep(msec);
}

void KviThread::sleep(unsigned long sec)
{
	QThread::sleep(sec);
}

void KviThread::postEvent(QObject * o,QEvent *e)
{
	// slave side
	g_pThreadManager->postSlaveEvent(o,e);
}



KviSensitiveThread::KviSensitiveThread()
: KviThread()
{
	m_pLocalEventQueueMutex = new QMutex();
	m_pLocalEventQueue = new QQueue<KviThreadEvent*>;
}

KviSensitiveThread::~KviSensitiveThread()
{
//	debug("Entering KviSensitiveThread::~KviSensitiveThread (this=%d)",this);
	terminate();
//	debug("KviSensitiveThread::~KviSensitiveThread : terminate called (This=%d)",this);
	m_pLocalEventQueueMutex->lock();
	foreach(KviThreadEvent *t,*m_pLocalEventQueue)
	{
		delete t;
	}
	delete m_pLocalEventQueue;
	m_pLocalEventQueue = 0;
	m_pLocalEventQueueMutex->unlock();
	delete m_pLocalEventQueueMutex;
    m_pLocalEventQueueMutex = 0;
//	debug("Exiting KviSensitiveThread::~KviSensitiveThread (this=%d)",this);
}

void KviSensitiveThread::enqueueEvent(KviThreadEvent *e)
{
//	debug(">>> KviSensitiveThread::enqueueEvent() (this=%d)",this);
	m_pLocalEventQueueMutex->lock();
	if(!m_pLocalEventQueue)
	{
		// ops...already terminated (???)...eat the event and return
		delete e;
		m_pLocalEventQueueMutex->unlock();
		return;
	}
	m_pLocalEventQueue->enqueue(e);
	m_pLocalEventQueueMutex->unlock();
//	debug("<<< KviSensitiveThread::enqueueEvent() (this=%d)",this);
}

KviThreadEvent * KviSensitiveThread::dequeueEvent()
{
//	debug(">>> KviSensitiveThread::dequeueEvent() (this=%d)",this);
	KviThreadEvent * ret = 0;
	m_pLocalEventQueueMutex->lock();
	if(!m_pLocalEventQueue->isEmpty()) ret = m_pLocalEventQueue->dequeue();
	m_pLocalEventQueueMutex->unlock();
//	debug("<<< KviSensitiveThread::dequeueEvent() (this=%d)",this);
	return ret;
}

void KviSensitiveThread::terminate()
{
//	debug("Entering KviSensitiveThread::terminate (this=%d)",this);
	enqueueEvent(new KviThreadEvent(KVI_THREAD_EVENT_TERMINATE));
//	debug("KviSensitiveThread::terminate() : event enqueued waiting (this=%d)",this);
	wait();
//	debug("Exiting KviSensitiveThread::terminate (this=%d)",this);
}

