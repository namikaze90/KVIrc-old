#ifndef	_CLASS_WINDOW_H_
#define	_CLASS_WINDOW_H_
//
//   File : class_window.h
//   Creation date : Tue Nov 26 21:37:48 CEST 2002 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2002 Szymon Stefanek (pragma at kvirc dot net)
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

#include "class_widget.h"
#include "object_macros.h"

#include "kvi_window.h"

class KviKvsObject_window : public KviKvsObject_widget
{
public:
	KVSO_DECLARE_OBJECT(KviKvsObject_window)
public:
	QWidget * widget() { return (QWidget *)object(); };

protected:
	virtual bool init(KviKvsRunTimeContext * pContext,KviKvsVariantList *pParams);

	bool functionsetCaption(KviKvsObjectFunctionCall *c);
	bool functionsetCentralWidget(KviKvsObjectFunctionCall *c);

};


class KviKvsScriptWindowWindow : public KviWindow
{
	Q_OBJECT
public:
	KviKvsScriptWindowWindow(KviFrame * pParent,const QString &szName);
	~KviKvsScriptWindowWindow();
protected:
	KviKvsObject_widget * m_pCentralWidgetObject;
	QWidget * m_pCentralWidget;
public:
	void setCentralWidget(KviKvsObject_widget *o,QWidget * w);
	void setCaptionString(const QString &s){ setFixedCaption(s); };
	virtual void resizeEvent(QResizeEvent *e);
protected slots:
	void centralWidgetObjectDestroyed();
	void centralWidgetDestroyed();
};

#endif	// !_CLASS_WINDOW_H_
