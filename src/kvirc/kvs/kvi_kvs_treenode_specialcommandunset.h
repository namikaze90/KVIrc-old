#ifndef _KVI_KVS_TREENODE_SPECIALCOMMANDUNSET_H_
#define _KVI_KVS_TREENODE_SPECIALCOMMANDUNSET_H_
//=============================================================================
//
//   File : kvi_kvs_treenode_specialcommandunset.h
//   Created on Thu 14 Apr 2005 02:44:00 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2005 Szymon Stefanek <pragma at kvirc dot net>
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

#include "kvi_settings.h"
#include "kvi_qstring.h"
#include "kvi_kvs_treenode_specialcommand.h"
#include "kvi_kvs_treenode_variable.h"

class KviKvsRunTimeContext;

class KVIRC_API KviKvsTreeNodeSpecialCommandUnset : public KviKvsTreeNodeSpecialCommand
{
public:
	KviKvsTreeNodeSpecialCommandUnset(const QChar * pLocation,QList<KviKvsTreeNodeVariable*> * pVarList);
	~KviKvsTreeNodeSpecialCommandUnset();
private:
	QList<KviKvsTreeNodeVariable*> * m_pVariableList;
public:
	virtual void contextDescription(QString &szBuffer);
	virtual void dump(const char * prefix);
	virtual bool execute(KviKvsRunTimeContext * c);
};

#endif //!_KVI_KVS_TREENODE_SPECIALCOMMANDUNSET_H_
