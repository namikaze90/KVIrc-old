#ifndef _KVI_MDIMANAGER_H_
#define _KVI_MDIMANAGER_H_
//=============================================================================
//
//   File : kvi_mdimanager.h
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

#include <QWidget>
#include <QFrame>
#include <QPixmap>
#include <QToolButton>
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
//class KviMdiCaptionButton;
class KviFrame;

class KviTalPopupMenu;
class KviSdiButtonBox;
class KviMenuBarToolButton;

#include <QMdiSubWindow>

class KVIRC_API KviMdiManager : public QMdiArea
{
	friend class KviMdiChild;
	friend class KviMdiCaption;
	Q_OBJECT
public:
	KviMdiManager(QWidget *p):QMdiArea(p) {};
	bool isInSDIMode() { return activeSubWindow()->isMaximized(); };
	KviMdiChild* topChild() { return (KviMdiChild*)(activeSubWindow()->widget()); };
};

#endif //_KVI_MDIMANAGER_H_
