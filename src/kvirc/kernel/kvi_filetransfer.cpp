//=============================================================================
//
//   File : kvi_filetransfer.cpp
//   Creation date : Mon Apr 21 22:14:31 2003 GMT by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2003 Szymon Stefanek (pragma at kvirc dot net)
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

#include "kvi_filetransfer.h"
#include "kvi_moduleextension.h"
#include "kvi_window.h"

#include "kvi_app.h"

#include "kvi_tal_listview.h"
#include "kvi_tal_popupmenu.h"

static KviFileTransferManager * g_pFileTransferManager = 0;


KviFileTransferManager::KviFileTransferManager()
: QObject()
{
	m_pTransferList = 0;
	m_pTransferWindow = 0;
}

KviFileTransferManager::~KviFileTransferManager()
{
	killAllTransfers();
}

KviFileTransferManager * KviFileTransferManager::instance()
{
	if(!g_pFileTransferManager)g_pFileTransferManager = new KviFileTransferManager();
	return g_pFileTransferManager;
}

void KviFileTransferManager::cleanup()
{
	if(!g_pFileTransferManager)return;
	delete g_pFileTransferManager;
	g_pFileTransferManager = 0;
}

void KviFileTransferManager::killAllTransfers()
{
	while(m_pTransferList)
	{
		KviFileTransfer * t = m_pTransferList->first();
		t->die();
	}
}

void KviFileTransferManager::killTerminatedTransfers()
{
	if(!m_pTransferList)return;

	QList<KviFileTransfer*> l;

	foreach(KviFileTransfer * f,*m_pTransferList)
	{
		if(f->terminated())l.append(f);
	}

	foreach(KviFileTransfer * d,l)
		d->die();
}

void KviFileTransferManager::invokeTransferWindow(KviWindow * pWnd,bool bCreateMinimized,bool bNoRaise)
{
	if(!m_pTransferWindow)
	{
		QHash<QString,QVariant> d;
		d.insert("bCreateMinimized",QVariant(bCreateMinimized,0));
		d.insert("bNoRaise",QVariant(bNoRaise,0));

		KviModuleExtensionManager::instance()->allocateExtension(
				"tool",
				KVI_FILE_TRANSFER_WINDOW_EXTENSION_NAME,
				pWnd,&d,0,
				"filetransferwindow");
	}
}

void KviFileTransferManager::registerTransfer(KviFileTransfer * t)
{
	if(!m_pTransferList)
	{
		m_pTransferList = new QList<KviFileTransfer*>;
	}

	m_pTransferList->append(t);

	emit transferRegistered(t);
}

void KviFileTransferManager::unregisterTransfer(KviFileTransfer * t)
{
	if(!m_pTransferList)
	{
		debug("Ops: unregistering transfer with no transfer list!");
		return;
	}

	emit transferUnregistering(t);

	m_pTransferList->removeAll(t);
	if(m_pTransferList->isEmpty())
	{
		delete m_pTransferList;
		m_pTransferList = 0;
	}
}



KviFileTransfer::KviFileTransfer()
: QObject()
{
	m_pDisplayItem = 0;
	m_iId = g_pApp->getGloballyUniqueId();
	manager()->registerTransfer(this);
}

KviFileTransfer::~KviFileTransfer()
{
	manager()->unregisterTransfer(this);
}

KviWindow * KviFileTransfer::outputWindow()
{
	KviWindow * o = transferWindow();
	if(o)return o;
	return g_pActiveWindow;
}

void KviFileTransfer::invokeTransferWindow(KviWindow * pWnd,bool bCreateMinimized,bool bNoRaise)
{
	manager()->invokeTransferWindow(pWnd,bCreateMinimized,bNoRaise);
}

QString KviFileTransfer::localFileName()
{
	return QString::null;
}

QString KviFileTransfer::retryCommand()
{
	return QString::null;
}

bool KviFileTransfer::terminated()
{
	return !active();
}

QString KviFileTransfer::tipText()
{
	return QString::null;
}

int KviFileTransfer::displayHeight(int iLineSpacing)
{
	return iLineSpacing * 2;
}

void KviFileTransfer::displayUpdate()
{
	if(m_pDisplayItem)m_pDisplayItem->repaint();
}
