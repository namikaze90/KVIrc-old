#ifndef _LINKSWINDOW_H_
#define _LINKSWINDOW_H_
//
//   File : linkswindow.h
//   Creation date : Thu Dec 21 2000 12:42:55 CEST by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 1999-2001 Szymon Stefanek (pragma at kvirc dot net)
//
//   This program is FREE software. You can redistribute it and/or
//   modify it under the linkss of the GNU General Public License
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

#include "kvi_window.h"
#include "kvi_string.h"
#include "kvi_ircsocket.h"
#include "kvi_sparser.h"
#include "kvi_console.h"
#include "kvi_irccontext.h"

#include "kvi_tal_listview.h"
#include "kvi_tal_popupmenu.h"
#include <qtoolbutton.h>

class KviThemedLabel;


typedef struct _KviLink
{
	KviStr host;
	KviStr parent;
	int hops;
	KviStr description;
} KviLink;


class KviLinksWindow : public KviWindow, public KviExternalServerDataParser
{
	Q_OBJECT
public:
	KviLinksWindow(KviFrame * lpFrm,KviConsole * lpConsole);
	~KviLinksWindow();
protected:
	QSplitter      * m_pVertSplitter;
	QSplitter      * m_pTopSplitter;
	KviTalListView      * m_pListView;
	QList<KviLink*> * m_pLinkList;
	KviTalPopupMenu     * m_pHostPopup;
	QString          m_szRootServer;
	QToolButton    * m_pRequestButton;
	KviThemedLabel * m_pInfoLabel;
public: // Methods
	virtual void control(int msg);
	virtual void processData(KviIrcMessage * msg);
protected:
	virtual QPixmap * myIconPtr();
	virtual void fillCaptionBuffers();
	virtual void applyOptions();
	virtual void resizeEvent(QResizeEvent *e);
	virtual void getBaseLogFileName(KviStr &buffer);
//	virtual void setProperties(KviWindowProperty *p);
//	virtual void saveProperties();
protected slots:
	void showHostPopup(KviTalListViewItem *i,const QPoint &p,int c);
	void hostPopupClicked(int id);
	void requestLinks();
	void connectionStateChange();
public:
	virtual QSize sizeHint() const;
private:
	void reset();
	void endOfLinks();
	KviTalListViewItem * insertLink(KviLink * l);
	KviTalListViewItem * getItemByHost(const char *host,KviTalListViewItem * par);
};

#endif //_KVI_LINKSWINDOW_H_
