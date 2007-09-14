#ifndef _KVI_TAL_POPUPMENU_H_
#define _KVI_TAL_POPUPMENU_H_

//=============================================================================
//
//   File : kvi_tal_popupmenu.h
//   Creation date : Mon Jan 22 2007 11:25:08 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2007 Szymon Stefanek (pragma at kvirc dot net)
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

#include "kvi_settings.h"
#include "kvi_qstring.h"

#include <qmenu.h>
#include <qaction.h>
#include <qwidgetaction.h>

class KVILIB_API KviTalPopupMenu : public QMenu
{
	Q_OBJECT
public:
	KviTalPopupMenu(QWidget * pParent=0,const QString &szName = KviQString::empty)
	: QMenu(pParent)
	{
		setName(szName);
	};
	virtual ~KviTalPopupMenu() {};

	QAction * insertItem(const QString &szText)
	{
		return QMenu::addAction(szText);
	}
	QAction * insertItem(const QPixmap &pix,const QString &szText)
	{
		return QMenu::addAction(QIcon(pix),szText);
	}
	QAction * insertItem(const QString &szText, QObject * pReceiver,const char * szSlot)
	{
		QAction * action = insertItem(szText);
		action->setParent(pReceiver);
		connect(action,SIGNAL(triggered(bool)),pReceiver,szSlot);
		return action;
	}
	QAction * insertItem(const QPixmap &pix,const QString &szText, QObject * pReceiver,const char * szSlot)
	{
		QAction * action = insertItem(pix,szText);
		action->setParent(pReceiver);
		connect(action,SIGNAL(triggered(bool)),pReceiver,szSlot);
		return action;
	}
	QAction * insertItem(const QPixmap &pix,const QString &szText,QMenu *pMenu)
	{
		QAction * action = QMenu::addMenu(pMenu);
		action->setText(szText);
		action->setIcon(QIcon(pix));
		return action;
	}
	QAction * insertItem(const QString &szText,QMenu *pMenu)
	{
		QAction * action = QMenu::addMenu(pMenu);
		action->setText(szText);
		return action;
	}
// FIXME: THEXCEPTION
/*	int insertItem(QWidget * pWidget)
	{
		// needs Qt 4.2
		QWidgetAction * pAct = new QWidgetAction(this);
		pAct->setDefaultWidget(pWidget);
		addAction(pAct);
		return 0;
	}*/
};

#endif // _KVI_TAL_POPUPMENU_H_
