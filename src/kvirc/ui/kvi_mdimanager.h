#ifndef _KVI_MDIMANAGER_H_
#define _KVI_MDIMANAGER_H_
//=============================================================================
//
//   File : kvi+_mdimanager.h
//   Creation date : Wed Jun 21 2000 17:28:04 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2000-2004 Szymon Stefanek (pragma at kvirc dot net)
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


#include "kvi_settings.h"
#include "kvi_pointerlist.h"
#include "kvi_tal_scrollview.h"

#include <QWidget>
#include <QFrame>
#include <QPixmap>
#include <QMdiArea>

#define KVI_MDICHILD_BORDER 4
#define KVI_MDICHILD_SPACING 2
#define KVI_MDICHILD_MIN_WIDTH 100
#define KVI_MDICHILD_MIN_HEIGHT 40
#define KVI_MDICHILD_HIDDEN_EDGE 3

#ifdef COMPILE_ON_WINDOWS
	#include "kvi_mdichild.h"
#else
	class KviMdiChild;
#endif

class KviFrame;
class KviTalPopupMenu;
class KviTalHBox;
class KviSdiButtonBox;

class KVIRC_API KviMdiManager : public QMdiArea
{
	friend class KviMdiChild;
	friend class KviMdiCaption;
	Q_OBJECT
public:
	KviMdiManager(QWidget * parent,KviFrame * pFrm, const char * name);
	~KviMdiManager();
public:
	/// Get the currently active window
	KviMdiChild * topChild(){ return m_pZ->last(); };

	KviMdiChild * highestChildExcluding(KviMdiChild * pChild);

	/// Add an KviMdiChild to the area
	void manageChild(KviMdiChild * lpC, bool bCascade = true, QRect * setGeom = 0);

	/// Bring the KviMdiChild to the front
	void setTopChild(KviMdiChild * lpC, bool bSetFocus);

	/// Show the KviMdiChild and bring it to the front
	void showAndActivate(KviMdiChild * lpC);

	KviTalPopupMenu * windowPopup() { return m_pWindowPopup; };

	/// Move the focus the the top window
	void focusTopChild();

	/// Remove and delete the subwindow
	void destroyChild(KviMdiChild * lpC, bool bFocusTopChild = true);

	/// Get all visible subwindows
	int getVisibleChildCount();

	/// deleted
	bool isInSDIMode();
protected:

	/// The window list of all childwindows
	// TODO: Replace by Qt one??????
	KviPointerList<KviMdiChild>		* m_pZ; // topmost child is the last in the list

	KviTalPopupMenu					* m_pWindowPopup;
	KviTalPopupMenu					* m_pTileMethodPopup;
	KviFrame						* m_pFrm;
protected:
	void updateContentsSize();
	//void childMaximized(KviMdiChild *lpC);
	void childMinimized(KviMdiChild *lpC,bool bWasMaximized);
	void childRestored(KviMdiChild *lpC,bool bWasMaximized);
	void childMoved(KviMdiChild * lpC);
	void maximizeChild(KviMdiChild * lpC);

	virtual void focusInEvent(QFocusEvent *e);
	virtual void mousePressEvent(QMouseEvent *e);
	virtual void resizeEvent(QResizeEvent *e);
	virtual void paintEvent(QPaintEvent * event);
	virtual bool focusNextPrevChild(bool pNext);
public slots:
	void relayoutMenuButtons();
	void cascadeWindows();
	void cascadeMaximized();
	void expandVertical();
	void expandHorizontal();
	void minimizeAll();
	void restoreAll(); //<-- this does nothing (not working?)
	void tile();
	void toggleAutoTile();

	void tileAnodine();
	void reloadImages();
protected slots:
	void minimizeActiveChild();
	void restoreActiveChild();
	void closeActiveChild();
	void activeChildSystemPopup();
	void menuActivated(int id);
	void tileMethodMenuActivated(int id);
	void fillWindowPopup();
private:
	void ensureNoMaximized();
	void tileAllInternal(int maxWnds,bool bHorizontal);
	QPoint getCascadePoint(int indexOfWindow);
//signals:
//	void enteredSdiMode();
//	void leftSdiMode();
};

#endif //_KVI_MDIMANAGER_H_
