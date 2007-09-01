//=============================================================================
//
//   File : kvi_avatarcache.cpp
//   Created on Sat 27 Dec 2003 21:19:47 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2003 Szymon Stefanek <pragma at kvirc dot net>
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



#include "kvi_avatarcache.h"
#include "kvi_list.h"
#include "kvi_config.h"

// this level triggers a cleanup
#define MAX_AVATARS_IN_CACHE 100
// this is the level that has be reached by a cleanup
#define CACHE_GUARD_LEVEL 85
// dictionary size
#define CACHE_DICT_SIZE 101
// keep the unaccessed avatars for 30 days
#define MAX_UNACCESSED_TIME (3600 * 24 * 30)


KviAvatarCache * KviAvatarCache::m_pAvatarCacheInstance = 0;

void KviAvatarCache::init()
{
	if(m_pAvatarCacheInstance)
	{
		debug("WARNING: trying to initialize the avatar cache twice");
		return;
	}
	
	m_pAvatarCacheInstance = new KviAvatarCache();
}

void KviAvatarCache::done()
{
	if(!m_pAvatarCacheInstance)
	{
		debug("WARNING: trying to destroy an uninitialized avatar cache");
		return;
	}
	
	delete m_pAvatarCacheInstance;
	m_pAvatarCacheInstance = 0;
}


KviAvatarCache::KviAvatarCache()
{
	m_pAvatarDict = new QHash<QString,KviAvatarCacheEntry*>;
}

KviAvatarCache::~KviAvatarCache()
{
	foreach(KviAvatarCacheEntry* e,*m_pAvatarDict)
	{
		delete e;
	}
	delete m_pAvatarDict;
}


void KviAvatarCache::replace(const QString &szIdString,const KviIrcMask &mask,const QString &szNetwork)
{
	QString szKey;

	mask.mask(szKey,KviIrcMask::NickCleanUserSmartNet);
	szKey.append(QChar('+'));
	szKey.append(szNetwork);
	
	KviAvatarCacheEntry * e = new KviAvatarCacheEntry;
	e->szIdString = szIdString;
	e->tLastAccess = kvi_unixTime();
	
	m_pAvatarDict->insert(szKey,e);
	
	if(m_pAvatarDict->count() > MAX_AVATARS_IN_CACHE)
	{
		cleanup();
	}
}

void KviAvatarCache::remove(const KviIrcMask &mask,const QString &szNetwork)
{
	QString szKey;

	mask.mask(szKey,KviIrcMask::NickCleanUserSmartNet);
	szKey.append(QChar('+'));
	szKey.append(szNetwork);
	
	if(m_pAvatarDict->contains(szKey))
	{
		delete m_pAvatarDict->take(szKey);
	}
}



const QString & KviAvatarCache::lookup(const KviIrcMask &mask,const QString &szNetwork)
{
	QString szKey;

	mask.mask(szKey,KviIrcMask::NickCleanUserSmartNet);
	szKey.append(QChar('+'));
	szKey.append(szNetwork);

	KviAvatarCacheEntry * e = m_pAvatarDict->value(szKey);
	if(!e)return KviQString::empty;
	e->tLastAccess = kvi_unixTime();
	return e->szIdString;
}

void KviAvatarCache::load(const QString &szFileName)
{
	foreach(KviAvatarCacheEntry *e,*m_pAvatarDict)
	{
		delete e;
	}
	m_pAvatarDict->clear();

	KviConfig cfg(szFileName,KviConfig::Read);

	kvi_time_t tNow = kvi_unixTime();

	KviConfigIterator it(cfg.dict()->begin());
	
	int cnt = 0;
	
	while(it != cfg.dict()->end())
	{
		cfg.setGroup(it.key());

		kvi_time_t tLastAccess = cfg.readUIntEntry("LastAccess",0);
		if((tNow - tLastAccess) < MAX_UNACCESSED_TIME)
		{
			QString szIdString = cfg.readQStringEntry("Avatar","");

			if(!szIdString.isEmpty())
			{
				KviAvatarCacheEntry * e = new KviAvatarCacheEntry;
				e->tLastAccess = tLastAccess;
				e->szIdString = szIdString;
				m_pAvatarDict->insert(it.key(),e);
				cnt++;
				if(cnt >= MAX_AVATARS_IN_CACHE)return; // done
			}
		}
		++it;
	}
}

void KviAvatarCache::save(const QString &szFileName)
{
	KviConfig cfg(szFileName,KviConfig::Write);
//	cfg.clear(); // not needed with KviConfig::Write

	QHash<QString,KviAvatarCacheEntry*>::iterator it(m_pAvatarDict->begin());

	while(it != m_pAvatarDict->end())
	{
		KviAvatarCacheEntry * e = it.value();
		if(e->tLastAccess)
		{
			cfg.setGroup(it.key());
			cfg.writeEntry("Avatar",e->szIdString);
			cfg.writeEntry("LastAccess",((unsigned int)(e->tLastAccess)));
		}
		++it;
	}
}

void KviAvatarCache::cleanup()
{
	// first do a quick run deleting the avatars really too old
	QHash<QString,KviAvatarCacheEntry*>::iterator it(m_pAvatarDict->begin());	
	kvi_time_t tNow = kvi_unixTime();

	while(it != m_pAvatarDict->end())
	{
		KviAvatarCacheEntry * e = it.value();
		if((tNow - e->tLastAccess) > MAX_UNACCESSED_TIME)
		{
			delete e;
			it = m_pAvatarDict->erase(it);
		} else {
			++it;
		}
	}

	if(m_pAvatarDict->count() < CACHE_GUARD_LEVEL)return;
	
	// not done.. need to kill the last accessed :/
	
	QMap<kvi_time_t,QString> times;
	
	
	// here we use the cache entries in another way
	// szAvatar is the KEY instead of the avatar name
	
	while(it != m_pAvatarDict->end())
	{
		times[it.value()->tLastAccess]=it.key();
	}
	
	// the oldest keys are at the beginning
	int uRemove = times.count() - CACHE_GUARD_LEVEL;
	if(uRemove < 1)return; // huh ?

	// remember that szAvatar contains the key!
	QMapIterator<kvi_time_t,QString> i(times);
	while (i.hasNext()) {
		 i.next();
		 uRemove--;
	     delete m_pAvatarDict->take(i.value());
	}
	// now we should be ok
}
