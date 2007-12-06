//
//   File : kvi_mediatype.cpp
//   Creation date : Mon Aug 21 2000 17:51:56 CEST by Szymon Stefanek
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



//#define _KVI_DEBUG_CHECK_RANGE_



#include "kvi_debug.h"
#include "kvi_mediatype.h"
#include "kvi_config.h"
#include "kvi_fileutils.h"
#include "kvi_locale.h"
#include "kvi_file.h"

#include "kvi_settings.h"

#include <QRegExp>
#include <QDir>

#include <sys/types.h>
#include <sys/stat.h>


#ifndef COMPILE_ON_WINDOWS
	#include <unistd.h>
	#include "kvi_malloc.h"
#endif



#ifndef S_ISDIR
#define S_ISDIR(__f) (__f & _S_IFDIR)
#endif

#ifndef S_ISFIFO
#define S_ISFIFO(__f) (__f & _S_IFIFO)
#endif

#ifndef S_ISREG
#define S_ISREG(__f) (__f & _S_IFREG)
#endif

#ifndef S_ISCHR
#define S_ISCHR(__f) (__f & _S_IFCHR)
#endif

#ifndef COMPILE_ON_WINDOWS
	#include <dirent.h>
#else
	#include "kvi_malloc.h"
#endif




KviMediaManager::KviMediaManager()
: QMutex()
{
	m_pMediaTypeList = new QSet<KviMediaType*>;
}

KviMediaManager::~KviMediaManager()
{
	foreach(KviMediaType*i,*m_pMediaTypeList)
	{
		delete i;
	}
	delete m_pMediaTypeList;
}

void KviMediaManager::clear()
{
	foreach(KviMediaType*i,*m_pMediaTypeList)
	{
		delete i;
	}
	m_pMediaTypeList->clear();
}

KviMediaType * KviMediaManager::findMediaTypeByIanaType(const QString& ianaType)
{
	__range_valid(locked());
	foreach(KviMediaType * mt,*m_pMediaTypeList)
	{
		if(mt->szIanaType == ianaType)return mt;
	}

	return 0;
}

KviMediaType * KviMediaManager::findMediaTypeByFileMask(const QString& filemask)
{
	__range_valid(locked());
	foreach(KviMediaType * mt,*m_pMediaTypeList)
	{
		if(mt->szFileMask == filemask)return mt;
	}

	return 0;
}

void KviMediaManager::copyMediaType(KviMediaType * dst,KviMediaType * src)
{
	dst->szFileMask              = src->szFileMask;
	dst->szMagicBytes            = src->szMagicBytes;
	dst->szIanaType              = src->szIanaType;
	dst->szDescription           = src->szDescription;
	dst->szSavePath              = src->szSavePath;
	dst->szCommandline           = src->szCommandline;
	dst->szRemoteExecCommandline = src->szRemoteExecCommandline;
	dst->szIcon                  = src->szIcon;
}


void KviMediaManager::insertMediaType(KviMediaType * m)
{
	__range_valid(locked());
	m_pMediaTypeList->insert(m);
}


KviMediaType * KviMediaManager::findMediaType(const QString& filename,bool bCheckMagic)
{
	__range_valid(locked());

	QString szFullPath = filename;
	if(!KviFileUtils::isAbsolutePath(szFullPath))
	{
		QString tmp = QDir::currentPath();

		tmp.append('/');
		szFullPath.prepend(tmp);
	}

	QString szFile = KviFileUtils::extractFileName(filename);

	QFileInfo fileInfo(szFullPath);
	
	// first of all , lstat() the file
	if(fileInfo.exists() && fileInfo.isFile())
	{
		//debug("Problems while stating file %s",szFullPath.ptr());
		// We do just the pattern matching
		// it's better to avoid magic checks
		// if the file is a device , we would be blocked while attempting to read data
		return findMediaTypeForRegularFile(szFullPath,szFile,false);
	}

	if(fileInfo.isSymLink())
	{
		return findMediaTypeForRegularFile(fileInfo.symLinkTarget(),szFile,false);
	}

	if(fileInfo.isDir())
	{
		// Directory : return default media type
		KviMediaType * mtd = findMediaTypeByIanaType("inode/directory");
		if(!mtd)
		{
			// Add it
			mtd = new KviMediaType;
			mtd->szIanaType = "inode/directory";
			mtd->szDescription = __tr("Directory");
			mtd->szCommandline = "dirbrowser.open -m $0";
			mtd->szIcon = "kvi_dbfolder.png"; // hardcoded ?
			insertMediaType(mtd);
		}
		return mtd;
	}

/*
#ifndef COMPILE_ON_WINDOWS
	if(fileInfo.is)
	{
		// Socket : return default media type
		KviMediaType * mtd = findMediaTypeByIanaType("inode/socket");
		if(!mtd)
		{
			// Add it
			mtd = new KviMediaType;
			mtd->szIanaType = "inode/socket";
			mtd->szDescription = __tr("Socket");
			mtd->szIcon = "kvi_dbsocket.png"; // hardcoded ?
			insertMediaType(mtd);
		}
		return mtd;
	}
#endif

	if(S_ISFIFO(st.st_mode))
	{
		// Fifo: return default media type
		KviMediaType * mtd = findMediaTypeByIanaType("inode/fifo");
		if(!mtd)
		{
			// Add it
			mtd = new KviMediaType;
			mtd->szIanaType = "inode/fifo";
			mtd->szDescription = __tr("Fifo");
			mtd->szIcon = "kvi_dbfifo.png"; // hardcoded ?
			insertMediaType(mtd);
		}
		return mtd;
	}

#ifndef COMPILE_ON_WINDOWS
	if(S_ISBLK(st.st_mode))
	{
		// Block device: return default media type
		KviMediaType * mtd = findMediaTypeByIanaType("inode/blockdevice");
		if(!mtd)
		{
			// Add it
			mtd = new KviMediaType;
			mtd->szIanaType = "inode/blockdevice";
			mtd->szDescription = __tr("Block device");
			mtd->szIcon = "kvi_dbblockdevice.png"; // hardcoded ?
			insertMediaType(mtd);
		}
		return mtd;
	}
#endif

	if(S_ISCHR(st.st_mode))
	{
		// Char device: return default media type
		KviMediaType * mtd = findMediaTypeByIanaType("inode/chardevice");
		if(!mtd)
		{
			// Add it
			mtd = new KviMediaType;
			mtd->szIanaType = "inode/chardevice";
			mtd->szDescription = __tr("Char device");
			mtd->szIcon = "kvi_dbchardevice.png"; // hardcoded ?
			insertMediaType(mtd);
		}
		return mtd;
	}

*/
	// this is a regular file (or at least it looks like one)
	return findMediaTypeForRegularFile(szFullPath,szFile,bCheckMagic);
}

KviMediaType * KviMediaManager::findMediaTypeForRegularFile(const QString& szFullPath,const QString& szFileName,bool bCheckMagic)
{
	char buffer[17];
	int len = 0;

	if(bCheckMagic)
	{
		KviFile f(szFullPath);
		if(f.openForReading())
		{
			len = f.readBlock(buffer,16);
			if(len > 0)
			{
				buffer[len] = '\0';
				if(buffer[0] == 0)len = 0; // no way to match it
			}
			f.close();
		}
	}

	foreach(KviMediaType * m,*m_pMediaTypeList)
	{
// FIXME: #warning "Should this be case sensitive ?"
		if(KviQString::matchWildExpressionsCI(m->szFileMask,szFileName))
		{
			if(len && !m->szMagicBytes.isEmpty())
			{
				QRegExp re(m->szMagicBytes);
				// It looks like they can't decide the name for this function :D
				// ... well, maybe the latest choice is the best one.

				if(re.indexIn(buffer) > -1)return m; // matched!

				// else magic failed...not a match
			} else return m; // matched! (no magic check)
		}
	}

	KviMediaType * mtd = findMediaTypeByIanaType("application/octet-stream");
	if(!mtd)
	{
		// Add it
		mtd = new KviMediaType;
		mtd->szIanaType = "application/octet-stream";
		mtd->szDescription = __tr("Octet stream (unknown)");
		mtd->szCommandline = "editor.open $0";
		mtd->szIcon = "kvi_dbunknown.png"; // hardcoded ?
		insertMediaType(mtd);
	}

	return mtd;
}

typedef struct _KviDefaultMediaType
{
	const char * filemask;
	const char * magicbytes;
	const char * ianatype;
	const char * description;
	const char * commandline;
} KviDefaultMediaType;


// FIXME : default handlers for windows ?

static KviDefaultMediaType g_defMediaTypes[]=
{
	{ "*.jpg","^\\0330\\0377","image/jpeg","JPEG image","run kview $0" },
	{ "*.jpeg","^\\0330\\0377","image/jpeg","JPEG image","run kview $0" },
	{ "*.png","","image/png","PNG image","run kview $0" },
	{ "*.mp3","","audio/mpeg","MPEG audio","run xmms -e $0" },
	{ "*.gif","","image/gif","GIF image","run kvirc $0" },
	{ "*.mpeg","","video/mpeg","MPEG video","run xanim $0" },
	{ "*.exe","","application/x-executable-file","Executable file","run $0" },
	{ "*.zip","^PK\\0003\\0004","application/zip","ZIP archive","run ark $0" },
	{ "*.tar.gz","","application/x-gzip","GZipped tarball","run ark $0" },
	{ "*.tar.bz2","","applicatoin/x-bzip2","BZipped tarball","run ark $0" },
	{ "*.tgz","","application/x-gzip","GZipped tarball","run ark $0" },
	{ "*.wav","","audio/wav","Wave audio","run play $0" },
	{ 0,0,0,0,0 }
};

void KviMediaManager::load(const QString& filename)
{
	__range_valid(locked());

	KviConfig cfg(filename,KviConfig::Read);
	cfg.setGroup("MediaTypes");
	unsigned int nEntries = cfg.readUIntEntry("NEntries",0);
	for(unsigned int i = 0; i < nEntries;i++)
	{
		KviMediaType * m = new KviMediaType;
		KviStr tmp(KviStr::Format,"%dFileMask",i);
		m->szFileMask = cfg.readEntry(tmp.ptr(),"");
		tmp.sprintf("%dMagicBytes",i);
		m->szMagicBytes = cfg.readEntry(tmp.ptr(),"");
		tmp.sprintf("%dIanaType",i);
		m->szIanaType = cfg.readEntry(tmp.ptr(),"application/unknown");
		tmp.sprintf("%dDescription",i);
		m->szDescription = cfg.readEntry(tmp.ptr(),"");
		tmp.sprintf("%dSavePath",i);
		m->szSavePath = cfg.readEntry(tmp.ptr(),"");
		tmp.sprintf("%dCommandline",i);
		m->szCommandline = cfg.readEntry(tmp.ptr(),"");
		tmp.sprintf("%dRemoteExecCommandline",i);
		m->szRemoteExecCommandline = cfg.readEntry(tmp.ptr(),"");
		tmp.sprintf("%dIcon",i);
		m->szIcon = cfg.readEntry(tmp.ptr(),"");
		insertMediaType(m);
	}

	for(int u = 0;g_defMediaTypes[u].filemask;u++)
	{
		if(!findMediaTypeByFileMask(g_defMediaTypes[u].filemask))
		{
			KviMediaType * m = new KviMediaType;
			m->szFileMask = g_defMediaTypes[u].filemask;
			m->szMagicBytes = g_defMediaTypes[u].magicbytes;
			m->szIanaType = g_defMediaTypes[u].ianatype;
			m->szDescription = g_defMediaTypes[u].description;
			m->szCommandline = g_defMediaTypes[u].commandline;
			insertMediaType(m);
		}
	}

}

void KviMediaManager::save(const QString& filename)
{
	__range_valid(locked());
	KviConfig cfg(filename,KviConfig::Write);

	cfg.clear();
	cfg.setGroup("MediaTypes");
	cfg.writeEntry("NEntries",m_pMediaTypeList->count());
	int index = 0;
	foreach(KviMediaType * m,*m_pMediaTypeList)
	{
		KviStr tmp(KviStr::Format,"%dFileMask",index);
		cfg.writeEntry(tmp.ptr(),m->szFileMask);
		tmp.sprintf("%dMagicBytes",index);
		cfg.writeEntry(tmp.ptr(),m->szMagicBytes);
		tmp.sprintf("%dIanaType",index);
		cfg.writeEntry(tmp.ptr(),m->szIanaType);
		tmp.sprintf("%dDescription",index);
		cfg.writeEntry(tmp.ptr(),m->szDescription);
		tmp.sprintf("%dSavePath",index);
		cfg.writeEntry(tmp.ptr(),m->szSavePath);
		tmp.sprintf("%dCommandline",index);
		cfg.writeEntry(tmp.ptr(),m->szCommandline);
		tmp.sprintf("%dRemoteExecCommandline",index);
		cfg.writeEntry(tmp.ptr(),m->szRemoteExecCommandline);
		tmp.sprintf("%dIcon",index);
		cfg.writeEntry(tmp.ptr(),m->szIcon);
		++index;
	}
}
