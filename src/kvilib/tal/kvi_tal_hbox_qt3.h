#ifndef _KVI_TAL_HBOX_QT3_H_
#define _KVI_TAL_HBOX_QT3_H_

//=============================================================================
//
//   File : kvi_tal_hbox_qt3.h
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

#include <qhbox.h>

class KVILIB_API KviTalHBox : public QHBox
{
	Q_OBJECT
public:
	KviTalHBox(QWidget * pParent)
	: QHBox(pParent) {};
	virtual ~KviTalHBox() {};
};

#endif // _KVI_TAL_HBOX_QT3_H_