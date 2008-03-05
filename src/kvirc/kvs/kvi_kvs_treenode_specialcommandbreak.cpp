//=============================================================================
//
//   File : kvi_kvs_treenode_specialcommandbreak.cpp
//   Created on Thu 06 Nov 2003 12:15:00 by Szymon Stefanek
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



#include "kvi_kvs_treenode_specialcommandbreak.h"
#include "kvi_kvs_runtimecontext.h"
#include "kvi_locale.h"

KviKvsTreeNodeSpecialCommandBreak::KviKvsTreeNodeSpecialCommandBreak(const QChar * pLocation)
: KviKvsTreeNodeSpecialCommand(pLocation,"break")
{
}

KviKvsTreeNodeSpecialCommandBreak::~KviKvsTreeNodeSpecialCommandBreak()
{
}

void KviKvsTreeNodeSpecialCommandBreak::contextDescription(QString &szBuffer)
{
	szBuffer = "Special Command \"break\"";
}


void KviKvsTreeNodeSpecialCommandBreak::dump(const char * prefix)
{
	debug("%s SpecialCommandBreak",prefix);
}

bool KviKvsTreeNodeSpecialCommandBreak::execute(KviKvsRunTimeContext * c)
{
	c->setBreakPending();
	return false;
}
