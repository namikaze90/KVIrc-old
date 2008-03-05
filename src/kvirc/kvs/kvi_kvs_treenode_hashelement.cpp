//=============================================================================
//
//   File : kvi_kvs_treenode_hashelement.cpp
//   Created on Tue 07 Oct 2003 03:04:18 by Szymon Stefanek
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



#include "kvi_kvs_treenode_hashelement.h"
#include "kvi_kvs_runtimecontext.h"
#include "kvi_locale.h"
#include "kvi_kvs_hash.h"
#include "kvi_kvs_object.h"

KviKvsTreeNodeHashElement::KviKvsTreeNodeHashElement(const QChar * pLocation,KviKvsTreeNodeData * pSource,KviKvsTreeNodeData * pKey)
: KviKvsTreeNodeArrayOrHashElement(pLocation,pSource)
{
	m_pKey = pKey;
	m_pKey->setParent(this);
}


KviKvsTreeNodeHashElement::~KviKvsTreeNodeHashElement()
{
	delete m_pKey;
}

void KviKvsTreeNodeHashElement::contextDescription(QString &szBuffer)
{
	szBuffer = "Hash Element Evaluation";
}

void KviKvsTreeNodeHashElement::dump(const char * prefix)
{
	debug("%s HashElement",prefix);
	QString tmp = prefix;
	tmp.append("  ");
	m_pSource->dump(tmp.utf8().data());
	m_pKey->dump(tmp.utf8().data());
}


bool KviKvsTreeNodeHashElement::evaluateReadOnlyInObjectScope(KviKvsObject *o,KviKvsRunTimeContext * c,KviKvsVariant * pBuffer)
{
	KviKvsVariant key;
	if(!m_pKey->evaluateReadOnly(c,&key))return false;

	QString szKey;
	key.asString(szKey);

	if(szKey.isEmpty())
	{
		c->warning(this,__tr2qs("Hash key evaluated to empty string: fix the script"));
		pBuffer->setNothing();
		return true;
	}

	KviKvsVariant val;
	if(o)
	{
		if(!m_pSource->evaluateReadOnlyInObjectScope(o,c,&val))return false;
	} else {
		if(!m_pSource->evaluateReadOnly(c,&val))return false;
	}

	if(!val.isHash())
	{
		if(!val.isNothing())
		{
			QString szType;
			val.getTypeName(szType);
			c->warning(this,__tr2qs("The argument of the {} subscript didn't evaluate to a hash: automatic conversion from type '%Q' supplied"),&szType);
		}
		pBuffer->setNothing();
		return true;
	}

	KviKvsVariant * v = val.hash()->find(szKey);
	if(!v)
	{
		pBuffer->setNothing();
		return true;
	}

	pBuffer->copyFrom(v);
	return true;
}

KviKvsRWEvaluationResult * KviKvsTreeNodeHashElement::evaluateReadWriteInObjectScope(KviKvsObject *o,KviKvsRunTimeContext * c)
{
	KviKvsVariant key;
	if(!m_pKey->evaluateReadOnly(c,&key))return 0;

	QString szKey;
	key.asString(szKey);

	if(szKey.isEmpty())
	{
		c->warning(this,__tr2qs("Hash key evaluated to empty string: fix the script"));
	}

	KviKvsRWEvaluationResult * result;
	if(o)result = m_pSource->evaluateReadWriteInObjectScope(o,c);
	else result = m_pSource->evaluateReadWrite(c);
	if(!result)return 0;

	if(!result->result()->isHash())
	{
		// convert to hash in some way
//#warning "Supply a *real* conversion from other types to array ?"
		if(!result->result()->isNothing())
		{
			QString szType;
			result->result()->getTypeName(szType);
			c->warning(this,__tr2qs("The argument of the {} subscript didn't evaluate to a hash automatic conversion from %Q supplied"),&szType);
		}
		result->result()->setHash(new KviKvsHash());
	}

	return new KviKvsHashElement(result,result->result()->hash()->get(szKey),result->result()->hash(),szKey);
}


bool KviKvsTreeNodeHashElement::evaluateReadOnly(KviKvsRunTimeContext * c,KviKvsVariant * pBuffer)
{
	return evaluateReadOnlyInObjectScope(0,c,pBuffer);
}

KviKvsRWEvaluationResult * KviKvsTreeNodeHashElement::evaluateReadWrite(KviKvsRunTimeContext * c)
{
	return evaluateReadWriteInObjectScope(0,c);
}
