//=============================================================================
//
//   File : kvi_mdichild.cpp
//   Creation date : Wed Jun 21 2000 17:35:45 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 1999-2007 Szymon Stefanek (pragma at kvirc dot net)
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

/**
* \file kvi_mdichild.cpp
* \brief MDI subwindow stuff
*/

#define _KVI_DEBUG_CHECK_RANGE_
#include "kvi_debug.h"
#include "kvi_mdichild.h"
#include "kvi_mdimanager.h"
#include "kvi_string.h"
#include "kvi_locale.h"
#include "kvi_options.h"
#include "kvi_settings.h"
#include "kvi_iconmanager.h"
#include "kvi_window.h"
#include "kvi_pointerlist.h"
#include "kvi_tal_popupmenu.h"

#include <QCursor>
#include <QApplication>
#include <QFontMetrics>
#include <QPixmap>
#include <QStyle>
#include <QPainter>
#include <QEvent>
#include <QMouseEvent>
#include <QBoxLayout>

#ifdef COMPILE_ON_MAC
	#include "kvi_app.h"  //Needed for g_pApp
	#include <QDesktopWidget>
#endif

#ifdef COMPILE_PSEUDO_TRANSPARENCY
	extern QPixmap * g_pShadedChildGlobalDesktopBackground;
#endif


#define KVI_MDI_NORESIZE 0
#define KVI_MDI_RESIZE_TOP 1
#define KVI_MDI_RESIZE_LEFT 2
#define KVI_MDI_RESIZE_RIGHT 4
#define KVI_MDI_RESIZE_BOTTOM 8
#define KVI_MDI_RESIZE_TOPLEFT (1|2)
#define KVI_MDI_RESIZE_TOPRIGHT (1|4)
#define KVI_MDI_RESIZE_BOTTOMLEFT (8|2)
#define KVI_MDI_RESIZE_BOTTOMRIGHT (8|4)


KviMdiChild::KviMdiChild(KviMdiManager * par, const char * name)
: QMdiSubWindow()
{
	setObjectName(name ? name : "mdi_child");

	m_pManager = par;

	m_pClient = 0;

	connect(this, SIGNAL(windowStateChanged(Qt::WindowStates,Qt::WindowStates)), this, SLOT(windowStateChangedEvent(Qt::WindowStates,Qt::WindowStates)));
	connect(this, SIGNAL(hideSignal()), this, SLOT(minimize()), Qt::QueuedConnection);

	m_restoredGeometry   = QRect(10,10,320,240);
	setMinimumSize(KVI_MDICHILD_MIN_WIDTH,KVI_MDICHILD_MIN_HEIGHT);
	m_bCloseEnabled = true;
	m_State = Normal;
	setAutoFillBackground(true);
}

void KviMdiChild::setRestoredGeometry(const QRect &r)
{
	m_restoredGeometry = r;
	setGeometry(r);
}

KviMdiChild::~KviMdiChild()
{
	if(m_pClient) delete m_pClient;
}

void KviMdiChild::closeEvent(QCloseEvent * e)
{
	debug("Closing client");
	widget()->close();
	e->ignore();
}

QRect KviMdiChild::restoredGeometry()
{
	return geometry();
}

KviMdiChild::MdiChildState KviMdiChild::state()
{
	return m_State;
}

void KviMdiChild::setBackgroundRole(QPalette::ColorRole)
{
	// hack
	//QFrame::setBackgroundRole(QPalette::Window);
}

void KviMdiChild::setIcon(QPixmap pix)
{
	m_pIcon = pix;
	QMdiSubWindow::setWindowIcon(QIcon(pix));
}

const QPixmap * KviMdiChild::icon()
{
	return &m_pIcon;
}

void KviMdiChild::enableClose(bool bEnable)
{
	m_bCloseEnabled = bEnable;
}

bool KviMdiChild::closeEnabled()
{
	return m_bCloseEnabled;
}

void KviMdiChild::setWindowTitle(const QString & plain,const QString & xmlActive,const QString & xmlInactive)
{
	m_szPlainCaption = plain;
	m_szXmlActiveCaption = xmlActive;
	m_szXmlInactiveCaption = xmlInactive;
	QMdiSubWindow::setWindowTitle(plain);
}

void KviMdiChild::windowStateChangedEvent( Qt::WindowStates oldState, Qt::WindowStates newState )
{
	if (!(oldState & Qt::WindowMinimized) && (newState & Qt::WindowMinimized))
	{
		emit hideSignal();
	}
}

void KviMdiChild::maximize()
{
	m_State = Maximized;
	showMaximized();
}

void KviMdiChild::restore()
{
	switch(state())
	{
		case Maximized:
			m_State = Normal;
			showNormal();
			m_pManager->childRestored(this,true);
		break;
		case Minimized:
			m_State = Normal;
			showNormal();
			m_pManager->childRestored(this,false);
		break;
		case Normal:
			m_State = Normal;
			if(!isVisible())
			{
				show();
			}
			return;
		break;
	}
}

void KviMdiChild::minimize()
{
	switch(state())
	{
		case Maximized:
			m_State = Minimized;
			showMinimized();
			hide();
			m_pManager->childMinimized(this,true);
		break;
		case Normal:
			m_State = Minimized;
			hide();
			m_pManager->childMinimized(this,false);
		break;
		case Minimized:
			m_State = Minimized;
			if(isVisible())
			{
				hide();
			}
			return;
		break;
	}
}

/*
void KviMdiChild::systemPopupAboutToShow()
{
	m_pSystemPopup->clear();
	if(m_state != Maximized)m_pSystemPopup->insertItem(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_MAXIMIZE)),__tr("&Maximize"),this,SLOT(maximize()));
	if(m_state != Minimized)m_pSystemPopup->insertItem(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_MINIMIZE)),__tr("M&inimize"),this,SLOT(minimize()));
	if(m_state != Normal)m_pSystemPopup->insertItem(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_RESTORE)),__tr("&Restore"),this,SLOT(restore()));
	if(closeEnabled())
	{
		m_pSystemPopup->insertSeparator();
		m_pSystemPopup->insertItem(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_CLOSE)),__tr("&Close"),this,SLOT(closeRequest()));
	}
}
*/

void KviMdiChild::moveEvent(QMoveEvent *e)
{
#ifdef COMPILE_PSEUDO_TRANSPARENCY
	if(m_pClient && g_pShadedChildGlobalDesktopBackground)
	{
		if(m_pClient->inherits("KviWindow")) // actually this is always the case
		{
			((KviWindow *)m_pClient)->updateBackgrounds();
		}
	}
#endif
	QMdiSubWindow::moveEvent(e);
}

void KviMdiChild::systemPopupSlot()
{
/*
	if(sender()->inherits("QToolButton"))
	{
		emit systemPopupRequest(((QToolButton *)sender())->mapToGlobal(QPoint(0,((QToolButton *)sender())->height())));
	} else {
*/
		//emit systemPopupRequest(m_pCaption->mapToGlobal(QPoint(5,5)));
//	}
}

void KviMdiChild::setClient(QWidget * w)
{
	__range_valid(m_pClient==0);
	__range_valid(w!=0);

	m_pClient = w;
	setWidget(w);

	KviStr tmp(KviStr::Format,"mdi_child_%s",w->objectName().toUtf8().data());
	w->setObjectName(tmp.ptr());
}

void KviMdiChild::unsetClient()
{
	__range_valid(m_pClient!=0);
	if(!m_pClient)return;

	setFocusProxy(0); //remove the focus proxy...
	//Kewl...the reparent function has a small prob now..
	//the new toplelvel widgets gets not reenabled for dnd
#ifndef COMPILE_ON_MAC
	QPoint p=m_pClient->mapToGlobal(QPoint(0,0));
	m_pClient->setParent(0, m_pClient->windowFlags() & ~Qt::WindowType_Mask);
	m_pClient->setGeometry(p.x(),p.y(),m_pClient->width(),m_pClient->height());
	m_pClient->show();
#else
	QRect r = g_pApp->desktop()->availableGeometry(m_pClient);
	r.moveBy(0, 22);
	m_pClient->reparent(0,r.topLeft(),true);
	m_pClient->setParent(0, m_pClient->windowFlags() & ~Qt::WindowType_Mask);
	m_pClient->setGeometry(r.topLeft().x(),r.topLeft().y(),m_pClient->width(),m_pClient->height());
	m_pClient->show();
#endif
	m_pClient=0;
	setObjectName("mdi_child");
}

void KviMdiChild::activate(bool bSetFocus)
{
	if(m_pManager->topChild() != this)
		m_pManager->setTopChild(this,bSetFocus);
	else if(bSetFocus)setFocus();
}

void KviMdiChild::focusInEvent(QFocusEvent *)
{
	// We gained focus by click , tab or from the caption label
	// Bring this child to top
	//m_pManager->setTopChild(this,false); //Do not focus by now...
	/*The client is our focusProxy ! it should be focused by Qt !*/
#ifdef _KVI_DEBUG_CLASS_NAME_
	//__range_valid(focusProxy() == m_pClient);
#endif
}
