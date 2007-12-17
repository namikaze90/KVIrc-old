//=============================================================================
//
//   File : kvi_modulemanager.cpp
//   Creation date : Sat Aug 12 2000 20:32:11 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2000-2005 Szymon Stefanek (pragma at kvirc dot net)
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


#define _KVI_MODULEMANAGER_CPP_

#include "kvi_modulemanager.h"
#include "kvi_fileutils.h"
#include "kvi_app.h"
#include "kvi_options.h"
#include "kvi_frame.h"
#include "kvi_console.h"
#include "kvi_locale.h"
#include "kvi_out.h"

#include <QDir>

KviModuleManager * g_pModuleManager = 0;


KviModuleManager::KviModuleManager()
{
	m_pModuleDict = new QHash<QString,KviModule*>;

	m_pCleanupTimer = new QTimer(this);
	connect(m_pCleanupTimer,SIGNAL(timeout()),this,SLOT(cleanupUnusedModules()));
}

KviModuleManager::~KviModuleManager()
{
	unloadAllModules();
	delete m_pModuleDict;
	delete m_pCleanupTimer;
}


void KviModuleManager::loadModulesByCaps(const QString& caps,const QString& dir)
{
	QString szCapsPath;
	KviQString::appendFormatted(szCapsPath,"%Q%ccaps%c%Q%c",&dir,KVI_PATH_SEPARATOR_CHAR,
			KVI_PATH_SEPARATOR_CHAR,&caps,KVI_PATH_SEPARATOR_CHAR);

	QDir d(szCapsPath);

	// FIXME: maybe check timestamps ? (old modules)

	QStringList filelist = d.entryList(QDir::Files | QDir::Readable | QDir::NoSymLinks);
	foreach(QString file,filelist)
	{
		KviQString::cutToLast(file,KVI_PATH_SEPARATOR_CHAR);
		getModule(file);
	}
}

void KviModuleManager::loadModulesByCaps(const QString& caps)
{
	QString szDir;
	g_pApp->getLocalKvircDirectory(szDir,KviApp::Plugins);
	loadModulesByCaps(caps,szDir);
	g_pApp->getGlobalKvircDirectory(szDir,KviApp::Plugins);
	loadModulesByCaps(caps,szDir);
}

void KviModuleManager::completeModuleNames(const QString &path,const QString &word,QStringList& matches)
{
	QDir d(path);
#ifdef COMPILE_ON_WINDOWS
	d.setNameFilter("kvi*4.dll");
#else
	d.setNameFilter("libkvi*.so.4");
#endif
	// FIXME: maybe check timestamps ? (old modules)

	QStringList sl = d.entryList(QDir::Files | QDir::Readable | QDir::NoSymLinks);
	for(QStringList::Iterator it = sl.begin();it != sl.end();++it)
	{
		QString modname = *it;
		KviQString::cutToLast(modname,KVI_PATH_SEPARATOR_CHAR);
		KviQString::cutToFirst(modname,"kvi");
		if(KviQString::equalCIN(word,modname,word.length()))
		{
			KviQString::cutFromLast(modname,".so.4");
			if(!modname.isEmpty())
				matches.append(modname);
			else
				delete modname;
		} else delete modname;
	}
}

void KviModuleManager::completeModuleNames(const QString &word,QStringList& matches)
{
	QString szDir;
	// FIXME: Should check for duplicate names here!
	g_pApp->getLocalKvircDirectory(szDir,KviApp::Plugins);
	completeModuleNames(szDir,word,matches);
	g_pApp->getGlobalKvircDirectory(szDir,KviApp::Plugins);
	completeModuleNames(szDir,word,matches);
}

KviModule * KviModuleManager::findModule(const QString& modName)
{
	KviModule * m = m_pModuleDict->value(modName);
	if(m)m->updateAccessTime();
	return m;
}

KviModule * KviModuleManager::getModule(const QString& modName)
{
	KviModule * m = m_pModuleDict->value(modName);
	if(!m)
	{
		if(!loadModule(modName)) return 0;
		m = m_pModuleDict->value(modName);
	}
	if(m)m->updateAccessTime();
	return m;
}

/*
static bool default_module_cmd_load(KviModule *,KviCommand *)
{
	return true;
}

static bool default_module_cmd_unload(KviModule *m,KviCommand *)
{
	g_pModuleManager->unloadModule(m->name());
	return true;
}
*/

bool KviModuleManager::loadModule(const QString& modName)
{
	if(findModule(modName))
	{
		//debug("MODULE %s ALREADY IN CORE MEMORY",modName);
		return true;
	}
	QString tmp;
	QString szName;
#ifdef COMPILE_ON_WINDOWS
	KviQString::appendFormatted(szName,"kvi%Q4.dll",&modName);
#else
	KviQString::appendFormatted(szName,"libkvi%Q.so.4",&modName);
#endif
	szName=szName.lower();

	g_pApp->getLocalKvircDirectory(tmp,KviApp::Plugins,szName);
	if(!KviFileUtils::fileExists(tmp))
	{
		g_pApp->getGlobalKvircDirectory(tmp,KviApp::Plugins,szName);
	}
	
	if(!KviFileUtils::fileExists(tmp)) return false;
	QLibrary* lib = new QLibrary(tmp);
	bool bSuccess = lib->load();
	if(!bSuccess)
	{
		m_szLastError = lib->errorString();
		lib->unload();
		delete lib;
		//debug("ERROR IN LOADING MODULE %s (%s): %s",modName,szName.ptr(),kvi_library_error());
		return false;
	}
	KviModuleInfo * info = (KviModuleInfo *)lib->resolve(KVIRC_MODULE_STRUCTURE_SYMBOL);
	if(!info)
	{
		m_szLastError = __tr2qs("No " KVIRC_MODULE_STRUCTURE_SYMBOL " symbol exported: not a kvirc module ?");
		lib->unload();
		delete lib;
		return false;
	}
	if(!info->szKVIrcVersion)
	{
		m_szLastError = __tr2qs("This module has no version informations: refusing to load it");
		lib->unload();
		delete lib;
		return false;
	}
	if(!KVI_OPTION_BOOL(KviOption_boolIgnoreModuleVersions))
	{
		if(!kvi_strEqualCS(info->szKVIrcVersion,KVI_VERSION))
		{
			m_szLastError = __tr2qs("This module was compiled for a different KVIrc version and can't be loaded");
			m_szLastError += " (";
			m_szLastError += info->szKVIrcVersion;
			m_szLastError += ")";
			lib->unload();
			delete lib;
			return false;
		}
	}
	KviModule * module = new KviModule(lib,info,modName,szName);

	// the module is probably up.. the only thing can fail is the init_routine now
	// load the message catalogue if any
	QString szDir;
	// it's more probable to have the translations in the global directory
	// try it as first... (yes, catalogue overriding is impossible this way.. but , anybody cares ?)
	g_pApp->getGlobalKvircDirectory(szDir,KviApp::Locale);

	if(!KviLocale::loadCatalogue(modName,szDir))
	{
		// try the local directory then
		g_pApp->getLocalKvircDirectory(szDir,KviApp::Locale);
		KviLocale::loadCatalogue(modName,szDir);
	}

	if(info->init_routine)
	{
		if(!((info->init_routine)(module)))
		{
			m_szLastError = __tr2qs("Failed to execute the init routine");
			//debug("ERROR IN LOADING MODULE %s (%s): failed to execute the init routine",modName,szName.ptr());
			delete module;
			// kill the message catalogue too then
			KviLocale::unloadCatalogue(modName);
			return false;
		}
	}
	m_pModuleDict->insert(modName,module);

	/*
	registerDefaultCommands(module);
	*/
	module->registerDefaultCommands();

	if(KVI_OPTION_BOOL(KviOption_boolCleanupUnusedModules))
	{
		if(!m_pCleanupTimer->isActive())
		{
			if(KVI_OPTION_UINT(KviOption_uintModuleCleanupTimerInterval) < 30)
				KVI_OPTION_UINT(KviOption_uintModuleCleanupTimerInterval) = 30;
			m_pCleanupTimer->start(KVI_OPTION_UINT(KviOption_uintModuleCleanupTimerInterval) * 1000);
		}
	}
	// be verbose if needed....just make sure that we're not shutting down...
	if(_OUTPUT_VERBOSE && !g_pApp->closingDown())
	{
		if(g_pFrame)g_pFrame->firstConsole()->output(KVI_OUT_VERBOSE,
			__tr2qs("Loaded module '%Q' (%Q)"),&modName,&szName);
	}
	return true;
}

/*
void KviModuleManager::registerDefaultCommands(KviModule * module)
{
	// Register the default commands
	module->registerCommand("load",default_module_cmd_load);
	module->registerCommand("unload",default_module_cmd_unload);
}
*/
bool KviModuleManager::unloadModule(const QString& modName)
{
	return unloadModule(findModule(modName));
}

bool KviModuleManager::unloadModule(KviModule * module)
{
	if(!module)return false;
	moduleAboutToUnload(module);

	if(module->moduleInfo()->cleanup_routine)
	{
		(module->moduleInfo()->cleanup_routine)(module);
	}
	QString szModName = module->name();
	//debug("Closing module %s, dlclose returns %d",szModName.ptr(),dlclose(module->handle()));

	m_pModuleDict->remove(szModName);
	delete module;

	// unload the message catalogues, if any
	KviLocale::unloadCatalogue(szModName);

	if(m_pModuleDict->isEmpty())
	{
		if(m_pCleanupTimer->isActive())m_pCleanupTimer->stop();
	}

	if(_OUTPUT_VERBOSE && !g_pApp->closingDown())
	{
		if(g_pFrame)g_pFrame->firstConsole()->output(KVI_OUT_VERBOSE,
			__tr2qs("Unloaded module '%Q'"),&szModName);
	}
	return true;
}

bool KviModuleManager::hasLockedModules()
{
	foreach(KviModule * m, *m_pModuleDict)
	{
		if(m->isLocked())return true;
	}
	return false;
}


void KviModuleManager::cleanupUnusedModules()
{
	foreach(KviModule * m, *m_pModuleDict)
	{
		if(m->secondsSinceLastAccess() > KVI_OPTION_UINT(KviOption_uintModuleCleanupTimeout))
		{
			if(m->moduleInfo()->can_unload)
			{
				if((m->moduleInfo()->can_unload)(m))
				{
					unloadModule(m);
					continue;
				} else {
					// the module don't want to be unloaded
					// keep it memory for a while
					m->updateAccessTime();
				}
			} else {
				if(!(m->isLocked()))
				{
					unloadModule(m);
					continue;
				}
			}
		}
	}
}

void KviModuleManager::unloadAllModules()
{
	foreach(KviModule* m, *m_pModuleDict)
	{
		unloadModule(m);
	}
}
