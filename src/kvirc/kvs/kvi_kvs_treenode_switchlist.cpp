//=============================================================================
//
//   File : kvi_kvs_treenode_switchlist.cpp
//   Created on Tue 07 Oct 2003 02:06:53 by Szymon Stefanek
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



#include "kvi_kvs_treenode_switchlist.h"
#include "kvi_kvs_runtimecontext.h"

KviKvsTreeNodeSwitchList::KviKvsTreeNodeSwitchList(const QChar * pLocation)
: KviKvsTreeNode(pLocation)
{
	m_pShortSwitchDict = 0;
	m_pLongSwitchDict = 0;
}

KviKvsTreeNodeSwitchList::~KviKvsTreeNodeSwitchList()
{
	if(m_pShortSwitchDict)delete m_pShortSwitchDict;
	if(m_pLongSwitchDict)delete m_pLongSwitchDict;
}

void KviKvsTreeNodeSwitchList::contextDescription(QString &szBuffer)
{
	szBuffer = "Switch List Evaluation";
}


void KviKvsTreeNodeSwitchList::dump(const char * prefix)
{
	debug("%s SwitchList",prefix);
	if(m_pShortSwitchDict)
	{
		QHash<int,KviKvsTreeNodeData*>::iterator it(m_pShortSwitchDict->begin());
		while(it != m_pShortSwitchDict->end())
		{
			QString tmp = prefix;
			tmp.append("  Sw(");
			QChar c((unsigned short)it.key());
			tmp.append(c);
			tmp.append("): ");
			it.value()->dump(tmp);
			++it;
		}
	}
	if(m_pLongSwitchDict)
	{
		QHash<QString,KviKvsTreeNodeData*>::iterator it(m_pLongSwitchDict->begin());
		while(it != m_pLongSwitchDict->end())
		{
			QString tmp = prefix;
			tmp.append("  Sw(");
			tmp.append(it.key());
			tmp.append("): ");
			it.value()->dump(tmp);
			++it;
		}
	}
}

void KviKvsTreeNodeSwitchList::addShort(int iShortKey,KviKvsTreeNodeData * p)
{
	if(!m_pShortSwitchDict)
	{
		m_pShortSwitchDict = new QHash<int,KviKvsTreeNodeData*>;
	}

	m_pShortSwitchDict->insert(iShortKey,p);
	p->setParent(this);
}

void KviKvsTreeNodeSwitchList::addLong(const QString &szLongKey,KviKvsTreeNodeData * p)
{
	if(!m_pLongSwitchDict)
	{
		m_pLongSwitchDict = new QHash<QString,KviKvsTreeNodeData*>;
	}

	m_pLongSwitchDict->insert(szLongKey,p);
	p->setParent(this);
}


bool KviKvsTreeNodeSwitchList::evaluate(KviKvsRunTimeContext * c,KviKvsSwitchList * pSwList)
{
	pSwList->clear();

	if(m_pShortSwitchDict)
	{
		QHash<int,KviKvsTreeNodeData*>::iterator it(m_pShortSwitchDict->begin());
		while(it != m_pShortSwitchDict->end())
		{
			KviKvsTreeNodeData * d = it.value();
			KviKvsVariant * v = new KviKvsVariant();
			if(!d->evaluateReadOnly(c,v))
			{
				delete v; 
				return false;
			}
			pSwList->addShort(it.key(),v);
			++it;
		}
	}
	if(m_pLongSwitchDict)
	{
		QHash<QString,KviKvsTreeNodeData*>::iterator it(m_pLongSwitchDict->begin());
		while(it != m_pLongSwitchDict->end())
		{
			KviKvsTreeNodeData * d = it.value();
			KviKvsVariant * v = new KviKvsVariant();
			if(!d->evaluateReadOnly(c,v))
			{
				delete v; 
				return false;
			}
			pSwList->addLong(it.key(),v);
			++it;
		}
	}
	return true;
}

KviKvsTreeNodeData * KviKvsTreeNodeSwitchList::getStandardRebindingSwitch()
{
	KviKvsTreeNodeData * d;
	if(m_pShortSwitchDict)
	{
		d = m_pShortSwitchDict->value('r');
		if(d)
		{
			m_pShortSwitchDict->remove('r');
			return d;
		}
	}
	if(m_pLongSwitchDict)
	{
		d = m_pLongSwitchDict->value("rebind");
		if(d)
		{
			m_pLongSwitchDict->remove("rebind");
			return d;
		}
	}
	return 0;
}

