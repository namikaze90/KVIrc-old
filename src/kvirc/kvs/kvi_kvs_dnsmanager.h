#ifndef _KVI_KVS_DNSMANAGER_H_
#define _KVI_KVS_DNSMANAGER_H_
//=============================================================================
//
//   File : kvi_kvs_dnsmanager.h
//   Created on Wed 04 Aug 2004 04:38:31 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2004 Szymon Stefanek <pragma at kvirc dot net>
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

#include <QObject>
#include <QString>

class KviDns;
class KviWindow;
class KviKvsScript;
class KviKvsVariantList;

class KviKvsDnsObject
{
public:
	KviKvsDnsObject(KviDns * pDns,
				KviWindow * pWnd,
				const QString &szQuery,
				bool bRebindOnWindowClose = true,
				KviKvsScript * pCallback = 0,
				KviKvsVariantList * pParameterList = 0);
	~KviKvsDnsObject();
protected:
	KviDns                    * m_pDns;
	KviWindow                 * m_pWindow;
	QString                     m_szQuery;
	KviKvsScript              * m_pCallback;        // callback to be executed when the dns terminates
	KviKvsVariantList         * m_pParameterList;   // parameter list (magic)
	bool                        m_bRebindOnWindowClose; // should we trigger the callback even if m_pWindow no longer exists ?
public:
	KviDns * dns(){ return m_pDns; };
	KviWindow * window(){ return m_pWindow; };
	KviKvsScript * callback(){ return m_pCallback; };
	KviKvsVariantList * parameterList(){ return m_pParameterList; };
	const QString & query(){ return m_szQuery; };
	bool rebindOnWindowClose(){ return m_bRebindOnWindowClose; };
	void setWindow(KviWindow * pWnd){ m_pWindow = pWnd; };
	void setParameterList(KviKvsVariantList * pList){ m_pParameterList = pList; };
};

class KviKvsDnsManager : public QObject
{
	Q_OBJECT
public:
	KviKvsDnsManager();
	~KviKvsDnsManager();
protected:
	static KviKvsDnsManager   * m_pInstance;
	QHash<KviDns*,KviKvsDnsObject*> * m_pDnsObjects;
public:
	static KviKvsDnsManager * instance(){ return m_pInstance; };
	static void init();
	static void done();

	void addDns(KviKvsDnsObject * pObject);
protected slots:
	void dnsLookupTerminated(KviDns * pDns);
private:
	void abortAll();
};

#endif //!_KVI_KVS_DNSMANAGER_H_
