//=============================================================================
//
//   File : kvi_actionmanager.cpp
//   Created on Sun 21 Nov 2004 03:37:57 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC Client distribution
//   Copyright (C) 2008 Szymon Stefanek <pragma at kvirc dot net>
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



#include "kvi_actionmanager.h"
#include "kvi_modulemanager.h"
#include "kvi_coreactions.h"
#include "kvi_customtoolbar.h"
#include "kvi_customtoolbarmanager.h"
#include "kvi_app.h"
#include "kvi_locale.h"
#include "kvi_kvs_useraction.h"
#include "kvi_config.h"
#include "kvi_qstring.h"
#include "kvi_frame.h"

KviActionManager * KviActionManager::m_pInstance = 0;
KviActionCategory * KviActionManager::m_pCategoryIrc = 0;
KviActionCategory * KviActionManager::m_pCategoryGeneric = 0;
KviActionCategory * KviActionManager::m_pCategorySettings = 0;
KviActionCategory * KviActionManager::m_pCategoryScripting = 0;
KviActionCategory * KviActionManager::m_pCategoryGUI = 0;
KviActionCategory * KviActionManager::m_pCategoryChannel = 0;
KviActionCategory * KviActionManager::m_pCategoryTools = 0;
bool KviActionManager::m_bCustomizingToolBars = false;
KviCustomToolBar * KviActionManager::m_pCurrentToolBar = 0;
// kvi_coreactions.cpp
extern void register_core_actions(KviActionManager *);

KviActionManager::KviActionManager()
: QObject()
{
	m_pActions = new QHash<QString,KviAction*>;

	m_pCategories = new QHash<QString,KviActionCategory*>;


#define CATEGORY(__var,__name,__vname,__descr) \
	__var = new KviActionCategory(__name,__vname,__descr); \
	m_pCategories->insert(__name,__var)

	CATEGORY(m_pCategoryIrc,"irc",__tr2qs("IRC"),__tr2qs("IRC Context related actions"));
	CATEGORY(m_pCategoryGeneric,"generic",__tr2qs("Generic"),__tr2qs("Generic actions"));
	CATEGORY(m_pCategorySettings,"settings",__tr2qs("Settings"),__tr2qs("Actions related to settings"));
	CATEGORY(m_pCategoryScripting,"scripting",__tr2qs("Scripting"),__tr2qs("Scripting related actions"));
	CATEGORY(m_pCategoryGUI,"gui",__tr2qs("GUI"),__tr2qs("Actions related to the Graphic User Interface"));
	CATEGORY(m_pCategoryChannel,"channel",__tr2qs("Channel"),__tr2qs("IRC Channel related actions"));
	CATEGORY(m_pCategoryTools,"tools",__tr2qs("Tools"),__tr2qs("Actions that will appear in the \"Tools\" menu"));

	m_bCustomizingToolBars = false;
	m_pCurrentToolBar = 0;
	m_bCoreActionsRegistered = false;
}

KviActionManager::~KviActionManager()
{
	// the customizeToolBars dialog has been already
	// destroyed since the module manager has already
	// killed all the modules at this point...
	//KviActionDialog::cleanup();

	foreach(KviAction * a,*m_pActions)
	{
		disconnect(a,SIGNAL(destroyed()),this,SLOT(actionDestroyed()));
	}
	delete m_pActions;
	
	delete m_pCategories;
}

void KviActionManager::load(const QString &szFileName)
{
	KviConfig cfg(szFileName,KviConfig::Read);
	
	KviConfigIterator it(cfg.dict()->begin());
	while(it != cfg.dict()->end())
	{
		cfg.setGroup(it.key());
		KviKvsUserAction * a = new KviKvsUserAction(this);
		if(a->load(&cfg))registerAction(a);
		else delete a;
		++it;
	}
}

void KviActionManager::save(const QString &szFileName)
{
	KviConfig cfg(szFileName,KviConfig::Write);
	cfg.clear();
	
	foreach(KviAction * a,*m_pActions)
	{
		if(a->isKviUserActionNeverOverrideThis())
		{
			cfg.setGroup(a->name());
			((KviKvsUserAction *)a)->save(&cfg);
		}
	}
}

void KviActionManager::killAllKvsUserActions()
{
	QList<KviKvsUserAction*> dying;

	foreach(KviAction * a,*m_pActions)
	{
		if(a->isKviUserActionNeverOverrideThis())
		{
			dying.append(((KviKvsUserAction *)a));
		}
	}
	qDeleteAll(dying);
}


bool KviActionManager::coreActionExists(const QString &szName)
{
	KviAction *a = m_pActions->value(szName);
	if(a)return (!a->isKviUserActionNeverOverrideThis());
	return false;
}

QString KviActionManager::nameForAutomaticAction(const QString &szTemplate)
{
	QString ret;

	int i = 1;
	do {
		KviQString::sprintf(ret,"%Q%d",&szTemplate,i);
		i++;
	} while(m_pActions->contains(ret));

	return ret;
}

void KviActionManager::emitRemoveActionsHintRequest()
{
	emit removeActionsHintRequest();
}

KviActionCategory * KviActionManager::category(const QString &szName)
{
	if(!szName.isEmpty())
	{
		KviActionCategory * c = m_pCategories->value(szName);
		if(c)return c;
	}
	return m_pCategoryGeneric;
}

void KviActionManager::customizeToolBarsDialogCreated()
{
	m_bCustomizingToolBars = true;
	m_pCurrentToolBar = KviCustomToolBarManager::instance()->firstExistingToolBar();
	if(m_pCurrentToolBar)m_pCurrentToolBar->update();
	emit beginCustomizeToolBars();
}

void KviActionManager::customizeToolBarsDialogDestroyed()
{
	m_bCustomizingToolBars = false;
	emit endCustomizeToolBars();
	if(m_pCurrentToolBar)
	{
		m_pCurrentToolBar->update();
		m_pCurrentToolBar = 0;
	}
	g_pApp->saveToolBars();
}

void KviActionManager::setCurrentToolBar(KviCustomToolBar * t)
{
	if(m_pCurrentToolBar == t)return;
	KviCustomToolBar * old = m_pCurrentToolBar;
	m_pCurrentToolBar = t;
	if(old)old->update();
	if(!m_pCurrentToolBar && m_bCustomizingToolBars)
		m_pCurrentToolBar = KviCustomToolBarManager::instance()->firstExistingToolBar();
	if(m_pCurrentToolBar)m_pCurrentToolBar->update();
	emit currentToolBarChanged();
}

void KviActionManager::loadAllAvailableActions()
{
	// make sure that the core actions are registered now
	if(!KviActionManager::instance()->m_bCoreActionsRegistered)
	{
		register_core_actions(KviActionManager::instance());
		KviActionManager::instance()->m_bCoreActionsRegistered = true;
	}
	g_pModuleManager->loadModulesByCaps("action");
}

void KviActionManager::init()
{
	if(!m_pInstance)m_pInstance = new KviActionManager();
}

void KviActionManager::done()
{
	if(m_pInstance)
	{
		delete m_pInstance;
		m_pInstance = 0;
	}
}

void KviActionManager::delayedRegisterAccelerators()
{
	foreach(KviAction * a,*m_pActions)
	{
		a->registerAccelerator();
	}
}

bool KviActionManager::registerAction(KviAction * a)
{
	if(m_pActions->contains(a->name()))return false;
	connect(a,SIGNAL(destroyed()),this,SLOT(actionDestroyed()));
	m_pActions->insert(a->name(),a);
	if(g_pFrame)a->registerAccelerator(); // otherwise it is delayed!
	return true;
}

void KviActionManager::actionDestroyed()
{
	KviAction * a = (KviAction *)sender();
	m_pActions->remove(a->name());
}

bool KviActionManager::unregisterAction(const QString &szName)
{
	KviAction * a = m_pActions->value(szName);
	if(!a)return false;
	disconnect(a,SIGNAL(destroyed()),this,SLOT(actionDestroyed()));
	a->unregisterAccelerator();
	return m_pActions->remove(szName);
}

KviAction * KviActionManager::getAction(const QString &szName)
{
	KviAction * a = m_pActions->value(szName);
	if(a)return a;
	int idx = szName.find('.');

	if((idx == 5) && (!m_bCoreActionsRegistered))
	{
		// the core actions are all like kvirc.name
		// so the dot is at poisition 5 (6th char)
		// the first requested core action will trigger this
		// the nice thing is that we will probably already have a frame when
		// the core actions are registered thus stuff like mdiManager() can be accessed...
		if(szName.left(5) == "kvirc")
		{
			register_core_actions(this);
			m_bCoreActionsRegistered = true;
			a = m_pActions->value(szName);
			return a;
		}
	}
	// try to preload the module that might register this action...
	QString szModule = szName.left(idx);
	if(!g_pModuleManager->getModule(szModule))return 0;
	return m_pActions->value(szName);
}

void KviActionManager::listActionsByCategory(const QString &szCatName,QList<KviAction*> * pBuffer)
{
	loadAllAvailableActions();
	KviActionCategory * pCat = category(szCatName);
	pBuffer->clear();
	if(!pCat)return;
	foreach(KviAction * a,*m_pActions)
	{
		if(a->category() == pCat)
			pBuffer->append(a);
	}
}

