//=============================================================================
//
//   File : kvi_kvs_aliasmanager.cpp
//   Created on Mon 15 Dec 2003 02:11:41 by Szymon Stefanek
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



#include "kvi_kvs_aliasmanager.h"
#include "kvi_config.h"

KviKvsAliasManager * KviKvsAliasManager::m_pAliasManager = 0;

KviKvsAliasManager::KviKvsAliasManager()
{
	m_pAliasManager = this;
	m_pAliasDict = new QHash<QString,KviKvsScript*>;
}

KviKvsAliasManager::~KviKvsAliasManager()
{
	foreach(KviKvsScript* s,*m_pAliasDict)
	{
		delete s;
	}
	delete m_pAliasDict;
}

void KviKvsAliasManager::clear()
{
	foreach(KviKvsScript* s,*m_pAliasDict)
	{
		delete s;
	}
	m_pAliasDict->clear();
}

void KviKvsAliasManager::init()
{
	if(KviKvsAliasManager::instance())
	{
		debug("WARNING: Trying to create the KviKvsAliasManager twice!");
		return;
	}
	(void)new KviKvsAliasManager();
}

void KviKvsAliasManager::done()
{
	if(!KviKvsAliasManager::instance())
	{
		debug("WARNING: Trying to destroy the KviKvsAliasManager twice!");
		return;
	}
	delete KviKvsAliasManager::instance();
}

void KviKvsAliasManager::completeCommand(const QString &word,QStringList& matches)
{
	foreach(KviKvsScript* s,*m_pAliasDict)
	{
		if(KviQString::equalCIN(word,s->name(),word.length()))
			matches.append(s->name());
	}
}

// FIXME: #warning "A binary config would work better and faster here!"

void KviKvsAliasManager::save(const QString & filename)
{
	KviConfig cfg(filename,KviConfig::Write);
	cfg.clear();

	foreach(KviKvsScript* s,*m_pAliasDict)
	{
		cfg.setGroup(s->name());
		cfg.writeEntry("_Buffer",s->code());
	}
}

void KviKvsAliasManager::load(const QString & filename)
{
	foreach(KviKvsScript* s,*m_pAliasDict)
	{
		delete s;
	}
	m_pAliasDict->clear();
	KviConfig cfg(filename,KviConfig::Read);

	QHash<QString,KviConfigGroup*>::iterator it(cfg.dict()->begin());

	while(it!=cfg.dict()->end())
	{
		QString s = it.key();
		cfg.setGroup(s);
		QString szCode = cfg.readQStringEntry("_Buffer","");
		if(!szCode.isEmpty())
		{
			KviKvsScript * m = new KviKvsScript(s,szCode);
			m_pAliasDict->insert(s,m);
		}
		++it;
	}
}


