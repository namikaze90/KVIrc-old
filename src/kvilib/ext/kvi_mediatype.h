#ifndef _KVI_MEDIATYPE_H_
#define _KVI_MEDIATYPE_H_
//
//   File : kvi_mediatype.h
//   Creation date : Mon Aug 21 2000 17:19:56 CEST by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2000-2001 Szymon Stefanek (pragma at kvirc dot net)
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
#include "kvi_heapobject.h"
#include "kvi_string.h"
#include <QMutex>
#include <QSet>



//
// IANA media-types matching subsystem
//
// WARNING: This class is used in multiple threads
//          Thus every usage of the classes and structures defined
//          in this file MUST be protected by calls
//          to KviMediaManager::lock() and KviMediaManager::unlock()
//


class KVILIB_API KviMediaType : public KviHeapObject
{
public:
	KviMediaType(){};
	~KviMediaType(){};
public:
	QString szFileMask;
	QString szMagicBytes;
	QString szIanaType;
	QString szDescription;
	QString szSavePath;
	QString szCommandline;
	QString szRemoteExecCommandline;
	QString szIcon;
};

class KVILIB_API KviMediaManager : public QMutex
{
public:
	KviMediaManager();
	~KviMediaManager();
protected:
	QSet<KviMediaType*> * m_pMediaTypeList;
private:
	KviMediaType * findMediaTypeForRegularFile(const QString& szFullPath,const QString& szFileName,bool bCheckMagic);
public:
	QSet<KviMediaType*> * mediaTypeList(){ return m_pMediaTypeList; };
	KviMediaType * findMediaTypeByFileMask(const QString& filemask);
	KviMediaType * findMediaTypeByIanaType(const QString& ianaType);
	bool removeMediaType(KviMediaType * t){ return m_pMediaTypeList->remove(t); };
	void clear();
	void insertMediaType(KviMediaType * t);
	KviMediaType * findMediaType(const QString& filename,bool bCheckMagic = true);
	static void copyMediaType(KviMediaType * dst,KviMediaType * src);

	void load(const QString& filename);
	void save(const QString& filename);
};


#endif //_KVI_MEDIATYPE_H_
