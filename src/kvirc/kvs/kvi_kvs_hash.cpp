//=============================================================================
//
//   File : kvi_kvs_hash.cpp
//   Created on Tue 07 Oct 2003 01:22:37 by Szymon Stefanek
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

#define __KVIRC__

#include "kvi_kvs_hash.h"

KviKvsHash::KviKvsHash()
{
	m_pDict = new QHash<QString, KviKvsVariant*>();
}

KviKvsHash::KviKvsHash(const KviKvsHash &h)
{
	m_pDict = new QHash<QString, KviKvsVariant*>();
	QHash<QString, KviKvsVariant*>::const_iterator i(
			h.dict()->constBegin()
			);
	 while (i != h.dict()->constEnd()) {
	     m_pDict->insert(i.key(),new KviKvsVariant(i.value()));
	     ++i;
	 } 
}

KviKvsHash::~KviKvsHash()
{
	foreach(KviKvsVariant* p,*m_pDict)
	{
		delete p;
	}
	delete m_pDict;
}

void KviKvsHash::appendAsString(QString &szBuffer) const
{
	QHash<QString, KviKvsVariant*>::iterator it(m_pDict->begin());
	bool bNeedComma = false;
	while (it != m_pDict->end()) {
		KviKvsVariant * s = it.value();
		if(bNeedComma)szBuffer.append(',');
		else bNeedComma = true;
		s->appendAsString(szBuffer);
		++it;
	}
}

void KviKvsHash::serialize(QString& result)
{
	QString tmpBuffer;
	result="{";
	QHash<QString, KviKvsVariant*>::iterator it(m_pDict->begin());
	bool bNeedComma = false;
	while (it != m_pDict->end()) {
		
		KviKvsVariant * s = it.value();
		if(bNeedComma)result.append(',');
		else bNeedComma = true;
		
		tmpBuffer = it.key();
		KviKvsVariant::serializeString(tmpBuffer);
		result.append(tmpBuffer);

		result.append(":");
		s->serialize(tmpBuffer);
		result.append(tmpBuffer);

		++it;
	}
	result.append('}');
}

KviKvsVariant * KviKvsHash::get(const QString &szKey)
{
	KviKvsVariant * v = m_pDict->value(szKey);
	if(v)return v;
	v = new KviKvsVariant();
	m_pDict->insert(szKey,v);
	return v;
}
