//=============================================================================
//
//   File : kvi_kvs_array.cpp
//   Created on Tue 07 Oct 2003 01:07:31 by Szymon Stefanek
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



#include "kvi_kvs_array.h"

KviKvsArray::KviKvsArray()
: KviHeapObject()
{

}

KviKvsArray::KviKvsArray(const KviKvsArray &a)
: KviHeapObject()
{
	for(int i=0;i<a.count();i++)
	{
		KviKvsVariant *v = a.at(i);
		if(v)
		{
			append(new KviKvsVariant(*v));
		} else {
			append(0);
		}
	}
}

KviKvsArray::~KviKvsArray()
{
	removeAll(0);
	qDeleteAll(begin(),end());
}

bool compareLessThan(const KviKvsVariant* s1, const KviKvsVariant* s2)
{
    return s1->compare(s2,true) > 0 ? true : false;
}

bool compareGreatherThan(const KviKvsVariant* s1, const KviKvsVariant* s2)
{
    return s1->compare(s2,true) < 0 ? true : false;
}

void KviKvsArray::sort()
{
	removeAll(0);
	qSort(begin(), end(), compareLessThan);
}

void KviKvsArray::rsort()
{
	removeAll(0);
	qSort(begin(), end(), compareGreatherThan);
}

void KviKvsArray::unset(kvs_uint_t uIdx)
{
	if(uIdx < size())
	{
		delete takeAt(uIdx);
	}
}

void KviKvsArray::set(kvs_uint_t uIdx,KviKvsVariant * pVal)
{
	if(uIdx < size())
		replace(uIdx,pVal);
	else
		insert(uIdx,pVal);
}

KviKvsVariant * KviKvsArray::getAt(kvs_uint_t uIdx)
{
	if(uIdx > size())
		insert(uIdx,new KviKvsVariant());
	return at(uIdx);
}

void KviKvsArray::serialize(QString& result)
{
	QString tmpBuffer;
	result="[";
	bool bNeedComma = false;
	KviKvsArray::const_iterator it(constBegin());
	while(it!=constEnd())
	{
		if(bNeedComma)result.append(',');
		else bNeedComma = true;
		if(*it) {
			(*it)->serialize(tmpBuffer);
			result.append(tmpBuffer);
		} else {
			result.append("null");
		}
		++it;
	}
	result.append(']');
}

void KviKvsArray::appendAsString(QString &szBuffer)
{
	bool bNeedComma = false;
	KviKvsArray::const_iterator it(constBegin());
	while(it!=constEnd())
	{
		if(bNeedComma)szBuffer.append(',');
		else bNeedComma = true;
		if(*it)(*it)->appendAsString(szBuffer);
		++it;
	}
}

