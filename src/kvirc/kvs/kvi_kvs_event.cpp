//=============================================================================
//
//   File : kvi_kvs_event.cpp
//   Created on Mon 23 Feb 2004 03:00:02 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2004 Szymon Stefanek <pragma at kvirc dot net>
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



#include "kvi_kvs_event.h"


KviKvsEvent::~KviKvsEvent()
{
	clear();
}

void KviKvsEvent::clear()
{
	if(m_pHandlers)delete m_pHandlers;
	m_pHandlers = 0;
}

void KviKvsEvent::removeHandler(KviKvsEventHandler * h)
{
	m_pHandlers->removeAll(h);
	if(m_pHandlers->isEmpty())
	{
		delete m_pHandlers;
		m_pHandlers = 0;
	}
}

void KviKvsEvent::addHandler(KviKvsEventHandler * h)
{
	if(!m_pHandlers)
	{
		m_pHandlers = new QList<KviKvsEventHandler*>;
	}
	m_pHandlers->append(h);
}

void KviKvsEvent::clearScriptHandlers()
{
	if(!m_pHandlers)return;
	QList<KviKvsEventHandler*>::iterator it(m_pHandlers->begin());
	while(it != m_pHandlers->end())
	{
		if((*it)->type() == KviKvsEventHandler::Script)
			it = m_pHandlers->erase(it);
		else
			++it;	
	}

	if(m_pHandlers->isEmpty())
	{
		delete m_pHandlers;
		m_pHandlers = 0;
	}
}
