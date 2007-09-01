//=============================================================================
//
//   File : kvi_customtoolbarmanager.cpp
//   Created on Sun 05 Dec 2004 18:20:18 by Szymon Stefanek
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



#include "kvi_customtoolbarmanager.h"
#include "kvi_customtoolbardescriptor.h"
#include "kvi_config.h"
#include "kvi_locale.h"

KviCustomToolBarManager * KviCustomToolBarManager::m_pInstance = 0;

KviCustomToolBarManager::KviCustomToolBarManager()
{
	m_pDescriptors = new QHash<QString,KviCustomToolBarDescriptor*>;
}

KviCustomToolBarManager::~KviCustomToolBarManager()
{
	foreach(KviCustomToolBarDescriptor*i,*m_pDescriptors)
	{
		delete i;
	}
	delete m_pDescriptors;
}

KviCustomToolBar * KviCustomToolBarManager::firstExistingToolBar()
{
	foreach(KviCustomToolBarDescriptor * d,*m_pDescriptors)
	{
		if(d->toolBar())return d->toolBar();
	}
	return 0;
}

void KviCustomToolBarManager::init()
{
	if(!m_pInstance)m_pInstance = new KviCustomToolBarManager();
}

void KviCustomToolBarManager::done()
{
	if(m_pInstance)
	{
		delete m_pInstance;
		m_pInstance = 0;
	}
}

QString KviCustomToolBarManager::idForNewToolBar(const QString &szTemplate)
{
	QString s;
	QString szTT = szTemplate.lower();
	szTT.remove(" ");
	szTT.remove("$tr");
	szTT.remove("(");
	szTT.remove(")");
	szTT.remove("\"");
	int idx = 0;
	for(;;)
	{
		s = szTT;
		if(idx > 0)
		{
			QString tmp;
			tmp.setNum(idx);
			s += tmp;
		}
		if(!m_pDescriptors->value(s))return s;
		idx++;
	}
	return s;
}

KviCustomToolBarDescriptor * KviCustomToolBarManager::findDescriptorByInternalId(int id)
{
	foreach(KviCustomToolBarDescriptor * d,*m_pDescriptors)
	{
		if(d->internalId() == id)return d;
	}
	return 0;
}

bool KviCustomToolBarManager::renameDescriptor(const QString &szId,const QString &szNewId,const QString &szNewLabelCode)
{
	KviCustomToolBarDescriptor * d = m_pDescriptors->value(szId);
	if(!d)return false;
	d->rename(szNewLabelCode);
	if(szId == szNewId)return true; // already done
	m_pDescriptors->remove(szId);
	m_pDescriptors->insert(szNewId,d);
	return true;
}

bool KviCustomToolBarManager::destroyDescriptor(const QString &szId)
{
	KviCustomToolBarDescriptor * d = m_pDescriptors->value(szId);
	if(!d)return false;
	delete m_pDescriptors->take(szId); // will delete it too!
	return true;
}

void KviCustomToolBarManager::clear()
{
	foreach(KviCustomToolBarDescriptor*i,*m_pDescriptors)
	{
		delete i;
	}
	m_pDescriptors->clear(); // bye!
}

KviCustomToolBarDescriptor * KviCustomToolBarManager::create(const QString &szId,const QString &szLabelCode)
{
	KviCustomToolBarDescriptor * d = m_pDescriptors->value(szId);
	if(d)return d;
	d = new KviCustomToolBarDescriptor(szId,szLabelCode);
	m_pDescriptors->insert(szId,d);
	return d;
}

void KviCustomToolBarManager::storeVisibilityState()
{
	foreach(KviCustomToolBarDescriptor * d,*m_pDescriptors)
	{
		d->m_bVisibleAtStartup = d->toolBar() != 0;
	}

}

int KviCustomToolBarManager::visibleToolBarCount()
{
	int cnt = 0;
	foreach(KviCustomToolBarDescriptor * d,*m_pDescriptors)
	{
		if(d->toolBar() != 0)cnt++;
	}
	return cnt;
}

void KviCustomToolBarManager::createToolBarsVisibleAtStartup()
{
	foreach(KviCustomToolBarDescriptor * d,*m_pDescriptors)
	{
		if(d->m_bVisibleAtStartup && (!d->toolBar()))
			d->createToolBar();
	}
}

void KviCustomToolBarManager::updateVisibleToolBars()
{
	foreach(KviCustomToolBarDescriptor * d,*m_pDescriptors)
	{
		if(d->toolBar())d->updateToolBar();
	}
}

void KviCustomToolBarManager::load(const QString &szFileName)
{
	KviConfig cfg(szFileName,KviConfig::Read);

	KviConfigIterator it(cfg.dict()->begin());
	while(it != cfg.dict()->end())
	{
		cfg.setGroup(it.key());
		KviCustomToolBarDescriptor * d = new KviCustomToolBarDescriptor(it.key(),QString::null);
		d->m_bVisibleAtStartup = (cfg.readIntEntry("Visible",0) > 0);
		if(!d->load(&cfg))delete d;
		else m_pDescriptors->insert(it.key(),d);
		++it;
	}
}

void KviCustomToolBarManager::save(const QString &szFileName)
{
	KviConfig cfg(szFileName,KviConfig::Write);
	foreach(KviCustomToolBarDescriptor * d,*m_pDescriptors)
	{
		cfg.setGroup(d->id());
		cfg.writeEntry("Visible",d->m_bVisibleAtStartup ? 1 : 0);
		d->save(&cfg);
	}
}


// THIS IS A COMPATIBILITY ENTRY ADDED AT 3.0.2 TIME THAT SHOULD BE DROPPED IN A COUPLE OF VERSION BUMPS!
#ifdef SCRIPTTOOLBAR_COMPAT

#include <qdir.h>
void KviCustomToolBarManager::loadScripttoolbarsCompat(const QString &szFileName)
{
	KviConfig cfg(szFileName,KviConfig::Read);
	
	unsigned int cnt = cfg.readUIntEntry("Count",0);
	KviStr tmp;
	
	for(unsigned int i=0;i<cnt;i++)
	{
		tmp.sprintf("%d",i);
		KviCustomToolBarDescriptor * d = new KviCustomToolBarDescriptor(QString::null,QString::null);
		if(!d->loadScripttoolbarCompat(tmp.ptr(),&cfg))delete d;
		else m_pDescriptors->insert(d->id(),d);
	}

	// get rid of the file now...
	QString szNewFileName = szFileName + ".old";
	QDir d;
	d.rename(szFileName,szNewFileName);
}
#endif
