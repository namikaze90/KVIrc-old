//
//   File : kvi_moduleextension.cpp
//   Creation date : Tue Sep 10 01:16:25 2002 GMT by Szymon Stefanek
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


#define _KVI_MODULEEXTENSION_CPP_

#include "kvi_moduleextension.h"
#include "kvi_module.h"
#include "kvi_modulemanager.h"
#include "kvi_app.h"

// created and destroyed in kvi_app.cpp
KVIRC_API KviModuleExtensionManager    * g_pModuleExtensionManager    = 0;


KviModuleExtensionDescriptor::KviModuleExtensionDescriptor(KviModule * m,const QString &szType,const QString &szName,const QString &szVisibleName,KviModuleExtensionAllocRoutine r,const QPixmap &pix)
{
	m_iId = KviApp::getGloballyUniqueId();

	m_pModule = m;
	m_szType = szType;
	m_szName = szName;
	m_szVisibleName = szVisibleName;
	m_allocRoutine = r;
	m_pObjectList = new QList<KviModuleExtension*>;
	if(pix.isNull())m_pIcon = 0;
	else m_pIcon = new QPixmap(pix);
}

KviModuleExtensionDescriptor::~KviModuleExtensionDescriptor()
{
	while(KviModuleExtension * e = m_pObjectList->first())e->die();
	delete m_pObjectList;
	if(m_pIcon)delete m_pIcon;
}

void KviModuleExtensionDescriptor::setIcon(const QPixmap &pix)
{
	if(m_pIcon)delete m_pIcon;
	if(pix.isNull())m_pIcon = 0;
	else m_pIcon = new QPixmap(pix);
}

KviModuleExtension * KviModuleExtensionDescriptor::allocate(KviWindow * pWnd,QHash<QString,QVariant> * pParams,void * pSpecial)
{
	KviModuleExtensionAllocStruct s;
	s.pDescriptor = this;
	s.pWindow = pWnd;
	s.pParams = pParams;
	s.pSpecial = pSpecial;
	return m_allocRoutine(&s);
}


void KviModuleExtensionDescriptor::registerObject(KviModuleExtension * e)
{
	m_pObjectList->append(e);
}

void KviModuleExtensionDescriptor::unregisterObject(KviModuleExtension * e)
{
	m_pObjectList->removeAll(e);
}








KviModuleExtensionManager::KviModuleExtensionManager()
{
	m_pExtensionDict = new QHash<QString,KviModuleExtensionDescriptorList*>;
}

KviModuleExtensionManager::~KviModuleExtensionManager()
{
	foreach(KviModuleExtensionDescriptorList* e, *m_pExtensionDict)
	{
		delete e;
	}
	delete m_pExtensionDict;
}

KviModuleExtensionDescriptorList * KviModuleExtensionManager::getExtensionList(const QString &szType)
{
	g_pModuleManager->loadModulesByCaps(szType);
	return m_pExtensionDict->value(szType);
}

KviModuleExtensionDescriptor * KviModuleExtensionManager::registerExtension(KviModule * m,const QString &szType,const QString &szName,const QString &szVisibleName,KviModuleExtensionAllocRoutine r,const QPixmap &icon)
{
	KviModuleExtensionDescriptor * d = new KviModuleExtensionDescriptor(m,szType,szName,szVisibleName,r,icon);
	KviModuleExtensionDescriptorList * l = m_pExtensionDict->value(szType);
	if(!l)
	{
		l = new KviModuleExtensionDescriptorList();
		m_pExtensionDict->insert(szType,l);
	}
	l->append(d);
	return d;
}

void KviModuleExtensionManager::unregisterExtensionsByModule(KviModule * m)
{
	QHash<QString,KviModuleExtensionDescriptorList*>::iterator it(m_pExtensionDict->begin());
	while(it != m_pExtensionDict->end())
	{
		KviModuleExtensionDescriptorList * l = it.value();
		KviModuleExtensionDescriptorList::iterator it2(l->begin());
		while(it2 != l->end())
		{
			if((*it2)->module() == m)
			{
				it2 = l->erase(it2);
			} else {
				++it2;
			}
		}

		if(l->isEmpty())
		{
			delete it.value();
			it = m_pExtensionDict->erase(it);
		} else {
			++it;
		}
	}
}

KviModuleExtensionDescriptorList * KviModuleExtensionManager::allocateExtensionGetDescriptorList(const QString &szType,const char * preloadModule)
{
	if(preloadModule)
	{
		KviModule * m = g_pModuleManager->getModule(preloadModule);
		(void)m; // get rid of the unused warning :D
	}

	KviModuleExtensionDescriptorList * l = m_pExtensionDict->value(szType);
	if(!l)
	{
		// retry : it might have been unloaded
		g_pModuleManager->loadModulesByCaps(szType);
		l = m_pExtensionDict->value(szType);
	}

	return l;
}

KviModuleExtensionDescriptor * KviModuleExtensionManager::findExtensionDescriptor(const QString &szType,const QString &szName)
{
	KviModuleExtensionDescriptorList * l = m_pExtensionDict->value(szType);
	if(!l)return 0;

	foreach(KviModuleExtensionDescriptor * d,*l)
	{
		if(KviQString::equalCI(d->name(),szName))return d;
	}

	return 0;
}

KviModuleExtension * KviModuleExtensionManager::allocateExtension(const QString &szType,const QString &szName,KviWindow * pWnd,QHash<QString,QVariant> * pParams,void * pSpecial,const char * preloadModule)
{
	KviModuleExtensionDescriptorList * l = allocateExtensionGetDescriptorList(szType,preloadModule);
	if(!l)return 0;

	KviModuleExtensionDescriptor * d;

	foreach(d,*l)
	{
		if(KviQString::equalCI(d->name(),szName))return d->allocate(pWnd,pParams,pSpecial);
	}

	// uhm... not there ?
	g_pModuleManager->loadModulesByCaps(szType);
	// try again after loading the modules
	// l = m_pExtensionDict->find(szType.ptr()); <--- this shouldn't change!
	foreach(d,*l)
	{
		if(KviQString::equalCI(d->name(),szName))return d->allocate(pWnd,pParams,pSpecial);
	}

	// no way : no such extension

	return 0;
}


KviModuleExtension * KviModuleExtensionManager::allocateExtension(const QString &szType,int id,KviWindow * pWnd,QHash<QString,QVariant> * pParams,void * pSpecial,const char * preloadModule)
{
	KviModuleExtensionDescriptorList * l = allocateExtensionGetDescriptorList(szType,preloadModule);
	if(!l)return 0;

	KviModuleExtensionDescriptor * d;
	foreach(d,*l)
	{
		if(d->id() == id)return d->allocate(pWnd,pParams,pSpecial);
	}

	// uhm... not there ?
	g_pModuleManager->loadModulesByCaps(szType);
	// try again after loading the modules
	// l = m_pExtensionDict->find(szType.ptr()); <--- this shouldn't change!
	foreach(d,*l)
	{
		if(d->id() == id)return d->allocate(pWnd,pParams,pSpecial);
	}
	// no way : no such extension

	return 0;
}







KviModuleExtension::KviModuleExtension(KviModuleExtensionDescriptor * d)
: KviHeapObject()
{
	m_pDescriptor = d;
	m_pDescriptor->registerObject(this);
}

KviModuleExtension::~KviModuleExtension()
{
	m_pDescriptor->unregisterObject(this);
}


