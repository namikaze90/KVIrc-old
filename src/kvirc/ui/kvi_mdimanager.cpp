//=============================================================================
//
//   File : kvi_mdimanager.cpp
//   Creation date : Wed Jun 21 2000 17:28:04 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2000-2003 Szymon Stefanek (pragma at kvirc dot net)
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



#include "kvi_debug.h"
#include "kvi_settings.h"
#include "kvi_mdimanager.h"
#include "kvi_mdichild.h"
#include "kvi_locale.h"
#include "kvi_options.h"
#include "kvi_iconmanager.h"
#include "kvi_frame.h"
#include "kvi_menubar.h"
#include "kvi_app.h"
#include "kvi_tal_popupmenu.h"
#include "kvi_tal_hbox.h"

#include <QMenuBar>
#include <QLayout>
#include <QPainter>
#include <QCursor>
#include <QEvent>
#include <QMouseEvent>
#include <QLabel>
#include <QToolButton>


#ifdef COMPILE_PSEUDO_TRANSPARENCY
	#include <QPixmap>
	extern QPixmap * g_pShadedParentGlobalDesktopBackground;
#endif

#include <math.h>


KviMdiManager::KviMdiManager(QWidget * parent,KviFrame * pFrm,const char * name)
: QMdiArea(parent)
{
	setFrameShape(NoFrame);
	m_pZ = new KviPointerList<KviMdiChild>;
	m_pZ->setAutoDelete(true);

	m_pFrm = pFrm;

	m_pWindowPopup = new KviTalPopupMenu(this);
	connect(m_pWindowPopup,SIGNAL(activated(int)),this,SLOT(menuActivated(int)));
	connect(m_pWindowPopup,SIGNAL(aboutToShow()),this,SLOT(fillWindowPopup()));
	m_pTileMethodPopup = new KviTalPopupMenu(this);
	connect(m_pTileMethodPopup,SIGNAL(activated(int)),this,SLOT(tileMethodMenuActivated(int)));

	viewport()->setAutoFillBackground(false);
//	setStaticBackground(true);

	setFocusPolicy(Qt::NoFocus);
	viewport()->setFocusPolicy(Qt::NoFocus);
	connect(g_pApp,SIGNAL(reloadImages()),this,SLOT(reloadImages()));
}

KviMdiManager::~KviMdiManager()
{
	delete m_pZ;
}

void KviMdiManager::reloadImages()
{
}

bool KviMdiManager::focusNextPrevChild(bool bNext)
{
	//bug("FFFFFF");
	// this is a QScrollView bug... it doesn't pass this
	// event to the toplevel window
	return m_pFrm->focusNextPrevChild(bNext);
}

void KviMdiManager::paintEvent(QPaintEvent * event)
{
	QPainter p(viewport());
#ifdef COMPILE_PSEUDO_TRANSPARENCY
	if(g_pShadedParentGlobalDesktopBackground)
	{
		QPoint pnt = viewport()->mapToGlobal(event->rect().topLeft());
		p.drawTiledPixmap(event->rect(),*(g_pShadedParentGlobalDesktopBackground), pnt);
		return;
	}
#endif

	if(KVI_OPTION_PIXMAP(KviOption_pixmapMdiBackground).pixmap())
	{
		p.drawTiledPixmap(event->rect(),*(KVI_OPTION_PIXMAP(KviOption_pixmapMdiBackground).pixmap()));
	} else {
		p.fillRect(event->rect(),KVI_OPTION_COLOR(KviOption_colorMdiBackground));
	}
}

void KviMdiManager::manageChild(KviMdiChild * lpC,bool bCascade,QRect *setGeom)
{
	__range_valid(lpC);

	QMdiSubWindow * w = this->addSubWindow((QMdiSubWindow*)lpC);
	qDebug("%p %p",w,lpC);
	//hidden -> last in the Z order
	m_pZ->insert(0,lpC);
	qDebug("Added window");

	if(KVI_OPTION_BOOL(KviOption_boolAutoTileWindows))tile();
}

void KviMdiManager::showAndActivate(KviMdiChild * lpC)
{
	lpC->show();
	//setTopChild(lpC,true);
	if(KVI_OPTION_BOOL(KviOption_boolAutoTileWindows))tile();
}

void KviMdiManager::setTopChild(KviMdiChild *lpC,bool bSetFocus)
{
	__range_valid(lpC);
	// The following check fails safely at startup....
	//	__range_valid(lpC->isVisible() || lpC->testWState(WState_ForceHide));

	KviMdiChild * pOldTop = m_pZ->last();
	if(pOldTop != lpC)
	{
		m_pZ->setAutoDelete(false);

		if(!m_pZ->removeRef(lpC))
		{
			m_pZ->setAutoDelete(true);
			return; // no such child ?
		}

		KviMdiChild * pMaximizedChild = pOldTop;
		if(pOldTop)
		{
			if(pOldTop->state() != KviMdiChild::Maximized) pMaximizedChild = 0;
		}

		m_pZ->setAutoDelete(true);
		m_pZ->append(lpC);

		if(pMaximizedChild) lpC->maximize(); //do not animate the change
		lpC->raise();
		if(pMaximizedChild) pMaximizedChild->restore();
	}

	if(bSetFocus)
	{
		if(!lpC->hasFocus())
		{
			lpC->setFocus();
		}
	}
}

void KviMdiManager::focusInEvent(QFocusEvent *)
{
	focusTopChild();
}

void KviMdiManager::destroyChild(KviMdiChild *lpC,bool bFocusTopChild)
{

#ifdef _KVI_DEBUG_CHECK_RANGE_
	//Report invalid results in a debug session
	__range_valid(m_pZ->removeRef(lpC));
#else
	m_pZ->removeRef(lpC);
#endif

	if(bFocusTopChild)focusTopChild();

	if(KVI_OPTION_BOOL(KviOption_boolAutoTileWindows)) tile();

	updateContentsSize();
}

KviMdiChild * KviMdiManager::highestChildExcluding(KviMdiChild * pChild)
{
	KviMdiChild * c = m_pZ->last();
	while(c && (c == pChild)) c = m_pZ->prev();
	return c;
}

QPoint KviMdiManager::getCascadePoint(int indexOfWindow)
{
	QPoint pnt(0,0);
	if(indexOfWindow==0)return pnt;
	KviMdiChild *lpC=m_pZ->first();
	int step=0;
	int availableHeight=viewport()->height()-(lpC ? lpC->minimumSize().height() : KVI_MDICHILD_MIN_HEIGHT);
	int availableWidth=viewport()->width()-(lpC ? lpC->minimumSize().width() : KVI_MDICHILD_MIN_WIDTH);
	int ax=0;
	int ay=0;
	for(int i=0;i<indexOfWindow;i++)
	{
		ax+=step;
		ay+=step;
		if(ax>availableWidth)ax=0;
		if(ay>availableHeight)ay=0;
	}
	pnt.setX(ax);
	pnt.setY(ay);
	return pnt;
}

void KviMdiManager::mousePressEvent(QMouseEvent *e)
{
	//Popup the window menu
	if(e->button() & Qt::RightButton)m_pWindowPopup->popup(mapToGlobal(e->pos()));
}

void KviMdiManager::childMoved(KviMdiChild *)
{

}

void KviMdiManager::maximizeChild(KviMdiChild * lpC)
{
}



void KviMdiManager::resizeEvent(QResizeEvent *e)
{
}


/*
void KviMdiManager::childMaximized(KviMdiChild * lpC)
{
	if(lpC == m_pZ->last())
	{
		enterSDIMode(lpC);
	}
	updateContentsSize();
}
*/

void KviMdiManager::childMinimized(KviMdiChild * lpC,bool bWasMaximized)
{
	__range_valid(lpC);
	if(m_pZ->findRef(lpC) == -1)return;
	if(m_pZ->count() > 1)
	{
		m_pZ->setAutoDelete(false);
#ifdef _KVI_DEBUG_CHECK_RANGE_
		//Report invalid results in a debug session
		__range_valid(m_pZ->removeRef(lpC));
#else
		m_pZ->removeRef(lpC);
#endif
		m_pZ->setAutoDelete(true);
		m_pZ->insert(0,lpC);
		if(bWasMaximized)
		{
			// Need to maximize the top child
			lpC = m_pZ->last();
			if(!lpC)return; //??
			if(lpC->state()==KviMdiChild::Minimized)
			{
				return;
			}
			lpC->maximize(); //do nrot animate the change
		} else {
			if(KVI_OPTION_BOOL(KviOption_boolAutoTileWindows))tile();
		}
		focusTopChild();
	} else {
		// Unique window minimized...it won't loose the focus...!!
		setFocus(); //Remove focus from the child
	}
	updateContentsSize();
}

void KviMdiManager::childRestored(KviMdiChild * lpC,bool bWasMaximized)
{
	if(bWasMaximized)
	{
		if(lpC != m_pZ->last())return; // do nothing in this case
		updateContentsSize();
	}
	if(KVI_OPTION_BOOL(KviOption_boolAutoTileWindows))tile();
}

void KviMdiManager::focusTopChild()
{
	KviMdiChild *lpC=m_pZ->last();
	if(!lpC)return;
	if(!lpC->isVisible())return;
	//	if(lpC->state()==KviMdiChild::Minimized)return;
	//	debug("Focusing top child %s",lpC->name());
	//disable the labels of all the other children
	/*for(KviMdiChild *pC=m_pZ->first();pC;pC=m_pZ->next())
	{
		if(pC != lpC)
			pC->captionLabel()->setActive(false);
	}*/
	lpC->raise();
	if(!lpC->hasFocus())lpC->setFocus();
}

void KviMdiManager::minimizeActiveChild()
{
	KviMdiChild * lpC = m_pZ->last();
	if(!lpC)return;
	if(lpC->state() != KviMdiChild::Minimized)lpC->minimize();
}

void KviMdiManager::restoreActiveChild()
{
	KviMdiChild * lpC = m_pZ->last();
	if(!lpC)return;
	if(lpC->state() == KviMdiChild::Maximized)lpC->restore();
}

void KviMdiManager::closeActiveChild()
{
	KviMdiChild * lpC = m_pZ->last();
	if(!lpC)return;
	lpC->close();
}

void KviMdiManager::updateContentsSize()
{
}

void KviMdiManager::activeChildSystemPopup()
{
	KviMdiChild * lpC = m_pZ->last();
	if(!lpC)return;
	QPoint pnt;
	/*if(m_pSdiIconButton)
	{
		pnt = m_pSdiIconButton->mapToGlobal(QPoint(0,m_pSdiIconButton->height()));
	} else {
		pnt = QCursor::pos();
	}*/
	lpC->emitSystemPopupRequest(pnt);
}

bool KviMdiManager::isInSDIMode()
{
	return false;
}


void KviMdiManager::relayoutMenuButtons()
{
	// force a re-layout of the menubar in Qt4 (see the note in enterSDIMode())
	// by resetting the corner widget
/*	if(m_pSdiControls)
	{
		m_pFrm->mainMenuBar()->setCornerWidget(0,Qt::TopRightCorner);
		m_pFrm->mainMenuBar()->setCornerWidget(m_pSdiControls,Qt::TopRightCorner);
	}
	// also force an activation of the top MdiChild since it probably didn't get it yet*/
	KviMdiChild * c = topChild();
	if(c) c->activate(false);
}

#define KVI_TILE_METHOD_ANODINE 0
#define KVI_TILE_METHOD_PRAGMA4HOR 1
#define KVI_TILE_METHOD_PRAGMA4VER 2
#define KVI_TILE_METHOD_PRAGMA6HOR 3
#define KVI_TILE_METHOD_PRAGMA6VER 4
#define KVI_TILE_METHOD_PRAGMA9HOR 5
#define KVI_TILE_METHOD_PRAGMA9VER 6

#define KVI_NUM_TILE_METHODS 7

void KviMdiManager::fillWindowPopup()
{
	m_pWindowPopup->clear();

	m_pWindowPopup->insertItem(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_CASCADEWINDOWS)),(__tr2qs("&Cascade Windows")),this,SLOT(cascadeWindows()));
	m_pWindowPopup->insertItem(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_CASCADEWINDOWS)),(__tr2qs("Cascade &Maximized")),this,SLOT(cascadeMaximized()));

	m_pWindowPopup->insertSeparator();
	m_pWindowPopup->insertItem(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_TILEWINDOWS)),(__tr2qs("&Tile Windows")),this,SLOT(tile()));

	m_pTileMethodPopup->clear();
	int id = m_pTileMethodPopup->insertItem(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_AUTOTILEWINDOWS)),(__tr2qs("&Auto Tile")),this,SLOT(toggleAutoTile()));
	m_pTileMethodPopup->setItemChecked(id,KVI_OPTION_BOOL(KviOption_boolAutoTileWindows));
	m_pTileMethodPopup->setItemParameter(id,-1);
	m_pTileMethodPopup->insertSeparator();
	int ids[KVI_NUM_TILE_METHODS];
	ids[KVI_TILE_METHOD_ANODINE] = m_pTileMethodPopup->insertItem(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_TILEWINDOWS)),(__tr2qs("Anodine's Full Grid")));
	m_pTileMethodPopup->setItemParameter(ids[KVI_TILE_METHOD_ANODINE],KVI_TILE_METHOD_ANODINE);
	ids[KVI_TILE_METHOD_PRAGMA4HOR] = m_pTileMethodPopup->insertItem(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_TILEWINDOWS)),(__tr2qs("Pragma's Horizontal 4-Grid")));
	m_pTileMethodPopup->setItemParameter(ids[KVI_TILE_METHOD_PRAGMA4HOR],KVI_TILE_METHOD_PRAGMA4HOR);
	ids[KVI_TILE_METHOD_PRAGMA4VER] = m_pTileMethodPopup->insertItem(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_TILEWINDOWS)),(__tr2qs("Pragma's Vertical 4-Grid")));
	m_pTileMethodPopup->setItemParameter(ids[KVI_TILE_METHOD_PRAGMA4VER],KVI_TILE_METHOD_PRAGMA4VER);
	ids[KVI_TILE_METHOD_PRAGMA6HOR] = m_pTileMethodPopup->insertItem(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_TILEWINDOWS)),(__tr2qs("Pragma's Horizontal 6-Grid")));
	m_pTileMethodPopup->setItemParameter(ids[KVI_TILE_METHOD_PRAGMA6HOR],KVI_TILE_METHOD_PRAGMA6HOR);
	ids[KVI_TILE_METHOD_PRAGMA6VER] = m_pTileMethodPopup->insertItem(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_TILEWINDOWS)),(__tr2qs("Pragma's Vertical 6-Grid")));
	m_pTileMethodPopup->setItemParameter(ids[KVI_TILE_METHOD_PRAGMA6VER],KVI_TILE_METHOD_PRAGMA6VER);
	ids[KVI_TILE_METHOD_PRAGMA9HOR] = m_pTileMethodPopup->insertItem(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_TILEWINDOWS)),(__tr2qs("Pragma's Horizontal 9-Grid")));
	m_pTileMethodPopup->setItemParameter(ids[KVI_TILE_METHOD_PRAGMA9HOR],KVI_TILE_METHOD_PRAGMA9HOR);
	ids[KVI_TILE_METHOD_PRAGMA9VER] = m_pTileMethodPopup->insertItem(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_TILEWINDOWS)),(__tr2qs("Pragma's Vertical 9-Grid")));
	m_pTileMethodPopup->setItemParameter(ids[KVI_TILE_METHOD_PRAGMA9VER],KVI_TILE_METHOD_PRAGMA9VER);

	if(KVI_OPTION_UINT(KviOption_uintTileMethod) >= KVI_NUM_TILE_METHODS)KVI_OPTION_UINT(KviOption_uintTileMethod) = KVI_TILE_METHOD_PRAGMA9HOR;
	m_pTileMethodPopup->setItemChecked(ids[KVI_OPTION_UINT(KviOption_uintTileMethod)],true);

	m_pWindowPopup->insertItem(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_TILEWINDOWS)),(__tr2qs("Tile Met&hod")),m_pTileMethodPopup);

	m_pWindowPopup->insertSeparator();
	m_pWindowPopup->insertItem(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_MAXVERTICAL)),(__tr2qs("Expand &Vertically")),this,SLOT(expandVertical()));
	m_pWindowPopup->insertItem(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_MAXHORIZONTAL)),(__tr2qs("Expand &Horizontally")),this,SLOT(expandHorizontal()));

	m_pWindowPopup->insertSeparator();
	m_pWindowPopup->insertItem(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_MINIMIZE)),(__tr2qs("Mi&nimize All")),this,SLOT(minimizeAll()));
    m_pWindowPopup->insertItem(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_RESTORE)),(__tr2qs("&Restore all")),this,SLOT(restoreAll()));
//
	m_pWindowPopup->insertSeparator();
	int i = 100;
	QString szItem;
	QString szCaption;
	for(KviMdiChild * lpC = m_pZ->first(); lpC; lpC = m_pZ->next())
	{
		szItem.setNum(((uint)i)-99);
		szItem+=". ";

		szCaption = lpC->windowTitle();
		if(szCaption.length() > 30)
		{
			QString trail = szCaption.right(12);
			szCaption.truncate(12);
			szCaption+="...";
			szCaption+=trail;
		}

		if(lpC->state()==KviMdiChild::Minimized)
		{
			szItem+="(";
			szItem+=szCaption;
			szItem+=")";
		} else szItem+=szCaption;

		const QPixmap * pix = lpC->icon();

		if (pix && !(pix->isNull()))
		{
			m_pWindowPopup->insertItem(*pix, szItem,i);
		} else {
			m_pWindowPopup->insertItem(szItem);
		}

		//m_pWindowPopup->setItemChecked(i, ((uint)i) == (m_pZ->count()+99) ); <- ????
		i++;
	}
}

void KviMdiManager::menuActivated(int id)
{
	if(id<100)return;
	id-=100;
	__range_valid(((uint)id) < m_pZ->count());
	KviMdiChild * lpC = m_pZ->at(id);
	if(!lpC)return;
	if(lpC->state()==KviMdiChild::Minimized)lpC->restore();
	setTopChild(lpC,true);
}

void KviMdiManager::ensureNoMaximized()
{
	KviMdiChild * lpC;

	for(lpC=m_pZ->first();lpC;lpC=m_pZ->next())
	{
		if(lpC->state()==KviMdiChild::Maximized)lpC->restore();
	}
}

void KviMdiManager::tileMethodMenuActivated(int id)
{
	int idx = m_pTileMethodPopup->itemParameter(id);
	if(idx < 0)idx = 0;
	if(idx >= KVI_NUM_TILE_METHODS)idx = KVI_TILE_METHOD_PRAGMA9VER;
	KVI_OPTION_UINT(KviOption_uintTileMethod) = idx;
	if(KVI_OPTION_BOOL(KviOption_boolAutoTileWindows))tile();
}

void KviMdiManager::cascadeWindows()
{
}

void KviMdiManager::cascadeMaximized()
{
}

void KviMdiManager::expandVertical()
{
}

void KviMdiManager::expandHorizontal()
{
}

void KviMdiManager::minimizeAll()
{
	KviPointerList<KviMdiChild> list;
	list.copyFrom(m_pZ);

	list.setAutoDelete(false);
	m_pFrm->setActiveWindow((KviWindow*)m_pFrm->firstConsole());
	while(!list.isEmpty())
	{
		KviMdiChild *lpC=list.first();
		if(lpC->state() != KviMdiChild::Minimized)lpC->minimize();
		list.removeFirst();
	}
	focusTopChild();
	updateContentsSize();
}


void KviMdiManager::restoreAll()
{
	int idx=0;
	KviPointerList<KviMdiChild> list;
	list.copyFrom(m_pZ);

	list.setAutoDelete(false);
	while(!list.isEmpty())
	{
		KviMdiChild *lpC=list.first();
		if(lpC->state() != KviMdiChild::Normal && (!(lpC->plainCaption()).contains("CONSOLE") ))
		lpC->restore();
		list.removeFirst();
	}
	focusTopChild();
}


int KviMdiManager::getVisibleChildCount()
{
	QList<QMdiSubWindow *> l = subWindowList();

	int cnt = 0;
	int i = 0;
	for(i = 0; i < l.count(); i++)
	{
		if(!l.at(i)->isHidden()) cnt++;
	}
	return cnt;
}

void KviMdiManager::tile()
{
	switch(KVI_OPTION_UINT(KviOption_uintTileMethod))
	{
		case KVI_TILE_METHOD_ANODINE:      tileAnodine(); break;
		case KVI_TILE_METHOD_PRAGMA4HOR:   tileAllInternal(4,true); break;
		case KVI_TILE_METHOD_PRAGMA4VER:   tileAllInternal(4,false); break;
		case KVI_TILE_METHOD_PRAGMA6HOR:   tileAllInternal(6,true); break;
		case KVI_TILE_METHOD_PRAGMA6VER:   tileAllInternal(6,false); break;
		case KVI_TILE_METHOD_PRAGMA9HOR:   tileAllInternal(9,true); break;
		case KVI_TILE_METHOD_PRAGMA9VER:   tileAllInternal(9,false); break;
		default:
			KVI_OPTION_UINT(KviOption_uintTileMethod) = KVI_TILE_METHOD_PRAGMA9HOR;
			tileAllInternal(9,true);
		break;
	}
}

void KviMdiManager::toggleAutoTile()
{
	if(KVI_OPTION_BOOL(KviOption_boolAutoTileWindows))
	{
		KVI_OPTION_BOOL(KviOption_boolAutoTileWindows) = false;
	} else {
		KVI_OPTION_BOOL(KviOption_boolAutoTileWindows) = true;
		tile();
	}
}


void KviMdiManager::tileAllInternal(int maxWnds,bool bHorizontal)
{
}

void KviMdiManager::tileAnodine()
{
	this->tileSubWindows();
}
