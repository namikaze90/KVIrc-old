#ifndef _KVI_TOOLWINDOWS_CONTAINER_H_
#define _KVI_TOOLWINDOWS_CONTAINER_H_
//============================================================================
//
//   File : kvi_toolwindows_container.h
//   Creation date : 12.11.2005 23.50 by Uzhva Alexey
//
//   This file is part of the KVirc irc client distribution
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
//============================================================================


#include "kvi_heapobject.h"
#include "kvi_styled_controls.h"

#include <QToolButton>
#include <QObjectCleanupHandler>
#include <QPushButton>


class KviWindowToolWidget;
class KviWindowToolPageButton;

// FIXME: these classes are probably useless now... no ?

//    Pragma: KviWindowToolPageButton is actually used in kvi_window.h and others (need to fix the name and move
//            it to its own file.



class KVIRC_API KviWindowToolWidget : public QWidget
{
	Q_OBJECT
public:
	KviWindowToolWidget( QWidget * parent,KviWindowToolPageButton* button/*, const char * name = 0, WFlags f = 0 */);
	~KviWindowToolWidget();
	
	void setAutoDelete(bool b) { m_bAutoDelete=b; };
	bool autoDelete() { return m_bAutoDelete; };
	
	virtual void registerSelf();
	virtual void unregisterSelf();
/*public slots:
	virtual void hide ();
	virtual void show ();*/
protected:
//	KviToolWindowsContainer		*m_pContainer;
	KviWindowToolPageButton		*m_pButton;
	bool 				 m_bAutoDelete;
	bool 				 m_bHidden;
//	QObjectCleanupHandler		 m_ObjectHandler;
	
};

class KVIRC_API KviWindowToolPageButton : public QPushButton
{
	Q_OBJECT
public:
	KviWindowToolPageButton ( int pixon,int pixoff, const QString & text, QWidget * parent,bool bOn=0,const char * name = 0 );
	~KviWindowToolPageButton();
/*protected:
	virtual void drawButton ( QPainter * painter);*/
};

#endif //_KVI_TOOLWINDOWS_CONTAINER_H_
