//=============================================================================
//
//   File : kvi_regchan.cpp
//   Creation date : Sat Jun 29 01:01:16 2002 GMT by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2001-2007 Szymon Stefanek (pragma at kvirc dot net)
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



#include "kvi_regchan.h"
#include "kvi_config.h"
#include "kvi_qstring.h"

KviRegisteredChannel::KviRegisteredChannel(const QString &name,const QString &netmask)
{
	m_szName = name;
	m_szNetMask = netmask;
	m_pPropertyDict = new QHash<QString,QString>;
}

KviRegisteredChannel::~KviRegisteredChannel()
{
	delete m_pPropertyDict;
}



KviRegisteredChannelDataBase::KviRegisteredChannelDataBase()
{
	m_pChannelDict = new QHash<QString,KviRegisteredChannelList*>;
}

KviRegisteredChannelDataBase::~KviRegisteredChannelDataBase()
{
	foreach(KviRegisteredChannelList* l,*m_pChannelDict)
	{
		delete l;
	}
	delete m_pChannelDict;
}

void KviRegisteredChannelDataBase::load(const QString& filename)
{
	KviConfig cfg(filename,KviConfig::Read);
	m_pChannelDict->clear();
	KviConfigIterator it(*(cfg.dict()));
	while(KviConfigGroup * d = it.current())
	{
		QString szMask = it.currentKey();
		QString szChan(szMask);
		KviQString::cutFromLast(szChan,'@',false);
		KviQString::cutToLast(szMask,'@');
		KviRegisteredChannel * c = new KviRegisteredChannel(szChan,szMask);
		add(c);
		KviConfigGroupIterator sit(*d);
		while(QString * s = sit.current())
		{
			c->setProperty(sit.currentKey(),*s);
			++sit;
		}
		++it;
	}
}

void KviRegisteredChannelDataBase::save(const QString& filename)
{
	KviConfig cfg(filename,KviConfig::Write);
	cfg.clear();

	foreach(KviRegisteredChannelList * l,*m_pChannelDict)
	{
		foreach(KviRegisteredChannel * c,*l)
		{
			QString szGrp=c->name()+"@"+c->netMask();
			cfg.setGroup(szGrp);
			QHash<QString, QString>::const_iterator i = c->propertyDict()->constBegin();
			 while (i != c->propertyDict()->constEnd()) {
				 cfg.writeEntry(i.key(),i.value());
			     ++i;
			 } 
		}
	}
}

KviRegisteredChannel * KviRegisteredChannelDataBase::find(const QString& name,const QString& net)
{
	KviRegisteredChannelList * l = m_pChannelDict->value(name);
	if(!l)return 0;
	foreach(KviRegisteredChannel * c,*l)
	{
		if(KviQString::matchStringCI(c->netMask(),net))return c;
	}
	
	return 0;
}

KviRegisteredChannel * KviRegisteredChannelDataBase::findExact(const QString& name,const QString& netmask)
{
	KviRegisteredChannelList * l = m_pChannelDict->value(name);
	if(!l)return 0;
	foreach(KviRegisteredChannel * c,*l)
	{
		if(c->netMask()==netmask) return c;
	}
	return 0;
}

void KviRegisteredChannelDataBase::remove(KviRegisteredChannel * c)
{
	KviRegisteredChannelList * l = m_pChannelDict->value(c->name());
	if(!l)return;
	foreach(KviRegisteredChannel * ch,*l)
	{
		if(ch == c)
		{
			if(l->count() <= 1)
			{
				m_pChannelDict->remove(c->name());
				delete c;
			} else {
				l->removeRef(c);
			}
			return;
		}
	}
}


void KviRegisteredChannelDataBase::add(KviRegisteredChannel * c)
{
	KviRegisteredChannel * old = findExact(c->name(),c->netMask());
	if(old)
	{
		QHash<QString, QString>::const_iterator i = c->propertyDict()->constBegin();
		 while (i != c->propertyDict()->constEnd()) {
			 if(c->property(i.key()).isNull())
			 		c->setProperty(i.key(),i.value());
		     ++i;
		 }
		remove(old);
	}
	KviRegisteredChannelList * l = m_pChannelDict->value(c->name());
	if(!l)
	{
		l = new KviRegisteredChannelList;
		m_pChannelDict->insert(c->name(),l);
	}
	// insert where there are less wildcards
	int o = c->netMask().count('*');
	int idx = 0;
	foreach(KviRegisteredChannel * rc,*l)
	{
		if(rc->netMask().count('*') > o)
		{
			// the existing has more wildcards , insert here!
			l->insert(idx,c);
			return;
		}
		idx++;
	}
	l->append(c);
}

