#ifndef _KVI_MODULEEXTENSION_H_
#define _KVI_MODULEEXTENSION_H_
//
//   File : kvi_moduleextension.h
//   Creation date : Tue Sep 10 01:16:24 2002 GMT by Szymon Stefanek
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

#include "kvi_settings.h"
#include "kvi_string.h"

#include "kvi_heapobject.h"

#include <QHash>
#include <QList>
#include <QPixmap>
#include <QVariant>


class KviWindow;
class KviModule;
class KviModuleExtension;
class KviModuleExtensionDescriptor;

typedef struct _KviModuleExtensionAllocStructTag
{
	KviModuleExtensionDescriptor * pDescriptor; // module extension that this alloc routine refers to
	KviWindow                    * pWindow;     // may be 0!
	QHash<QString,QVariant>      * pParams;     // parameter dict (may be 0!)
	void                         * pSpecial;    // special parameter passed to the alloc routine, may be 0
} KviModuleExtensionAllocStruct;

typedef KviModuleExtension * (*KviModuleExtensionAllocRoutine)(KviModuleExtensionAllocStruct *);

class KVIRC_API KviModuleExtensionDescriptor
{
	friend class KviModuleExtension;
	friend class KviModuleExtensionManager; // only the manager allocates and deallocates these structures
protected:
	KviModuleExtensionDescriptor(KviModule * m,const QString &szType,const QString &szName,const QString &szVisibleName,KviModuleExtensionAllocRoutine r,const QPixmap &pix);
public: // this has to be public because of QPtrList... but should be protected
	~KviModuleExtensionDescriptor();
private:
	int                              m_iId;            // unique id assigned at creation time
	QString                           m_szType;         // name of the service (toolbar , crypt engine...)
	QString                           m_szName;         // name of the extension
	QString                          m_szVisibleName;  // name that is VISIBLE and possibly translated
	QString                           m_szAuthor;       // Author (visible)
	QString                           m_szDescription;  // Description (visible!)
	int                              m_iFlags;         // Flags (0 if not applicable)
	QPixmap                        * m_pIcon;          // Icon (may be null!)
	KviModuleExtensionAllocRoutine   m_allocRoutine;
	QList<KviModuleExtension*> * m_pObjectList;

	KviModule                      * m_pModule;        // module pointer
public:
	// pParams ownership is NOT taken
	KviModuleExtension * allocate(KviWindow * pWnd = 0,QHash<QString,QVariant> * pParams = 0,void * pSpecial = 0);

	int id(){ return m_iId; };
	KviModule * module(){ return m_pModule; };
	const QString &type(){ return m_szType; };
	const QString &name(){ return m_szName; };
	const QString &visibleName(){ return m_szVisibleName; };
	const QString &author(){ return m_szAuthor; };
	const QString &description(){ return m_szDescription; };
	const QPixmap * icon(){ return m_pIcon; };
	int flags(){ return m_iFlags; };

	void setAuthor(const QString &szAuthor){ m_szAuthor = szAuthor; };
	void setDescription(const QString &szDescription){ m_szDescription = szDescription; };
	void setVisibleName(const QString &szVisibleName){ m_szVisibleName = szVisibleName; };
	void setFlags(int iFlags){ m_iFlags = iFlags; };
	void setIcon(const QPixmap &pix);
	
protected:
	void registerObject(KviModuleExtension * e);
	void unregisterObject(KviModuleExtension * e);
};

typedef QList<KviModuleExtensionDescriptor*> KviModuleExtensionDescriptorList;

class KviModuleExtensionManager;

extern KVIRC_API KviModuleExtensionManager * g_pModuleExtensionManager;

class KVIRC_API KviModuleExtensionManager
{
	friend class KviModule;
	friend class KviApp;
protected:
	KviModuleExtensionManager(); // KviApp calls this
	~KviModuleExtensionManager(); // and this
protected:
	QHash<QString,KviModuleExtensionDescriptorList*> * m_pExtensionDict;
protected:
	// Only KviModule can call this
	KviModuleExtensionDescriptor * registerExtension(KviModule * m,const QString &szType,const QString &szName,const QString &szVisibleName,KviModuleExtensionAllocRoutine r,const QPixmap &icon);
	void unregisterExtensionsByModule(KviModule * m);
public:
	KviModuleExtensionDescriptor * findExtensionDescriptor(const QString &szType,const QString &szName);
	static KviModuleExtensionManager * instance(){ return g_pModuleExtensionManager; };
	KviModuleExtensionDescriptorList * getExtensionList(const QString &szType);
	KviModuleExtension * allocateExtension(const QString &szType,const QString &szName,KviWindow * pWnd = 0,QHash<QString,QVariant> * pParams = 0,void * pSpecial = 0,const char * preloadModule = 0);
	KviModuleExtension * allocateExtension(const QString &szType,int id,KviWindow * pWnd = 0,QHash<QString,QVariant> * pParams = 0,void * pSpecial = 0,const char * preloadModule = 0);
private:
	KviModuleExtensionDescriptorList * allocateExtensionGetDescriptorList(const QString &szType,const char * preloadModule);
};



class KVIRC_API KviModuleExtension : public KviHeapObject
{
public:
	KviModuleExtension(KviModuleExtensionDescriptor * d);
	virtual ~KviModuleExtension();
private:
	KviModuleExtensionDescriptor * m_pDescriptor;
public:
	static KviModuleExtensionManager * manager(){ return g_pModuleExtensionManager; };
	KviModuleExtensionDescriptor * descriptor(){ return m_pDescriptor; };
};



#endif //_KVI_MODULEEXTENSION_H_
