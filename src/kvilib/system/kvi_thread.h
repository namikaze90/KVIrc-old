#ifndef _KVI_THREAD_H_
#define _KVI_THREAD_H_
//
//   File : kvi_thread.h
//   Creation date : Mon May 17 1999 04:26:41 CEST by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 1999-2000 Szymon Stefanek (pragma at kvirc dot net)
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

#include "kvi_settings.h"
#include "kvi_heapobject.h"
#include "kvi_string.h"

#include <qnamespace.h>
#include <qobject.h>
#include <qsocketnotifier.h>

#include <qevent.h>
#include <QList>
#include <QQueue>
#include <QThread>
#include <QMutex>

//
// Simple thread implementation
// This is enough for KVIrc needs
// HANDLE WITH CARE
//


// simple thread class implementation
// this is also called "Blind" thread class

class KVILIB_API KviThread : public QThread
{
public:
	KviThread();
	virtual ~KviThread();
private:
	QMutex         * m_pRunningMutex;
	QList<QEvent*> * m_pLocalEventQueue;
public:
	static void sleep(unsigned long sec);
	static void msleep(unsigned long msec);
	static void usleep(unsigned long usec);
protected:
	// The tricky part: threadsafe event dispatching
	// Slave thread -> main thread objects
	void postEvent(QObject *o,QEvent *e);
};

// QEvent::Type for Thread events
#define KVI_THREAD_EVENT (((int)QEvent::User) + 2000)

// CONSTANTS FOR KviThreadEvent::eventId();

///////////////////////////////////////////////////////////////
// extern -> slave thread

// Your reimplementation of KviSensitiveThread MUST handle this
// and exit when this event is received

// Terminate is a plain KviThreadEvent
#define KVI_THREAD_EVENT_TERMINATE 0

///////////////////////////////////////////////////////////////
// slave thread -> master object

// The following standard events are sent from the thread to the master object

// The following are plain KviThreadEvent objects
#define KVI_THREAD_EVENT_SUCCESS 100

// The following are KviThreadDataEvent<int>
#define KVI_THREAD_EVENT_STATECHANGE 150

// The following are KviThreadDataEvent<KviStr>
#define KVI_THREAD_EVENT_MESSAGE 200
#define KVI_THREAD_EVENT_WARNING 201
#define KVI_THREAD_EVENT_ERROR 202
#define KVI_THREAD_EVENT_DATA 203

// The following is KviThreadDataEvent<KviDataBuffer>
#define KVI_THREAD_EVENT_BINARYDATA 300

// The user events
#define KVI_THREAD_USER_EVENT_BASE 1000

// #warning "Get rid of the m_szMessage member of KviThreadEvent : eventual data should be passed with a KviThreadDataEvent"

// Base class for all thread events
class KVILIB_API KviThreadEvent : public QEvent, public KviHeapObject
{
protected:
	int         m_eventId;
	KviThread * m_pSender;
public:
	KviThreadEvent(int evId,KviThread * sender = 0)
		: QEvent((QEvent::Type)KVI_THREAD_EVENT) , m_eventId(evId) , m_pSender(sender) {};
	virtual ~KviThreadEvent(){};
public:
	// This is the sender of the event
	// WARNING : this MAY be null , threads CAN send anonymous events
	KviThread * sender(){ return m_pSender; };
	int id(){ return m_eventId; };
};

template<class TData> class KviThreadDataEvent : public KviThreadEvent
{
protected:
	TData * m_pData;
public:
	KviThreadDataEvent(int evId,TData * pData = 0,KviThread * sender = 0)
		: KviThreadEvent(evId,sender){ m_pData = pData; };
	virtual ~KviThreadDataEvent(){ if(m_pData)delete m_pData; };
public:
	void setData(TData * d){ if(m_pData)delete m_pData; m_pData = d; };
	TData * getData(){ TData * aux = m_pData; m_pData = 0; return aux; };
	TData * data(){ return m_pData; };
};

// A thread that has also an internal event queue
// so events can be posted from the master side to the slave one
// Reimplementations of this class should periodically check
// dequeueEvent() and eventually process the incoming events (and then DELETE it)

// KVI_THREAD_EVENT_TERMINATE should be always handled by the reimplementation
// and it should always exit (cleanly) when this event is received


class KVILIB_API KviSensitiveThread : public KviThread
{
public:
	KviSensitiveThread();
	virtual ~KviSensitiveThread();
protected:
	QMutex                  * m_pLocalEventQueueMutex;
	QQueue<KviThreadEvent*> * m_pLocalEventQueue;
public:
	// enqueues an event directed to THIS thread
	// the event must be allocated with NEW and
	// will be destroyed on the slave side
	void enqueueEvent(KviThreadEvent *e);
	// enqueues a terminate event and waits() for the slave thread
	// the slave thread MUST handle KVI_THREAD_EVENT_TERMINATE
	void terminate();
protected:
	// slave side:
	// returns the first event in the local queue
	// the event MUST BE DELETED after processing
	KviThreadEvent * dequeueEvent();
};

// =============================================================================================//
// This is private stuff...only KviThread and KviApp may use it
// and may call only specific functions...don't touch.

typedef struct _KviThreadPendingEvent
{
	QObject *o;
	QEvent  *e;
} KviThreadPendingEvent;

class KVILIB_API KviThreadManager : public QObject
{
	friend class KviApp;
	friend class KviThread;
	Q_OBJECT
protected:
	// These should be private...but we don't want anyone to complain
	// Treat as private plz.
	KviThreadManager();
	~KviThreadManager();
private:
	QMutex * m_pMutex; // This class performs only atomic operations
	QList<KviThread*> * m_pThreadList;
	int m_iWaitingThreads;
protected:
	// Public to KviThread only
	void registerSlaveThread(KviThread *t);
	void unregisterSlaveThread(KviThread *t);

	void threadEnteredWaitState();
	void threadLeftWaitState();

	// Public to KviApp only
	static void globalInit();
	static void globalDestroy();
	
	void postSlaveEvent(QObject *o,QEvent *e);
};


#endif //!_KVI_THREAD_H_
