//=============================================================================
//
//   File : kvi_kvs_moduleinterface.cpp
//   Created on Tue 16 Dec 2003 00:27:54 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2003-2005 Szymon Stefanek <pragma at kvirc dot net>
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



#include "kvi_kvs_moduleinterface.h"
#include "kvi_kvs_eventmanager.h"
#include "kvi_module.h"
#include "kvi_modulemanager.h"
#include "kvi_locale.h"
#include "kvi_kvs_treenode_data.h"
#include "kvi_kvs_treenode_datalist.h"

bool KviKvsModuleCallbackCommandCall::getParameterCode(unsigned int uParamIdx,QString &szParamBuffer)
{
	if(!m_pParameterDataList)return false;
	KviKvsTreeNodeData * d = m_pParameterDataList->item(uParamIdx);
	if(!d)return false;
	const QChar * pBegin = d->location();
	const QChar * pEnd = d->endingLocation();
	if(!(pBegin && pEnd))return false;
	szParamBuffer.setUnicode(pBegin,pEnd - pBegin);
	szParamBuffer.stripWhiteSpace();
	return true;
}


KviKvsModuleInterface::KviKvsModuleInterface()
{
	m_pModuleSimpleCommandExecRoutineDict = new QHash<QString,KviKvsModuleSimpleCommandExecRoutine*>;
	m_pModuleFunctionExecRoutineDict = new QHash<QString,KviKvsModuleFunctionExecRoutine*>;
	m_pModuleCallbackCommandExecRoutineDict = new QHash<QString,KviKvsModuleCallbackCommandExecRoutine*>;
}

KviKvsModuleInterface::~KviKvsModuleInterface()
{
	kvsUnregisterAllEventHandlers();
	foreach(KviKvsModuleSimpleCommandExecRoutine* i , *m_pModuleSimpleCommandExecRoutineDict) {delete i;}
	foreach(KviKvsModuleFunctionExecRoutine* i , *m_pModuleFunctionExecRoutineDict) {delete i;}
	foreach(KviKvsModuleCallbackCommandExecRoutine* i , *m_pModuleCallbackCommandExecRoutineDict) {delete i;}
	delete m_pModuleSimpleCommandExecRoutineDict;
	delete m_pModuleFunctionExecRoutineDict;
	delete m_pModuleCallbackCommandExecRoutineDict;
}

#define COMPLETE_WORD_BY_DICT(__word,__list,__type,__dict) \
	{ \
		QHash<QString,__type *>::iterator it(__dict->begin()); \
		int l = __word.length(); \
		while(it != __dict->end()) \
		{ \
			if(KviQString::equalCIN(__word,it.key(),l)) \
				__list.append(it.key()); \
			++it; \
		} \
	}

void KviKvsModuleInterface::completeCommand(const QString &szCommandBegin,QStringList& pMatches)
{
	COMPLETE_WORD_BY_DICT(szCommandBegin,pMatches,KviKvsModuleSimpleCommandExecRoutine,m_pModuleSimpleCommandExecRoutineDict)
	COMPLETE_WORD_BY_DICT(szCommandBegin,pMatches,KviKvsModuleCallbackCommandExecRoutine,m_pModuleCallbackCommandExecRoutineDict)
}

void KviKvsModuleInterface::completeFunction(const QString &szFunctionBegin,QStringList& pMatches)
{
	COMPLETE_WORD_BY_DICT(szFunctionBegin,pMatches,KviKvsModuleFunctionExecRoutine,m_pModuleFunctionExecRoutineDict)
}


void KviKvsModuleInterface::kvsRegisterSimpleCommand(const QString &szCommand,KviKvsModuleSimpleCommandExecRoutine r)
{
	m_pModuleSimpleCommandExecRoutineDict->insert(szCommand,new KviKvsModuleSimpleCommandExecRoutine(r));
}

void KviKvsModuleInterface::kvsRegisterCallbackCommand(const QString &szCommand,KviKvsModuleCallbackCommandExecRoutine r)
{
	m_pModuleCallbackCommandExecRoutineDict->insert(szCommand,new KviKvsModuleCallbackCommandExecRoutine(r));
}

void KviKvsModuleInterface::kvsRegisterFunction(const QString &szFunction,KviKvsModuleFunctionExecRoutine r)
{
	m_pModuleFunctionExecRoutineDict->insert(szFunction,new KviKvsModuleFunctionExecRoutine(r));
}


bool KviKvsModuleInterface::kvsRegisterAppEventHandler(unsigned int iEventIdx,KviKvsModuleEventHandlerRoutine r)
{
	KviKvsModuleEventHandler * h = new KviKvsModuleEventHandler(this,r);
	if(!KviKvsEventManager::instance()->addAppHandler(iEventIdx,h))
	{
		delete h;
		return false;
	}
	return true;
}

bool KviKvsModuleInterface::kvsRegisterRawEventHandler(unsigned int iRawIdx,KviKvsModuleEventHandlerRoutine r)
{
	KviKvsModuleEventHandler * h = new KviKvsModuleEventHandler(this,r);
	if(!KviKvsEventManager::instance()->addRawHandler(iRawIdx,h))
	{
		delete h;
		return false;
	}
	return true;
}

void KviKvsModuleInterface::kvsUnregisterAppEventHandler(unsigned int uEventIdx)
{
	KviKvsEventManager::instance()->removeModuleAppHandler(uEventIdx,this);
}

void KviKvsModuleInterface::kvsUnregisterRawEventHandler(unsigned int uRawIdx)
{
	KviKvsEventManager::instance()->removeModuleRawHandler(uRawIdx,this);
}
void KviKvsModuleInterface::kvsUnregisterAllSimpleCommands()
{
	foreach(KviKvsModuleSimpleCommandExecRoutine* i,*m_pModuleSimpleCommandExecRoutineDict)
	{
		delete i;
	}
	m_pModuleSimpleCommandExecRoutineDict->clear();
};

void KviKvsModuleInterface::kvsUnregisterAllCallbackCommands()
{
	foreach(KviKvsModuleCallbackCommandExecRoutine* i,*m_pModuleCallbackCommandExecRoutineDict)
	{
		delete i;
	}
	m_pModuleCallbackCommandExecRoutineDict->clear();
};

void KviKvsModuleInterface::kvsUnregisterAllFunctions()
{
	foreach(KviKvsModuleFunctionExecRoutine* i,*m_pModuleFunctionExecRoutineDict)
	{
		delete i;
	}
	m_pModuleFunctionExecRoutineDict->clear();
};

void KviKvsModuleInterface::kvsUnregisterAllAppEventHandlers()
{
	KviKvsEventManager::instance()->removeAllModuleAppHandlers(this);
}

void KviKvsModuleInterface::kvsUnregisterAllRawEventHandlers()
{
	KviKvsEventManager::instance()->removeAllModuleRawHandlers(this);
}

void KviKvsModuleInterface::kvsUnregisterAllEventHandlers()
{
	kvsUnregisterAllAppEventHandlers();
	kvsUnregisterAllRawEventHandlers();
}

static bool default_module_kvs_cmd_load(KviKvsModuleCommandCall *)
{
	return true;
}

static bool default_module_kvs_cmd_unload(KviKvsModuleCommandCall *c)
{
	if(c->module()->isLocked())
	{
		if(!c->switches()->find('f',"force"))
		{
			c->warning(__tr2qs("Can't unload the module: it has locked itself in memory"));
			return true;
		}
	}
	g_pModuleManager->unloadModule(c->module()->name());
	return true;
}

void KviKvsModuleInterface::registerDefaultCommands()
{
	kvsRegisterSimpleCommand("load",default_module_kvs_cmd_load);
	kvsRegisterSimpleCommand("unload",default_module_kvs_cmd_unload);
}


