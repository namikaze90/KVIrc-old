//
//   File : kvi_tal_toolbar.cpp
//   Creation date : Mon Aug 13 05:05:45 2001 GMT by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2001 Szymon Stefanek (pragma at kvirc dot net)
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

#define __KVILIB__


#include "kvi_tal_toolbar.h"


KviTalToolBar::KviTalToolBar(const QString &label,QMainWindow *w,QT_TOOLBARDOCK_TYPE dock,bool bNewLine,const char * nam)
: QToolBar(label,w)
{
	setObjectName(nam);
	if (!layout()) this->setLayout(new QBoxLayout(QBoxLayout::LeftToRight));
	w->addToolBar(this);
}
KviTalToolBar::KviTalToolBar(QMainWindow *w,const char * name)
: QToolBar(name,w)
{
	setObjectName(name);
	if (!layout()) this->setLayout(new QBoxLayout(QBoxLayout::LeftToRight));
	w->addToolBar(this);
}

KviTalToolBar::~KviTalToolBar()
{
}

QBoxLayout * KviTalToolBar::boxLayout()
{
	return (QBoxLayout*)this->layout();
}

void KviTalToolBar::setBoxLayout(QBoxLayout * l)
{
	this->setLayout(l);
}

bool KviTalToolBar::usesBigPixmaps()
{
	return (iconSize().width() > 40);
}

void KviTalToolBar::setUsesBigPixmaps(bool b)
{
	if (b) setIconSize(QSize(48,48));
		else setIconSize(QSize(22,22));
}

