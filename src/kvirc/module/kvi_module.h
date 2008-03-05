#ifndef _KVI_MODULE_H_
#define _KVI_MODULE_H_

//=============================================================================
//
//   File : kvi_module.h
//   Creation date : Sat Aug 12 2000 18:34:22 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2000-2008 Szymon Stefanek (pragma at kvirc dot net)
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
#include "kvi_string.h"

#include "kvi_moduleextension.h"
#include "kvi_kvs_moduleinterface.h"

#include <QLibrary>

#ifdef COMPILE_CRYPT_SUPPORT
	#include "kvi_crypt.h"
#endif


#ifdef COMPILE_ON_WINDOWS
	#define KVIMODULEEXPORT extern "C" __declspec(dllexport)
	#define KVIMODULEEXPORTDATA KVIMODULEEXPORT
	#define KVIMODULEEXPORTFUNC KVIMODULEEXPORT
#else
	#define KVIMODULEEXPORT extern "C"
	#define KVIMODULEEXPORTDATA
	#define KVIMODULEEXPORTFUNC KVIMODULEEXPORT
#endif

class KviModule;


typedef bool (*KviModuleSystemRoutine)(KviModule *);
typedef bool (*KviModuleCtrlRoutine)(KviModule *,const char *,void *);


typedef struct _KviModuleInfo
{
	const char * szKVIrcVersion; // must match KVI_VERSION if module version checking is in force
	const char * szModuleName;
	const char * szVersion;
	const char * szCopyright;
	const char * szDescription;
	/*
	* This routine is called when the module is loaded
	*/
	KviModuleSystemRoutine init_routine;
	/*
	* This should return true if the module is actually
	* not providing any service and can be unloaded from memory.
	* Please note that this is not a mandatory lock: KVIrc may still unload
	* the module even when this function returns false.
	* The only assumption you can make is that KVIrc will not try
	* to unload the module spontaneously: it will do it only if forced to
	* (actually only by the user, but maybe later also by constrained resources).
	* If this pointer is zero, KVIrc will assume that the module
	* does not provide any service and will unload the module at the
	* first spontaneous cleanup.
	* There is a yet stronger locking method in KviModule::lock()
	*/
	KviModuleSystemRoutine can_unload;
	/*
	* This is a generic control routine with prototype
	* bool <name>(KviModule * m,const char * operation,void * param)
	* KVIrc uses it to comunicate with bundled modules
	* in most user-build modules this will be 0
	*/
	KviModuleCtrlRoutine   ctrl_routine;
	/*
	* This routine is called when the module is being unloaded
	* Note that the module can be unloaded even if can_unload returns false:
	* that's the user choice, KVIrc can only forcibly unload the module,
	* so better cleanup everything here :)
	*/
	KviModuleSystemRoutine cleanup_routine; // WARNING : g_pApp may be in the destructor and may have no frames open!
} KviModuleInfo;

// NOTE: The init and cleanup routines should NEVER rely on g_pApp existing!
//       so only "initialization and cleanup INTERNAL to the module" goes there!

// A module should be prepared to be unloaded at ANY time, even if it is locked
// or if can_unload returns false; locking is only a "suggestion" to the KVIrc core.

// When unloaded, a module must ensure the destruction of all the resources that depend
// on the module core code

#define KVIRC_MODULE_STRUCTURE_SYMBOL "KVIrc_module_info"

#define KVIRC_MODULE(_szModuleName,_szVersion,_szCopyright,_szDescription,_init_routine,_can_unload,_ctrl_routine,_cleanup_routine) \
	\
	KVIMODULEEXPORTDATA KviModuleInfo KVIrc_module_info= \
	{ \
		KVI_VERSION, \
		_szModuleName, \
		_szVersion, \
		_szCopyright, \
		_szDescription, \
		_init_routine, \
		_can_unload, \
		_ctrl_routine, \
		_cleanup_routine \
	};
	

// old type parsing procedures
/*
typedef bool (*KviModuleCommandParseProc)(KviModule *,KviCommand *);
typedef bool (*KviModuleFunctionParseProc)(KviModule *,KviCommand *,KviParameterList *,KviStr &);
typedef bool (*KviModuleEventParseProc)(KviModule *,KviWindow *,KviParameterList *);
*/


class KVIRC_API KviModule : public KviKvsModuleInterface
{
	friend class QHash<QString,KviModule*>;
	friend class KviModuleManager;
	friend class KviUserParser;
protected:
	KviModule(QLibrary* library,KviModuleInfo * info,const QString& name,const QString& filename);
	~KviModule();
private:
	QString                                     m_szName;
	QString                                     m_szFileName;
	KviModuleInfo                            * m_pModuleInfo;
	QLibrary                                 * m_pLibrary;
	unsigned int                               m_uLock;
	long int                                   m_lastAccessTime;
protected:
	void updateAccessTime();
	unsigned int secondsSinceLastAccess();
public:
	// name of this module: always low case , single word
	QString name(){ return m_szName; };
	// filename of this module (with NO path): formatted as "libkvi%s.so",name()
	QString filename(){ return m_szFileName; };
	QLibrary*       libraryHandle(){ return m_pLibrary; };
	KviModuleInfo * moduleInfo(){ return m_pModuleInfo; };
	
	//
	// This is a locking method a bit stronger than the can_unload routine
	// in the descriptor. It will behave in the same way plus
	// the user will be unable to unload the module unless he will specify
	// the -f switch to the <module>.unload command. Without the -f switch
	// he will be just warned that the module is locked in memory and
	// don't want to be unloaded.
	// The usage of this method is encouraged
	// only when you have blocking dialogs inside the module code, like the
	// QMessageBox or QFileDialog static methods.
	// In this case you're entering a modal event loop that you can't control
	// and if some script will attempt to forcibly unload the module
	// it will surely lead to a crash when the static method returns (jumping into no mans land).
	// <module>.unload -f is in fact undocumented so people will substantially
	// not use it (unless they are developers and they are reading this comment).
	//
	void            lock(){ m_uLock++; };
	void            unlock(){ if(m_uLock > 0)m_uLock--; };
	bool            isLocked(){ return (m_uLock > 0); };
	
	void          * getSymbol(const char * symname);
	bool            ctrl(const char * operation,void * param);

	void getDefaultConfigFileName(QString &szBuffer);

	static void unregisterMetaObject(const char * metaObjName);

#ifdef COMPILE_CRYPT_SUPPORT
	void registerCryptEngine(KviCryptEngineDescription * d);
	void unregisterCryptEngine(const char * szName);
	void unregisterCryptEngines();
#endif

	KviModuleExtensionDescriptor * registerExtension(const QString &szType,const QString &szName,const QString &szVisibleName,KviModuleExtensionAllocRoutine r);
	KviModuleExtensionDescriptor * registerExtension(const QString &szType,const QString &szName,const QString &szVisibleName,KviModuleExtensionAllocRoutine r,const QPixmap &icon);
	KviModuleExtensionDescriptor * findExtensionDescriptor(const QString &szType,const QString &szName);
	void unregisterAllExtensions();
};

#endif //_KVI_MODULE_H_
